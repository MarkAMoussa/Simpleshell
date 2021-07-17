#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
pid_t pid1;
/*handler for log file*/
void handler(int sig)
{

    FILE *fptr;
    fptr = fopen("logfile.txt", "a");
    fprintf(fptr, "Child process %d was terminated.\n", pid1);
    fclose(fptr);
}
int main()
{
    /*clears file data*/
    FILE *fptr;
    fptr = fopen("logfile.txt", "w");
    fclose(fptr);

	char input[20];
	char s[100];
	char temp[20];
	char temp1[20];
	while(1)
	{
		printf("%s: ", getcwd(s,100));
		fgets(input, 20, stdin);
		/*because using fgets() it takes the enter
          as a character so i remove it*/
		if(input[strlen(input) - 1] == '\n')
		{
			input[strlen(input) - 1] = '\0';
		}

		/*if the user didn't input any command force
		  next iteration*/
        if(strlen(input) == 0)
        {
            continue;
        }

		char space[] = " ";

		/*copied input in a temp so when i use
		  strtok() the input is still in place*/
		strcpy(temp, input);

		/*word counter*/
		int i = 0;

		/*flag for ampersand*/
		int ampersand = 0;

		/*initialized with NULL to run with execvp()*/
		char *parts[10] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

		/*parsing over spaces*/
		char *ptr = strtok(temp, space);
		while(ptr != NULL)
		{
			parts[i] = ptr;
			ptr = strtok(NULL, space);
			i++;
		}

        /*used another variable for the sake
          of simplicity*/
		strcpy(temp1, parts[i-1]);

        /*checking if last character is '&'
          if so remove it and set flag*/
		if(temp1[strlen(temp1) -1] == '&')
        {
            ampersand = 1;
            temp1[strlen(temp1) -1] = '\0';
            strcpy(parts[i-1], temp1);
        }
        else
        {
            ampersand = 0;
        }

        /*handling exit case*/
        if(strcmp(input, "exit") == 0)
        {
            exit(0);
        }

        if(strcmp(parts[0], "cd") == 0)
        {
            if(chdir(parts[1]) != 0)
            {
                printf("No such file or directory.\n");
            }
            continue;
        }

		int pid = fork();
		/*child executing the command because
          execvp() auto terminates*/
		if(pid == 0)
		{
            execvp(parts[0], parts);
        }

        /*parent waiting for child if there's
          '&' or just keeps on going*/
        else if(pid > 0)
        {
            /*writes terminated childs in log file*/
            signal(SIGCHLD, handler);
            if(ampersand == 0)
            {
                pid1 = wait(&pid);
            }
        }

        /*forking failed*/
        else
        {
            printf("Creating child failed!");
        }
	}
	return 0;
}

