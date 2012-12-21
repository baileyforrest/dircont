/* hashtab.c - generic hashtable implementation in C
 *
 * Bailey Forrest - baileycforrest@gmail.com
 *
 */

#include "hashtab.h"

hashtab* ht_create(void (*freeMem)(), unsigned hash (*hash)(void *input),
                   int (*keyEqual)(void *k1, void *k2))
{
    if((hashtab* ht = malloc(sizeof(hashtab))) < 0)
        return NULL;
    if((ht->array) = malloc(sizeof(char *) * HT_ARRAY_START) < 0)
        return NULL;

    ht->asize = HT_ARRAY_START;
    ht->members = 0;
    ht->freeMem = freeMem;
    ht->hash = hash;
    ht->keyEqual = keyEqual;
}

int ht_insert(hashtab *ht, void* key, void* elem)
{
    unsigned hash = ht->hash(key);
    hash = hash % ht->asize;
    
    htelem *p;
    for(p = ht->array[hash]; p != NULL; p = p->next)
    {
        if(ht->
    }
}
