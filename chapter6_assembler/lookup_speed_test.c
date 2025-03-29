/*lookup performance test: linear search vs binary search vs hash table*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char *key;
    char *value;
} comp_entry;

comp_entry *linear_search(int num_keys, char *key);
comp_entry *binary_search(int num_keys, char *key);

comp_entry comp_table[] = {
    {"0", "0101010"}, {"1", "0111111"}, {"-1", "0111010"}, {"D", "0001100"},
    {"A", "0110000"}, {"M", "1110000"}, {"!D", "0001101"}, {"!A", "0110001"},
    {"!M", "1110001"}, {"-D", "0001111"}, {"-A", "0110011"}, {"-M", "1110011"},
    {"D+1", "0011111"}, {"A+1", "0110111"}, {"M+1", "1110111"}, {"D-1", "0001110"},
    {"A-1", "0110010"}, {"M-1", "1110010"}, {"D+A", "0000010"}, {"D+M", "1000010"},
    {"D-A", "0010011"}, {"D-M", "1010011"}, {"A-D", "0000111"}, {"M-D", "1000111"},
    {"D&A", "0000000"}, {"D&M", "1000000"}, {"D|A", "0010101"}, {"D|M", "1010101"}
};

struct node { 
    char *key; 
    char *value;
    struct node *next;  
};
struct node *hashtabcomp[28];

unsigned hash(char *input_key, int nbuckets);
struct node *lookup(char *input_key);
int insert(char *input_key, char *input_value);
void delete(struct node *ptr);
int fill_tables(char *comp_valid_symbols[], char *comp_valid_values[]);
void delete_tables();

char *comp_valid_symbols[28] = {"0", "1", "-1", "D", "A", "M", "!D", "!A", "!M", "-D", "-A", "-M", "D+1", "A+1", "M+1", "D-1", "A-1", "M-1", "D+A", "D+M", "D-A", "D-M", "A-D", "M-D", "D&A", "D&M", "D|A", "D|M"};
char *comp_valid_values[28] = {"0101010", "0111111", "0111010", "0001100", "0110000", "1110000", "0001101", "0110001", "1110001", "0001111", "0110001", "1110011", "0011111", "0110111", "1110111", "0001110",
"0110010", "1110010", "0000010", "1000010", "0010011", "1010011", "0000111", "1000111", "0000000", "1000000", "0010101", "1010101"}; 

comp_entry *linear_search(int num_keys, char *key) {
    for (int i=0; i<num_keys; i++) {
        if (strcmp(comp_table[i].key, key) == 0) {
            return &comp_table[i];
        }
    }
    return NULL;
}

comp_entry *binary_search(int num_keys, char *key) {
    int low=0;
    int high=num_keys;
    int mid, c;
    while (low < high) {
        mid = (low+high) / 2;
        c = strcmp(comp_table[mid].key, key);
        if (c == 0) {
            return &comp_table[mid];
        } else if (c < 0) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }
    if (low < 28 && strcmp(comp_table[low].key, key) == 0) {
        return &comp_table[low];
    }
    return NULL;
}

//simple djb2 string hash function
unsigned hash(char *input_key, int nbuckets) {
    unsigned hashval;
    for (hashval=0; *input_key!='\0'; input_key++) 
        hashval = *input_key + 31 * hashval;
    return hashval % nbuckets;
}    

//hash table lookup function 
struct node *lookup(char *input_key) {
    struct node *ptr;  
    for (ptr = hashtabcomp[hash(input_key, 28)]; ptr != NULL; ptr = ptr->next) { 
        if (strcmp(ptr->key, input_key) == 0) { 
            return ptr; 
        }
    }
    return NULL; //if key not found in table 
}

//allocate memory for new node in table   
int insert(char *input_key, char *input_value) {
    struct node *ptr;
    unsigned index; 
    if ((ptr = lookup(input_key)) == NULL) { 
        ptr = (struct node *) malloc(sizeof(*ptr)); 
        if (ptr == NULL) {  
            return -1; //insufficient memory 
        } else {
            ptr->key = input_key; 
            ptr->value = input_value;
            index = hash(input_key, 28); 
            ptr->next = hashtabcomp[index];
            hashtabcomp[index] = ptr;  
            return 0; 
        } 
    } else { 
        ptr->value = input_value; 
        return 0; 
    }
}

//free node referenced by input_key
void delete(struct node *ptr) { 
    ptr->key = NULL; 
    ptr->value = NULL;
    free(ptr);   
}

//fill hash table with Hack assembly commands and their binary translations
int fill_tables(char *comp_valid_symbols[], char *comp_valid_values[]) {
    int i, status;
    for (i=0; i<28; i++) {
        status = insert(comp_valid_symbols[i], comp_valid_values[i]);
        if (status == -1) {
            printf("ERROR: insufficient memory to create new node in comp table at bucket index %d", i);
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
    for (i=0; i<28; i++) { 
        ptr = hashtabcomp[i]; 
        while (ptr != NULL) {
            temp = ptr->next; 
            delete(ptr);  
            ptr = temp; 
        }  
    }
}

int main() {
    int num_runs = 10000;
    comp_entry *found_entry;
    struct node *found_node;
    int fill_status = fill_tables(comp_valid_symbols, comp_valid_values); 

    for (int num_keys=1; num_keys<29; num_keys++) {
        printf("NUM KEYS: %d\n", num_keys);
        
        //linear search 
        clock_t start = clock();
        for (int key_index=0; key_index<num_keys; key_index++) {
            char *key = comp_table[key_index].key;
            for (int run=0; run<num_runs; run++) {
                found_entry = linear_search(num_keys, key);
            }
        }
        clock_t end = clock();
        double elapsed = (double)(end - start) / CLOCKS_PER_SEC / num_keys;
        printf("  linear, %d runs: %.09fs\n", num_runs, elapsed);
        
        //binary search 
        start = clock();
        for (int item_index=0; item_index<num_keys; item_index++) {
            char *key = comp_table[item_index].key;
            for (int run=0; run<num_runs; run++) {
                found_entry = binary_search(num_keys, key);
            }
        }
        end = clock();
        elapsed = (double)(end - start) / CLOCKS_PER_SEC / num_keys;
        printf("  binary, %d runs: %.09fs\n", num_runs, elapsed);  

        //hash table 
        if (fill_status == -1) {
            delete_tables();
            return -1; //exit if insufficient memory to create hash table 
        } else {
            start = clock();
            for (int item_index=0; item_index<num_keys; item_index++) {
                char *key = comp_valid_symbols[item_index];
                for (int run=0; run<num_runs; run++) {
                        found_node = lookup(key);
                        if (found_node == NULL) {
                            return -1;
                        }
                }
            }
            end = clock();
            elapsed = (double)(end - start) / CLOCKS_PER_SEC / num_keys;
            printf("  hash  , %d runs: %.09fs\n", num_runs, elapsed);
        }
    }
    delete_tables();
}