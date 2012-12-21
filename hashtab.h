/* hashtab.h - generic hashtable implementation in C
 *
 * Bailey Forrest - baileycforrest@gmail.com
 *
 */

#ifndef HASHTAB_H
#define HASHTAB_H

#define HT_ARRAY_START 100

typedef struct htelem
{
    void* key;
    void* elem;
    htelem* next;
} htelem;

typedef struct hashtab
{
    htelem* array;
    int asize;
    int members;
    void (*freeMem)();
    unsigned (*hash) (void *key);
    int (*keyEqual)(void *k1, void*k2);
} hashtab;

hashtab* ht_create(void (*freeMem)(), unsigned hash (*hash)(void *input));
int ht_insert(hashtab *ht, void* key, void* elem);
int ht_remove(hashtab *ht, void* key);
void *ht_get(hashtab *ht, void* key);
int ht_free(hashtab *ht);

#endif
