#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// used in the open/openat functions
// need more research on this though
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// need to be compiled with -pthread
// used in the watcher routine
#include <pthread.h>

// need to be compiled with -lseccomp
// used in the seccomp things
// need to install the library first with
// sudo apt-get install libseccomp-dev
#include <seccomp.h>

#define ARGC_AMOUNT 9


int main( int argc, char *argv[])
{
    /* getting the limits and files by arguments listed in the order below
     * any changes here should change ARGC_AMOUNT accordingly
     * 
     * program.c
     * file_stdin
     * file_stdout
     * file_stderr
     * file_result ( for the parent program)
     * time_limit
     * memory_limit
     * ouput_limit
     * process_limit
     * 
     */

    // check the argument amout
    if( argc != ARGC_AMOUNT)
    {
        printf("The amount of arguments is incorrect\n");

        printf("Given %d, Expected %d\n", argc, ARGC_AMOUNT);

        return 1;
    }// if

    // naming the arguments
    char *program = argv[ 0]; // is this the right one??
    char *file_stdin = argv[ 1];
    char *file_stdout = argv[ 2];
    char *file_stderr = argv[ 3];
    char *file_result = argv[ 4];

    int32_t time_limit = strtol( argv[ 5], NULL, 10);
    int32_t memory_limit = strtol( argv[ 6], NULL, 10);
    int32_t output_limit = strtol( argv[ 7], NULL, 10);
    int32_t process_limit = strtol( argv[ 8], NULL, 10);


    // need to check the file io permissions

    // multiprocess needed here

    return 0;
}
