/* dircont.c - Directory Contents
 *
 * Bailey Forrest - baileycforrest@gmail.com
 *
 */

#include "dircont.h"
#include "hashes.h"

const char* usage = "Usage: %s [-hH] [-n] <limit> <directory>\n";
const char* executableFile = "Executable File";
const char* noExtension = "No Extension";

static int dircount = 0;
static int limit = INT_MAX;

int main(int argc, char** argv)
{
    int human = 1;
    int opt;
    while((opt = getopt(argc, argv, "hHn:")) != -1)
    {
        switch(opt)
        {
        case 'h':
            printf(usage, argv[0]);
            exit(0);
            break;
        case 'H':
            human = 0;
            break;
        case 'n':
            if((limit = atoi(optarg)) <= 0)
            {
                fprintf(stderr, "invalid number for [-n]\n");
                exit(EXIT_FAILURE);
            }
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
        printf("Enter Directory to count files: ");
        dir = malloc(MAXLINE);
        if(fgets(dir, MAXLINE, stdin) == NULL)
            exit(EXIT_FAILURE);
        dir[MAXLINE - 1] = '\0';
        dir[strlen(dir) - 1] = '\0';
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
            // Prevent infinite loops on ., ..
            if(!streql(pDirent->d_name, ".") && 
               !streql(pDirent->d_name, ".."))
            {
                dircount++;
                DIR* npDir;
                if((npDir = opendir(newPath)) != NULL)
                    doDirCont(ht, npDir, newPath);
                closedir(npDir);
            }
        }
        else
        {
            char *type = getExt(pDirent->d_name);
            if(type == NULL)
            {
                if((s.st_mode & S_IEXEC) != 0) // executable file
                {
                    type = malloc(strlen(executableFile) + 1);
                    strcpy(type, executableFile);
                }
                else //no extension, might change this to give file type
                {
                    type = malloc(strlen(noExtension) + 1);
                    strcpy(type, noExtension);
                    /*
                    type = malloc(MAX_EXT);//strlen(pDirent->d_name) + 1);
                    if(strlen(pDirent->d_name) < MAX_EXT)
                        strcpy(type, pDirent->d_name);
                    else
                    {
                        strncpy(type, pDirent->d_name, MAX_EXT - 4);
                        type[MAX_EXT - 3] = '\0';
                        strcat(type, "...");
                    }
                    */
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

                ht_insert(ht, type, fi);
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
    char* hsize;

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
                        prev->next->next = p;
                        break;
                    }
                }
                prev = p;
                p = p->next;
            }
            //got to the end of the list (its the smallest)
            if(p == NULL)
            {
                prev->next = elemlist;
                elemlist = elemlist->next;
                prev->next->next = NULL;
            }
        }
    }

    printf("File Contents of '%s':\n", dir);
    for(p = sorted; p != NULL;)
    {
        fi = (fileInfo *)p->elem;
        hsize = humanFormat(fi->size, human);

        if(limit > 0)
        {
            printf("%-15s %-5d %20s %3.2f %% \n", fi->name, fi->count, hsize,
                   (float)fi->count * 100.0 / (float)totalfiles);
        }
        
        htelem *next = p->next;

        free(hsize);
        free(fi->name);
        free(p->elem);
        //free(p->key);
        free(p);

        p = next;
        limit--;
    }

    hsize = humanFormat(totalsize, human);
    printf("%d files in %d directories (%s)\n", totalfiles, dircount, hsize);
    free(hsize);
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

char *humanFormat(unsigned long size, int human)
{
    char *buf;

    char *unit;
    int divisions = 0;

    if((buf = malloc(SIZE_LEN)) == NULL)
        exit(EXIT_FAILURE);

    if(human)
    {
        double dsize = (double)size;
        while(size / 1024 > 0)
        {
            dsize /= 1024;
            size /= 1024;
            divisions++;
        }

        switch(divisions)
        {
        case 0: unit = "B"; break;
        case 1: unit = "KiB"; break;
        case 2: unit = "MiB"; break;
        case 3: unit = "GiB"; break;
        case 4: unit = "TiB"; break;
        default: unit = "too_big"; break;
        }

        sprintf(buf, "%.1f%s", dsize, unit);
    }
    else
        sprintf(buf, "%luB", size);

    return buf;
}
