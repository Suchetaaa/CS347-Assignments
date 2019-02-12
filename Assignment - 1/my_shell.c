#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

void my_shell_cd(char **Tokens);
int my_shell_others(char **Tokens);

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

void my_shell_cd(char **Tokens)
{
	
	//printf("%s\n", "Reached checkpoint 1");
	if (Tokens[1] == NULL) 
	{
		perror("Argument not specified");
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
}

int my_shell_others(char **Tokens)
{
	pid_t check;
	check = fork();

	if (check == 0) 
	{
		int check_exec;
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

	return 0;
}


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
   
       //do whatever you want with the commands, here we just print them

		if (strcmp(tokens[0], "cd") == 0) my_shell_cd(tokens);
		else if (strcmp(tokens[0], "ls") == 0) my_shell_others(tokens);
		else if (strcmp(tokens[0], "cat") == 0) my_shell_others(tokens);
		else if (strcmp(tokens[0], "sleep") == 0) my_shell_others(tokens);
		else if (strcmp(tokens[0], "echo") == 0) my_shell_others(tokens);
		else 
		{
			printf("%s\n", "Command not found" );
		}



		// for(i=0;tokens[i]!=NULL;i++){
		// 	printf("found token %s\n", tokens[i]);
		// }
       
		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);

	}
	return 0;
}