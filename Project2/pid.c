#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int value = 5;

int main() {
    pid_t pid;
    
    pid = fork();
    printf("%d\n", pid);
    
    if (pid == 0){
        value += 15;
        printf("Children: value = %d\n", value);
    }
    else if (pid > 0) {
        wait(NULL);
        printf("PARENT: value = %d\n", value);
        exit(0);
    }
}

