/*nand2tetris project 6: my implementation of an assembler that translates a Hack assembly files (.asm) into a Hack machine language file (.hack)*/
//see hack_language_specification.png for syntax details
/* potential improvements:
- there is no error-handling for incorrect Hack assembly syntax 
- dynamically allocate space for assembly commands to reduce memory wasteage 
- make the symbol checking routine cleaner */

#include "table.c"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define NAMELENGTH 1000
#define MAXCOMMANDLENGTH 50 //max length of a Hack Assembly command

char infilename[NAMELENGTH] = "/Users/benstewart/Desktop/code/nand2tetris/chapter6_assembler/test/pong/Pong.asm";
char outfilename[NAMELENGTH] = "/Users/benstewart/Desktop/code/nand2tetris/chapter6_assembler/test/pong/Pong.hack";  
char assemblycommand[MAXCOMMANDLENGTH]; //holds single assembly command
char destfield[4]; //hold fields of assembly C command (max 3 chars + \0)
char compfield[4];
char jumpfield[4];
char labelsymbol[MAXCOMMANDLENGTH]; //holds user-defined instruction label 
char acommand[MAXCOMMANDLENGTH]; //holds single A instruction 
char binaryinstruction[17]; //holds binary instruction (16-bit machine + \0)

FILE *input_file_access(char *inputfilename);
FILE *output_file_access(char *outfilename);
char *fetchcommand(FILE *ifp, char *assemblycommand);
char command_type(char *assemblycommand);
int command_length(char *assemblycommand);
char *parse_dest(char *assemblycommand, char *destfield);
char *parse_comp(char *assemblycommand, char *compfield, char *destfield);
char *parse_jump(char *assemblycommand, char *jumpfield, char *destfield, int command_length);
char *parse_a(char *assemblycommand, char *acommand, int asscom_length);
int string2int(char *acommand);
int decint2binstring(int decimal_a, char *binaryinstruction);
char symbol_type(char *assemblycommand);
char *translate_symbol(char *acommand, char variable_type, ht *table);
int translate_a(char *acommand, char *binaryinstruction, ht *table);
int translate_c(char *compfield, char *destfield, char *jumpfield, char *binaryinstruction); 
void write_command(char *binaryinstruction, FILE *ofp);
char *int2string(int value);

/*PARSER MODULE*/

//access the input .asm file 
FILE *input_file_access(char *infilename) {
    FILE *ifp;
    ifp = fopen(infilename, "r");
    if (ifp == NULL) {
        printf("ERROR: Unable to open .asm file %s\n", infilename);  
    } else {
        printf("Accessed .asm file\n");
    }
    return ifp;
}

//create the output .hack file 
FILE *output_file_access(char *outfilename) {
    FILE *ofp;
    ofp = fopen(outfilename, "w");
    if (ofp == NULL) {
        printf("ERROR: Unable to create .hack file %s\n", outfilename);  
    } else {
        printf("Created empty .hack file\n");
    }
    return ofp;
}

//skip comments and blank lines at start of .asm file 
int skipheader(FILE *ifp) {
    char c = getc(ifp);
    while (c != EOF) {
        if (c == '/') {
            while (c != '\n') {
                c = getc(ifp);
            }
        } else if (isspace(c) != 0) {
           c = getc(ifp);
        } else { 
            fseek(ifp, -1, SEEK_CUR); //ifp points to char before first char of valid assembly command  
            printf("Header in .asm file skipped\n");
            return 0; 
        } 
    }
    printf("ERROR: File contains no valid commands\n");
    return 1; 
}

//fetch next assembly command, read into assemblycommand array. Ignores comments and whitespace 
char *fetchcommand(FILE *ifp, char *asscom) {
    int n=0;
    char c = getc(ifp); 
    while (c != EOF) {  
        //skip any space at start of line 
        while (isspace(c) != 0) {
            c = getc(ifp);
        }
        //skip comment lines 
        if (c == '/') {
            while (c != '\n') {
                c = getc(ifp); 
            }
            c = getc(ifp);
            continue; //to next line 
        }
        //copy command
        while (isspace(c) == 0 && c != '/' && c != '\n') {
            *asscom++ = c;
            ++n;
            c = getc(ifp);
        }
        if (c == '\n') {
            *asscom = c;
            asscom -= n; //point to start of fetched assembly command
            //printf("\nNEXT COMMAND FETCHED\n");
            return asscom; 
        } else if (isspace(c) != 0 || c == '/') {
            while (c != '\n') {
                c = getc(ifp); 
            }
            *asscom = c; 
            asscom -= n;
            //printf("\nNEXT COMMAND FETCHED\n");
            return asscom;   
        } 
    }
    printf("\nEOF REACHED\n");
    return NULL;
}

//returns type of fetched assembly command: A command, C command or instruction label 
char command_type(char *asscom) {
    if (*asscom == '@') {
        //printf("Command type = A\n");
        return 'a';
    } else if (*asscom == '(') {
        //printf("Command type = Label\n");
        return 'l'; 
    } else {
        //printf("Command type = C\n");
        return 'c';
    }
}

//returns length of fetched assembly command 
int command_length(char *asscom) {
    int n=0;
    while (*asscom != '\n') {
        if (n > MAXCOMMANDLENGTH) {
            printf("ERROR: fetched command %s too long, max command length is 10 characters", asscom);
            return -1;
        }
        ++asscom;
        ++n;
    }
    //printf("Command length = %d\n", n);
    asscom -= n;  
    return n;
}

//parses label into array
char *parse_label(char *asscom, char *label, int asscom_length) {
    ++asscom; //skip '(' at front of label
    while (*asscom != ')') { 
        *label++ = *asscom++;
    }
    *label = '\0'; 
    label -= (asscom_length - 2); 
    asscom -= asscom_length; //point to start of assembly command, to pass back to fetchcommand()
    //printf("Instruction label parsed as %s\n", label);
    return label;
}

//parses assembly A-command into array 
char *parse_a(char *asscom, char *acom, int asscom_length) {
    ++asscom; //skip '@' at front of A-command 
    while (*asscom != '\n') { 
        *acom++ = *asscom++;
    }
    *acom = '\0'; 
    acom -= (asscom_length - 1); 
    asscom -= asscom_length; 
    //printf("A-command parsed as %s\n", acom);
    return acom;
}

/*next three funcs split assembly C-command into subarray for each field (dest, comp, jump)*/
char *parse_dest(char *asscom, char *destfield) {
    int n=0; 
    while (*asscom != '=') { //while in dest field 
        if (n == 4) { //if no dest field
            destfield -= n; 
            strncpy(destfield, "nul", 4);
            asscom -= n; 
            //printf("Dest field parsed as: %s\n", destfield);
            return destfield;   
        } else {
            *destfield++ = *asscom++;
            ++n;
        } 
    }
    *destfield = '\0';
    destfield -= n;
    asscom -= n; //point to start of asscom
    //printf("Dest field parsed as: %s\n", destfield);
    return destfield; 
}

char *parse_comp(char *asscom, char *compfield, char *destfield) {
    int n=0;
    if (*destfield != 'n') { //if dest field present
        while (*asscom != '=') { //skip to end of dest field in assembly command 
            ++asscom;
        } 
        ++asscom; //point to start of comp field 
    } 
    while (*asscom != ';') { //while in comp field 
        if (*asscom == '\n') { //if no jump field 
            *compfield = '\0';
            compfield -= n;
            asscom -= n; //point to start of comp field in asscom 
            //printf("Comp field parsed as: %s\n", compfield); 
            return compfield;
        } else {  
            *compfield++ = *asscom++;
            ++n;
        }
    }
    *compfield = '\0'; 
    compfield -= n;  
    asscom -= n; 
    //printf("Comp field parsed as: %s\n", compfield); 
    return compfield;
}

char *parse_jump(char *asscom, char *jumpfield, char *destfield, int asscom_length) {
    int n=0;
    while (*asscom != ';' && *asscom != '\n') { //skip through comp field 
        ++asscom;
    }
    if (*asscom == '\n') { //if no jump field 
        strncpy(jumpfield, "nul", 4);
        asscom -= asscom_length; //points at start of assembly command, to pass to fetchcommand()
        //printf("Jump field parsed as: %s\n", jumpfield); 
        return jumpfield;
    } else { //if jump field present 
        ++asscom; //skip past ';'
        while (*asscom != '\n') { 
            *jumpfield++ = *asscom++;
            ++n;
        }
    }
    *jumpfield = '\0'; 
    jumpfield -= n; 
    asscom -= asscom_length; 
    //printf("Jump field parsed as: %s\n", jumpfield);  
    return jumpfield;
}


/*CODE MODULE*/

/*next five functions translate assembly A-command into its binary instruction*/
//returns type of symbol in A-command: pre-defined symbol ('p') or user-defined variable/label ('u') 
char symbol_type(char *acom) {
    entry *dest = binary_lookup(predef_symbol_table, 23, acom);
    if (dest == NULL) { //if command not in predef_table
        return 'u';
    } 
    return 'p';
}

//translates A-command with a user-defined variable into decimal value string
char *translate_symbol(char *acom, char variable_type, ht *table) { 
    entry *dest;
    if (variable_type == 'p') { //if a pre-defined symbol
        dest = binary_lookup(predef_symbol_table, 23, acom);
    } else { //if user-defined symbol
        dest = ht_lookup(table, acom);
    }
    if (dest == NULL) { //if symbol not in table
        printf("ERROR: symbol %s is not in the symbol table", acom);
        return NULL;
    } 
    //printf("Symbol translated from %s to ", acom);
    strcpy(acom, dest->value);
    //printf("%s\n", acom);
    return acom;
}

//converts string into its integer form e.g. "321\0" -> 321
int string2int(char *acom) {
    int n=0;
    while (isdigit(*acom) != 0) {
        n = 10 * n + (*acom - '0'); 
        ++acom;
    }
    if (*acom == '\0') {
        return n;
    } else {
        printf("ERROR: non-decimal character in fetched A-command: %s\n", acom);
        return -1; 
    }
}

//converts int assembly A-command into its binary instruction 
int decint2binstring(int decimal_a, char *binaryinst) {
    //convert decimal A-command to binary int array
    int binary_a[15]; 
    int i, j, temp;
    if (decimal_a == 0) {
        for (i=0;i<15;i++) {
            binary_a[i] = 0;
        }
    } else {
        for (i=0;i<15;i++) { //fill array with binary digits
            binary_a[i] = decimal_a % 2;
            decimal_a /= 2;
        }
        for (i=0, j=14;i<7 && j>7;i++, j--) { //reverse array 
            temp = binary_a[i];
            binary_a[i] = binary_a[j];
            binary_a[j] = temp;
        }
    }
    //convert array of binary int to its string form  
    *binaryinst++ = '0'; //add marker bit at front of binary instruction 
    for (i=0;i<15;i++) {
        if (binary_a[i] == 0 || binary_a[i] == 1) { 
            *binaryinst++ = binary_a[i] + '0'; //convert int to char
        } else {
            printf("ERROR: invalid non-binary integer in A-command translation: %d at position %d\n", binary_a[i], i);
            return 1;
        }
    }
    *binaryinst = '\0';
    binaryinst -= (i+1); 
    //printf("A-command translated to: %s\n", binaryinst);
    return 0; 
}

//translates assembly A-command into its binary instruction
int translate_a(char *acom, char *binaryinst, ht *table) { 
    if (isdigit(*acom) == 0) { //if A-command contains symbol
        char symb_type = symbol_type(acom);
        acom = translate_symbol(acom, symb_type, table);
        if (acom == NULL) {
            return 1; 
        }
    }   
    //convert decimal string to integer 
    int decimal_a = string2int(acom); 
    if (decimal_a == -1) {
        return 1; 
    } 
    //convert decimal int to binary string
    if (decint2binstring(decimal_a, binaryinst) == 1) {
        return 1; 
    } 
    return 0; 
}

//translates assembly C-command into its binary instruction
int translate_c(char *compfield, char *destfield, char *jumpfield, char *binaryinst) {
    entry *comp_entry = binary_lookup(comp_table, 28, compfield);
    entry *dest_entry = binary_lookup(dest_table, 10, destfield);
    entry *jump_entry = binary_lookup(jump_table, 8, jumpfield);
    int i;
    if (comp_entry != NULL && dest_entry != NULL && jump_entry != NULL) { //if all fields in tables
        for (i=0;i<3;i++) { //add three '1' marker bits at start of binary instruction
            *binaryinst++ = '1';
        }
        for (i=3;i<10;i++) {
            *binaryinst++ = *(comp_entry->value)++; //add 7 comp bits  
        }
        comp_entry->value -= 7; //point back to start of value 
        for (i=10;i<13;i++) {
            *binaryinst++ = *(dest_entry->value)++; //add 3 dest bits 
        } 
        dest_entry->value -= 3;
        for (i=13;i<16;i++) {
            *binaryinst++ = *(jump_entry->value)++; //add 3 jump bits 
        } 
        jump_entry->value -= 3;
        *binaryinst = '\0'; 
        binaryinst -= i;   
        //printf("C-command translated to: %s\n", binaryinst); 
        return 0;
    } else {
        if (comp_entry == NULL) {
            printf("ERROR: comp field '%s' is not in the Hack assembly specification\n", compfield);
        }
        if (dest_entry == NULL) {
            printf("ERROR: dest field '%s' is not in the Hack assembly specification\n", destfield);
        }
        if (jump_entry == NULL) {
            printf("ERROR: jump field '%s' is not in the Hack assembly specification\n", jumpfield);
        } 
        return 1;
    }
}

//writes binary translation into output .hack file  
void write_command(char *binaryinst, FILE *ofp) {
    while (*binaryinst != '\0') {
        putc(*binaryinst, ofp);
        ++binaryinst;
    }
    putc('\n', ofp); //add newline after each instruction in file 
    //printf("Command written to .hack file\n");     
}

/*MISC FUNCTIONS*/

//converts int into its string form e.g. 321 -> "321\0"
//for use in first pass of program in main() to convert symbol count values to string, so can store in hash table
char *int2string(int value) {
    //calculate num of digits in value 
    int num_digits;
    if (value == 0) {
        num_digits = 1; // Special case for 0
    } else {
        num_digits = log10(abs(value)) + 1;
    }
    char *str = malloc(num_digits+1); //enough for digits of value + \0
    if (str == NULL) {
        printf("ERROR: memory allocation for new stored symbol value failed\n");
        return NULL;
    }
    sprintf(str, "%d", value);
    return str; 
}

int main() {
    /*open files, skip header of input .asm file, create hash table for user-defined symbols*/
    FILE *ifp = input_file_access(infilename);
    FILE *ofp = output_file_access(outfilename);
    int skip_status = skipheader(ifp);
    ht *symbol_table = ht_create();
    if (ifp == NULL || ofp == NULL || skip_status == 1 || symbol_table == NULL) {
        fclose(ifp);
        fclose(ofp);
        printf("Closed .asm file\n");
        printf("Closed .hack file\n");
        return 1; 
    } 
    
    /*first pass of .asm file - stores all labels in symbol_table*/
    char *asscom = assemblycommand, *acom = acommand, *label = labelsymbol;
    char asscom_type;
    int asscom_length, insert_status;
    int ROM_count = 0; //program loaded from ROM[0]
    printf("FIRST PASS START\n");
    while ((asscom = fetchcommand(ifp, asscom)) != NULL) { //while still commands in .asm file 
        asscom_type = command_type(asscom);
        asscom_length = command_length(asscom);
        if (asscom_length == -1) {
            fclose(ifp);
            fclose(ofp);
            printf("Closed .asm file\n");
            printf("Closed .hack file\n");
            ht_delete(symbol_table);
            return 1; 
        }
        //if fetched command is an instruction label 
        if (asscom_type == 'l') {
            label = parse_label(asscom, label, asscom_length);
            char *stored_ROM_count = int2string(ROM_count); //value of label = ROM address of instruction that follows label in program 
            if (stored_ROM_count == NULL) {
                fclose(ifp);
                fclose(ofp);
                printf("Closed .asm file\n");
                printf("Closed .hack file\n");
                ht_delete(symbol_table);
                return 1;
            } 
            insert_status = ht_insert(symbol_table, label, stored_ROM_count); 
            free(stored_ROM_count); //ht_insert uses strdup so stored_ROM_count is redundant  
            if (insert_status == 1) {
                fclose(ifp);
                fclose(ofp);
                printf("Closed .asm file\n");
                printf("Closed .hack file\n");
                ht_delete(symbol_table);
                return 1;
            }
        } 
        else {
            ++ROM_count;
        }  
    }
    printf("FIRST PASS END\n\n");

    /*prepare for second pass: reset ifp and asscom (becomes NULL at EOF) pointers, skip header*/
    fseek(ifp, 0, SEEK_SET); //move ifp to start of program 
    asscom = assemblycommand;
    printf("File pointer reset\n");
    skip_status = skipheader(ifp); 
    if (skip_status == 1) {
        fclose(ifp);
        fclose(ofp);
        printf("Closed .asm file\n");
        printf("Closed .hack file\n");
        ht_delete(symbol_table);
        return 1; 
    } 
    
    /*second pass of .asm file - stores all user-defined variables in symbol table and translates assembly commands into their binary instructions*/
    char *dest = destfield, *comp = compfield, *jump = jumpfield, *bininst = binaryinstruction;
    int a_status, c_status;
    int RAM_count = 16; //user-defined variables stored from RAM[16]
    printf("\nSECOND PASS START\n\n");
    while ((asscom = fetchcommand(ifp, asscom)) != NULL) { 
        asscom_type = command_type(asscom);
        asscom_length = command_length(asscom);
        if (asscom_length == -1) {
            fclose(ifp);
            fclose(ofp);
            printf("Closed .asm file 1\n");
            printf("Closed .hack file\n");
            ht_delete(symbol_table);
            return 1; 
        }
        //if fetched command is A-command
        if (asscom_type == 'a') {
            acom = parse_a(asscom, acom, asscom_length); 
            //if symbol is user-defined (label or variable) and not already in table, add it to table first 
            if (isdigit(*acom) == 0 && symbol_type(acom) == 'u' && ht_lookup(symbol_table, acom) == NULL) {  //this could be cleaner
                char *stored_RAM_count = int2string(RAM_count); //want to store next free RAM address (RAM[16] onwards) as value of variable
                if (stored_RAM_count == NULL) {
                    fclose(ifp);
                    fclose(ofp);
                    printf("Closed .asm file\n");
                    printf("Closed .hack file\n");
                    ht_delete(symbol_table);
                    return 1;
                }
                insert_status = ht_insert(symbol_table, acom, stored_RAM_count); 
                free(stored_RAM_count); //ht_insert uses strdup, stored_RAM_count is redundant
                if (insert_status == 1) {
                    fclose(ifp);
                    fclose(ofp);
                    printf("Closed .asm file\n");
                    printf("Closed .hack file\n");
                    ht_delete(symbol_table);
                    return 1;
                } 
                ++RAM_count;
            } 
            a_status = translate_a(acom, bininst, symbol_table);
            if (a_status == 1) {
                fclose(ifp);
                fclose(ofp);
                printf("Closed .asm file 2\n");
                printf("Closed .hack file\n");
                ht_delete(symbol_table);
                return 1; 
            } 
            write_command(bininst, ofp);
        } 
        //if fetched command is C-command
        else if (asscom_type == 'c') { 
            dest = parse_dest(asscom, dest);
            comp = parse_comp(asscom, comp, dest);
            jump = parse_jump(asscom, jump, dest, asscom_length);
            c_status = translate_c(comp, dest, jump, bininst);
            if (c_status == 1) {
                fclose(ifp);
                fclose(ofp);
                printf("Closed .asm file 3\n");
                printf("Closed .hack file\n");
                ht_delete(symbol_table);
                return 1; 
            } 
            write_command(bininst, ofp);
        }
    }
    printf("\n\nSECOND PASS END\n\n");
    fclose(ifp);
    fclose(ofp);
    printf("Closed .asm file 4\n");
    printf("Closed .hack file\n");
    ht_delete(symbol_table);
    return 0;
}


