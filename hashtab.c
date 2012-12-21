/* hashtab.c - generic hashtable implementation in C
 *
 * Bailey Forrest - baileycforrest@gmail.com
 *
 */

#include "hashtab.h"

hashtab* ht_create(void (*freeElem)(), unsigned hash (*hash)(void *input),
                   int (*keyEqual)(void *k1, void *k2))
{
    if((hashtab* ht = malloc(sizeof(hashtab))) < 0)
        return NULL;
    if((ht->array) = malloc(sizeof(char *) * HT_ARRAY_START) < 0)
        return NULL;

    ht->asize = HT_ARRAY_START;
    ht->members = 0;
    ht->freeElem = freeElem;
    ht->freeKey = freeKey;
    ht->hash = hash;
    ht->keyEqual = keyEqual;
}

int ht_insert(hashtab *ht, void* key, void* elem)
{
    htelem* htelem = malloc(sizeof(htelem));
    htelem->key = key;
    htelem->elem = elem;

    if(ht_ins_htelem(ht, htelem) < 0)
        return -1;

    return 0;
}

int ht_ins_htelem(hashtab *ht, htelem* htelem)
{
    unsigned hash = ht->hash(htelem->key);
    hash = hash % ht->asize;

    htelem *p;
    htelem *start = ht->array[hash];
    for(p = start; p != NULL; p = p->next)
    {
        if(ht->keyEqual(p->key, htelem->key))
        {
            p->elem = htelem->elem;
            free(htelem);
            return 0;
        }
    }

    // Not found in hash table, add to front of linked list
    p = start;
    ht->array[hash] = htelem;
    start->next = p;
    ht->members++;

    if((float)ht->members / (float)ht->asize > MAX_LOAD_FACTOR)
        if(ht_resize(ht) < 0)
            return -1;
    
    return 0;
}

// Load factor too high, double number of buckets
int ht_resize(hashtab *ht)
{
    htelem* aold = ht->array;
    int oldasize = ht->asize;
    ht->asize = ht->asize * 2;
    ht->members = 0;
    if((ht->array = malloc(sizeof(htelem) * ht->asize)) < 0)
        return -1;

    int i;
    htelem *p;

    for(i = 0; i < oldasize; i++)
    {
        for(p = aold[i]; p != NULL; p = p->next)
            if(ht_ins_htelem(ht, p) < 0)
                return -1;
    }

    free(aold);
    return 0;
}

int ht_remove(hashtab *ht, void* key)
{
    hash = ht->hash(key);
    hash = hash % ht->asize;

    htelem* p;
    htelem* prev = ht->array[hash];
    for(p = prev; p != NULL; p = p->next)
    {
        if(ht->keyEqual(key, p->key))
        {
            if(p == prev)
                ht->array[hash] = p->next;
            else
                prev->next = p->next;

            ht->freeElem(p->elem);
            ht->freeKey(p->key);
            free(p);

            return 0;
        }
    }

    return -1;
}

// Returns pointer to element given key
void *ht_get(hashtab *ht, void* key)
{
    unsigend hash = ht->hash(key);
    hash = hash % ht->asize;
    
    htelem *p;
    for(p = hash->array[hash]; p != NULL; p = p->next)
    {
        if(ht->keyEqual(key, p->key))
            return p->elem;
    }

    return NULL;
}

// Frees hashtable, and all elements
int ht_free(hashtab *ht)
{
    int i;
    htelem *p, *next;
    for(i = 0; i < ht->asize; i++)
    {
        for(p = ht->array[i]; p != NULL; p = next)
        {
            next = p->next;
            ht->freeElem(p->elem);
            ht->freeKey(p->key);
            free(p);
        }
    }

    free(ht->array);
    free(ht);

    return 0;
}
