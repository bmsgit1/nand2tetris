#include "table.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INITIAL_CAPACITY 10
#define MAX_TABLE_LOAD 0.75
#define EXPANSION_FACTOR 1.5

entry *binary_lookup(entry* table, int num_entries, char *key);
ht *ht_create();
void ht_delete(ht *table);
size_t hash(char *key);
entry *entry_find(entry *entries, int nbuckets, char *key);
int ht_expand(ht *table);
int ht_insert(ht *table, char *key, char *value);
entry *ht_lookup(ht *table, char *key);

/* BINARY SEARCH (of statically allocated tables in table.h)*/
//only works with lexicographically sorted tables 

entry *binary_lookup(entry *table, int num_entries, char *key) {
    int low = 0;
    int high = num_entries - 1;
    while (low <= high) {  
        int mid = (high + low) / 2; 
        int cmp = strcmp(table[mid].key, key);
        if (cmp == 0) {
            return &table[mid];
        } else if (cmp < 0) {
            low = mid + 1;
        } else {
            high = mid - 1;  
        }
    }
    return NULL;
}

/* HASH TABLE API */ 
//inspired by implementations from Ben Hoyt (https://benhoyt.com/writings/hash-table-in-c/) and Robert Nystrom (https://craftinginterpreters.com/hash-tables.html)
//uses 'open addressing' collision handling, where hash table entries are stored in the contiguous memory units of the bucket array 
//this is better for modern CPU cache usage than 'separate chaining' collision handling, which uses linked lists with nodes scattered in memory 

ht *ht_create() {
    //allocate memory for new hash table struct
    ht *table = (ht *) malloc(sizeof(ht));
    if (table == NULL) {
        printf("ERROR: memory allocation for hash table failed\n");
        return NULL;  
    }
    table->count = 0;
    table->nbuckets = INITIAL_CAPACITY;
    //allocate (zero'd) memory for new bucket array
    table->entries = (entry *) calloc(table->nbuckets, sizeof(entry)); //calloc ensures all keys, values initialised to NULL (0x00)
    if (table->entries == NULL) {
        printf("ERROR: memory allocation for bucket array failed\n");
        free(table);
        return NULL; 
    }
    printf("Hash table created\n\n");
    return table;
}

void ht_delete(ht *table) {
    //free k/v pairs stored in bucket array via strdup
    for (int i=0; i < table->nbuckets; i++) {
        if (table->entries[i].key != NULL) {
            free(table->entries[i].key);
            free(table->entries[i].value);
        }
    }
    //free bucket array and hash table 
    free(table->entries);
    free(table);
    printf("\nHash table deleted\n");
}

//djb2 string hash function - simplest, not the fastest 
size_t hash(char *key) {
    int key_length = 0;
    size_t hashval = 0; //size_t is unsigned int (type returned by sizeof, malloc)
    for (; *key!='\0'; key++) {
        hashval = *key + 31 * hashval;
        ++key_length;
    }
    key -= key_length; //reset key 
    //printf("Key hashed to %zu\n", hashval);
    return hashval; 
} 

//the linear probing logic used in key lookup and k/v pair insertion
//NOTE: if entire bucket array filled, and key not found, would get infinfite loop. THIS CANNOT HAPPEN as I prevent the load factor increasing above MAX_TABLE_LOAD 
entry* entry_find(entry *entries, int nbuckets, char *key) {
    //printf("Searching for entry %s\n", key);
    int index = hash(key) % nbuckets;
    //loop until key found (for lookup) or empty bucket found (for insert) 
    for(;;) { 
        //printf("Checking bucket %d\n", index);
        if (entries[index].key == NULL || strcmp(key, entries[index].key) == 0) {  //order important: strcmp cannot dereference NULL ptr (if entries[index].key == NULL)
            if (entries[index].key == NULL) {
                //printf("Empty bucket (NULL key) reached\n");
            } else {
                //printf("Entry found - key is %s and value is %d\n", entries[index].key, entries[index].value);
            }
            return &entries[index]; 
        } else {
            index++; 
            //if reach end of bucket array, wrap around to start  
            if (index >= nbuckets) {
                index = 0; 
            }
        }
    }
}

//increases capacity of table  
int ht_expand(ht *table) {
    //allocate new bucket array, EXPANSION_FACTORx larger than old one
    int new_nbuckets = table->nbuckets*EXPANSION_FACTOR; 
    entry *new_entries = (entry *) calloc(new_nbuckets, sizeof(entry));
    if (new_entries == NULL) {
        printf("ERROR: memory allocation for expanded bucket array failed\n");
        return 1; 
    }
    //reinsert all old entries (rehash keys)
    entry *dest; 
    for (int i=0; i < table->nbuckets; i++) { 
        if (table->entries[i].key != NULL) {
            entry *dest = entry_find(new_entries, new_nbuckets, table->entries[i].key);  
            if (dest->key == NULL) { //if destination bucket empty, re-insert old entry 
                dest->key = strdup(table->entries[i].key);  
                dest->value = strdup(table->entries[i].value);
                if (dest->key == NULL || dest->value == NULL) {
                    printf("ERROR: memory allocation when re-inserting k/v pair into expanded table failed\n");
                    return 1;
                }
                //printf("Old entry successfully re-inserted\n");
            } else {
                printf("ERROR: re-insertion of entries into expanded table failed\n");
                return 1;
            }
        } 
    } 
    //free k/v pairs stored in old bucket array via strdup
    for (int i=0; i < table->nbuckets; i++) {
        if (table->entries[i].key != NULL) {
            free(table->entries[i].key);
            free(table->entries[i].value);
        }
    }
    //free old bucket array, then update table struct 
    free(table->entries);
    table->entries = new_entries;
    table->nbuckets = new_nbuckets;
    //printf("Table expanded to %d buckets\n\n", table->nbuckets);
    return 0;
}

//inserts new entry 
int ht_insert(ht *table, char *key, char *value) {
    //if load factor (count/nbuckets) > MAX_TABLE_LOAD, expand bucket array
    if (table->count+1 > table->nbuckets*MAX_TABLE_LOAD) {
        //printf("Max load factor exceeded at %d buckets\n\n", table->nbuckets);
        int expansion_status = ht_expand(table);
        if (expansion_status == 1) {
            return 1; 
        }
    }
    entry *dest = entry_find(table->entries, table->nbuckets, key);
    //if destination bucket empty, insert new entry. If entry already in table, overwrite it with new value
    if (dest->key == NULL) { 
        dest->key = strdup(key); 
        dest->value = strdup(value);
        if (dest->key == NULL || dest->value == NULL) {
            printf("ERROR: memory allocation when inserting new k/v pair into table failed\n");
            return 1;
        }
        //printf("Entry %s added to the symbol table with value %s\n", dest->key, dest->value);
        ++table->count;
        return 0;  
    } else if (strcmp(dest->key, key) == 0) { 
        //printf("Entry %s with old value %s overwritten with new value ", dest->key, dest->value);
        free(dest->value);
        dest->value = strdup(value); 
        if (dest->value == NULL) {
            printf("ERROR: memory allocation when overwriting value in table failed\n");
            return 1;
        }
        //printf("%s in symbol table\n", dest->value);
        return 0; 
    } 
    printf("ERROR: insertion of entry into table failed\n");
    return 1; 
}

//key lookup, returns associated entry if found   
entry *ht_lookup(ht *table, char *key) {
    entry *dest = entry_find(table->entries, table->nbuckets, key);
    if (dest->key == NULL) { 
        //printf("Entry %s is not in the symbol table\n", key);
        return NULL; 
    } else if (strcmp(key, dest->key) == 0) {
        return dest;
    } else {
        printf("ERROR: wrong entry recovered from symbol table during lookup\n");
        return NULL;
    }
}

//no ht_remove - no real need for it here & it is difficult with open addressing collision handling as leaves memory 'holes' in contiguous bucket array.














