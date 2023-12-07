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
#include <sys/wait.h>

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

int is_dir(struct stat buff){
 

  if(S_ISDIR(buff.st_mode)){
      return 1;
  }
  return 0;

}

int is_reg(struct stat buff){
  if(S_ISREG(buff.st_mode)){
      return 1;
  }
  
  return 0;
}

int is_link(struct stat buff){
  if(S_ISLNK(buff.st_mode)){
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

void info_files(int reg, int file_size, int user_id, struct tm *time, long int no_links, mode_t access){

  if(reg){
    sprintf(line, "dimensiune: %d\n", file_size);
    write_file();
  }

  sprintf(line, "identificatorul utilizatorului: %d\n", user_id);
  write_file();

  if(reg){
    sprintf(line, "timpul ultimei modificari: %d.%d.%d\n", time->tm_mday, time->tm_mon+1, time->tm_year+1900);
    write_file();

    sprintf(line, "contorul de legaturi: %ld\n", no_links);
    write_file();
  }

  sprintf(line, "drepturi de acces user: %s\n", access_user(access));
  write_file();

  sprintf(line, "drepturi de acces grup: %s\n", access_group(access));
  write_file();

  sprintf(line, "drepturi de acces altii: %s\n", access_others(access));
  write_file();

}

void info_symlink(char *name, int file_size, mode_t access){
  struct stat sym_link;
  if(stat(cale, &sym_link) == -1){
    perror("Eroare lstat target");
    exit(EXIT_FAILURE);
  }
  
  sprintf(line, "nume legatura: %s\n", name);
  write_file();

  sprintf(line, "dimensiune: %d\n", file_size);
  write_file();

  sprintf(line, "dimensiune fisier: %ld\n", sym_link.st_size);
  write_file();

  sprintf(line, "drepturi de acces user: %s\n", access_user(access));
  write_file();

  sprintf(line, "drepturi de acces grup: %s\n", access_group(access));
  write_file();

  sprintf(line, "drepturi de acces altii: %s\n", access_others(access));
  write_file();
  
}

void statistica(char* dir, char* name){
  char filepath[300];
  sprintf(filepath, "%s/%s_statistica.txt", dir, name);
	
  int fd = open(filepath, O_RDONLY | O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if(fd == -1){
    perror("Can't open file for writing directory");
    exit(EXIT_FAILURE);
  }
}

void waiting(pid_t pid){
  int status;
  pid_t w = waitpid(pid, &status, 0);
      
  if(w == -1){
    perror("Error waitpid directory");
    exit(EXIT_FAILURE);
  }
  else if(w == pid){
    printf("Process with id %d ended with the status %d\n", pid,status);
}
}



int main(int args, char **argv){

  if(args != 3){
    perror("Usage ./program <director_intrare> <director_iesire>");
    exit(EXIT_FAILURE);
  }

  if(stat(argv[1], &buff) == -1){
    perror("stat not working");
    exit(EXIT_FAILURE);
    }

  if(!is_dir(buff)){
    perror("Incorrect first file type");
    exit(EXIT_FAILURE);
  }

  if(stat(argv[2], &buff) == -1){
    perror("stat 2 not working");
    exit(EXIT_FAILURE);
    }

  if(!is_dir(buff)){
    perror("Incorrect second file type");
    exit(EXIT_FAILURE);
  }

  file2 = open("statistica.txt", O_RDONLY | O_WRONLY | O_CREAT | O_TRUNC, 0666);
  
  if(file2 == -1){
    perror("Nu s-a putut deschide fisierul pentru scriere");
    exit(EXIT_FAILURE);
  }

  DIR* director=opendir(argv[1]);
  if(director == NULL){
    perror("Can't open directory");
    exit(EXIT_FAILURE);
  }

  while((f=readdir(director)) != NULL){
    if(strcmp(f->d_name,".") == 0 || strcmp(f->d_name,"..") == 0){
      continue;
    }
    sprintf(cale, "%s/%s", argv[1], f->d_name);
    printf("%s\n", cale);
    
    if(lstat(cale, &buff) == -1){
    perror("lstat not working");
    exit(EXIT_FAILURE);
    }

    uid_t user_id = buff.st_uid;
    struct timespec lastModifiedTime = buff.st_mtim;
    struct tm *time = localtime(&lastModifiedTime.tv_sec);
    nlink_t no_links = buff.st_nlink;
    mode_t access = buff.st_mode;
    int reg = 0;
    
    if(is_image(cale,buff)){
      
      file=open(cale, O_RDONLY);
      if(file == -1){
	 perror("Nu s-a putut deschide imaginea");
	 exit(EXIT_FAILURE);
       }
      
       read_header();
       reg = 1;

       sprintf(line, "nume fisier: %s\n", f->d_name);
       write_file();

       sprintf(line, "inaltime: %d\n", width);
       write_file();

       sprintf(line, "lungime: %d\n", height);
       write_file();

       info_files(reg, file_size, user_id, time, no_links, access);

    }
    else if(is_reg(buff)){
      pid_t id_reg = fork();
      if(id_reg < 0){
	perror("Error fork directory");
	exit(EXIT_FAILURE);
      }
      if(id_reg == 0){
	statistica(argv[2], f->d_name);

	sprintf(line, "nume fisier: %s\n", f->d_name);
	write_file();
	reg = 1;
	info_files(reg, buff.st_size, user_id, time, no_links, access);

	exit(EXIT_SUCCESS);
      }

      waiting(id_reg);
      
    }

    else if(is_dir(buff)){
      pid_t id_dir = fork();
      if(id_dir < 0){
	perror("Error fork directory");
	exit(EXIT_FAILURE);
      }
      
      if(id_dir == 0){
	statistica(argv[2], f->d_name);
	sprintf(line, "nume director: %s\n", f->d_name);
	write_file();

	info_files(reg, buff.st_size, user_id, time, no_links, access);

	exit(EXIT_SUCCESS);
      }
      
      waiting(id_dir);
    }

    else if(is_link(buff)){
      pid_t link_id = fork();
      if(link_id < 0){
	perror("Error fork directory");
	exit(EXIT_FAILURE);
      }
      
      if(link_id == 0){
	statistica(argv[2], f->d_name);

        info_symlink(f->d_name, buff.st_size, access);

	exit(EXIT_SUCCESS);
      }

      waiting(link_id);
    }
  }
  
}
