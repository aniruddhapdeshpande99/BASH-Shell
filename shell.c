#include "stdio.h"
#include "sys/stat.h"
#include "stdlib.h"
#include "fcntl.h"
#include "unistd.h"
#include "string.h"
#include "pwd.h"
#include "sys/wait.h"

#include "builtin_commands.c"
char homedir[1024];

char *builtin_commands[] = {"cd","pwd","echo","ls","pinfo"};
int (*builtin_func[]) (char **) = {&built_cd,&built_pwd,&built_echo,&ls,&pinfo};
int flag = 0;
int launch_command(char** args)
{
    pid_t pid,wpid;
    int status,number_of_args=0;
    int length = strlen(args[0]);
    char x[2];
    x[0] = args[0][length-1];
    if(!strcmp(x,"&")) // ex : "gedit&" and not "gedit &"
    {
        flag = 1;
        args[0][length-1] = '\0';
    }
    pid = fork();
    if(pid > 0 ) // parent process
    {
        if(!flag) // if & is not in the end then    parents wait for the child to end
        {
            waitpid(pid,&status,0);
        }
    }
    else if(!pid) // child process
    {
        if(execvp(args[0], args) == -1){
            printf("Invalid command\n");
            perror("");
        }
        exit(0);
    }
    else
    {
        perror("Error Forking");
    }
    return 1;
}

int compare_path(char* homedir,char* cwd,int homedir_length,int cwd_length)
{
    char temp[1023] = "\0";
    if(cwd_length < homedir_length) // if cwd_length is less than homedirectories length then pwd is not inside homedirect
        return 0;
    strncpy(temp,cwd,homedir_length);
    if(!strcmp(temp,homedir)) // if path of cwd upto that of homedirectory is  the same then it is inside homedirectory
        return 1;
    else // else it is not
        return 0;
}


int run_command(char** args)
{
    int i = 0;
    for (i = 0; i <5; i++) // checking whether it is a builtin command
        {
                if (!strcmp(args[0], builtin_commands[i]))
                {
                        return (*builtin_func[i])(args);
                }
        }
    return launch_command(args);
}

int split_command(char* command)
{
        // working code for just one command
	// int count = 0,i = 0;
	// char* token;
	// char ** tokens = malloc(1000*sizeof(char*));
 //    token = strtok(command," \t\n");
	// for(;token;token=strtok(NULL," \t\n"))
	// {
	// 	tokens[count++] = token;
	// }
	// tokens[count] = NULL;
 //     return tokens;
        // working code for just one command
    int status;
    char * token;
    int number_commands = 0, i , j,count = 0;
    char ** command_list = malloc(1000*sizeof(char*));
    token = strtok(command, ";");
    for(;token;token=strtok(NULL,";"))  // To seperate all the commands
    {
        command_list[number_commands++] = token;
    }

    command_list[number_commands] = NULL;
    char ** tokens = malloc(1000*sizeof(char*));
    for(i = 0 ; i < number_commands ; i++) // To get arguments for a command
    {
        flag = 0;
        token = strtok(command_list[i], " \t\n\"");
        for(;token;token=strtok(NULL," \t\n\""))
        {
            tokens[count++] = token;
        }
        tokens[count] = NULL;
        if(!strcmp(tokens[count-1],"&"))
        {
            // printf("YES\n");
            flag = 1;
            tokens[count-1] = NULL;
        }
        count = 0;
        status = run_command(tokens);
    }
    return status;
}

char * read_command()
{
        char *command = NULL;
        ssize_t bufsize = 0;
        getline(&command, &bufsize, stdin);
        return command;
}

void prompt(char* homedir)
{
	char hostname[1023] = "\0";
	gethostname(hostname,sizeof(hostname));
	int uid = getuid(); //returns the user for calling process
	struct passwd *pw = getpwuid(uid);
	int i = 0;
	char cwd[1023] = "\0", effective_cwd[1023] = "\0" ;
    getcwd(cwd,sizeof(cwd));
    if(!strcmp(cwd,homedir))
    {
		printf("<%s@%s:~>", pw->pw_name, hostname);
	}
	else if(compare_path(homedir,cwd,strlen(homedir),strlen(cwd)))
	{
		int start = strlen(homedir), end = strlen(cwd);
		strncpy(effective_cwd,cwd + start,end - start);
		printf("<%s@%s:~%s>",pw->pw_name, hostname,effective_cwd);
	}
	else
        printf ("<%s@%s:%s>",pw->pw_name,hostname,cwd);
}


int main()
{
    char * command;
    char** args;
    int status = 1;
    getcwd(homedir,sizeof(homedir));
    while(status)
    {
        prompt(homedir);
	   command = read_command();
	   status = split_command(command);
	}
    return 0;
}
