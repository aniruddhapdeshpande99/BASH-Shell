#include "stdio.h"
#include "sys/stat.h"
#include "stdlib.h"
#include "fcntl.h"
#include "unistd.h"
#include "string.h"
#include "pwd.h"
#include "sys/wait.h"
#include "errno.h"
#include "time.h"
#include "dirent.h"
#include "grp.h"

char tilda[2] = "~";
char homedir[1024];
int built_cd(char ** args)
{
	if(args[1] == NULL)
	{
		chdir(homedir);
		return 1;
	}
	else if(!strcmp(args[1],tilda))
	{
		chdir(homedir);
	}
	else if(chdir(args[1]) != 0)
	{
		perror("Given pathname does not exist\n");
	}
	return 1;
}

int built_pwd()
{
	char cwd[1024];
	if(getcwd(cwd,sizeof(cwd)) != NULL)
	{
		printf("%s\n", cwd);
	}
	else
	{
		perror("");
	}
	return 1;
}

int built_echo(char ** args)
{
	for(int i = 1 ; args[i] != NULL ; i++)
	{
		printf("%s\n",args[i]);
	}
	printf("\n");
	return 1;
}

int ls(char** args)
{
    DIR *mydir;
    struct dirent *myfile;
    struct stat filestat;
    struct passwd *user;
    struct group *group;
    char pwd[1024];

    int num_args=1;
    char flags[3];
    char* fdname[50];
    int i,k=0,j=0;
    while (args[num_args]!=NULL) {
        if (args[num_args][0] == '-') {
            for(i = 1; i< strlen(args[num_args]) && k<3; i++) {
                flags[k++] = args[num_args][i];
            }
        }
        else {
            fdname[j] = args[num_args];
            j++;
        }
        num_args++;
    }
    flags[k] = '\0';
    if (args[1] == NULL	) {
        getcwd(pwd,sizeof(pwd));
        mydir = opendir(pwd);

        while((myfile = readdir(mydir)) != NULL)
        {
            stat(myfile->d_name, &filestat);
            if (myfile->d_name[0] != '.') {
                printf("%s\n", myfile->d_name);
            }
        }

        return 1;
    }
    else if((strcmp(flags,"a") == 0) && j==0) {
        getcwd(pwd,sizeof(pwd));
        mydir = opendir(pwd);

        while((myfile = readdir(mydir)) != NULL)
        {
            stat(myfile->d_name, &filestat);
            printf("%s\n", myfile->d_name);
        }

        return 1;
    }
    else if((strcmp(flags,"l") == 0) && j==0) {

        char filepath[512];
        getcwd(pwd,sizeof(pwd));
        mydir = opendir(pwd);

        while((myfile = readdir(mydir)) != NULL)
        {
            sprintf(filepath, "%s/%s", pwd , myfile->d_name);

            stat(filepath, &filestat);

            if(myfile->d_name[0] != '.') {

                switch (filestat.st_mode & S_IFMT) {

                case S_IFBLK:
                    printf("b");
                    break;
                case S_IFCHR:
                    printf("c");
                    break;
                case S_IFDIR:
                    printf("d");
                    break; //It's a (sub)directory
                case S_IFIFO:
                    printf("p");
                    break; //fifo
                case S_IFLNK:
                    printf("l");
                    break; //Sym link
                case S_IFSOCK:
                    printf("s");
                    break; //Filetype isn't identified
                default:
                    printf("-");
                    break;

                }


                if (filestat.st_mode & S_IRUSR)
                    printf("r");
                else
                    printf("-");

                if (filestat.st_mode & S_IWUSR)
                    printf("w");
                else
                    printf("-");

                if (filestat.st_mode & S_IXUSR)
                    printf("x");
                else
                    printf("-");


                if (filestat.st_mode & S_IRGRP)
                    printf("r");
                else
                    printf("-");

                if (filestat.st_mode & S_IWGRP)
                    printf("w");
                else
                    printf("-");

                if (filestat.st_mode & S_IXGRP)
                    printf("x");
                else
                    printf("-");


                if (filestat.st_mode & S_IROTH)
                    printf("r");
                else
                    printf("-");

                if (filestat.st_mode & S_IWOTH)
                    printf("w");
                else
                    printf("-");

                if (filestat.st_mode & S_IXOTH)
                    printf("x");
                else
                    printf("-");



                printf("\t%ld",filestat.st_nlink);

                user = getpwuid(filestat.st_uid);
                printf("\t%s",user->pw_name);

                group = getgrgid(filestat.st_gid);
                printf("\t%s",group->gr_name);

                printf("\t%ld", filestat.st_size);

                printf("\t%s", myfile->d_name);

                printf("\t%s", ctime(&filestat.st_mtime));

            }
        }
    }

    else if(((strcmp(flags,"al") == 0) || (strcmp(flags,"la") == 0))&& j==0) {

        char filepath[512];
        getcwd(pwd,sizeof(pwd));
        mydir = opendir(pwd);

        while((myfile = readdir(mydir)) != NULL)
        {
            sprintf(filepath, "%s/%s", pwd , myfile->d_name);

            stat(filepath, &filestat);

            switch (filestat.st_mode & S_IFMT) {

            case S_IFBLK:
                printf("b");
                break;
            case S_IFCHR:
                printf("c");
                break;
            case S_IFDIR:
                printf("d");
                break; //It's a (sub)directory
            case S_IFIFO:
                printf("p");
                break; //fifo
            case S_IFLNK:
                printf("l");
                break; //Sym link
            case S_IFSOCK:
                printf("s");
                break; //Filetype isn't identified
            default:
                printf("-");
                break;

            }


            if (filestat.st_mode & S_IRUSR)
                printf("r");
            else
                printf("-");

            if (filestat.st_mode & S_IWUSR)
                printf("w");
            else
                printf("-");

            if (filestat.st_mode & S_IXUSR)
                printf("x");
            else
                printf("-");


            if (filestat.st_mode & S_IRGRP)
                printf("r");
            else
                printf("-");

            if (filestat.st_mode & S_IWGRP)
                printf("w");
            else
                printf("-");

            if (filestat.st_mode & S_IXGRP)
                printf("x");
            else
                printf("-");


            if (filestat.st_mode & S_IROTH)
                printf("r");
            else
                printf("-");

            if (filestat.st_mode & S_IWOTH)
                printf("w");
            else
                printf("-");

            if (filestat.st_mode & S_IXOTH)
                printf("x");
            else
                printf("-");



            printf("\t%ld",filestat.st_nlink);

            user = getpwuid(filestat.st_uid);
            printf("\t%s",user->pw_name);

            group = getgrgid(filestat.st_gid);
            printf("\t%s",group->gr_name);

            printf("\t%ld", filestat.st_size);

            printf("\t%s", myfile->d_name);

            printf("\t%s", ctime(&filestat.st_mtime));

        }
    }
    else{
    	printf("Wrong usage of ls\n");
    }
    return 1;
}

int pinfo(char** args)
{
  int i;
  char pid[512];
  char buf[512];

  char state;
  char virtual_mem[512];

  if (args[1] != NULL) {
    for (i = 0; i < strlen(args[1]); i++) {
      pid[i]=args[1][i];
    }
    pid[i]='\0';
  }
  else if(args[1] == NULL){
    pid[0]='s';
    pid[1]='e';
    pid[2]='l';
    pid[3]='f';
    pid[4]='\0';
  }

  char exe_location[] = "/proc/";
  strcat(exe_location,pid);
  strcat(exe_location,"/exe");

  char stat_location[] = "/proc/";
  strcat(stat_location,pid);
  strcat(stat_location,"/status");

  FILE *fptr;

  if((fptr = fopen(stat_location,"r")) == NULL){
    printf("Process doesnt exist\n");
    return 1;
  }
  int slen = readlink(exe_location,buf,sizeof(buf)-1);
  buf[slen] = '\0';

  char* line = NULL;
  size_t len = 0;

  int count=2;
  while (count--) {
    getline(&line, &len, fptr);
  }
  state = line[7];

  count = 15;
  while (count--) {
    getline(&line, &len, fptr);
  }
  i=0;
  while (line[i]!=':') {
    i++;
  }
  i++;
  while (!(line[i] >= 48 && line[i]<= 57)) {
    i++;
  }
  int k = 0;
  while (line[k+i]!=' ') {
    virtual_mem[k] = line[k+i];
    k++;
  }
  virtual_mem[k] = '\0';

  printf("pid -- ");
  if (args[1] == NULL) {
    printf("%d\n",getpid());
  }
  else
    printf("%s\n",args[1]);

  printf("Process Status -- %c\n",state);
  printf("Virtual Memory -- %s\n",virtual_mem);
  printf("Executable Path -- %s\n",buf);

  fclose(fptr);

  return 1;
}
