#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// used in the open/openat functions
// need more research on this though
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// used in close
#include <unistd.h>

// need to be compiled with -pthread
// used in the watcher routine
#include <pthread.h>

// need to be compiled with -lseccomp
// used in the seccomp things
// need to install the library first with
// sudo apt-get install libseccomp-dev
#include <seccomp.h>

// for compiling the program that is sent in
#include "compile.h"

#define ARGC_AMOUNT 10

int32_t file_test( char *name, int32_t mode); // 1 = read, 2 = write


int main( int argc, char *argv[])
{
    /* getting the limits and files by arguments listed in the order below
     * any changes here should change ARGC_AMOUNT accordingly
     * 
     * ./sandbox ( must be here)
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
    char *program = argv[ 1];
    char *file_stdin = argv[ 2];
    char *file_stdout = argv[ 3];
    char *file_stderr = argv[ 4];
    char *file_result = argv[ 5];

    int32_t time_limit = strtol( argv[ 6], NULL, 10);
    int32_t memory_limit = strtol( argv[ 7], NULL, 10);
    int32_t output_limit = strtol( argv[ 8], NULL, 10);
    int32_t process_limit = strtol( argv[ 9], NULL, 10);

    // testing the information
    /*
    printf("program %s)\n", program);
    printf("file in %s)\n", file_stdin);
    printf("file out %s)\n", file_stdout);
    printf("file err %s)\n", file_stderr);
    printf("file result %s)\n", file_result);
    printf("time limit %d\n", time_limit);
    printf("memory limit %d\n", memory_limit);
    printf("output limit %d\n", output_limit);
    printf("process limit %d\n", process_limit);
    */

    // need to check the file io permissions

	if ( file_test( program, 1) || file_test( file_stdin, 1) || file_test( file_stdout, 2) ||
			file_test( file_stderr, 2) || file_test( file_result, 2))
	{
		printf("File open error\n");

		return -1;
	}// if

	// printf("compile %d\n", compile( program));

	int32_t compile_result = compile( program);

    if ( compile_result != 0)
    {
        printf("Compile error %d\n", compile_result);

        return -1;
    }// if
   

    // multiprocess needed here


    return 0;
}


int32_t file_test( char *name, int32_t mode)
{
	int fd = 0;
	
	// open according to given mode
	switch ( mode)
	{
		case 1:
		{
			fd = open( name, O_RDONLY);

			break;
		}
		case 2:
		{
			fd = open( name, O_WRONLY);

			break;
		}
		default:
		{
			printf("Mode error\n");

			fd = -1;

			break;
		}
	}// switch

	// failed
    if ( fd == -1)
    {
        perror( name);

        return 1;
    }// if
    
    close( fd);

	// success
	return 0;
}
