#ifndef BSTRUCT_H
#define BSTRUCT_H

#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

typedef struct Tracker
{
    char *trackers;
} Tracker;

typedef struct FileItem
{
    char *name;
    int64_t length;
} FileItem;

typedef struct Torrent
{
    char *announce;
    // Pointer to an array of Tracker structs
    Tracker *trackers;
    size_t trackerCount;
    char *comment;
    char *createdBy;
    time_t creationDate;
    char *encoding;
    struct
    {
        // This struct is used when there are multiple files
        FileItem *files;
        size_t fileCount;
        // These two are only used when there is only one file
        int64_t length;
        char *name;
    } Info;
    int64_t pieceLength;
    char **pieces;
    size_t pieceCount;
    bool isPrivate;
} Torrent;

#endif