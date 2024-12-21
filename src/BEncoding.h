#ifndef BENCODING_H
#define BENCODING_H

char *EncodeFile(const char *path);
void Encode(char *bytes);
void *EncodeNextObject(char **iterator);
long *EncodeNumber(char **iterator);
char *EncodeByteArray(char **iterator);
void *EncodeList(char **iterator);
void *EncodeDictionnary(char **iterator);

#endif