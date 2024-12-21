#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include "BDecoding.h"
#include "BStruct.h"

char DictionaryStart = 'd';
char DictionaryEnd = 'e';
char ListStart = 'l';
char ListEnd = 'e';
char NumberStart = 'i';
char NumberEnd = 'e';
char ByteArrayDivider = ':';

int countList = 1;
int countDict = 1;

bool insideTorrentAnnounce = false;
bool insideTorrentCreatedBy = false;
bool insideTorrentCreationDate = false;
bool insideTorrentEncoding = false;

// Opens the file and puts its content in a char array
char *decodeFile(const char *path)
{
    FILE *fp = fopen(path, "r");
    if (fp == NULL)
    {
        printf("Unable to find the file");
        return NULL;
    }

    // Go to the end of the file to get the size
    fseek(fp, 0L, SEEK_END);
    int file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char c;
    // To return a char array we have to malloc
    char *bytes = malloc(file_size);
    bytes[0] = '\0';
    while ((c = fgetc(fp)) != EOF)
    {
        strncat(bytes, &c, 1);
    }
    fclose(fp);
    return bytes;
}

// Creates an iterator and loops through the array
void decode(char *bytes, Torrent *torrent)
{
    // The iterator takes the address of bytes as it is the address of the first element of the array (where to start)
    char *iterator = bytes;
    while (*iterator != '\0')
    {
        decodeNextObject(&iterator, torrent);
    }
}

void *decodeNextObject(char **iterator, Torrent *torrent)
{
    if (**iterator == NumberStart)
    {
        long *number = decodeNumber(iterator);
        printf("Number: %ld\n", *number);
        free(number);
        return NULL;
    }
    else if (isdigit(**iterator))
    {
        char *string = decodeByteArray(iterator);
        if (insideTorrentAnnounce)
        {
            torrent->announce = string;
            insideTorrentAnnounce = false;
        }
        if (strcmp(string, "announce") == 0)
        {
            insideTorrentAnnounce = true;
        }
        printf("String: %s\n", string);
        free(string);
        return NULL;
    }
    else if (**iterator == ListStart)
    {
        decodeList(iterator, torrent);
        return NULL;
    }
    else if (**iterator == DictionaryStart)
    {
        decodeDictionnary(iterator, torrent);
        return NULL;
    }

    return NULL;
}

// TODO: allouer le buffer dynamiquement
long *decodeNumber(char **iterator)
{
    // The buffer will contain the extracted long int
    char buffer[256];
    int index = 0;

    // Skips 'i'
    (*iterator)++;

    while (**iterator != NumberEnd)
    {
        buffer[index] = **iterator;
        index++;
        (*iterator)++;
    }

    // Skips 'e'
    (*iterator)++;

    // Not forget to terminate the string correctly
    buffer[index] = '\0';
    long *resp = malloc(sizeof(long));
    *resp = atol(buffer);
    return resp;
}

// TODO: allouer le buffer dynamiquement
char *decodeByteArray(char **iterator)
{
    // The string_length will contain the extracted int at the beginning
    char string_length[256];
    int index_length = 0;

    while (**iterator != ByteArrayDivider)
    {
        if (index_length >= 255)
        {
            printf("Buffer overflow\n");
            printf("%c\n", **iterator);
            return NULL;
        }
        string_length[index_length] = **iterator;
        index_length++;
        (*iterator)++;
    }
    string_length[index_length] = '\0';

    // Skips ':'
    (*iterator)++;

    int length = atoi(string_length);
    char *buffer = malloc(length);
    int index = 0;
    while (index < length)
    {
        buffer[index] = **iterator;
        index++;
        (*iterator)++;
    }
    buffer[index] = '\0';

    return buffer;
}

void *decodeList(char **iterator, Torrent *torrent)
{
    //  Skips 'l'
    (*iterator)++;

    while (**iterator != ListEnd)
    {
        if (**iterator == NumberStart)
        {
            // DecodeNumber(iterator);
            long *number = decodeNumber(iterator);
            printf("Number in list %d: %ld\n", countList, *number);
            free(number);
            continue;
        }
        else if (isdigit(**iterator))
        {
            // DecodeByteArray(iterator);
            char *string = decodeByteArray(iterator);
            // Warning here, don't put *res in printf, doesn't work with %s
            printf("String in list %d: %s\n", countList, string);
            free(string);
            continue;
        }
        else if (**iterator == ListStart)
        {
            countList++;
            decodeList(iterator, torrent);
            continue;
        }
        else if (**iterator == DictionaryStart)
        {
            countDict++;
            decodeDictionnary(iterator, torrent);
            continue;
        }
        (*iterator)++;
    }

    // Skips 'e'
    (*iterator)++;

    return NULL;
}

// TODO: trier par ordre les éléments du dict
// TODO: associer les éléments par pair
void *decodeDictionnary(char **iterator, Torrent *torrent)
{
    //  Skips 'd'
    (*iterator)++;

    while (**iterator != DictionaryEnd)
    {
        // Numbers
        if (**iterator == NumberStart)
        {
            // DecodeNumber(iterator);
            long *number = decodeNumber(iterator);
            printf("Number in dict %d: %ld\n", countDict, *number);
            free(number);
            continue;
        }
        // Strings
        else if (isdigit(**iterator))
        {
            // Warning here, don't put *string in printf, doesn't work with %s
            char *string = decodeByteArray(iterator);
            match(string, torrent);
            printf("String in dict %d: %s\n", countDict, string);
            free(string);
            continue;
        }
        else if (**iterator == ListStart)
        {
            countList++;
            decodeList(iterator, torrent);
            continue;
        }
        else if (**iterator == DictionaryStart)
        {
            countDict++;
            decodeDictionnary(iterator, torrent);
            continue;
        }
        (*iterator)++;
    }

    // Skips 'e'
    (*iterator)++;

    return NULL;
}

// TODO: faire ça avec une boucle (peut être hashmap + switch)
// TODO: s'occuper des tous les options de la struct
void *match(char *string, Torrent *torrent)
{
    if (insideTorrentAnnounce)
    {
        torrent->announce = malloc(strlen(string) + 1);
        strcpy(torrent->announce, string);
        insideTorrentAnnounce = false;
    }
    else if (insideTorrentCreatedBy)
    {
        torrent->createdBy = malloc(strlen(string) + 1);
        strcpy(torrent->createdBy, string);
        insideTorrentCreatedBy = false;
    }
    else if (insideTorrentCreationDate)
    {

        torrent->creationDate = malloc(strlen(string) + 1);
        strcpy(torrent->creationDate, string);
        insideTorrentCreationDate = false;
    }
    else if (insideTorrentEncoding)
    {
        torrent->encoding = malloc(strlen(string) + 1);
        strcpy(torrent->encoding, string);
        insideTorrentEncoding = false;
    }

    if (strcmp(string, "announce") == 0)
    {
        insideTorrentAnnounce = true;
        return NULL;
    }
    else if (strcmp(string, "created by") == 0)
    {
        insideTorrentCreatedBy = true;
        return NULL;
    }
    else if (strcmp(string, "creation date") == 0)
    {
        insideTorrentCreationDate = true;
        return NULL;
    }
    else if (strcmp(string, "encoding") == 0)
    {
        insideTorrentEncoding = true;
        return NULL;
    }
    return NULL;
}

int freeAll(char *bytes, Torrent *torrent)
{
    free(torrent->announce);
    free(torrent->createdBy);
    free(torrent->creationDate);
    free(torrent->encoding);
    free(bytes);
    printf("You are free\n");
    return 0;
};

int main()
{
    const char *path = "TestTorrent.txt.torrent";
    Torrent torrent;
    char *bytes = decodeFile(path);
    decode(bytes, &torrent);
    printf("Torrent announce: %s\n", torrent.announce);
    printf("Torrent created by: %s\n", torrent.createdBy);
    printf("Torrent creation date: %lu\n", torrent.creationDate);
    printf("Torrent encoding: %s\n", torrent.encoding);
    freeAll(bytes, &torrent);
    return 0;
}