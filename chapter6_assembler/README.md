**nand2tetris project 6**   
My implementation of an assembler that translates a Hack assembly files (.asm) into a Hack machine language file (.hack). Works for all test files provided by nand2tetris course.   
  
- see hack_language_specification.png for Hack language syntax
- potential improvements:
  there is no error-handling for incorrect Hack assembly syntax 
  dynamically allocate space for assembly commands to reduce memory wasteage 
  make the symbol checking routine cleaner
- lookup_speed_test.c provides comparison of linear search, binary search and hash table lookup times for given number of table entries 
