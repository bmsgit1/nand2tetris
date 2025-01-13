//header file for the hash tables (inspired by chapter 6.6 Kernighan&Ritchie 2nd edn)

//node of hash table (node in linked list)
struct node { 
    char *key; 
    char *value;
    struct node *next; //ptr to next node in linked list  
};
struct node *hashtabdest[8], *hashtabcomp[28], *hashtabjump[8], *hashtabpredef[23]; //bucket arrays 
//memory is allocated here for the bucket arrays only, not entries of linked lists
//=> initialised automatically as an array of null ptrs, as statically stored

//Hardcoded comp binary mnemonics for C-instructions. 
//Use initialiser lists {} to allocate static memory for each string literal storing a key/value, and the array of ptrs itself  
char *comp_valid_symbols[28] = {"0", "1", "-1", "D", "A", "M", "!D", "!A", "!M", "-D", "-A", "-M", "D+1", "A+1", "M+1", "D-1", "A-1", "M-1", "D+A", "D+M", "D-A", "D-M", "A-D", "M-D", "D&A", "D&M", "D|A", "D|M"};
char *comp_valid_values[28] = {"0101010", "0111111", "0111010", "0001100", "0110000", "1110000", "0001101", "0110001", "1110001", "0001111", "0110001", "1110011", "0011111", "0110111", "1110111", "0001110",
"0110010", "1110010", "0000010", "1000010", "0010011", "1010011", "0000111", "1000111", "0000000", "1000000", "0010101", "1010101"};

//Hardcoded dest binary mnemonics for C-instructions
char *dest_valid_symbols[8] = {"M", "D", "MD", "A", "AM", "AD", "AMD", "nul"};
char *dest_jump_valid_values[8] = {"001", "010", "011", "100", "101", "110", "111", "000"};

//Hardcoded jump binary mnemonics for C-instructions
char *jump_valid_symbols[8] = {"JGT", "JEQ", "JGE", "JLT", "JNE", "JLE", "JMP", "nul"};

//Hardcoded pre-defined register symbols 
char *predefined_symbols[23] = {"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15", "SP", "LCL", "ARG", "THIS", "THAT", "SCREEN", "KBD"};
char *predefined_values[23] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "0", "1", "2", "3", "4", "16384", "24576"};

