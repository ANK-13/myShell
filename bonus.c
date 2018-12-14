#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

// Node for storing job id in Linked list
struct jobs{
	int num;
	pid_t pid;
	int state;
	struct jobs *next;
};

static int count = 0;
struct jobs *head = NULL;


// This fuction adds new Job node to joblist. 
// It also assigns number to process
void addJob(pid_t pid)
{	
	struct jobs* tmp = (struct jobs*)malloc(sizeof(struct jobs));

	tmp->pid = pid;
	tmp->state = 1;	
	tmp->next = NULL;

	if(head == NULL)
	{
		head = tmp;
		tmp->num = 1;
	}
	else
	{
		int num = 2;
		struct jobs* counter = head;
		while(counter->next !=NULL)
		{
			num++;
			counter = counter->next;
		}
		counter->next = tmp;
		tmp->num = num;
	}
	printf("[%d] %d\n",tmp->num,pid);
return;
}

// This is custom funtion is used to store output of any command 
// char* cmd in the char* var. It is usefull to get process current
// state
void command(const char* cmd,char* var)
{
	FILE *fp,*outputfile;

  	fp = popen(cmd, "r");
  	while (fgets(var, sizeof(var), fp) != NULL) ;
  	pclose(fp);
}


// This funtion displays the process id that are running in background
// It parses the linkedList that holds all the backgraound process
void displayJobs()
{
	struct jobs* counter = head;
	if(counter != NULL)
	{
		while(counter!=NULL)
		{
			char cmd[40] = "\0";
			sprintf(cmd,"ps -o stat --no-headers %d",counter->pid);
			char state[5];
			command(cmd,state);
			printf("[%d] %d %s\n",counter->num,counter->pid,state);
			counter = counter->next;
		}
	}	
}


// Function: void parse(char *line, char **argv)
// Purpose : This function takes in a null terminated string pointed to by
//           <line>.  It also takes in an array of pointers to char <argv>.
//           When the function returns, the string pointed to by the
//           pointer <line> has ALL of its whitespace characters (space,
//           tab, and newline) turned into null characters ('\0').  The
//           array of pointers to chars will be modified so that the zeroth
//           slot will point to the first non-null character in the string
//           pointed to by <line>, the oneth slot will point to the second
//           non-null character in the string pointed to by <line>, and so
//           on. In other words, each subsequent pointer in argv will point
//           to each subsequent "token" (characters separated by white space)
//           IN the block of memory stored at the pointer <line>.  Since all
//           the white space is replaced by '\0', every one of these "tokens"
//           pointed to by subsequent entires of argv will be a valid string
//           The "last" entry in the argv array will be set to NULL.  This
//           will mark the end of the tokens in the string.
//

void  parse(char *line, char **argv)
{
     // We will assume that the input string is NULL terminated.  If it
     // is not, this code WILL break.  The rewriting of whitespace characters
     // and the updating of pointers in argv are interleaved.  Basically
     // we do a while loop that will go until we run out of characters in
     // the string (the outer while loop that goes until '\0').  Inside
     // that loop, we interleave between rewriting white space (space, tab,
     // and newline) with nulls ('\0') AND just skipping over non-whitespace.
     // Note that whenever we encounter a non-whitespace character, we record
     // that address in the array of address at argv and increment it.  When
     // we run out of tokens in the string, we make the last entry in the array
     // at argv NULL.  This marks the end of pointers to tokens.  Easy, right?

     while (*line != '\0') // outer loop.  keep going until the whole string is read
        { // keep moving forward the pointer into the input string until
          // we encounter a non-whitespace character.  While we're at it,
          // turn all those whitespace characters we're seeing into null chars.

          while (*line == ' ' || *line == '\t' || *line == '\n' || *line == '\r')
           { *line = '\0';
             line++;
           }

          // If I got this far, I MUST be looking at a non-whitespace character,
          // or, the beginning of a token.  So, let's record the address of this
          // beginning of token to the address I'm pointing at now. (Put it in *argv)
          // then we'll increment argv so that the next time I store an address, it
          // will be in the next slot of the array of integers.

          *argv++ = line;          /* save the argument position     */

          // Ok... now let's just keep incrementing the input line pointer until
          // I'm looking at whitespace again.  This "eats" the token I just found
          // and sets me up to look for the next.

          while (*line != '\0' && *line != ' ' &&
                 *line != '\t' && *line != '\n' && *line !='\r')
               line++;             /* skip the argument until ...    */
        }

     // Heh, I ran out of characters in the input string.  I guess I'm out of tokens.
     // So, whatever slot of the array at argv I'm pointing at?  Yeah, put a NULL
     // there so we can mark the end of entries in the table.

     *argv = NULL;                 /* mark the end of argument list  */
}
/*
void SIGCHLDHandler(int num)
{ 
 pid_t pid = wait(NULL);
 
 printf("Pid %d exited.\n", pid);
}
*/

// Function: void execute(char **argv)
// Purpose : This fuction creates a child process and executes
//  the command specified in the form of seperated tokens. The 
//  function used execvp to execute commands at backend. The  
//  function forks a child process and waits for its execution.  
//  We use wait to wait for child to complete and show output.
// It has additional feature by which we can run process in
// background. Along with the command mention '&' character 
// a additional arguments. This funtion first checks the last 
// character if it is found to be & it forks the process and 
// sets its group id different so that it does not have impact
// of inputs in the working shell. When process in running in 
// background parent does not wait for its completion.
//

void execute(char **argv)
{
    pid_t pid;
    int countArg = 0;
    char** tmp = argv;
    while(*argv !=NULL)
    {
            *argv++;
            countArg++;
    }
    argv = tmp;
    //printf("%s",argv[countArg-1]);

    int flg = 0;
    if(strcmp(argv[countArg-1], "&") == 0)
    {
        //printf("Running in Background\n");
        flg = 1;
        argv[countArg-1] = NULL;
    }

    if((pid=fork()) < 0)
    {
        printf("Could not create child process");
        exit(1);
    }
    else if(pid == 0)
    {
	setpgid(0,0);  //Changinf the group Id so that shell input does not affect it
        if (execvp(*argv, argv) < 0)
        {
            printf("Could not execute command\n");
            exit(2);
        }
    }
    else
    {
        if(flg == 1)
        {
            // Parent is not waiting for background process

	    addJob(pid); //addJob add job to Linked List
	    // signal(SIGCHLD,SIGCHLDHandler);
            flg=0;
            return;
        }
        else
        {
            int status;
            waitpid(pid, &status,0);
        }
    }

}


void SIGINTHandler(int n)
{
    signal(SIGINT, SIGINTHandler);
    printf("\n Cannot be terminated using Ctrl+C \nPress Enter to continue");
    fflush(stdout);
}


int main(int argc, char **argval, char **envp)
{
    // When Ctrl-C is entered SIGINT signal is generated. This signal in captured and
    // handled using function SIGINTHandler(int). 
     signal(SIGINT, SIGINTHandler);
     char  line[1024];   // This is the string buffer that will hold
                         // the string typed in by the user.  This
                         // string will be parsed.  The shell will do
                         // what it needs to do based on the tokens it
                         // finds.  Note that a user may NOT type in
                         // an input line of greater than 1024 characters
                         // because that's the size of the array.

     char  *argv[64];    // This is a pointer to an array of 64 pointers to
                         // char, or, an array of pointers to strings.
                         // after parsing, this array will hold pointers
                         // to memory INSIDE of the string pointed to by
                         // the pointer line.  argv[0] will be the string
                         // version of the first token inside of line...
                         // argv[1] will be the second... and so on...
                         // See the routine parse() for details.

     char shell_prompt[15]; // This string will hold the shell prompt string

     // set the default prompt
     strcpy(shell_prompt, "SillyShell");

     // The shell by default goes forever... so... while forever ;)

     while (1)
       {

          printf("%s> ",shell_prompt);  // display the shell prompt
          // When Ctrl-D is pressed, it returns EOF file thus the data in 
          // input stream is blank. Thus fgets get NULL pointer as a return
          // And once this situation is occured we exit form shell
          if(fgets(line, 1024, stdin) == NULL)
          {
                printf("Exiting from shell\n");
                exit(0);
          } // use the safe fgets() function to read
                                     // the user's command line.  Why wouldn't
                                     // we use gets() here?

          line[strlen(line)-1]='\0'; // This is a dirty hack.  Figure it out maybe?

          if (*line != '\0') // If something was actually typed, then do something...
            { // First, get all the addresses of all of the tokens inside the input line
              parse(line, argv);     //   parse the line to break it into token references

              // Check the first token to see if there are any built in commands
              // we want to handle directly.  Do this with an "if/then/else" ladder.
              // if we hit the end of the ladder, we assume the command line was requesting
              // an external program be run as a child process and do that....
                if (strcmp(argv[0] , "printenv") == 0)
                {
                    char* token;
                    while (*envp != NULL)
                    {
                        // If the command is printenv then instead of executing command
                    // *envp is checked and to print environment variables the value
                    // for the variables are removed using strtok() and output is printed
                        token = strtok(*envp, " =");
                        printf("%s\n", token);
                        token = strtok(NULL, "\n");
                        envp++;
                    }
                }
                else if ((strcmp(argv[0], "exit")      == 0))
                        exit(0);
                else if (strcmp(argv[0], "done")      == 0) exit(0); 
                else if (strcmp(argv[0], "newprompt") == 0) 
                { 
                    if (argv[1] != NULL)
                        strncpy(shell_prompt, argv[1], 15);
                    else
                        strncpy(shell_prompt, "SillyShell", 15);
                }
		        else if(strcmp(argv[0], "showjobs")== 0)
                { 
                    // This is custom command to introduce Job Control
                    // It can list Jobs and there current state
                    displayJobs(); 
                }
	            else
                    execute(argv);           /* otherwise, execute the command */
            }
     }
     return 0;
}
