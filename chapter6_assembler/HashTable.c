//hash table API using separate chaining collision handling and a linear search lookup for the linked lists (inspired by chapter 6.6 Kernighan&Ritchie 2nd edn) 
#include "HashTable.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

unsigned hash(char *key, int nbuckets);
struct node *lookup(char *key, char field);
int insert(char *key, char *value, char field);
void delete(struct node *ptr, char field);
int fill_tables(char *comp_valid_symbols[], char *jump_valid_symbols[], char *dest_valid_symbols[], char *predefined_symbols[], char *comp_valid_values[], char *dest_jump_valid_values[], char *predefined_values[]);
void delete_tables();

//simple djb2 string hash function
unsigned hash(char *input_key, int nbuckets) {
    unsigned hashval;
    for (hashval=0; *input_key!='\0'; input_key++) 
        hashval = *input_key + 31 * hashval;
    return hashval % nbuckets;
}     

//hash table lookup function 
struct node *lookup(char *input_key, char field) {
    struct node *ptr; 
    if (field == 'd') {  
        for (ptr = hashtabdest[hash(input_key, 8)]; ptr != NULL; ptr = ptr->next) { //linear search of linked list at the key's bucket index
            if (strcmp(ptr->key, input_key) == 0) { //if input_key is found, return pointer to its node
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
    } else {
        for (ptr = hashtabpredef[hash(input_key, 23)]; ptr != NULL; ptr = ptr->next) { 
            if (strcmp(ptr->key, input_key) == 0) { 
                return ptr; 
            }
        }
    }
    return NULL; //if key not found in table 
}

//allocate memory for new node in table   
int insert(char *input_key, char *input_value, char field) {
    struct node *ptr;
    unsigned index; 
    if ((ptr = lookup(input_key, field)) == NULL) { //if input_key not found in table, create its node and make it head node of linked list  
        ptr = (struct node *) malloc(sizeof(*ptr)); 
        if (ptr == NULL) {  
            return -1; //insufficient memory 
        } else {
            ptr->key = input_key; //have used original pts from array - could also use strdup, which copies input_key/value pointer into heap allocated to new node
            ptr->value = input_value;
            if (field == 'd') {
                index = hash(input_key, 8); 
                ptr->next = hashtabdest[index]; //make next of new node to point to head node 
                hashtabdest[index] = ptr; //set new node as head node 
            } else if (field == 'c') {
                index = hash(input_key, 28); 
                ptr->next = hashtabcomp[index];
                hashtabcomp[index] = ptr;  
            } else if (field == 'j') {
                index = hash(input_key, 8); 
                ptr->next = hashtabjump[index];
                hashtabjump[index] = ptr;  
            } else {
                index = hash(input_key, 23); 
                ptr->next = hashtabpredef[index];
                hashtabpredef[index] = ptr; 
            }
            return 0; 
        } 
    } else { //if key already inserted, overwrite old value 
        ptr->value = input_value; 
        return 0; 
    }
}

//free node referenced by input_key
//Actual keys and values - and the original pointer arrays, and the bucket arrays - are statically allocated so auto released at EOP
void delete(struct node *ptr, char field) { 
    ptr->key = NULL; 
    ptr->value = NULL;
    free(ptr);   
}

//fill hash table with Hack assembly commands and their binary translations
int fill_tables(char *comp_valid_symbols[], char *jump_valid_symbols[], char *dest_valid_symbols[], char *predefined_symbols[], char *comp_valid_values[], char *dest_jump_valid_values[], char *predefined_values[]) {
    int i, status;
    for (i=0; i<28; i++) {
        status = insert(comp_valid_symbols[i], comp_valid_values[i], 'c');
        if (status == -1) {
            printf("ERROR: insufficient memory to create new node in comp table at bucket index %d", i);
            return status;
        }
    }
    for (i=0; i<23; i++) {
        status = insert(predefined_symbols[i], predefined_values[i], 'p');
        if (status == -1) {
            printf("ERROR: insufficient memory to create new node in predefined_symbols table at bucket index %d", i);
            return status;
        }
    }
    for (i=0; i<8; i++) {
        status = insert(dest_valid_symbols[i], dest_jump_valid_values[i], 'd');
        if (status == -1) {
            printf("ERROR: insufficient memory to create new node in dest table at bucket index %d", i);
            return status;
        }
        status = insert(jump_valid_symbols[i], dest_jump_valid_values[i], 'j');
        if (status == -1) {
            printf("ERROR: insufficient memory to create new node in jump table at bucket index %d", i);
            return status;
        }
    }
    printf("Hash tables filled\n");
    return status; 
}

//free heap of all hash tables 
void delete_tables() {
    int i;
    struct node *ptr, *temp;
    for (i=0; i < 28; i++) { //traverse bucket array 
        ptr = hashtabcomp[i]; 
        while (ptr != NULL) {
            temp = ptr->next; //save next node 
            delete(ptr, 'c'); //free current node 
            ptr = temp; //move to next node 
        }  
    }
    for (i=0; i < 23; i++) { 
        ptr = hashtabpredef[i]; 
        while (ptr != NULL) {
            temp = ptr->next; 
            delete(ptr, 'p');  
            ptr = temp; 
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













