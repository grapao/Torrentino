#ifndef BDECODING_H
#define BDECODING_H

#include "BStruct.h"

char *decodeFile(const char *path);
void decode(char *bytes, Torrent *torrent);
void *decodeNextObject(char **iterator, Torrent *torrent);
long *decodeNumber(char **iterator);
char *decodeByteArray(char **iterator);
void *decodeList(char **iterator, Torrent *torrent);
void *decodeDictionnary(char **iterator, Torrent *torrent);
void *match(char *string, Torrent *torrent);
int freeAll(char *bytes, Torrent *torrent);

#endif