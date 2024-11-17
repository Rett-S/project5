//Rett Swyers - CS 4760

#include<unistd.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include"string.h"
#include<signal.h>
#include<sys/ipc.h>
#include<sys/shm.h>

void signal_handler();
void clean();

#define SHMKEY 44197
#define BUFF_SZ sizeof (int)
#define PERMS 0771

int n,s,t,it,totalLaunched,currentProc,childPid,currentSec,currentNano;

typedef struct msgbuffer {
        long mtype;
        char strData[100];
        int intData;
} msgbuffer;

typedef struct PCB_0 {
    int occupied; // either true or false
    pid_t pid; // process id of this child
    int seconds; // time when it was forked
    int nanoseconds; // time when it was forked
    int serviceTimeSeconds; // total seconds it has been scheduled
    int serviceTimeNano; // total nanoseconds it has been scheduled
    int eventWaitSec; // when does its event happen in seconds?
    int eventWaitNano; // when does its event happen in nano?
    int blocked; // is this process waiting on event?
} PCB;
//struct PCB processTable[20];

PCB *pTable;
PCB *create(int size){
        PCB *table = (PCB *)malloc(sizeof(PCB) * size);
        int i;
        for (i=0;i<size;i++){
                table[i].occupied=0;
                table[i].pid=0;
                table[i].seconds=0;
                table[i].nanoseconds=0;
                table[i].serviceTimeSeconds=0;
                table[i].serviceTimeNano=0;
                table[i].eventWaitSec=0;
                table[i].eventWaitNano=0;
                //table[i].blocked=0;
        }
        return table;
}

void add(PCB *pTable, int size, pid_t pid, int seconds, int nanoseconds){
        printf("adding to process table %d\n", pid);
        for (int i=0;i<size;i++){
                if (pTable[i].pid==0) {
                        pTable[i].pid = pid;
                        pTable[i].occupied = 1;
                        pTable[i].seconds = seconds;
                        pTable[i].nanoseconds = nanoseconds;
                        pTable[i].serviceTimeSeconds=0;
                        pTable[i].serviceTimeNano=0;
                        pTable[i].eventWaitSec=0;
                        pTable[i].eventWaitNano=0;
                        //pTable[i].blocked=0;

                break;
                }
        }
}


void outputPCB(PCB *pTable, int size, FILE *output_stream) {
        fprintf(output_stream, "Occupied\tPID\tSeconds\tNanoseconds\n");
        for(int i=0;i<size;i++){
                if (pTable[i].pid==0){
                        break;
                }
                fprintf(output_stream, "%8d\t%4d\t%8d\t%d\n", pTable[i].occupied, pTable[i].pid, pTable[i].seconds, pTable[i].nanoseconds);
        }
}

void printPCB(PCB *pTable, int size){
        outputPCB(pTable, size, stdout);
}



int main(int argc, char** argv) {

  char* x = "-h"; //x and y are used later to compare two strings
  char* y = "-n";


  msgbuffer buf0, buf1;
  int msqid;
  key_t key;
  system( "touch msgq.txt");

  if ((key = ftok("msgq.txt", 1)) == -1) {
      perror("ftok");
      exit(1);
  }

  if ((msqid = msgget(key, PERMS | IPC_CREAT)) == -1) {
      perror("msgget in parent");
      exit(1);
  }

  switch (fork()) {
    case -1:
        fprintf(stderr,"Failed to fork\n");
        exit(1);
    default:
        signal(SIGALRM, signal_handler);  // Turn on alarm handler
        alarm(60);  // set up alarm call
        int shmid = shmget(SHMKEY, BUFF_SZ, 0777 | IPC_CREAT);

        if ( shmid == -1 ) {
          fprintf(stderr,"Parent: ... Error in shmget ...\n");
          exit (1);
        }

        int *pint = (int *)(shmat (shmid, (void*)0, 0 ) );
        //int *nint = (int *)(shmat (nanoid, (void*)0, 0) );
        int *nint = pint + 1;
        for (int i=0;i<60;i++) {
          *pint = i;
          currentSec = i;
          //*systemclock->seconds = 1*i;
          for (int x=0;x<10;x++) {
            usleep(100000);
            *nint = 100000*x;
            currentNano = 100000*x;
            //printf("Time: %d : %d\n",*pint,*nint);
          }
          //printPCB(pTable, n);
          //printf("Seconds: %d Nanoseconds: %d\n",*pint,*nint);

        }

        shmdt(pint);
        shmctl(shmid,IPC_RMID,NULL);

        break;
    case 0:
        if (strcmp(argv[1], x) == 0) { //if -h is present, displays a help message only
        printf("- Intructions on how to run this file: \n- use the 'make' command to generate executable files for oss and worker \n- worker will start a clock and terminate after a random amount of time, based on the seed of the child process's PID\n- oss will immediately fork to create two processes, one will keep track of a global system time that the workers will use to keep track of, and the other will create as many worker processes that the user specifies\n- to run oss, you can first run './oss -h' to display this help message in the terminal. To specify how many workers to run, enter '-n' followed by a number. To specify how many processes can run at once, enter '-s' followed by a number. To specify the interval to launch each child process, enter '-t' followed by a number. For example, you could enter './oss -n 5 -s 3 -t 6' to run 5 workers total, 3 processes to run at most, with each child process starting every 6 seconds\n- to only run worker, enter in './worker', but this will wait for a message from a nonexistant parent in order to proceed with the rest of it's program \n- After a total of 60 seconds, the program will close entirely.\n- A logfile will be created to show the process of OSS and the workers.");
         exit(1);
        } else if (strcmp(argv[1], y) != 0) {
          printf("Invalid user input, closing program.\n");
          exit(1);
        } else if (strcmp(argv[1], y) == 0) {//if -h is not present, and -n is, the process continues as normal
         n = atoi(argv[2]); //int n stores how many processes to run, based on user input
         printf("Processes to launch: %d \n",n);

        } else if (strcmp(argv[1], y) == 0) {//if -h is not present, and -n is, the process continues as normal
         n = atoi(argv[2]); //int n sotres how many processes to run, based on user input
         printf("Processes to launch: %d \n",n);
         int m = atoi(argv[2]);
         pid_t child[m];
         pTable=create(n);
         s = atoi(argv[4]); //int s stores how many processes are allowed to run at once, based on user input
         printf("Simultaneous processes: %d \n",s);
         it = atoi(argv[6]); //int t stores how long processes are allowed to run
         printf("Time between each process launch: %d \n",t);
         t = 0;
         //it = atoi(argv[8]); //int it stores a number that will be used to keep track of when to launch each process
         //printf("Interval between each process: %d \n",it);

         totalLaunched = 0; //keeps track of total processes
         currentProc = 0; //keeps track of active processes

         pid_t pid = getpid();
         add(pTable, n, pid, currentSec, currentNano);
         printPCB(pTable,n);

         int shmid =shmget(SHMKEY, BUFF_SZ, 0777 | IPC_CREAT);
         if (shmid ==-1){
                 printf("OSS:error with shmget\n");
                 exit(1);
         }

         int *pint = (int*)(shmat(shmid,0,0));
         int *nint = pint + 1;

         remove("LogFile.log");

         FILE *log;
         log = fopen("LogFile.log", "a");
         if (log == NULL){
                printf("Error creating log file");
                exit(1);
         }

         while (totalLaunched < n) {
          if (currentProc < s) {
           childPid = fork();
           currentProc++;
           //sleep(it);
           if (childPid == 0) {
            char* args [] = {"./worker", argv[2]};
            execlp(args[0],args[1]);
           } else {
            child[totalLaunched] = childPid;
            totalLaunched++;
            //printf("Occupied\tPID\tSeconds\tNanoseconds\n");
            //printPCB(pTable, n);
           }
          } else {
           for (int i=0;i<totalLaunched;i++) {
            currentProc--;
            break;
           }
          }
         }

        for (int y=0;y<n;y++) {
         buf1.mtype = child[y];
         buf1.intData = child[y];
         strcpy(buf1.strData,"begin");
         fprintf(log,"OSS: Generating process %d with PID %d and putting it into ready queue at time %d:%d\n",y,child[y],*pint,*nint);
         if (msgsnd(msqid, &buf1, sizeof(msgbuffer)-sizeof(long), 0) == -1) {
                perror("msgsend to worker failed\n");
                exit(1);
         }
         fprintf(log,"OSS: Dispatching process %d with PID %d from ready queue at time %d:%d\n",y,child[y],*pint,*nint);

         fprintf(log,"OSS: Receiving process %d with PID %d at time %d:%d\n",y,child[y],*pint,*nint);
         msgbuffer rcvbuf;

         /*if (msgrcv(msqid, &rcvbuf, sizeof(msgbuffer), getpid(),0) == -1) {
                perror("failed to receive message in oss\n");
                exit(1);

         }*/
         //int toend;
         sleep(it);
         printf("Oss received message from worker: Done %s\n", rcvbuf.strData);

         /*for (int h=0;h<65;h++) {
                if (msgrcv(msqid, &rcvbuf, sizeof(msgbuffer), getpid(), 0) == -1) {
                   perror ("failed to receive message from child\n");
                   exit(1);
                }
                toend = strcmp(rcvbuf.strData,"done");
                if (toend == 0) {
                   printf("Parent received message, moving to next child.");
                   break;
                }
                if (h=60) {
                   exit(1);
                }

         }*/

         //printf("Parent received message:%s\n", rcvbuf.strData);
        }
        fclose(log);
        break;
    }
 }
 return 0;
}




  /*if (strcmp(argv[1], x) == 0) { //if -h is present, displays a help message only
    printf("-h test\n");
  } else if (strcmp(argv[1], y) == 0) {//if -h is not present, and -n is, the process continues as normal
    n = atoi(argv[2]); //int n sotres how many processes to run, based on user input
    printf("Processes to launch: %d \n",n);
    s = atoi(argv[4]); //int s stores how many processes are allowed to run at once, based on user input
    printf("Simultaneous processes: %d \n",s);
    t = atoi(argv[6]); //int t stores how long processes are allowed to run
    printf("Time limit for each process: %d \n",t);
    it = 0;
    it = atoi(argv[8]); //int it stores a number that will be used to keep track of when to launch each process
    printf("Interval between each process: %d \n",it);

    totalLaunched = 0; //keeps track of total processes
    currentProc = 0; //keeps track of active processes



    while (totalLaunched < n) {
      if (currentProc < s) {
        childPid = fork();
        currentProc++;
        if (childPid == 0) {
          oss();
          char* args [] = {"./worker", argv[2]};
          execlp(args[0],args[1]);
        } else {
          totalLaunched++;
        }
      } else {
        for (int i=0;i<totalLaunched;i++) {
          currentProc--;
          break;
        }
      }
    }
    oss();
  }

  return 0;
}*/



/*int forkProcess() {
  switch (fork()) {
    case -1:
        fprintf(stderr,"Failed to fork\n");
        return ( 1 );
    case 0:
        //worker();
        break;
    default:
        oss();
        break;
    }
  return 0;
}*/


/*void oss() {
  signal(SIGALRM, signal_handler);  // Turn on alarm handler
  alarm(60);  // set up alarm call
  int shmid = shmget(SHMKEY, BUFF_SZ, 0777 | IPC_CREAT);

  if ( shmid == -1 ) {
    fprintf(stderr,"Parent: ... Error in shmget ...\n");
    exit (1);
  }

  int *pint = (int *)(shmat (shmid, (void*)0, 0 ) );
  //int *nint = (int *)(shmat (nanoid, (void*)0, 0) );
  int *nint = pint + 1;
  for (int i=0;i<10;i++) {
    *pint = i;
    systemclock->seconds = 1*i;
    for (int x=0;x<10;x++) {
      usleep(100000);
      *nint = 100000*x;
      //printf("Time: %d : %d\n",*pint,*nint);
    }
    //printf("Seconds: %d Nanoseconds: %d\n",*pint,*nint);

  }

  shmdt(pint);
  shmctl(shmid,IPC_RMID,NULL);

}*/



void signal_handler(int sig) {
  // code to send kill signal to all children based on their PIDs in process table

  //free(processTable);
  for (int i=0;i<n;i++){
        if (pTable[i].pid==0){
                break;
        }
        kill(pTable[i].pid, SIGKILL);
  }

  exit(1);
}
                                                                 