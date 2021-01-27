#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// used in the open/openat functions
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// need to be compiled with -pthread
// used in the watcher routine
#include <pthread.h>


int main( void)
{
    // think of a way to get the file io and the limits that are needed

    // need to check the file io permissions

    // multiprocess needed here

    return 0;
}
