//nand2tetris project 6: build an Assembler to translate Hack assembly file into Hack machine language  
//https://www.nand2tetris.org/course (course directory)
//https://drive.google.com/file/d/1CITliwTJzq19ibBF5EeuNBZ3MJ01dKoI/view (project specification)
//version 1.0
#include "HashTable.c"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define NAMELENGTH 1000

//routine deals with one command at a time
char infilename[NAMELENGTH] = "path_to_assembly_input_file";
char outfilename[NAMELENGTH] = "path_to_binary_output_file";  
char assemblycommand[12]; //max possible length of a Hack Assembly command is 11 chars + newline \n
char destfield[4]; //fields of assembly C-command all up to 3 symbols + terminating '\0' char
char compfield[4];
char jumpfield[4];
char acommand[6]; //assembly A-command up to 5 decimal digits + '\0' 
char binaryinstruction[17]; //binary instruction always 16 bits + '\0'

FILE *input_file_access(char *inputfilename);
FILE *output_file_access(char *outfilename);
char *fetchcommand(FILE *ifp, char *assemblycommand);
char command_type(char *assemblycommand);
int command_length(char *assemblycommand, char command_type);
char *parse_dest(char *assemblycommand, char *destfield);
char *parse_comp(char *assemblycommand, char *compfield, char *destfield);
char *parse_jump(char *assemblycommand, char *jumpfield, char *destfield, int command_length);
char *parse_a(char *assemblycommand, char *acommand, int asscom_length);
int translate_c(char *compfield, char *destfield, char *jumpfield, char *binaryinstruction); 
int translate_a(char *acommand, char *binaryinstruction);
int string2int(char *acommand);
int int2string(int decimal_a, char *binaryinstruction);
void write_command(char *binaryinstruction, FILE *ofp);

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
    return -1; 
}

//fetch next assembly command, read into assemblycommand array. Ignores comments and whitespace 
char *fetchcommand(FILE *ifp, char *asscom) {
    int n=0;
    char c = getc(ifp); 
    while (c != EOF && isspace(c) == 0 && c != '/') {
        *asscom++ = c;
        ++n;
        c = getc(ifp);
    }
    if (isspace(c) != 0 || c == '/') {
        while (c != '\n') {
            c = getc(ifp); 
        }
        *asscom = c;    
    } else {
       printf("\nEOF REACHED\n");
       return NULL; 
    } 
    asscom -= n;
    printf("\nNEXT COMMAND FETCHED\n");
    return asscom; //points to start of fetched assembly command
}

//returns type of fetched assembly command (per Hack ml API): A or C
char command_type(char *asscom) {
    if (*asscom == '@') {
        printf("Command type = A\n");
        return 'a';
    } else {
        printf("Command type = C\n");
        return 'c'; 
    }
}

int command_length(char *asscom, char command_type) {
    int n=0;
    if (command_type == 'c') {
        while (*asscom != '\n') {
            if (n == 11) {
                printf("ERROR: fetched command %s too long to be correct Hack assembly C-command, max C-command length is 11 characters", asscom);
                return -1;
            }
            ++asscom;
            ++n;
        }
    } else {
        while (*asscom != '\n') {
            if (n == 6) {
                printf("ERROR: fetched command %s too long to be correct Hack assembly A-command, max A-command length is 6 characters\n", asscom);
                return -1;
            }
            ++asscom;
            ++n;
        }
    }
    printf("Command length = %d\n", n);
    asscom -= n;  
    return n;
}

//parses assembly A-command into array containing its decimal representation
char *parse_a(char *asscom, char *acom, int asscom_length) {
    ++asscom; //skip '@' char at front of A-command 
    while (*asscom != '\n') { 
        *acom++ = *asscom++;
    }
    *acom = '\0'; 
    acom -= (asscom_length - 1); 
    asscom -= asscom_length; //point to start of assembly command, to pass back to fetchcommand()
    //printf update start 
    printf("A-command parsed as: ");
    while (*acom != '\0') {
        printf("%c", *acom);
        ++acom;
    }
    printf("\n");
    acom -= (asscom_length - 1);
    //end
    return acom;
}

/*next three funcs split assembly C-command into subarray for each field*/
//returns ptr to array containing dest field of assembly C-command
char *parse_dest(char *asscom, char *destfield) {
    int n=0; 
    while (*asscom != '=') { //whilst in dest field 
        if (n == 4) { //if no dest field
            destfield -= n; 
            *destfield++ = 'n'; 
            *destfield++ = 'u';
            *destfield++ = 'l';
            *destfield = '\0';
            destfield -= 3; 
            asscom -= n; 
            //printf update start 
            printf("Dest field parsed as: ");
            while (*destfield != '\0') {
                printf("%c", *destfield);
                ++destfield;
            }
            printf("\n");
            destfield -= 3;
            //end 
            return destfield;   
        } else {
            *destfield++ = *asscom++;
            ++n;
        }
    }
    *destfield = '\0';
    destfield -= n;
    asscom -= n;
    //printf update start 
    printf("Dest field parsed as: ");
    while (*destfield != '\0') {
        printf("%c", *destfield);
        ++destfield;
    }
    printf("\n");
    destfield -= n;
    //end
    return destfield; 
}

//returns ptr to array containing comp field of assembly C-command
char *parse_comp(char *asscom, char *compfield, char *destfield) {
    int n=0;
    if (*destfield != 'n') { //if dest field present
        while (*asscom != '=') { //skip to end of dest field in assembly command 
            ++asscom;
        } 
        ++asscom; //point to start of comp field 
    } 
    while (*asscom != ';') { //whilst in comp field 
        if (*asscom == '\n') { //if no jump field 
            *compfield = '\0';
            compfield -= n;
            asscom -= n;
            //printf update start 
            printf("Comp field parsed as: ");
            while (*compfield != '\0') {
                printf("%c", *compfield);
                ++compfield;
            }
            printf("\n");
            compfield -= n;
            //end  
            return compfield;
        } else {  
            *compfield++ = *asscom++;
            ++n;
        }
    }
    *compfield = '\0'; 
    compfield -= n;  
    asscom -= n; 
    //printf update start 
    printf("Comp field parsed as: ");
    while (*compfield != '\0') {
        printf("%c", *compfield);
        ++compfield;
    }
    printf("\n");
    compfield -= n;
    //end 
    return compfield;
}

//returns ptr to array containing jump field of assembly C-command 
char *parse_jump(char *asscom, char *jumpfield, char *destfield, int asscom_length) {
    int n=0;
    if (*destfield != 'n') { 
        while (*asscom != '=') { 
            ++asscom;
        } 
        ++asscom; 
    }
    while (*asscom != ';' && *asscom != '\n') { //skip through comp field 
        ++asscom;
    }
    if (*asscom == '\n') { //if no jump field 
        *jumpfield++ = 'n'; 
        *jumpfield++ = 'u';
        *jumpfield++ = 'l';
        *jumpfield = '\0';
        jumpfield -= 3; 
        asscom -= asscom_length; //points at start of assembly command, to pass to fetchcommand() 
        //printf update start 
        printf("Jump field parsed as: ");
        while (*jumpfield != '\0') {
            printf("%c", *jumpfield);
            ++jumpfield;
        }
        printf("\n");
        jumpfield -= 3;
        //end 
        return jumpfield;
    } else { //if jump field present 
        while (*asscom != '\n') { 
            *jumpfield++ = *asscom++;
            ++n;
        }
    }
    *jumpfield = '\0'; 
    jumpfield -= n; 
    asscom -= asscom_length; //points at start of assembly command, to pass to fetchcommand()
    //printf update start 
    printf("Jump field parsed as: ");
    while (*jumpfield != '\0') {
        printf("%c", *jumpfield);
        ++jumpfield;
    }
    printf("\n");
    jumpfield -= n;
    //end
    return jumpfield;
}


/*CODE MODULE*/

/*next three functions translate assembly A-command into its binary instruction*/
//converts string assembly A-command into its integer form e.g. "321\0" -> 321
int string2int(char *acom) {
    int n=0;
    while (*acom >= '0' && *acom <= '9') {
        n = 10 * n + (*acom - '0'); 
        ++acom;
    }
    if (*acom == '\0') {
        return n;
    } else {
        printf("ERROR: non-decimal character in fetched A-command: %c\n", *acom);
        return -1;
    }
}

//converts int assembly A-command into its binary instruction 
int int2string(int decimal_a, char *binaryinst) {
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
            printf("ERROR: Invalid non-binary integer in A-command translation: %d at position %d\n", binary_a[i], i);
            return -1;
        }
    }
    *binaryinst = '\0';
    binaryinst -= (i+1); 
    //printf update start 
    printf("A-command translated to: ");
    while (*binaryinst != '\0') {
        printf("%c", *binaryinst);
        ++binaryinst;
    }
    printf("\n");
    binaryinst -= (i+1);
    //end
    return 0; 
}

//translates an assembly A-command into its binary instruction
int translate_a(char *acom, char *binaryinst) { 
    int decimal_a = string2int(acom); 
    if (decimal_a == -1) {
        return -1; //exit if non-decimal character in fetched A-command
    } else {
        int conversion_status = int2string(decimal_a, binaryinst);
        if (conversion_status == -1) {
            return -1; //exit if non-binary integer found in A-command translation
        } else {
            return 0; 
        }
    }
}

//translates assembly C-command into its binary instruction
int translate_c(char *compfield, char *destfield, char *jumpfield, char *binaryinst) {
    struct Entry *comp_entry = lookup(compfield, 'c');
    struct Entry *dest_entry = lookup(destfield, 'd');
    struct Entry *jump_entry = lookup(jumpfield, 'j');
    int i;
    if (comp_entry != NULL && dest_entry != NULL && jump_entry != NULL) { //if all fields in hash tables
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
        //printf update start 
        printf("C-command translated to: ");
        while (*binaryinst != '\0') {
            printf("%c", *binaryinst);
            ++binaryinst;
        }
        printf("\n");
        binaryinst -= i;
        //end 
        return 0;
    } else {
        if (comp_entry == NULL) {
            printf("ERROR: comp field '%c' is not in Hack API\n", *(comp_entry->key));
        }
        if (dest_entry == NULL) {
            printf("ERROR: dest field '%c' is not in Hack API\n", *(dest_entry->key));
        }
        if (jump_entry == NULL) {
            printf("ERROR: jump field '%c' is not in Hack API\n", *(jump_entry->key));
        } 
        return -1;
    }
}

//writes binary translation into output .hack file  
void write_command(char *binaryinst, FILE *ofp) {
    while (*binaryinst != '\0') {
        putc(*binaryinst, ofp);
        ++binaryinst;
    }
    putc('\n', ofp); //add newline after each instruction in file 
    printf("Command written to .hack file\n");     
}

int main() {
    FILE *ifp = input_file_access(infilename);
    FILE *ofp = output_file_access(outfilename);
    char *asscom = assemblycommand, *acom = acommand, *dest = destfield, *comp = compfield, *jump = jumpfield, *bininst = binaryinstruction; 
    char asscom_type;
    int asscom_length, skip_status, a_status, c_status, fill_status, delete_status;
    if (ifp == NULL) {
        fclose(ifp);
        fclose(ofp);
        printf("Closed .asm file\n");
        printf("Closed .hack file\n");
        return -1; //exit if the .asm file cannot be opened
    } else {
        if (ofp == NULL) {
            fclose(ifp);
            fclose(ofp);
            printf("Closed .asm file\n");
            printf("Closed .hack file\n");
            return -1; //exit if .hack file cannot be created 
        } else {
            if ((skip_status = skipheader(ifp)) == -1) {
                fclose(ifp);
                fclose(ofp);
                printf("Closed .asm file\n");
                printf("Closed .hack file\n");
                return -1; //exit if no commands in .asm file 
            } else {
                fill_status = fill_tables(comp_valid_symbols, jump_valid_symbols, dest_valid_symbols, comp_valid_values, dest_jump_valid_values); //create hash tables
                if (fill_status == -1) {
                    fclose(ifp);
                    fclose(ofp);
                    printf("Closed .asm file\n");
                    printf("Closed .hack file\n");
                    delete_tables();
                    return -1; //exit if insufficient memory to create hash table 
                } else {
                    while ((asscom = fetchcommand(ifp, asscom)) != NULL) { //while still commands in .asm file 
                        asscom_type = command_type(asscom);
                        if ((asscom_length = command_length(asscom, asscom_type)) == -1) {
                            fclose(ifp);
                            fclose(ofp);
                            printf("Closed .asm file\n");
                            printf("Closed .hack file\n");
                            delete_tables();
                            return -1; //exit if fetched command too long to be correct Hack assembly command 
                        } else {
                            if (asscom_type == 'a') {
                                acom = parse_a(asscom, acom, asscom_length); 
                                a_status = translate_a(acom, bininst);
                                if (a_status == -1) {
                                    fclose(ifp);
                                    fclose(ofp);
                                    printf("Closed .asm file\n");
                                    printf("Closed .hack file\n");
                                    delete_tables();
                                    return -1; //exit if non-decimal character in fetched A-command or non-binary integer found in A-command translation
                                } else {
                                    write_command(bininst, ofp);
                                }
                            } else { 
                                dest = parse_dest(asscom, dest);
                                comp = parse_comp(asscom, comp, dest);
                                jump = parse_jump(asscom, jump, dest, asscom_length);
                                c_status = translate_c(comp, dest, jump, bininst);
                                if (c_status == -1) {
                                    fclose(ifp);
                                    fclose(ofp);
                                    printf("Closed .asm file\n");
                                    printf("Closed .hack file\n");
                                    delete_tables();
                                    return -1; //exit if fetched field not in Hack API hash table 
                                } else {
                                    write_command(bininst, ofp);
                                }
                            }
                        }
                    }
                    fclose(ifp);
                    fclose(ofp);
                    printf("Closed .asm file\n");
                    printf("Closed .hack file\n");
                    delete_tables();
                    return 0;
                }
            } 
        }
    }
}

