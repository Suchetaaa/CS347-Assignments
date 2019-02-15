#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>


#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

int my_shell_cd(char **Tokens, int background);
int my_shell_ls(char **Tokens, int background);
int my_shell_others(char **Tokens, int background);

pid_t child_pid;
pid_t fg_group;
bool fg_group_set = 0;
pid_t bg_group; 

/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}

int my_shell_cd(char **Tokens, int background)
{
	
	//printf("%s\n", "Reached checkpoint 1");
	if (Tokens[1] == NULL) 
	{
		printf("%s\n", "Shell: Incorrect command");
	}
	else
	{
		int check;
		check = chdir(Tokens[1]);

		if (check == -1)
		{
			perror("Error");
		}
		else 
		{
			printf("%s\n", "Changed directory successfully" );
		}

	}
	return 0;
}

int my_shell_ls(char **Tokens, int background)
{
	bool stop = 0;
	if (Tokens[1] != NULL) 
		{
			stop = 1;
			printf("%s\n", "Shell: Incorrect command" );

		}

	if (!stop)
	{
		pid_t check;
		check = fork();

		if (check == 0) 
		{
			int check_exec;
			if (!fg_group_set) fg_group = setsid();
			if (fg_group_set) setpgid(getpid(), fg_group);
			check_exec = execvp(Tokens[0], Tokens);
			if (check_exec == -1) perror("Exec failed");
			exit(0);
		}

		if (check == -1)
		{
			perror("Fork failure");
			exit(EXIT_FAILURE);
		}

		else
		{
			wait(NULL);
		}
	}
	
	return 0;
}

int my_shell_others(char **Tokens, int background)
{
	bool stop = 0;
	if (Tokens[1] == NULL) 
		{
			stop = 1;
			printf("%s\n", "Shell: Incorrect command" );

		}

	if (!stop)
	{
		//printf("%s\n", "HI2" );
		pid_t check;
		check = fork();

		if (check == 0) 
		{
			int check_exec;
			if (!fg_group_set) fg_group = setsid();
			if (fg_group_set) setpgid(getpid(), fg_group);
			//printf("%s\n","HI3" );
			check_exec = execvp(Tokens[0], (Tokens));
			//printf("%d\n", check_exec );
			if (check_exec == -1) perror("Exec failed");
			exit(0);
		}

		if (check == -1)
		{
			perror("Fork failure");
			exit(EXIT_FAILURE);
		}

		else
		{
			wait(NULL);
			return 0;
		}
	}
	
	return 0;
}

void wait_bg(int signal)
{
	int status;
	waitpid(child_pid, &status, 0);
	printf("%s\n", "Shell: Background process has terminated" );
}

int commands_to_int(char *command)
{
	if (strcmp(command, "ls") == 0) return 0;
	if (strcmp(command, "cd") == 0) return 1;
	return 3;
}

typedef int (*f)(char **Tokens, int background);
f funcs[4] = {&my_shell_ls, &my_shell_cd, &my_shell_cd, &my_shell_others};

int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i;

	FILE* fp;
	if(argc == 2) {
		fp = fopen(argv[1],"r");
		if(fp != 0) {
			printf("File doesn't exists.");
			return -1;
		}
	}
	int background = 0;

	


	while(1) {	
		int status;
		
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		if(argc == 2) { // batch mode
			if(fgets(line, sizeof(line), fp) == NULL) { // file reading finished
				break;	
			}
			line[strlen(line) - 1] = '\0';
		} else { // interactive mode
			printf("$ ");
			scanf("%[^\n]", line);
			getchar();
		}

		
		//printf("Command entered: %s\n", line);

		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);

		// printf("%d\n", getpid() );
   
       //do whatever you want with the commands, here we just print them
		int j = 0, k = 0;
		if (
			( ( (strcmp(tokens[0], "cd") == 0) || (strcmp(tokens[0], "echo") == 0) || (strcmp(tokens[0], "cat") == 0) || (strcmp(tokens[0], "sleep") == 0) ) && 
				(strcmp(tokens[2], "&&&") == 0) ) 
			|| 
			( (strcmp(tokens[0], "ls") == 0) && (strcmp(tokens[1], "&&&") == 0) ) 
			)
		{
			// printf("%d\n", getpid() );

			while (tokens[0] != NULL)
			{
				// printf("%s\n", "Loop" );
				int identifier = commands_to_int(tokens[0]);
				tokens[0 + 1 + identifier%2] = NULL;
				// funcs[identifier](tokens, background);



				if (strcmp(tokens[0], "cd") == 0) 
				{
					if (chdir(tokens[1]) != 0) perror("Error");
					else printf("%s\n", "&&& cd success" );
				}

				else if (strcmp(tokens[0], "ls") == 0 || strcmp(tokens[0], "echo") == 0 || strcmp(tokens[0], "sleep") == 0 || strcmp(tokens[0], "cat") == 0)
				{
					printf("%s\n", "Reached here!" );

					if (fork() == 0)
					{
						if (!fg_group_set) fg_group = setsid();
						if (fg_group_set) setpgid(getpid(), fg_group);
						if (execvp(tokens[0], tokens) != 0) perror("Failure");
					}
				}

				j = 2 + identifier%2;
				// printf("%d\n", j );
				
				int i = 0;
				while(tokens[i] != NULL) 
					{
						free(tokens[i]);
						i = i + 1;
					}

			// 	printf("%s\n", "Bye" );
				
				tokens = &tokens[j];
				k = k + 1;
			}

			for (int i = 0; i < k; i++) wait(NULL);
			

		}

		
		else if (
			( ( (strcmp(tokens[0], "cd") == 0) || (strcmp(tokens[0], "echo") == 0) || (strcmp(tokens[0], "cat") == 0) || (strcmp(tokens[0], "sleep") == 0) ) && 
				(strcmp(tokens[2], "&&") == 0) ) 
			|| 
			( (strcmp(tokens[0], "ls") == 0) && (strcmp(tokens[1], "&&") == 0) ) 
			)
		{
			//printf("%s\n", "HI" );
			while (tokens[0] != NULL)
			{
				int identifier = commands_to_int(tokens[0]);
				tokens[0 + 1 + identifier%2] = NULL;
				funcs[identifier](tokens, background);
				//printf("%d\n", commands_to_int(tokens[j]) );
				j = 2 + identifier%2;
				int i = 0;
				while(tokens[i] != NULL) 
					{
						free(tokens[i]);
						i = i + 1;
					}
				
				tokens = &tokens[j];
				//printf("%s\n", tokens[1] );
				
				
				//printf("%s\n", tokens[j]);
				
			}	
		}

		else if (
			( ( (strcmp(tokens[0], "cd") == 0) || (strcmp(tokens[0], "echo") == 0) || (strcmp(tokens[0], "cat") == 0) || (strcmp(tokens[0], "sleep") == 0) ) && 
				(strcmp(tokens[2], "&") == 0) ) 
			|| 
			( (strcmp(tokens[0], "ls") == 0) && (strcmp(tokens[1], "&") == 0) ) 
			)
		{
			
			background = 1;
			
			int identifier = commands_to_int(tokens[0]);
			printf("%s\n", "Background mode");
			
			if (strcmp(tokens[0], "cd") == 0) 
				{
					if (chdir(tokens[1]) != 0) perror("Error");
					else printf("%s\n", "&&& cd success" );
				}

			else if (strcmp(tokens[0], "ls") == 0 || strcmp(tokens[0], "echo") == 0 || strcmp(tokens[0], "sleep") == 0 || strcmp(tokens[0], "cat") == 0)
			{
				printf("%s\n", "Reached here!" );

				int identifier = commands_to_int(tokens[0]);
				tokens[0 + 1 + identifier%2] = NULL;

				child_pid = fork();
				if (child_pid == 0)
				{
					// printf("%d\n", child_pid );
					// if (!fg_group_set) fg_group = setsid();
					// if (fg_group_set) setpgid(getpid(), fg_group);
					int check = execvp(tokens[0], tokens);
					if (check != 0) perror("Failure");
					//exit(0);
				}

				if (child_pid == -1) perror("Fork failed");

				else printf("%s\n", "Next" );;


			}

		}


		// if (background == 1)
		// {
		// 	printf("%s\n", "Terminate" );
		// 	printf("%d\n", child_pid );
		// 	waitpid(child_pid, &status, WNOHANG);
		// 	if (status) printf("%s\n","Shell: Background process has been terminated" );
		// }
		// printf("%s\n",tokens[0] );
		// else if (strcmp(tokens[0], "cd") == 0) my_shell_cd(tokens, background);
		// else if (strcmp(tokens[0], "ls") == 0) my_shell_ls(tokens, background);
		// else if (strcmp(tokens[0], "cat") == 0) my_shell_others(tokens, background);
		// else if (strcmp(tokens[0], "sleep") == 0) my_shell_others(tokens, background);
		// else if (strcmp(tokens[0], "echo") == 0) my_shell_others(tokens, background);
		else 
		{
			printf("%s\n", "Shell: Incorrect command" );
		}

		signal(SIGCHLD, wait_bg);


		// printf("%d\n", fg_group );
		// killpg(fg_group, SIGINT);

		




		// for(i=0;tokens[i]!=NULL;i++){
		// 	printf("found token %s\n", tokens[i]);
		// }
       
		// Freeing the allocated memory	
		// printf("%s\n", "Out" );
		printf("%d\n", status );
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		// free(tokens);

	}

	return 0;
}