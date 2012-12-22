/* hashtab.c - Generic hashtable implementation in C
 *
 * Bailey Forrest - baileycforrest@gmail.com
 *
 */

#include "hashtab.h"

// Creates a new hashtable
hashtab* ht_create(void (*freeElem)(void *elem), void (*freeKey)(void *key),
                   unsigned (*hash)(void *key),
                   int (*keyEqual)(void *k1, void *k2))
{
    hashtab *ht;
    if((ht = malloc(sizeof(hashtab))) == NULL)
        return NULL;
    if((ht->array = calloc(HT_ARRAY_START, sizeof(htelem *))) == NULL)
        return NULL;

    ht->asize = HT_ARRAY_START;
    ht->members = 0;
    ht->freeElem = freeElem;
    ht->freeKey = freeKey;
    ht->hash = hash;
    ht->keyEqual = keyEqual;

    return ht;
}

// Insert elem, key pair into hash table
int ht_insert(hashtab *ht, void* key, void* elem)
{
    htelem* htelem;
    if((htelem = malloc(sizeof(struct htelem))) == NULL)
       return -1;
    htelem->key = key;
    htelem->elem = elem;

    if(ht_ins_htelem(ht, htelem) < 0)
        return -1;

    return 0;
}

// Insert htelem struct into hash table
int ht_ins_htelem(hashtab *ht, htelem *htelem)
{
    unsigned hash = ht->hash(htelem->key);
    hash = hash % ht->asize;

    struct htelem *p;
    struct htelem *start = ht->array[hash];
    for(p = start; p != NULL; p = p->next)
    {
        if(ht->keyEqual(p->key, htelem->key))
        {
            ht->freeElem(p->elem);
            ht->freeKey(htelem->key);
            p->elem = htelem->elem;
            free(htelem);
            return 0;
        }
    }

    // Not found in hash table, add to front of linked list
    p = start;
    ht->array[hash] = htelem;
    htelem->next = p;
    ht->members++;

    if((float)ht->members / (float)ht->asize > MAX_LOAD_FACTOR)
        if(ht_resize(ht) < 0)
            return -1;
    
    return 0;
}

// Load factor too high, double number of buckets
int ht_resize(hashtab *ht)
{
    htelem **aold = ht->array;
    int oldasize = ht->asize;
    ht->asize = ht->asize * 2;
    ht->members = 0;
    if((ht->array = calloc(ht->asize, sizeof(htelem *))) == NULL)
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

// Remove htelem corresponding to key in the hashtable
int ht_remove(hashtab *ht, void* key)
{
    unsigned hash = ht->hash(key);
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
        prev = p;
    }

    return -1;
}

// Returns pointer to element given key
void *ht_get(hashtab *ht, void* key)
{
    unsigned hash = ht->hash(key);
    hash = hash % ht->asize;
    
    htelem *p;
    for(p = ht->array[hash]; p != NULL; p = p->next)
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

//creates linked list of htelem from the hashtable
//frees the ht structure
htelem *toList(hashtab *ht)
{
    htelem *head = NULL;
    htelem *tail = NULL;
    int i;
    htelem *p;
    for(i = 0; i < ht->asize; i++)
    {
        for(p = ht->array[i]; p != NULL; p = p->next)
        {
            if(head == NULL)
            {
                head = p;
                tail = p;
            }
            else
            {
                tail->next = p;
                tail = p;
            }
        }
    }
    if(tail != NULL)
        tail->next = NULL;

    free(ht->array);
    free(ht);

    return head;
}
