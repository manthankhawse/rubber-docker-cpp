#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

void contain(char *args[]){
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