/* dircont.h - Directory Contents
 *
 * Bailey Forrest - baileycforrest@gmail.com
 *
 */

#ifndef DIRCONT_H
#define DIRCONT_H

#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include "hashtab.h"

#define MAXLINE 128

typedef struct fileInfo
{
    unsigned long size;
    char *name;
    int count;
    
} fileInfo;

void doDirCont(hashtab *ht, DIR *pDirent, char* path);
void displayResults(hashtab *ht);
void freeFileInfo(fileInfo *fi);
void freeName(char *name);
int streql(char *s1, char *s2);
char* getExt(char* name);

#endif

