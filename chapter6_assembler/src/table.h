/*definition of table used to store the three types of symbols in the Hack assembly language: pre-defined symbols, labels and variables.*/

//entry of table 
typedef struct { 
    char *key; 
    char *value; 
} entry;

//hash table 
typedef struct {
    int count; //number of k/v pairs (entries) currently stored 
    int nbuckets; //size of bucket array 
    entry *entries; //ptr to bucket array. Each bucket can hold a single entry 
} ht;

//pre-defined Hack assembly variables stored in tables with lexicographically ordered keys  
//the tables are implemented as static arrays of elements with type entry 
//i.e. the entries and the string literals themselves are statically allocated in read-only data  
entry comp_table[] = {
    {"!A", "0110001"}, {"!D", "0001101"}, {"!M", "1110001"}, {"-1", "0111010"}, 
    {"-A", "0110011"}, {"-D", "0001111"}, {"-M", "1110011"}, {"0", "0101010"}, 
    {"1", "0111111"}, {"A", "0110000"}, {"A+1", "0110111"}, {"A-1", "0110010"}, 
    {"A-D", "0000111"}, {"D", "0001100"}, {"D&A", "0000000"}, {"D&M", "1000000"}, {"D+1", "0011111"}, 
    {"D+A", "0000010"}, {"D+M", "1000010"}, {"D-1", "0001110"}, {"D-A", "0010011"}, 
    {"D-M", "1010011"}, {"D|A", "0010101"}, {"D|M", "1010101"}, 
    {"M", "1110000"}, {"M+1", "1110111"}, {"M-1", "1110010"}, {"M-D", "1000111"}
};

entry dest_table[] = {
    {"A", "100"}, {"AD", "110"}, {"ADM", "111"}, {"AM", "101"}, {"AMD", "111"}, 
    {"D", "010"}, {"DM", "011"}, {"M", "001"}, {"MD", "011"}, {"nul", "000"}
};

entry jump_table[] = {
    {"JEQ", "010"}, {"JGE", "011"}, {"JGT", "001"}, {"JLE", "110"}, 
    {"JLT", "100"}, {"JMP", "111"}, {"JNE", "101"}, {"nul", "000"}
};

entry predef_symbol_table[] = {
    {"ARG", "2"}, {"KBD", "24576"}, {"LCL", "1"}, {"R0", "0"},
    {"R1", "1"}, {"R10", "10"}, {"R11", "11"}, {"R12", "12"},
    {"R13", "13"}, {"R14", "14"}, {"R15", "15"}, {"R2", "2"},
    {"R3", "3"}, {"R4", "4"}, {"R5", "5"}, {"R6", "6"},
    {"R7", "7"}, {"R8", "8"}, {"R9", "9"}, {"SCREEN", "16384"},
    {"SP", "0"}, {"THAT", "4"}, {"THIS", "3"}
};


