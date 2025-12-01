#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/mount.h>
using namespace std;

const char* new_root = "/new-root/rootfs";

void contain(char *args[]){
    if(unshare(CLONE_NEWNS)!=0){
        perror("Error unsharing mount");
        exit(1);
    }

    if(mount(NULL, "/", NULL, MS_PRIVATE | MS_REC, NULL)!=0){
        perror("Error making / private");
        exit(1);
    }
    if(chdir(new_root)!=0){
        perror("Error changing root");
        exit(1);
    }

    if(chroot(new_root)!=0){
        perror("Error chrooting");
        exit(1);
    }
    if(chdir("/")!=0){
        perror("Error changing dir to /");
        exit(1);
    }

    if(mount("proc", "/proc", "proc", 0, nullptr)!=0){
        perror("Error mounting proc");
        exit(1);
    }

    if(mount("tmpfs", "/dev", "tmpfs", 0, nullptr)!=0){
        perror("Error mounting dev");
        exit(1);
    }

    if(mkdir("/dev/pts", 0755)!=0){
        perror("Error creating directory");
        exit(1);
    }

    mount("devpts", "/dev/pts", "devpts", 0, nullptr);
    mknod("dev/null", 0666 | S_IFCHR, makedev(1,3));
    mknod("dev/zero", 0666 | S_IFCHR, makedev(1,5));
    mknod("dev/tty", 0666 | S_IFCHR, makedev(5,0));
    execvp(args[0], args);
    perror("execvp");
    exit(1);
}

void run(char *args[]){
    int pid = fork();
    if(pid==0){
        contain(args);
    }else if(pid>0){
        int status;
        waitpid(pid, &status, 0);
        cout<<pid<<" exited with status "<<status<<endl;
    }else{
        perror("fork");
    }
}

int main(int argc, char* argv[]){
    if(argc<2){
        cerr<<"Usage: "<<argv[0] << " <program> [args...]\n";
        return 1;
    }
    
    run(&argv[1]);
    return 0;
}