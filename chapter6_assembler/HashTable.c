//Linked-list hash table implementation (inspired by chapter 6.6 Kernighan&Ritchie 2nd edn) to lookup Hack assembly commands and their binary translations
#include "HashTable.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

unsigned hash(char *key, int nbuckets);
struct Entry *lookup(char *key, char field);
int record(char *key, char *value, char field);
void delete(struct Entry *ptr, char field);
int fill_tables(char *comp_valid_symbols[], char *jump_valid_symbols[], char *dest_valid_symbols[], char *comp_valid_values[], char *dest_jump_valid_values[]);
void delete_tables();

//simple hash function from Kernighan&Ritchie 2nd edn. Returns index of bucket containing the input_key 
unsigned hash(char *input_key, int nbuckets) {
    unsigned hashval;
    for (hashval=0; *input_key!='\0'; input_key++) //parse through input_key string
        hashval = *input_key + 31 * hashval;
    return hashval % nbuckets;
}     

//hash table lookup function 
//O(n) where n=length of linked list at chosen bucket. As no. of buckets known, should be O(1) lookup if hash func works correctly 
struct Entry *lookup(char *input_key, char field) {
    struct Entry *ptr; 
    if (field == 'd') {  
        for (ptr = hashtabdest[hash(input_key, 8)]; ptr != NULL; ptr = ptr->next) { //traverse the linked list at the key's bucket index
            if (strcmp(ptr->key, input_key) == 0) { //if input_key is found, return pointer to its Entry
                return ptr;  
            }
        }
    } else if (field == 'j') {
        for (ptr = hashtabjump[hash(input_key, 8)]; ptr != NULL; ptr = ptr->next) { 
            if (strcmp(ptr->key, input_key) == 0) {
                return ptr; 
            }
        }
    } else if (field == 'c') {
        for (ptr = hashtabcomp[hash(input_key, 28)]; ptr != NULL; ptr = ptr->next) { 
            if (strcmp(ptr->key, input_key) == 0) { 
                return ptr; 
            }
        }
    }
    return NULL; //if key not found in table 
}

//allocate memory for new Entry in table   
int record(char *input_key, char *input_value, char field) {
    struct Entry *ptr;
    unsigned index; 
    if ((ptr = lookup(input_key, field)) == NULL) { //if input_key not found in table, create its Entry and make it head node of linked list  
        ptr = (struct Entry *) malloc(sizeof(*ptr)); //allocate heap for a new Entry
        if (ptr == NULL) {  
            return -1; //if insufficient memory 
        } else {
            ptr->key = input_key; //have used original pts from array - could also use strdup, which copies input_key/value pointer into heap allocated to new Entry
            ptr->value = input_value;
            if (field == 'd') {
                index = hash(input_key, 8); 
                ptr->next = hashtabdest[index]; //make next of new node to point to head node 
                hashtabdest[index] = ptr; //set new node as head node 
            } else if (field == 'c') {
                index = hash(input_key, 28); 
                ptr->next = hashtabcomp[index];
                hashtabcomp[index] = ptr;  
            } else {
                index = hash(input_key, 8); 
                ptr->next = hashtabjump[index];
                hashtabjump[index] = ptr;  
            }
            return 0; 
        } 
    } else { //if key already recorded, overwrite old value 
        ptr->value = strdup(input_value); 
        return 0; 
    }
}

//free Entry referenced by input_key
//Actual keys and values - and the original pointer arrays, and the bucket arrays - are statically allocated so auto released at EOP
void delete(struct Entry *ptr, char field) { 
    ptr->key = NULL; 
    ptr->value = NULL;
    free(ptr); //free the Entry structure itself  
}

//fill hash table with Hack assembly commands and their binary translations
int fill_tables(char *comp_valid_symbols[], char *jump_valid_symbols[], char *dest_valid_symbols[], char *comp_valid_values[], char *dest_jump_valid_values[]) {
    int i, status;
    for (i=0; i<28; i++) {
        status = record(comp_valid_symbols[i], comp_valid_values[i], 'c');
        if (status == -1) {
            printf("ERROR: insufficient memory to create new Entry in comp table at hash index %d", i);
            return status;
        }
    }
    for (i=0; i<8; i++) {
        status = record(dest_valid_symbols[i], dest_jump_valid_values[i], 'd');
        if (status == -1) {
            printf("ERROR: insufficient memory to create new Entry in dest table at hash index %d", i);
            return status;
        }
        status = record(jump_valid_symbols[i], dest_jump_valid_values[i], 'j');
        if (status == -1) {
            printf("ERROR: insufficient memory to create new Entry in jump table at hash index %d", i);
            return status;
        }
    }
    printf("Hash tables filled\n");
    return status; 
}

//free heap of all hashtables 
void delete_tables() {
    int i;
    struct Entry *ptr, *temp;
    for (i=0; i < 28; i++) { //traverse bucket array 
        ptr = hashtabcomp[i]; 
        while (ptr != NULL) {
            temp = ptr->next; //save next node 
            delete(ptr, 'c'); //free current node 
            ptr = temp; //move to next node 
        }  
    }
    for (i=0; i < 8; i++) {
        ptr = hashtabdest[i]; 
        while (ptr != NULL) {
            temp = ptr->next; 
            delete(ptr, 'd'); 
            ptr = temp; 
        }
        ptr = hashtabjump[i]; 
        while (ptr != NULL) {
            temp = ptr->next; 
            delete(ptr, 'j'); 
            ptr = temp; 
        }
    }
    printf("Hash tables deleted\n");
}













