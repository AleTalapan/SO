#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <dirent.h>

int file, file2;
struct stat buff;
uint32_t file_size;
uint32_t width;
uint32_t height;
char line[350];
char u[4];
char g[4];
char o[4];
struct dirent *f;
char cale[300];

int is_image(char *path, struct stat buff){
 

  if(S_ISREG(buff.st_mode) && strstr(path, ".bmp")){
      return 1;
  }
  return 0;

}

int is_dir(char *path, struct stat buff){
 

  if(S_ISDIR(buff.st_mode)){
      return 1;
  }
  return 0;

}

int is_reg(char *path, struct stat buff){
 

  if(S_ISREG(buff.st_mode)){
      return 1;
  }
  return 0;

}

void read_header(){
  uint16_t buff1;
  uint32_t buff2;
  read(file, &buff1, 2);
  read(file, &file_size, 4);
  read(file, &buff2, 4);
  read(file, &buff2, 4);
  read(file, &buff2, 4);
  read(file, &width, 4);
  read(file, &height, 4);
}

void write_file(){
  if(write(file2, line, strlen(line)) == -1){
    perror("Nu se scrie");
    exit(EXIT_FAILURE);
  }
}

char *access_user(mode_t access){
  int i=0;
  if(access & S_IRUSR)
    u[i]='R';
  else
    u[i]='-';
  i++;
  if(access & S_IWUSR)
    u[i]='W';
  else
    u[i]='-';
  i++;
  if(access & S_IXUSR)
    u[i]='X';
  else
    u[i]='-';
  i++;
  u[i]='\0';
  return u;
}

char *access_group(mode_t access){
  int i=0;
  if(access & S_IRGRP)
    g[i]='R';
  else
    g[i]='-';
  i++;
  if(access & S_IWGRP)
    g[i]='W';
  else
    g[i]='-';
  i++;
  if(access & S_IXGRP)
    g[i]='X';
  else
    g[i]='-';
  i++;
  g[i]='\0';
  return g;
}

char *access_others(mode_t access){
  int i=0;
  if(access & S_IROTH)
    o[i]='R';
  else
    o[i]='-';
  i++;
  if(access & S_IWOTH)
    o[i]='W';
  else
    o[i]='-';
  i++;
  if(access & S_IXOTH)
    o[i]='X';
  else
    o[i]='-';
  i++;
  o[i]='\0';
  return o;
}

int main(int args, char **argv){

  if(args > 2){
    perror("Usage ./program <fisier_intrare>");
    exit(EXIT_FAILURE);
  }

  if(stat(argv[1], &buff) == -1){
    perror("stat not working");
    exit(EXIT_FAILURE);
    }

  if(!is_dir(argv[1], buff)){
    perror("Incorrect file type");
    exit(EXIT_FAILURE);
  }

  file2=open("statistica.txt", O_WRONLY);
  if(file2 == -1){
    perror("Nu s-a putut deschide fisierul pentru scriere");
    exit(EXIT_FAILURE);
  }

  DIR* director=opendir(argv[1]);
  if(director == NULL){
    perror("Can't open directory");
    exit(EXIT_FAILURE);
  }

  readdir(director);
  readdir(director);
  while((f=readdir(director)) != NULL){
    sprintf(cale, "%s/%s", argv[1], f->d_name);
    printf("%s\n", cale);
    
    if(stat(cale, &buff) == -1){
    perror("stat not working");
    exit(EXIT_FAILURE);
    }
    
    if(is_image(cale,buff)){
      file=open(cale, O_RDONLY);
      if(file == -1){
	 perror("Nu s-a putut deschide imaginea");
	 exit(EXIT_FAILURE);
       }
       read_header();

       uid_t user_id = buff.st_uid;
       struct timespec lastModifiedTime = buff.st_mtim;
       struct tm *time = localtime(&lastModifiedTime.tv_sec);
       nlink_t no_links = buff.st_nlink;
       mode_t access = buff.st_mode;

       sprintf(line, "nume fisier: %s\n", cale);
       write_file();
  
       sprintf(line, "inaltime: %d\n", width);
       write_file();

       sprintf(line, "lungime: %d\n", height);
       write_file();

       sprintf(line, "dimensiune: %d\n", file_size);
       write_file();

       sprintf(line, "identificatorul utilizatorului: %d\n", user_id);
       write_file();

       sprintf(line, "timpul ultimei modificari: %d.%d.%d\n", time->tm_mday, time->tm_mon+1, time->tm_year+1900);
       write_file();

       sprintf(line, "contorul de legaturi: %ld\n", no_links);
       write_file();

       sprintf(line, "drepturi de acces user: %s\n", access_user(access));
       write_file();

       sprintf(line, "drepturi de acces grup: %s\n", access_group(access));
       write_file();

       sprintf(line, "drepturi de acces altii: %s\n", access_others(access));
       write_file();

    }
  }

 

  
  
}
