#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <string.h>
int exitStatus = 0; // final exit status of program
//Struct that keeps track of where commands
//are in the ordered argv array
struct simpCommand{
  pid_t commandPid;
  int startIndex;
  int endIndex;
};
//checks for a FD in the FDTABLE
int checkFD(int FDTABLECOPY[],int target);

//Runs the command in a child process using exec
pid_t runCommand(struct simpCommand commands[], char** ordered,int i,int FDTABLECOPY[],int *exits); 
void segFaultCreator();
void signalHandler();
int main(int argc, char *argv[]){
  int signalFlag = 0;
  int commandPID;
  int pipeReader[2] = {0,0};
  int flagBits = 0;
  int argCount; // checks to make sure --command has enough args
  char* orderedArgv[argc];
  int argCounter = 1; 
  int verboseFlag = 0;
  char c;
  int signalArg;
  struct simpCommand commandArr[200]; // store commands
  int simpCommandCount = 0; // how many commands have come in
  int numFD = 0; // number of fds currently open
  int FDTABLE[200]; // array for storing opened fds
  int loopCounter;
  for(loopCounter = 0; loopCounter < 200; loopCounter++){
    FDTABLE[loopCounter] = -1;
  }
  struct option long_options[] =
    {
      {"rdonly", required_argument, NULL, 'a'}, // *
      {"wronly", required_argument, NULL, 'b'}, // *
      {"command", required_argument, NULL, 'c'}, // *
      {"verbose", no_argument, NULL, 'd'}, // *
      {"append", no_argument, NULL, 'e'}, // *
      {"cloexec", no_argument, NULL, 'f'}, // *
      {"creat", no_argument, NULL, 'g'}, // *
      {"directory", no_argument, NULL, 'h'}, // *
      {"dsync", no_argument, NULL, 'i'}, // *
      {"excl", no_argument, NULL, 'j'}, // * 
      {"nofollow", no_argument, NULL, 'k'}, // *
      {"nonblock", no_argument, NULL, 'l'}, // *
      {"rsync", no_argument, NULL, 'm'}, // *
      {"sync", no_argument, NULL, 'n'}, // *
      {"trunc", no_argument, NULL, 'o'}, // *
      {"rdwr", required_argument, NULL, 'p'}, // *
      {"pipe", no_argument, NULL, 'q'}, //*
      {"wait", no_argument, NULL, 'r'},//*
      {"close", required_argument, NULL, 's'},
      {"abort", no_argument, NULL, 't'}, // *
      {"catch", required_argument, NULL, 'u'}, // *
      {"ignore", required_argument, NULL, 'v'}, // *
      {"default", required_argument, NULL, 'w'},// *
      {"pause", no_argument, NULL, 'x'},// *
      {"chdir", required_argument, NULL, 'y'},// * 
      {0,0,0,0}
    };

  while((c = getopt_long(argc, argv,"",long_options,NULL)) != -1){
    int childStatus;
    pid_t waitReturn;
    int loopCnt;
    int y;
    switch(c){
      int fd;
    case 0:
      break;
      
//RDONLY
    case 'a':
      if (verboseFlag){
	printf("--rdonly %s\n",optarg);
      }
      fflush( stdout );
      fd = open(optarg,O_RDONLY | flagBits,0666);
      if (fd < 0){
        fprintf(stderr,"Error opening file: %s, exiting with status 1\n", strerror(errno));
        if (exitStatus < 1){
          exitStatus = 1;
        }
      }
      FDTABLE[numFD] = fd;
      numFD++;
      flagBits = 0;
      break;

//WRONLY
    case 'b':
      if (verboseFlag){
        printf("--wronly %s\n",optarg);
      }
      fflush( stdout );
      fd = open(optarg,O_WRONLY | flagBits,0666);
      if (fd < 0){
        fprintf(stderr,"Error opening file: %s, exiting with status 1\n", strerror(errno));
        if (exitStatus < 1){
          exitStatus = 1;
        }
      }
      FDTABLE[numFD] = fd;
      numFD++;
      flagBits = 0;
      break;
    
//COMMAND
    case 'c':
      argCount = 0;
      if (verboseFlag){
        printf("--command");
      }
      commandArr[simpCommandCount].startIndex = argCounter;
      orderedArgv[argCounter] ="--command";
      argCounter++;
      argCount++;
      orderedArgv[argCounter] = optarg;
      if(verboseFlag){
	printf(" %s",optarg);
      }
      argCounter++;
      while (optind < argc){
	if(argv[optind][0] == '-' && argv[optind][1] == '-'){
	  break;
	}
	orderedArgv[argCounter] = argv[optind];
	if(verboseFlag){
	  printf(" %s",argv[optind]);
	}
	argCounter++;
        optind++;
	argCount++;
      }
      if(verboseFlag){
	printf("\n");
      }
      fflush( stdout );
      commandArr[simpCommandCount].endIndex = argCounter;
      simpCommandCount++;
      //checking if enough args were passed to command
      if (argCount < 4){
      	fprintf(stderr,"Error exiting with status 1\n");
        if (exitStatus < 1){
          exitStatus = 1;
        }
      }
      commandPID = runCommand(commandArr, orderedArgv,simpCommandCount - 1,FDTABLE,&exitStatus);
      commandArr[simpCommandCount - 1].commandPid = commandPID;


      break;
   
//VERBOSE
      case 'd':
      verboseFlag = 1;
      break;
      
//RDWR
      case 'p':
	if (verboseFlag){
	  printf("--rdwr %s\n",optarg);
	}
	fflush( stdout );
	fd = open(optarg,O_RDWR | flagBits,0666);
	if (fd < 0){
	  fprintf(stderr,"Error opening file: %s, exiting with status 1\n", strerror(errno));
	  if (exitStatus < 1){
	    exitStatus = 1;
	  }
	}
	FDTABLE[numFD] = fd;
	numFD++;
	flagBits = 0;
	break;

//FLAGS
    case 'e':
      if (verboseFlag){
	printf("--append \n");
      }
      flagBits |= O_APPEND;
      break;
    case 'f':
      if (verboseFlag){
        printf("--cloexec \n");
      }
      flagBits |= O_CLOEXEC;
      break;
    case 'g':
      if (verboseFlag){
        printf("--creat \n");
      }
      flagBits |= O_CREAT;
      break;
    case 'h':
      if (verboseFlag){
        printf("--directory \n");
      }
      flagBits |= O_DIRECTORY;
      break;
    case 'i':
      if (verboseFlag){
        printf("--dsync \n");
      }
      flagBits |= O_DSYNC;
      break;
    case 'j':
      if (verboseFlag){
        printf("--excl \n");
      }
      flagBits |= O_EXCL;
      break;
    case 'k':
      if (verboseFlag){
        printf("--nofollow \n");
      }
      flagBits |= O_NOFOLLOW;
      break;
    case 'l':
      if (verboseFlag){
        printf("--nonblock \n");
      }
      flagBits |= O_NONBLOCK;
      break;
    case 'm':
      if (verboseFlag){
        printf("--rsync \n");
      }
      flagBits |= O_RSYNC;
      break;
    case 'n':
      if (verboseFlag){
        printf("--sync \n");
      }
      flagBits |= O_SYNC;
      break;
    case 'o':
      if (verboseFlag){
        printf("--trunc \n");
      }
      flagBits |= O_TRUNC;
      break;
     
//abort
    case 't':
      if (verboseFlag){
        printf("--abort\n");
      }
      fflush( stdout );
      segFaultCreator();
      break;

//catch
    case 'u':
      if (verboseFlag){
        printf("--catch %s\n",optarg);
      }
      fflush( stdout );
      signalArg = atoi(optarg);
      signal(signalArg,signalHandler);
      if (signalArg > exitStatus){
	exitStatus = signalArg;
      }
      break;
     
 //default
    case 'w':
      if (verboseFlag){
        printf("--default %s\n",optarg);
      }
      fflush( stdout );
      signalArg = atoi(optarg);
      signal(signalArg,SIG_DFL);
      break;

//pause
    case 'x':
      if (verboseFlag){
        printf("--pause\n");
      }
      fflush( stdout );
      pause();
      break;

//ignore
    case 'v':
      if (verboseFlag){
        printf("--default %s\n",optarg);
      }
      fflush( stdout );
      signalArg = atoi(optarg);
      signal(signalArg,SIG_IGN);
      break;

//pipe
    case 'q':
      if (verboseFlag){
        printf("--pipe\n");
      }
      fflush( stdout );
      if (pipe(pipeReader) < 0){
	fprintf(stderr,"Piping error: %s, exiting with status 1\n", strerror(errno));
	if (exitStatus < 1){
	  exitStatus = 1;
	}
      }
      FDTABLE[numFD] = pipeReader[0];
      numFD++;
      FDTABLE[numFD] = pipeReader[1];
      numFD++;
      break;

//close
    case 's':
      if (verboseFlag){
        printf("--close %s\n",optarg);
      }
      fflush( stdout );
      close(FDTABLE[atoi(optarg)]);
      FDTABLE[atoi(optarg)] = -1;
      break;

//wait
    case 'r':
      if (verboseFlag){
        printf("--wait\n");
      }
      fflush( stdout );
     
      // waiting
     
      while(1){
	waitReturn = waitpid(-1,&childStatus,0);
	if (waitReturn == -1){ //no more children to wait on
	  break;
	}

	if (WIFEXITED(childStatus)){
	  childStatus = WEXITSTATUS(childStatus);
	  printf("exit %d",childStatus);
	  if (childStatus > exitStatus && signalFlag == 0){
	    exitStatus = childStatus;
	  }
	}
	else if (WIFSIGNALED(childStatus)){
	  if (signalFlag == 0){
	    signalFlag = 1;
	    exitStatus = WTERMSIG(childStatus);
	  }
      	  childStatus = WTERMSIG(childStatus);
	  printf("signal %d",WTERMSIG(childStatus));
	  if ((childStatus) > exitStatus){
            exitStatus = childStatus;
          }
	}
	else if (WIFSTOPPED(childStatus)){
	  childStatus = WIFSTOPPED(childStatus);
	  if (childStatus > exitStatus){
            exitStatus = childStatus;
          } 
	}
	if (childStatus == 0){
	  printf("exit %d",childStatus);
	}
	for (loopCnt = 0; loopCnt < simpCommandCount; loopCnt++){// loop through all commands
	  if (commandArr[loopCnt].commandPid == waitReturn){//if command[li] == child exiting
	    for (y = commandArr[loopCnt].startIndex + 4; y < commandArr[loopCnt].endIndex; y++) {
	      printf(" %s",orderedArgv[y]);
	      }
	    printf("\n");
	  }
	}
      }
      //end waiting
       break;
    case 'y':
      if (verboseFlag){
        printf("--chdir %s\n",optarg);
      }
      fflush( stdout );
      fd = chdir(optarg);
      if (fd != 0){
	fprintf(stderr,"Error opening file: %s, exiting with status 1\n", strerror(errno));
	if (exitStatus < 1){
	  exitStatus = 1;
	}
      }
      break;
    case '?':
      if (exitStatus < 1){
	exitStatus = 1;
      }
      break;
    default:
      fprintf(stderr, "getopt");
    }
  }
  //END ARG PARSING WHILE LOOP
 

  if(signalFlag){
    fflush(stdout);
    signal(exitStatus,SIG_DFL);
    raise(exitStatus);
  }
  fflush(stdout);
    exit(exitStatus);
}







pid_t runCommand(struct simpCommand commands[], char** ordered,int i,int FDTABLECOPY[],int *exits){
  int numArgs = commands[i].endIndex - commands[i].startIndex;
  char* args[numArgs];
  int x;
  int argCount = 0;
  int input;
  int output;
  int errorOut;
  int loopCounter;
  pid_t toReturn;
  for (x = commands[i].startIndex; x < commands[i].endIndex; x++){
    args[argCount] = ordered[x];
    argCount++;
  }
  args[argCount] = NULL;

  char* commandArgs[numArgs - 3];
  argCount = 0;
  for (x = commands[i].startIndex + 4; x < commands[i].endIndex; x++){
    commandArgs[argCount] = ordered[x];
    argCount++;
  }
  commandArgs[argCount] = NULL;

  //Check if is in FDTABLE                                                                                                                                      
  int fd;
  int goodFD = 1;
  
  for (fd = 1; fd < 4; fd++){
    int newFD = atoi(args[fd]);
    if (checkFD(FDTABLECOPY,newFD + 3) == 0){
      fprintf(stderr,"Bad file descriptor exiting with status 1\n");
      if(*exits < 1){
	*exits = 1;
	goodFD = 0;
	return -1;
      }
    }
  }

  //FORKING
  if(goodFD == 1){
    input = atoi(args[1]);
    output = atoi(args[2]);
    errorOut = atoi(args[3]);
    pid_t pid = fork();
    toReturn = pid;
    if (pid < 0){
      fprintf(stderr,"Forking error: %s, exiting with status 1\n", strerror(errno));
      if (*exits < 1){
	*exits = 1;
	return -1;
      }
    }
    //CHILD PROCESS
    else if (pid == 0){
      dup2(FDTABLECOPY[input],0);
      dup2(FDTABLECOPY[output],1);
      dup2(FDTABLECOPY[errorOut],2);
      //close all FD's
      for(loopCounter = 0; loopCounter < 200; loopCounter++){
	if (FDTABLECOPY[loopCounter] != -1){
	  close(FDTABLECOPY[loopCounter]);
	}
      }
      execvp(commandArgs[0],commandArgs);
      //exec failed if here
      if (*exits < 1)
	*exits = 1;
      return -1;
    }
  }
   
  return toReturn;
}

int checkFD(int FDTABLECOPY[],int target){
int i;
int found = 0;
for(i = 0; i < 200; i++){
  if (FDTABLECOPY[i] == target){
    found = 1;
  }
 }
return found;
}


void segFaultCreator(){
  char* ptr = NULL;
  *ptr = 'a';
}

void signalHandler(){
  fprintf(stderr,"Error, %d caught\n",exitStatus);
  exit(exitStatus);
}
