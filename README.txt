# Sender-Receiver
Written in C++
Members:
  Marco Botello, Section 1, marco.botello@csu.fullerton.edu


How to Run Program:
  Step 1.) In terminal use 'cd ... ' to change directory to the folder named Sender-Receiver
  Step 2.) Type make, this will execute the makefile which will compile the cpp files
  Step 3.) Use './sender <TEXTFILE>' to run sender on a file



Member Contributions:
  Marco: 
    -cleanUp() function delcared to be called in main and detach memory
    -Implemented Ctrl-C Signal handling (deallocate mem and delete message queue)
    -Implemented sending a message to sender from recv when ready for next set of bytes





Extra Credit Not Implemented
