
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"





char* turnToSanityFormat(char* text, char* buff){
    
    char* oldBuff;
    oldBuff= buff;
    
    char *tmp;
    char *newline;
 //   char buf[256];
    tmp = text;
    int count;

    tmp = strchr( tmp, ':');

    while(tmp){
        tmp =tmp+ 2;
        newline = strchr(tmp, '\n');
        memmove(buff, tmp, newline - tmp); //entering to buf
        count++;
        buff = buff +(int)(newline - tmp);
        memmove(buff, (char*)' ', 1);
        buff++;
        
        if(count == 5){ //starting major
          memmove(buff, (char*)'(', 1);
          buff ++;
        }
        printf(1, buff);
        if(count == 5){ //ending major
            memmove(buff, (char*)')', 1);
            buff++;
        }
       // printf(1, " ");
    }        
    
        
        
        
               return oldBuff;
        
        
    }



int main() {
       

   // char* buffer = "Hello my name is : kk \n hey : ksks";
  //  char bu[256];
   // turnToSanityFormat(buffer, bu);
   // printf(1,bu);
   // char [] buffer = "Hello my name is : kk \n hey : ksks";
    
    
    int fd, inodeinfo_fd;
    char readBuff[512], filename[50];
    struct dirent dir_entry;
    
        char buf[256];

    
    if ((inodeinfo_fd = open("/proc/inodeinfo", 0)) < 0) {
        printf(1, "error in opening info\n");
        exit();
    }
    read(inodeinfo_fd, &dir_entry, sizeof(dir_entry)); // reading: .
//    read(inodeinfo_fd, &dir_entry, sizeof(dir_entry)); // reading: ..
    //icacche
    while (read(inodeinfo_fd, &dir_entry, sizeof(dir_entry)) == sizeof(dir_entry)) {// read inode at a time
      if(dir_entry.inum != 0){
       // sprintf(filename, "/proc/inodeinfo/%s", dir_entry.name);
        fd = open(filename,0);
        if(fd >= 0){
          if (read(fd, readBuff, 512) <= 0) {
              printf(2, "error in reading inode file!\n");
              return -1;
          }
          turnToSanityFormat(readBuff, buf);
          printf(1, buf);
          memset(buf,0,256);
          printf(1, "\n");
          close(fd);
        }
      }
    }
    close(inodeinfo_fd);

    return 0;
}
