//Rett Swyers - CS 4760

#include<unistd.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<string.h>

#define SHMKEY 44197
#define BUFF_SZ sizeof (int)
#define PERMS 0771

typedef struct msgbuffer {
        long mtype;
        char strData[100];
        int intData;
} msgbuffer;

int main(int argc, char** argv) {

    //int s = atoi(argv[1]); //this is an integer to store the amount of seconds
    //int n = atoi(argv[2]); //this is an integer to store the amount of nanoseconds

    int shmid = shmget(SHMKEY, BUFF_SZ, 0777 | IPC_CREAT);
    if (shmid == -1) {
      printf(stderr,"Worker: Error with shmget\n");
      exit(1);
    }

    srand(getpid()); // random seed for the next two integers

    int s = rand() % 16;  //the child should only be active between 1 and 15 seconds
    int n = rand() % 1000000; //this should add a random amount of nanoseconds between 1 and 999999

    int *cint = (int*)(shmat(shmid,0,0));
    int *xint = cint + 1;
    s += *cint;
    n += *xint;

    msgbuffer buf;
    buf.mtype = 1;
    int msqid = 0;
    key_t key;

    if ((key = ftok("msgq.txt", 1)) == -1) {
            perror("ftok");
            exit(1);
    }
    if ((msqid = msgget (key, PERMS)) == -1) {
            perror("msgget in child");
            exit(1);
    }


    //printf("Child has access to the queue\n", getpid());
    int toend;
    for(int i=0;i<65;i++) {
        if (msgrcv(msqid, &buf, sizeof(msgbuffer), getpid(), 0) == 1) {
            perror ("failed to receive message from oss\n");
            exit(1);
        }
        toend = strcmp(buf.strData,"begin");
        if (toend == 0) {
                break;
        }
        if (i=60) {
                exit(1);
        }
    }
    printf("Worker %d received message, starting clock.\n", getpid());
    printf("Occupied\tPID\t\tSeconds\tNanoseconds\n");
    for (int i=0;i<s;i++) {
      sleep(1);
      printf("\t1\t%4d\t%8d\t%d\n", getpid(), *cint, *xint);
    //SysClockS: %d SysClockNano: %d TermTimeS: %d TermTimeNano: %d - %d have passed since starting
      //printf("WORKER PID:%d PPID:%d SysClockS:%d SysClockNano:%d TermTimeS:%d TermTimeNano:%d\n", getpid(),getppid(),*cint,*xint,s,n);
    }

    buf.mtype =getppid();
    buf.intData =getppid();
    strcpy(buf.strData, "worker done");

    if (msgsnd(msqid,&buf,sizeof(msgbuffer)-sizeof(long),0)==-1){
        perror("msgsnd to oss failed\n");
        exit(1);
    }

    shmdt(cint);
    shmdt(xint);
    return EXIT_SUCCESS;
}
                                                        