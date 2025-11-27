#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mount.h>
using namespace std;

const char* new_root = "/new-root/rootfs";

void contain(char *args[]){
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
    // add mount here "/proc"
    if(mount("proc", "/proc", "proc", 0, nullptr)!=0){
        perror("Error mounting proc");
        exit(1);
    }
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