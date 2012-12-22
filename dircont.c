/* dircont.c - Directory Contents
 *
 * Bailey Forrest - baileycforrest@gmail.com
 *
 */

#include "dircont.h"
#include "hashes.h"

const char* usage = "Usage: %s [-hH] <directory>\n";

int main(int argc, char** argv)
{
    int human = 1;
    int opt;
    while((opt = getopt(argc, agrv, "hH:") != -1))
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

    struct dirent *pDirent;
    DIR *pDir;

    if((pDir = opendir(dir)) == NULL)
    {
        fprintf(stderr, "Cannot open directory '%s'\n", dir);
        exit(EXIT_FAILURE);
    }

    while((pDirent = readdir(pDir)) != NULL)
    {
        printf("[%s]\n", pDirent->d_name);
    }
    closedir(pDir);
    
    return 0;
}
