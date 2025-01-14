The Hack assembler main.c translates a Hack assembly file into its binary machine instructions, per the Hack machine language specification. 
Project details found at https://www.nand2tetris.org/_files/ugd/44046b_89a8e226476741a3b7c5204575b8a0b2.pdf 

v1.0:
- no handling of pre-defined symbols, user-defined variables or symbolic labels. Need to add extra hash tables to handle these translations.
- includes full error handling
- uses hash tables with separate chaining collision handling - TOTAL LOOKUP = O(1) table lookup + O(N) linear search along an N-element linked list

v1.1:
- handles pre-defined symbols

comparison of linear search, binary search and hash table lookup as the number of key/value pairs increases (via lookup_speed_test.c):  
_% gcc lookup_speed_test.c_  
NUM KEYS: 1  
  linear, 10000 runs: 0.000073000s  
  binary, 10000 runs: 0.000118000s  
  hash  , 10000 runs: 0.000159000s  
NUM KEYS: 2  
  linear, 10000 runs: 0.000096500s  
  binary, 10000 runs: 0.000157500s  
  hash  , 10000 runs: 0.000193000s  
NUM KEYS: 3  
  linear, 10000 runs: 0.000124333s  
  binary, 10000 runs: 0.000177333s  
  hash  , 10000 runs: 0.000233667s  
NUM KEYS: 4  
  linear, 10000 runs: 0.000142000s  
  binary, 10000 runs: 0.000194250s  
  hash  , 10000 runs: 0.000255250s  
NUM KEYS: 5  
  linear, 10000 runs: 0.000167400s  
  binary, 10000 runs: 0.000241400s  
  hash  , 10000 runs: 0.000253400s  
NUM KEYS: 6  
  linear, 10000 runs: 0.000258000s  
  binary, 10000 runs: 0.000280500s  
  hash  , 10000 runs: 0.000316167s  
NUM KEYS: 7  
  linear, 10000 runs: 0.000172714s  
  binary, 10000 runs: 0.000277714s  
  hash  , 10000 runs: 0.000251429s  
NUM KEYS: 8  
  linear, 10000 runs: 0.000309875s  
  binary, 10000 runs: 0.000270875s  
  hash  , 10000 runs: 0.000208125s  
NUM KEYS: 9  
  linear, 10000 runs: 0.000289333s  
  binary, 10000 runs: 0.000251333s  
  hash  , 10000 runs: 0.000148889s  
NUM KEYS: 10  
  linear, 10000 runs: 0.000218400s  
  binary, 10000 runs: 0.000229700s  
  hash  , 10000 runs: 0.000190000s  
NUM KEYS: 11  
  linear, 10000 runs: 0.000250182s  
  binary, 10000 runs: 0.000198455s  
  hash  , 10000 runs: 0.000165545s  
NUM KEYS: 12  
  linear, 10000 runs: 0.000274083s  
  binary, 10000 runs: 0.000179917s  
  hash  , 10000 runs: 0.000155833s  
NUM KEYS: 13  
  linear, 10000 runs: 0.000264846s  
  binary, 10000 runs: 0.000193462s  
  hash  , 10000 runs: 0.000142385s  
NUM KEYS: 14  
  linear, 10000 runs: 0.000262286s  
  binary, 10000 runs: 0.000197000s  
  hash  , 10000 runs: 0.000132071s  
NUM KEYS: 15  
  linear, 10000 runs: 0.000264867s  
  binary, 10000 runs: 0.000191733s  
  hash  , 10000 runs: 0.000127400s  
NUM KEYS: 16  
  linear, 10000 runs: 0.000259813s  
  binary, 10000 runs: 0.000172937s  
  hash  , 10000 runs: 0.000126063s  
NUM KEYS: 17  
  linear, 10000 runs: 0.000259412s  
  binary, 10000 runs: 0.000165471s  
  hash  , 10000 runs: 0.000116588s  
NUM KEYS: 18  
  linear, 10000 runs: 0.000263444s  
  binary, 10000 runs: 0.000168889s  
  hash  , 10000 runs: 0.000113833s  
NUM KEYS: 19  
  linear, 10000 runs: 0.000267105s  
  binary, 10000 runs: 0.000155368s  
  hash  , 10000 runs: 0.000110000s  
NUM KEYS: 20  
  linear, 10000 runs: 0.000291100s  
  binary, 10000 runs: 0.000158900s  
  hash  , 10000 runs: 0.000109450s  
NUM KEYS: 21  
  linear, 10000 runs: 0.000296429s  
  binary, 10000 runs: 0.000172905s  
  hash  , 10000 runs: 0.000114238s  
NUM KEYS: 22  
  linear, 10000 runs: 0.000306864s  
  binary, 10000 runs: 0.000162864s  
  hash  , 10000 runs: 0.000119409s  
NUM KEYS: 23  
  linear, 10000 runs: 0.000318478s  
  binary, 10000 runs: 0.000161174s  
  hash  , 10000 runs: 0.000112217s  
NUM KEYS: 24  
  linear, 10000 runs: 0.000338667s  
  binary, 10000 runs: 0.000163750s  
  hash  , 10000 runs: 0.000110667s  
NUM KEYS: 25  
  linear, 10000 runs: 0.000351080s  
  binary, 10000 runs: 0.000154840s  
  hash  , 10000 runs: 0.000111520s  
NUM KEYS: 26  
  linear, 10000 runs: 0.000364038s  
  binary, 10000 runs: 0.000173769s  
  hash  , 10000 runs: 0.000113231s  
NUM KEYS: 27  
  linear, 10000 runs: 0.000378667s  
  binary, 10000 runs: 0.000174185s  
  hash  , 10000 runs: 0.000112259s  
NUM KEYS: 28  
  linear, 10000 runs: 0.000393429s  
  binary, 10000 runs: 0.000173143s  
  hash  , 10000 runs: 0.000116357s  


