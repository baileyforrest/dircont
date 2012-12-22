/* dircont.c - Directory Contents
 *
 * Bailey Forrest - baileycforrest@gmail.com
 *
 */

#include "dircont.h"
#include "hashes.h"

const char* usage = "Usage: %s [-hH] <directory>\n";
const char* executableFile = "Executable File";

static int dircount = 0;

int main(int argc, char** argv)
{
    int human = 1;
    int opt;
    while((opt = getopt(argc, argv, "hH:") != -1))
    {
        switch(opt)
        {
        case 'h':
            printf(usage, argv[0]);
            break;
        case 'H':
            human = 0;
            break;
        default:
            fprintf(stderr, usage, argv[0]);
            exit(EXIT_FAILURE);
            break;
        }
    }
    
    char* dir;
    if(argc > 1)
        dir = argv[argc - 1];
    else
    {
        dir = malloc(MAXLINE);
        if(fgets(dir, MAXLINE, stdin) == NULL)
            exit(EXIT_FAILURE);
        dir[MAXLINE - 1] = '\0';
    }

    DIR *pDir;

    if((pDir = opendir(dir)) == NULL)
    {
        fprintf(stderr, "Cannot open directory '%s'\n", dir);
        exit(EXIT_FAILURE);
    }

    hashtab *ht = ht_create(&freeFileInfo, &freeName, &hash_sstring, streql);
                        
    doDirCont(ht, pDir, dir);
    closedir(pDir);

    displayResults(ht, dir, human);
    
    return 0;
}

void doDirCont(hashtab *ht, DIR *pDir, char *path)
{
    struct dirent *pDirent;
    struct stat s;
    int pathlen = strlen(path);
    pathlen += 2; // 2 extra for '/' and '\0'

    while((pDirent = readdir(pDir)) != NULL)
    {
        char *newPath = malloc(pathlen + strlen(pDirent->d_name));
        if(newPath == NULL)
            exit(EXIT_FAILURE);

        sprintf(newPath, "%s/%s", path, pDirent->d_name);

        if(stat(newPath, &s) < 0)
        {
            free(newPath);
            continue;
        }

        if(s.st_mode & S_IFDIR) // is directory
        {
            dircount++;
            // Prevent infinite loops on ., ..
            if(!strcmp(pDirent->d_name, ".") && 
               !strcmp(pDirent->d_name, ".."))
            {
                DIR* npDir;
                if((npDir = opendir(newPath)) != NULL)
                    doDirCont(ht, npDir, newPath);
                closedir(npDir);
            }
        }
        else
        {
            char *type = getExt(newPath);
            if(type == NULL)
            {
                if((s.st_mode & S_IEXEC) != 0) // executable file
                {
                    type = malloc(strlen(executableFile) + 1);
                    strcpy(type, executableFile);
                }
                else
                {
                    type = malloc(strlen(pDirent->d_name) + 1);
                    strcpy(type, pDirent->d_name);
                }
                    
            }

            fileInfo* fi = ht_get(ht, type);

            if(fi) // type already in hashtable
            {
                fi->count++;
                fi->size += s.st_size;
                free(type);
            }
            else
            {
                if((fi = malloc(sizeof(fileInfo))) == NULL)
                    exit(EXIT_FAILURE);
                fi->size = s.st_size;
                fi->name = type;
                fi->count = 1;
            }
        }
        free(newPath);
    }
}

void displayResults(hashtab *ht, char* dir, int human)
{
    htelem *elemlist = toList(ht);
    htelem *sorted = NULL;
    htelem *p, *prev;

    unsigned long totalsize = 0;
    unsigned totalfiles = 0;

    fileInfo *fi;

    //insertion sort into sorted list (greatest to least)
    while(elemlist != NULL)
    {
        fi = (fileInfo *)elemlist->elem;
        int count = fi->count;
        totalsize += fi->size;
        totalfiles += fi->count;

        if(sorted == NULL)
        {
            sorted = elemlist;
            elemlist = elemlist->next;
            sorted->next = NULL;
        }
        else
        {
            p = sorted;
            prev = sorted;
            while(p != NULL)
            {
                if(count > ((fileInfo *)p->elem)->count)
                {
                    if(p == prev) // begining of list
                    {
                        sorted = elemlist;
                        elemlist = elemlist->next;
                        sorted->next = p;
                        break;
                    }
                    else
                    {
                        prev->next = elemlist;
                        elemlist = elemlist->next;
                        prev->next->next = p->next;
                        break;
                    }
                }
                prev = p;
                p = p->next;
            }
        }
    }

    //need to add nonhuman mode
    human = human;
    
    printf("File Contents of %s\n", dir);
    for(p = sorted; p != NULL;)
    {
        fi = (fileInfo *)p->elem;
        printf("%10s %5d %20lu %2.2f\n", fi->name, fi->count, fi->size,
               (float)fi->count / (float)totalfiles);
        
        htelem *next = p->next;

        free(fi->name);
        free(p->elem);
        free(p->key);
        free(p);

        p = next;
    }

    printf("%d files in %d directories (%lu B)", totalfiles, dircount,
           totalsize);
}

void freeFileInfo(fileInfo *fi)
{
    free(fi->name);
    free(fi);
}

void freeName(char *name)
{
    free(name);
}

int streql(char *s1, char* s2)
{
    return !strcmp(s1, s2);
}

//return the extension
char* getExt(char* name)
{
    char *dot;
    if((dot = strrchr(name, '.')) == NULL)
        return NULL;
    

    char *ext = malloc(strlen(dot) + 1);
    strcpy(ext, dot);

    return ext;
}
