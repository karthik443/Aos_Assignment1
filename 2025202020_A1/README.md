Language Used: C++

Problem statement for Q1: Create a programm to reverse a file using system calls . 

file created : 2025202020_A1/2025202020_A1_Q1.cpp
to compile   : g++  2025202020_A1/2025202020_A1_Q1.cpp

default chunk size taken for flag 1 & flag 2 500KB.

programm will create a folder "Assignment1" upon execution and create a "<flag>_<input_file>.txt   file as output . 

1-   chunk reversal programm execution : ./a.out <input file> 0 <block size>
2-   total file reversal               : ./a.out <input file> 1
3-   reverse [0-st), keep as it is [st,end] and reverse (end,EOF]  : ./a.out <input file> 2 <start index> <end index>



problem statement Q2: 

1- Check the permissions for the two files and the directory.
2- Check whether the content in the new file follows the expected reversal pattern
   based on the flag used in Q1.

file created : 2025202020_A1/2025202020_A1_Q2.cpp
to compile   : g++  2025202020_A1/2025202020_A1_Q1.cpp

default chunk size taken for flag 1 & flag 2 500KB.

 Flag 0 : ./a.out <newfilepath> <oldfilepath> <directory> 0 <blockSize>

 Flag 1 : ./a.out <newfilepath> <oldfilepath> <directory> 1 

 Flag 2 : ./a.out <newfilepath> <oldfilepath> <directory> 2 <start> <end>


Logic used for Q1------>

For progress Percentage used cout<<"\r some text";   to overwrite the existing line of terminal output.

Flag 0 : 
        -Took chunk size provided in input.
        -Read the input file as given chunks
        -Reversed the that chunk and and write the reversed chunk in new file
        -move the fd to next chunk
        -repeat step 2 - 4.
        (if the last piece of file is less than chunk took the piece length instead of the entire chunk length).
Flag 1 : 
        -Same as flag 0. but instead read the chunks from [(EOF-chunksize)-EOF]
        -move the fd  one chunk back.
        -repeat step 1-2 till you read starting chunk.
         (if the starting piece of file is less than chunk took the piece length instead of the entire chunk length).

Flag 2 : Used the functions of flag 0 and flag 1 here.
         flag 1 func [0,st)
         flag 0 func[st,end] (without reversing chunk buffer)
         flag 1 func (end,EOF]

Logic used for Q2 ------>

        -To find permission I have used stat and got the octal code . based on that I have found out each and              every  permission.
        -Used the stat to check directory is present or not.
        -Used lseek to find filesize of new_file and old_file 
    
    Flag 0 , 1 , 2 Logic for verifying the output. 

        - I have used the above existing functions. But, Took old file and my input file and new file as my output file.
        - Opened both the files.
        - Instead of writing to newfile. read two buffers of same length, for new file and old_file.
        - for new_file read the buffer in continuos manner.
        - for old_file read the buffer using the above functions logic which I used on Q1. (did not reveresed the buffer now.)
        - checked the Reversed buffer of old file is same as new file buffer.

        [Note : For flag 2 in range [st,end] both buffers should be same instead of reverse ].


End.

Thankyou for reading.


    
