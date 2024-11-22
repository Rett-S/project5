CS 4760 - Project 5: Resource Management
Rett Swyers

Intructions on how to run this file:
- use the 'make' command to generate executable files for oss and worker 
- worker will wait for a message from oss before starting, in which they will start a clock and terminate after 
a random amount of time, based on the seed of the child process's PID. There is a high chance worker will terminate 
before starting the clock, and a high chance for the worker to deadlock.
- oss will immediately fork to create two processes, one will keep track of a global system time that the workers 
will use to keep track of, and the other will create as many worker processes that the user specifies
- to run oss, you can first run './oss -h' to display this help message in the terminal. To specify how many 
workers to run, enter '-n' followed by a number. To specify how many processes can run at once, enter '-s' followed 
by a number. To specify the interval to launch each child process, enter '-t' followed by a number. For example, you 
could enter './oss -n 5 -s 3 -t 6' to run 5 workers total, 3 processes to run at most, with each child process 
starting every 6 seconds 
- to only run worker, enter in './worker', but this will wait for a message from a nonexistant parent in order to 
proceed with the rest of it's program 
- After a total of 60 seconds, the program will close entirely.
- A logfile will be created to show the process of OSS and the workers."

Things this program does not do:
- The program does not take the user's input to generate a logfile or not. The logfile will always be created.