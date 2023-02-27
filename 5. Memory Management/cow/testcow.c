#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define BUF_SIZE 150

int main(int argc, char *argv[]) {
    if(argc > 1){
        fprintf(2, "Usage: %s sys_call_num command\n", argv[0]);
        exit(1);
    }

    int fd;
    char buf[BUF_SIZE];
    char filename[] = "example.txt";

    // Write some text to a file
    fd = open(filename, O_WRONLY | O_CREATE | O_TRUNC);
    if (fd < 0) {
        printf("Error: could not open file\n");
        exit(1);
    }
    write(fd, "Reading example.txt\n", 21);
    close(fd);

    int pid = fork();

    if (pid < 0) {
        printf("Error: fork failed\n");
        exit(1);
    }

    if (pid == 0) {  // Child process
        // Read data from the file and print it to the console
        fd = open(filename, O_RDONLY);
        if (fd < 0) {
            printf("Error: could not open file\n");
            exit(1);
        }
        read(fd, buf, BUF_SIZE);
        printf("Child process: %s\n", buf);
        close(fd);

        fd = open(filename, O_WRONLY);
        //write to the file for the parent to read
        write(fd, "Goodbye from child\n", 20);
        close(fd);
        exit(0);
    } else {  // Parent process
        wait(0);
        // Parent reads file after child appends text to it
        fd = open(filename, O_RDONLY);
        if (fd < 0) {
            printf("Error: could not open file\n");
            exit(1);
        }

        read(fd, buf, BUF_SIZE);
        printf("Parent process: %s", buf);
        
        close(fd);
        exit(0);
    }
}
