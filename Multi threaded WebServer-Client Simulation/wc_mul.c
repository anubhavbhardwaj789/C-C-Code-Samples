/*************************************************
	* C program to count no of lines, words and 	 *
	* characters in a file.			 *
	*************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#define FILEPATH "/home/arkh/Desktop/OS/books"

typedef struct count_t {
		int linecount;
		int wordcount;
		int charcount;
} count_t;

count_t word_count(char *file)
{
		FILE *fp;
		char ch;

		count_t count;
		// Initialize counter variables
		count.linecount = 0;
		count.wordcount = 0;
		count.charcount = 0;

		// Open file in read-only mode
		fp = fopen(file, "r");

		// If file opened successfully, then write the string to file
		if ( fp )
		{
				//Repeat until End Of File character is reached.	
				while ((ch=getc(fp)) != EOF) {
						// Increment character count if NOT new line or space
						if (ch != ' ' && ch != '\n') { ++count.charcount; }

						// Increment word count if new line or space character
						if (ch == ' ' || ch == '\n') { ++count.wordcount; }

						// Increment line count if new line character
						if (ch == '\n') { ++count.linecount; }
				}

				if (count.charcount > 0) {
						++count.linecount;
						++count.wordcount;
				}

				fclose(fp);
		}
		else
		{
				printf("Failed to open the file: %s\n", file);
		}
		
		return count;
}

#define MAX_PROC 100

int main(int argc, char **argv)
{
		int i,j, numFiles,pid_count[10],pid;
		
		count_t count;

		if(argc < 2) {
				printf("usage: wc <# of files to count(1-10)>\n");
				return 0;
		}

		numFiles = atoi(argv[1]);
		if(numFiles <= 0 || numFiles > 10) {
				printf("usage: wc <# of files to count(1-10)>\n");
				return 0;
		}

		count.linecount = 0;
		count.wordcount = 0;
		count.charcount = 0;
		
		int pipefd[2];
		char buf[256],input[256];
		pipe(pipefd);
		
		
		printf("counting %d files..\n", numFiles);
		j=0;
		for(i = 0; i < numFiles; i++)
		{
				
				
				pid_count[i] = fork();
				
				    if(pid_count[i] < 0) {
					printf("Error creating the child process\n");
					
				    } else if (pid_count[i] == 0) {
					
					char filename[100];
					sprintf(filename, "%s/text.%02d", FILEPATH, i);
					printf("read: %s\n", filename);
					count_t tmp;
					printf("Child PID: %d Handling File No : %d\n",getpid(), i);
					tmp = word_count(filename);
					
					count.charcount += tmp.charcount;
					count.linecount += tmp.linecount;
					count.wordcount += tmp.wordcount;
					printf("CHILD: %d FILE %d Character count :%d Line Count :%d Word Count :%d \n",getpid(),i,tmp.charcount,tmp.linecount,tmp.wordcount);
					
					exit(0);
					
				    } else  {
					continue;
				    }
				

				
		}
		int wstatus;
		
		for(i = 0; i < numFiles; i++)
		{
				waitpid(pid_count[i], &wstatus, 0);
				
		}
		

			printf("\n=========================================\n");
			printf("Total Lines : %d \n", count.linecount);
			printf("Total Words : %d \n", count.wordcount);
			printf("Total Characters : %d \n", count.charcount);
			printf("=========================================\n");
		
		return(0);
}
