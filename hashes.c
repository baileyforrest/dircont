/* hashes.h - Hash functions for generic hashtable implementation in C
 *
 * Bailey Forrest - baileycforrest@gmail.com
 *
 */

// Hash function for short strings
// djb2 hash function
unsigned hash_sstring(unsigned char* str)
{
    register unsigned hash = 5381;
    int c;
    while((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}
