#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include <sys/wait.h>
#define MAX_LINE_LENGTH 1024

char *commands[400];
char *split;
char *ui = NULL;

char *args1[400];
char *args2[400];


size_t capacity = 0;

static char *history[500];
static int history_count = 0;


int nl(char *argv[]) {
    int i=1, scount=0, spoz=0, dpoz=0, dcount=0;
while(argv[i]!=NULL){
    	if(strcmp(argv[i], "-s")==0){
    		scount++;
    		spoz=i;
    		}
    	if(strcmp(argv[i], "-d")==0){
    		dcount++;
    		dpoz=i;
    		}
    	i++;
    	
    }
    i--;
    char* filename = argv[i];

    FILE* fpntr = fopen(filename, "r");

    if (!fpntr) {
        fprintf (stderr, "Error opening file %s: %s\n", filename, strerror(errno));
        
        return EXIT_FAILURE;
    }

    if ((fpntr = fopen(filename, "r")) == NULL) {
        printf("cannot open file \n");
        exit(0);
    }

    static char line[MAX_LINE_LENGTH];
    int begin = 1;
    size_t lineNumber = 1;
    int c;
    int ok = 1;
    while (fgets(line, MAX_LINE_LENGTH, fpntr) && line!=NULL){
    	if(dcount==0){
		    printf("%-4zu ", lineNumber++);
		    if(scount!=0)
		   	printf(" %s ", argv[spoz+1]);
		    
		printf("%s", line);
        }
	else{
		line[strcspn(line, "\n")] = 0;
		if(strcmp(line, argv[dpoz+1])==0){
			ok = 0;
			lineNumber = 1;
		}
		if(strstr(line, argv[dpoz+1])!=NULL){
			if(strlen(line) == 3*strlen(argv[dpoz+1])){
				if(ok==1)
					printf("\n");
				ok=0;
				
			}
			if(strlen(line) == 2*strlen(argv[dpoz+1])){
					lineNumber=1;
					ok=1;		
					}
			
		}
		if(ok==1){
			if(strstr(line, argv[dpoz+1])==NULL){
			    printf("%-4zu ", lineNumber++);
			    if(scount!=0)
			   	printf(" %s ", argv[spoz+1]);
			    
			    printf("%s\n", line);
			    }
		}
		else{
		   if(strcmp(line, argv[dpoz+1])!=0 && strstr(line, argv[dpoz+1])==NULL){
		   	printf("%s\n", line);
		   }
		   
		}
			
	}
    }
    fclose(fpntr);
    
    return EXIT_SUCCESS;
}
int head(char *argv[]) {
    int i=1, ncount=0, vcount=0, qcount=0, tcount=0, npoz=0, cpoz=0, ccount=0;
    int firstText=0, textCount=0;
    while(argv[i]!=NULL){
    	if(strstr(argv[i], ".txt")!=NULL){
    		textCount++;
    		if(firstText==0)
    			firstText=i;		
    		}
    	if(strcmp(argv[i], "-n")==0){
    		ncount++;
    		npoz=i;
    		}
    	if(strcmp(argv[i], "-q")==0){
    		if(qcount==0)
    			qcount=1;qcount++;
    		}
    	if(strcmp(argv[i], "-v")==0){
    		if(vcount==0)
    			vcount=1;
    		}
    	if(strcmp(argv[i], "-c")==0){
    		ccount++;
    		cpoz=i;
    	}
    	i++;
    	
    }

	i--;

    char* filename = argv[firstText];
    
    FILE* fpntr = fopen(filename, "r");

    if (strcmp(filename, "head")==0) {
       fprintf (stderr, "Error opening file %s: %s\n", filename, strerror(errno));
        
        return EXIT_FAILURE;
    }
    if (!fpntr) {
        fprintf (stderr, "Error opening file %s: %s\n", filename, strerror(errno));
        
        return EXIT_FAILURE;
    }

    if ((fpntr = fopen(filename, "r")) == NULL) {
        printf("cannot open file \n");
        exit(0);
    }

    static char line[MAX_LINE_LENGTH];

    int begin = 1;
    size_t lineNumber = 0;
    int c;

   int ncounter,ccounter;

    while(argv[firstText]!=NULL){
    	    		if((vcount!=0 || textCount>1)&&qcount==0){
    				printf("==> %s <==\n", argv[firstText]);
    				
    			}
		if(ncount==0)
	    		ncounter=10;
	    	else
	    		ncounter=atoi(argv[npoz+1]);
	    	if(ccount!=0)
	    		ccount=atoi(argv[cpoz+1]);

    		//if(ncount!=0){
    			if(npoz > cpoz || ccount==0){
		    		while ((c = fgetc(fpntr)) != EOF && ncounter>0){
		    			if (begin) {
		    			//ncounter--;
		    			begin = 0;
					}

					if (c == '\n') {
					ncounter--;
		    			begin = 1;
					}
				
				putc(c, stdout);
		    		}
	    		}else{
	    			while ((c = fgetc(fpntr)) != EOF && ccount>1){
			    		if (begin) {
			    			//ncounter--;
			    			begin = 0;
					}
					if (c == '\n') {
			    		begin = 1;
					}
				
				putc(c, stdout);
				ccount--;
		    		}
	    			
	    		}
			printf("\n");
	    		fclose(fpntr);
	    		firstText++;
	    		fpntr = fopen(argv[firstText], "r");
	    		if ((fpntr = fopen(argv[firstText], "r")) == NULL) {
        			//printf("cannot open file %s \n", argv[firstText-1]);
        			
    			}
    	}

    return EXIT_SUCCESS;
}

void add_command_to_history( const char *command ){
   if (history_count < 500) {
        history[history_count++] = strdup( command );
   } else {
        free( history[0] );
        for (unsigned index = 1; index < 500; index++) {
            history[index - 1] = history[index];
        }
        history[500 - 1] = strdup( command );
    }
}


void take_commands(char *a, char *b[]){
	int i = 0;
	split = strtok(a, "\n ");
	while (split != NULL) { 
		b[i++] = split;
		split = strtok(NULL, "\n ");	
	}
	b[i] = NULL;
}

void exec_commands(){
	int pid = fork();
	if(pid == 0){
		if(execvp(commands[0], commands) == -1){
			perror("command not found");
			exit(errno);
		}
	}
	else{
		int s;
		waitpid(-1, &s, 0);
	}
}

void exec_commands_redirect(char file[]){
	int fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0666);
	//int fd = fopen(split2, "a");
	int pid = fork();
	if(pid==0){
		dup2(fd, 1);
		close(fd);
	execvp(args1[0], args1);				
	}
	else{
		close(fd);
		wait(NULL);
	}
}
void exec_commands_chroot(){
	int pid = fork();
	if(pid == 0){
		if(execv(commands[0], commands) == -1){
			perror("command not found");
			exit(errno);
		}
	}
	else{
		int s;
		waitpid(-1, &s, 0);
	}
}
int chroot_command(char *argv[]){

     /* chroot */
     chdir(argv[1]);
     if (chroot(argv[1]) != 0) {
         perror(argv[1]);
         return EXIT_FAILURE;
         
     }
     while(1){
     exec_commands_chroot();
     getline(&ui, &capacity, stdin);
     if(strcmp(ui, "exit\n")==0)
     	return EXIT_FAILURE;
     take_commands(ui, commands);
     }
     return 0;
}


void pipe_commands(){
	int fds[2];
	int child_status;
	pipe(fds);
	int i;

	int pid = fork();
		if(pid == 0){
			close(fds[1]);
			dup2(fds[0], 0);
			if(execvp(args2[0], args2) == -1){
				perror("command not found");
				exit(errno);
			}
		}else{
			int pid = fork();
			if(pid == 0){
				close(fds[0]);
				dup2(fds[1], 1);
				if(execvp(args1[0], args1) == -1){
					perror("command not found");
					exit(errno);
				}
			}
				else{
				close(fds[1]);
				close(fds[0]);
				int s;
				wait(NULL);
				waitpid(-1, &s, 0);
				}
			}		
}

void shell_msg(){
	printf("SHELL > ");
}


int main(){
	int predown = 0, preup = 0, pre=0;
	while(1){
		shell_msg();
		getline(&ui, &capacity, stdin);
		if(strchr(ui, '|') != NULL){
			char split1[100];
			char split2[100];
			char *p;
			p = strtok(ui, "|");
			strcpy(split1, p);
			//printf("\n%s\n", p);
			p = strtok(NULL, "\n");
			strcpy(split2, p);
			take_commands(split1, args1);
			take_commands(split2, args2);
			pipe_commands();
			continue;
		}
		if(strchr(ui, '>') != NULL){
			char split1[100];
			char split2[100];
			char *p;
			p = strtok(ui, ">");
			strcpy(split1, p);
			p = strtok(NULL, "\n");
			strcpy(split2, p);
			take_commands(split1, args1);
			exec_commands_redirect(split2);
			continue;
		}
		if(strcmp(ui, "preup\n")!=0 && strcmp(ui, "predown\n")!=0)
			add_command_to_history(ui);
		if(strcmp(ui,"\n")==0){
			perror("Please type in a command " );
			continue;
		}		
		take_commands(ui, commands);
		if (strcmp(commands[0], "q") == 0) {
			printf("SYSTEM : Shell is exiting\n");
			return 0;
		}
		if(strcmp(commands[0], "preup")== 0){
			if(history[history_count-1]!=NULL){
				history_count--;
				printf("%s",history[history_count]);
			}
			continue;
		}
		if(strcmp(commands[0], "predown")== 0){
			if(history[history_count+1]!=NULL){
				history_count++;
				printf("%s",history[history_count]);
			}
			continue;
		}
		if (strcmp(commands[0], "nl") == 0) {
			char split1[100];
			char split2[100];
			char *p;
			nl(commands);
			continue;
		}
		if (strcmp(commands[0], "head") == 0) {
			head(commands);
			continue;
		}
		if(strcmp(commands[0], "chroot")==0){
			chroot_command(commands);
			continue;
		}
		exec_commands();
	}
}
