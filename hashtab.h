/* hashtab.h - Generic hashtable implementation in C
 *
 * Bailey Forrest - baileycforrest@gmail.com
 *
 */

#ifndef HASHTAB_H
#define HASHTAB_H

#define HT_ARRAY_START 128
#define MAX_LOAD_FACTOR 6.0

typedef struct htelem
{
    void* key;
    void* elem;
    htelem* next;
} htelem;

typedef struct hashtab
{
    htelem** array;
    int asize;
    int members;
    void (*freeElem)(void *elem);
    void (*freeKey)(void *key);
    unsigned (*hash) (void *key);
    int (*keyEqual)(void *k1, void*k2);
} hashtab;

hashtab* ht_create(void (*freeElem)(), void (*freeKey)(),
                   unsigned hash (*hash)(void *key));
int ht_insert(hashtab *ht, void* key, void* elem);
int ht_ins_htelem(hashtab *ht, htelem* htelem);
int ht_resize(hashtab *ht);
int ht_remove(hashtab *ht, void* key);
void *ht_get(hashtab *ht, void* key);
int ht_free(hashtab *ht);

#endif
