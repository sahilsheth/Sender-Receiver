# Sender-Receiver
Written in C++
Members:
  Sahil Sheth,   Section 1, sahilsheth642@csu.fullerton.edu
  Marco Botello, Section 1, marco.botello@csu.fullerton.edu
  Emmanuel Khlot Section 1, ekhlot@csu.fullerton.edu
  Cesar Martinez Melgoza Section 1, cesmartz10@csu.fullerton.edu


How to Run Program:
  Step 1.) In terminal use 'cd ... ' to change directory to the folder named Sender-Receiver
  Step 2.) Type make, this will execute the makefile which will compile the cpp files
  Step 3.) Use './sender <TEXTFILE>' to run sender on a file



Member Contributions:
  Marco: 
    -cleanUp() function delcared to be called in main and detach memory
    -Implemented Ctrl-C Signal handling (deallocate mem and delete message queue)
    -Implemented sending a message to sender from recv when ready for next set of bytes
  Emmanuel
    -syntax correction
    -Linux implementation
    - Documentation
    
  Sahil:
    -init() generates a key for the queue and the shared memory segment
    -recvFileName() receiving the name of the file
    -Counting the bytes sent and message to the recevier in sendFile(const char* fileName): main send function
    -Set the file name in the message in sendFileName(const char* fileName)


  Cesar:
    - Finished init function for sender.cpp
    - Attached message to shared memory and message queue
    - An object of the message struct was passed into the function 



Extra Credit Not Implemented
