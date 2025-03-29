**nand2tetris project 6**   
My implementation of an assembler (src/main.c) that translates a Hack assembly file (.asm) into a Hack machine language file (.hack). Works for all test .asm files provided by the nand2tetris course (see .asm files in test folder). 
  
- see hack_language_specification.png for Hack language syntax
- see project_6_description.pdf for details of project 
- potential improvements:  
  there is incomplete error-handling for incorrect Hack assembly syntax   
  could dynamically allocate space for fetched assembly commands to reduce memory wasteage   
  could make the symbol checking routine cleaner  
- lookup_speed_test.c provides comparison of linear search, binary search and hash table lookup times for given number of table entries 
