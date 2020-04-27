#include "types.h"
#include "stat.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"

int ninodes = 0;

void itoa(int num, char* s){
    int len = 0;
    if (num == 0){
        s[0] = '0';
        return;
    }
    while(num != 0){
        s[len] = num % 10 + '0';
        num = num / 10; 
        len++;
    }
    for(int i = 0; i < len/2; i++){
        char tmp = s[i];
        s[i] = s[len - 1 - i];
        s[len - 1 - i] = tmp;
    }
}

void appendStrToBuffer(char* buffer, char* str){
  int strLen = strlen(str);
  int bufLen = strlen(buffer);
  memmove(buffer + bufLen, str, strLen);
}

void appendNumToBuffer(char* buffer, int num){
  char numStr[16] = {0};
  itoa(num, numStr);
  appendStrToBuffer(buffer, numStr);
}

int procideinfo(char* buffer){
    appendStrToBuffer(buffer, "Waiting operations: ");
    appendNumToBuffer(buffer, getNumWaitingOperations());
    appendStrToBuffer(buffer, "Read waiting operations: ");
    appendNumToBuffer(buffer, getNumReadWaitingOperations());
    appendStrToBuffer(buffer, "Write waiting operations: ");
    appendNumToBuffer(buffer, getNumWriteWaitingOperations());
    appendStrToBuffer(buffer, "Working blocks: ");
    char ans[512] = {};
    getWorkingBlocks(ans);
    appendStrToBuffer(buffer, ans);
    return strlen(buffer);
}

int procfilestat(char* buffer){
    appendStrToBuffer(buffer, "Free fds: ");
    appendNumToBuffer(buffer, getFreeFds());
    appendStrToBuffer(buffer, "Unique inode fds: ");
    appendNumToBuffer(buffer, getUniqueInodeFds());
    appendStrToBuffer(buffer, "Writeable fds: ");
    appendNumToBuffer(buffer, getWriteableFds());
    appendStrToBuffer(buffer, "Readable fds: ");
    appendNumToBuffer(buffer, getReadableFds());
    appendStrToBuffer(buffer, "Refs per fds: ");
    appendNumToBuffer(buffer, getTotalNumberOfRefs());
    appendStrToBuffer(buffer, " / ");
    appendNumToBuffer(buffer, getNumberOfUsedFds());
    return strlen(buffer);
}

/*
int PIDname(char* buffer){}
int PIDstatus (char* buffer){}
*/

int map(struct inode *ip, char* buffer){
    if (ip->inum == (ninodes+1))  //1 direct
            return procideinfo(buffer);
    if (ip->inum == (ninodes+2))
           return procfilestat(buffer);
   
    //TODO: map inodeinfo & PID directories
    return 0;
}


int 
procfsisdir(struct inode *ip) {
    if (ninodes == 0){
            struct superblock sb;
    readsb(ip->dev, &sb);
    ninodes = sb.ninodes;
    }
    if (!(ip->type == T_DEV) || !(ip->major == PROCFS))
            return 0;
    int inum = ip->inum;
    if (inum == (ninodes+1) || inum == (ninodes+2))
            return 0; //ideinfo and filestat are files (not directories)
   //TODO return 1 for inodeinfo & PID directories
   return 1;
}

void 
procfsiread(struct inode* dp, struct inode *ip) {
  ip->valid = 1;
  ip->type = T_DEV;
  ip->major = PROCFS;
}

int
procfsread(struct inode *ip, char *dst, int off, int n) {
    if (ninodes == 0){
        struct superblock sb;
        readsb(ip->dev, &sb);
        ninodes = sb.ninodes;
    }
    char buf[NPROC+5] = {0};
    int size = map(ip, buf);
    memmove(dst, buf+off, n);
    if(n < size - off)
        return n;
    return size - off;
}

int
procfswrite(struct inode *ip, char *buf, int n)
{
  panic("procfs - call to write in a read-only system");
  return 0;
}

void
procfsinit(void)
{
  devsw[PROCFS].isdir = procfsisdir;
  devsw[PROCFS].iread = procfsiread;
  devsw[PROCFS].write = procfswrite;
  devsw[PROCFS].read = procfsread;
}
