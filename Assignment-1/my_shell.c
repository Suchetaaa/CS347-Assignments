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

pid_t bg_child_pid;
pid_t fg_group;
//bool fg_group_set = 0;
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


void check_and(char **Tokens, int &bg, int &sr, int &pr)
{
	int i = 0;
	while(Tokens[i] != NULL)
	{
		if (strcmp(Tokens[i], "&") == 0) bg = 1;
		if (strcmp(Tokens[i], "&&") == 0) sr = 1;
		if (strcmp(Tokens[i], "&&&") == 0) pr = 1;
		i = i + 1;
	}
}

void sigint_handler(int num)
{
	signal(SIGINT, sigint_handler);
	//kill(fg_group, SIGTERM);
	//printf("%s\n", "Yay, shell is still alive" );
}

int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i;

	signal(SIGINT, sigint_handler);
	kill(fg_group, SIGINT);

	FILE* fp;
	if(argc == 2) {
		fp = fopen(argv[1],"r");
		if(fp != 0) {
			printf("File doesn't exists.");
			return -1;
		}
	}

	while(1) {	
		
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
		int j = 0, k = 0, a, status;
		int bg, sr, pr;
		
		check_and(tokens, bg, sr, pr);
		if (strcmp(tokens[0],"exit") == 0) exit(EXIT_SUCCESS);

		else if ( (pr == 1 && (sr == 1 || bg == 1)) || (sr == 1 && (pr == 1 || bg == 1)) || (bg == 1 && (sr == 1 || pr == 1)) )
			printf("%s\n","Shell: Incorrect command" );

		else if (pr != 1 && bg != 1 && sr != 1) 
		{
			if (strcmp(tokens[0], "cd") == 0)
			{
				if (tokens[1] == NULL) 
					{
						printf("%s\n","Shell: Incorrect command" );
						break;
					}
				int check;

				check = chdir(tokens[1]);
				if (check == -1) printf("%s\n", "Shell: Incorrect command" );
			}
			else 
			{
				pid_t child_pid;
				child_pid = fork();
				if (child_pid == 0)
				{
					int check;
					//printf("%s\n","h" );
					// kill(0, SIGINT);
					//printf("%d\n",fg_group_set );
					
					fg_group = child_pid;
					// printf("%d\n",getpid() );
					// printf("%d\n",fg_group );
					// printf("%d\n",fg_group_set );
					check = execvp(tokens[0], tokens);
					//printf("%d\n", check );
					if (check == -1) printf("%s\n", "Shell:Incorrect command" );
					exit(0);
				}
				else if (child_pid < 0) printf("%s\n", "Fork failed" );
				else if (child_pid > 0) wait(NULL);
			}
		}

		else if (sr)
		{
			//printf("%s\n", "Oye" );
			int i = 0;
			while(tokens[i] != NULL)
			{
				if (strcmp(tokens[i], "&&") == 0) tokens[i] = NULL;
				i = i + 1;
			}

			i = 0;
			while(tokens[0] != NULL)
			{
				if (strcmp(tokens[0], "cd") == 0)
				{
					int check;
					if (tokens[1] == NULL) 
						{
							printf("%s\n", "Shell: Incorrect command" );
						}
					else 
					{
						check = chdir(tokens[1]);
						if (check == -1) printf("%s\n", "Shell: Incorrect command" );
					}
				}

				else
				{
					pid_t child_pid;
					child_pid = fork();
					if (child_pid == 0)
					{
						int check;
						//printf("%s\n","h" );
						// kill(0, SIGINT);
						fg_group = child_pid;
						check = execvp(tokens[0], tokens);
						//printf("%d\n", check );
						if (check != 0) printf("%s\n", "Shell:Incorrect command" );
					}
					else if (child_pid < 0) printf("%s\n", "Fork failed" );
					else if (child_pid > 0) wait(NULL);
				}

				int k = 0;

				while(tokens[k] != NULL) 
					{
						free(tokens[k]);
						k = k + 1;
					}
				i = k + 1;
				//printf("%d\n",i );
				tokens = &tokens[i];
			}

		}

		else if (pr)
		{
			//printf("%s\n", "Oye" );
			int i = 0, counter = 0;
			while(tokens[i] != NULL)
			{
				if (strcmp(tokens[i], "&&&") == 0) tokens[i] = NULL;
				i = i + 1;
			}

			i = 0;
			while(tokens[0] != NULL)
			{
				if (strcmp(tokens[0], "cd") == 0)
				{
					int check;
					if (tokens[1] == NULL) 
						{
							printf("%s\n", "Shell: Incorrect command" );
						}
					else 
					{
						check = chdir(tokens[1]);
						if (check == -1) printf("%s\n", "Shell: Incorrect command" );
					}

				}

				else
				{
					pid_t child_pid;
					child_pid = fork();
					
					if (child_pid == 0)
					{
						int check;
						//printf("%s\n","h" );
						// kill(0, SIGINT);
						fg_group = child_pid;
						check = execvp(tokens[0], tokens);
						//printf("%d\n", check );
						if (check != 0) printf("%s\n", "Shell:Incorrect command" );
					}
					else if (child_pid < 0) printf("%s\n", "Fork failed" );
					else if (child_pid > 0) counter = counter + 1;;
				}

				int k = 0;

				while(tokens[k] != NULL) 
					{
						free(tokens[k]);
						k = k + 1;
					}
				i = k + 1;
				//printf("%d\n",i );
				tokens = &tokens[i];
			}

			for (int i = 0; i < counter; i++) wait(NULL);

		}

		else 
		{
			int i = 0;
			while (strcmp(tokens[i],"&") != 0) 
			{
				i = i + 1;
			}
			tokens[i] = NULL;
			
			if (strcmp(tokens[0], "cd") == 0) 
			{
				if (chdir(tokens[1]) == -1) printf("%s\n","Shell: Incorrect command" );
			}
			else
			{
				// printf("%s\n","Here" );
				bg_child_pid = fork();
				if (bg_child_pid == 0)
				{
					int check;
					check = execvp(tokens[0], tokens);
					if (check == -1) printf("%s\n","Shell: Incorrect command" );

				}
				else if (bg_child_pid == -1) printf("%s\n", "Fork failed");
				else if (bg_child_pid > 0) bg = 0;
			}
		}
		bg = 0;
		pr = 0;
		sr = 0;

		a = waitpid(-1, &status, WNOHANG);
		//printf("%d\n",a );
		if (a > 0) printf("%s\n","Shell: Background process finished" );


		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}


	}

	return 0;
}