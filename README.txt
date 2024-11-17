CS 4760 - Project 4: Process Scheduling
Rett Swyers

Intructions on how to run this file:
- use the "make" command to generate executable files for oss and worker
- worker will start a clock and terminate after a set amount of time, based on user input
- oss will immediately fork to create two processes, one will keep track of a global system time that the 
workers will use to keep track of, and the other will create as many worker processes that the user specifies
- to run oss, you can first run "./oss -h" to display this help message in the terminal. To specify how many 
workers to run, enter "-n" followed by a number. To specify how many processes can run at the same time, enter "-s" 
followed by a number. To specify the interval to launch child processes, enter "-t" followed by a number. For example,
you could enter "./oss -n 5 -s 3 -t 6 " to run 5 workers total, 3 processes to run at most, with each child process starting every 6 seconds.
- to only run worker enter in "./worker", but it will wait for a message from a nonexistant parent in order to proceed with the rest of it's program.
- After a total of 60 seconds, the program will close.
- A logfile will be created to show the process of OSS and the workers

Things this program does not do:
- The program does not take the user's input to generate a logfile or not. The logfile will always be created.