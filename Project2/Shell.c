#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define MAX_LINE 80
#define BUFFER_SIZE 50

char buffer[BUFFER_SIZE];
char *history[10][10];
int nextPosition = 0;
int commandLength[10] = {0};
int success[10] = {1,1,1,1,1,1,1,1,1,1};
int flag = 0;

void setup(char inputBuffer[], char *args[], int *background){
	int length, i, start, cnt;
	cnt = 0;
	length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
	start = -1;
	
	if (length == 0){ // Ctrl+D
		printf("\n");
		exit(0);
	}
	if (length < 0){
		perror("error reading the command\n");
		exit(-1);
	}
	
	for (i = 0; i < length; ++i){
		switch (inputBuffer[i]){
			case ' ':
			case '\t':
				if (start != -1){
					args[cnt] = &inputBuffer[start];
					++cnt;
				}
				inputBuffer[i] = '\0';
				start = -1;
				break;
			case '\n':
				if (start != -1){
					args[cnt] = &inputBuffer[start];
					++cnt;
				}
				inputBuffer[i] = '\0';
				args[cnt] = NULL;
				break;
			case '&':
				*background = 1;
				inputBuffer[i] = '\0';
				break;
			default:
				if (start == -1)
					start = i;
		}
	}
	args[cnt] = NULL;
}

void handle_SIGINT(){
	int i, j, cnt;
	
	write(STDOUT_FILENO, buffer, strlen(buffer));
	printf("COMMAND HISTORY:\n");
	i = nextPosition;
	cnt = 10;
	while (cnt--){
		printf("# ");
		for (j = 0; j < commandLength[i]; ++j){
			printf("%s ", history[i][j]);
		}
		printf("\n");
		i = (i + 1) % 10;
	}
	printf("COMMAND->");
	fflush(0);
	return;
}

int main(void){
	char inputBuffer[MAX_LINE]; /*buffer to hold command entered*/
	int background; /*equals 1 if a command is followed by '&'*/
	char *args[MAX_LINE / 2 + 1]; /*command line arguments*/
	char *Pargs[MAX_LINE / 2 + 1];
	int i, j, cnt;
	
	FILE *fp;
	fp = fopen("success.txt", "w");
	for (i = 0; i < 10; ++i){
		fprintf(fp, "%d ", success[i]);
	}
	fclose(fp);
	
	for (i = 0; i < 10; ++i){
		for (j = 0; j < 10; ++j){
			history[i][j] = (char*)malloc(sizeof(char) * 80);
		}
	}
    
	//struct sigaction handler;
	//handler.sa_handler = handle_SIGINT;
	//sigaction(SIGINT, &handler, NULL);
		
	strcpy(buffer, "Caught Control C\n");
	signal(SIGINT, handle_SIGINT);
	
	while(1){
		background = 0;
		printf("COMMAND->");
		fflush(0);
		setup(inputBuffer, args, &background);
		
		if (args[0] == NULL){
			printf("No instruction!\n");
			continue;
		}
		
		i = 0;
		if (strcmp(args[0], "r") != 0){
			while (args[i] != NULL){
				strcpy(history[nextPosition][i], args[i]);
				++i;
			}
			commandLength[nextPosition] = i;
			nextPosition = (nextPosition + 1) % 10;
		}
		else { // r command
			if (args[1] == NULL){ // only r command
				i = (nextPosition + 9) % 10;
				if (commandLength[i] == 0){
					printf("No instruction!\n");
					continue;
				}
				if (success[i] == 1){
					for (j = 0; j < commandLength[i]; ++j){
						strcpy(history[nextPosition][j], history[i][j]);
						Pargs[j] = history[i][j];
					}
					Pargs[j] = NULL;
					commandLength[nextPosition] = j;
					nextPosition = (nextPosition + 1) % 10;
					flag = 1;
				}
				else{
					printf("Wrong Instruction!\n");
					flag = 0;
				}
			}
			else{
				i = nextPosition;
				cnt = 10;
				flag = 0;
				while (cnt--){
					i = (i + 9) % 10;
					if (strncmp(args[1], history[i][0], 1) == 0){
						if (success[i] == 1){
							for (j = 0; j < commandLength[i]; ++j){
								strcpy(history[nextPosition][j], history[i][j]);
								Pargs[j] = history[i][j];
							}
							Pargs[j] = NULL;
							commandLength[nextPosition] = j;
							nextPosition = (nextPosition + 1) % 10;
							flag = 1;
							break;
						}
						else{
							break;
						}
					}
				}
				if (flag == 0){
					printf("Wrong or No such Instruction!\n");
				}
			}
		}
		
		pid_t pid = fork();
		if (pid < 0){
			printf("fork failed\n");
		}
		else if (pid == 0){
			if (strcmp(args[0], "r") != 0){
				execvp(args[0], args);
				success[(nextPosition + 9) % 10] = 0;
				printf("Wrong Instruction!\n");
				
				fp = fopen("success.txt", "w");
				for (i = 0; i < 10; ++i){
					fprintf(fp, "%d ", success[i]);
				}
				fclose(fp);
			}
			else{
				if (flag == 1){
					execvp(Pargs[0], Pargs);
				}
			}
			exit(0);
		}
		else{
			if (background == 0){
				wait(NULL);
			}
			else{
				setup(inputBuffer, args, &background);
			}
			
			fp = fopen("success.txt", "r");
			for (i = 0; i < 10; ++i){
				fscanf(fp, "%d ", &success[i]);
			}
			fclose(fp);
		}
	}
}

