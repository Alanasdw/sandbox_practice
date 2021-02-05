#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// used in the open/openat functions
// need more research on this though
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// used for setrlimit
#include <sys/time.h>
#include <sys/resource.h>

// used for waitid
#include <sys/wait.h>

// used to kill
#include <signal.h>

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
#define MAX_TIME 30

typedef struct _sWatcher_Data
{
    time_t time;

    pid_t child;
} sWatcher_Data;

int time_limit_kill;

int32_t file_test( char *name, int32_t mode); // 1 = read, 2 = write
void *watcher( void *data);

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

    // handle compile errors
    if ( compile_result != 0)
    {
        printf("Compile error %d\n", compile_result);

        FILE *result = fopen( file_result, "w");

        fprintf( result, "CE\n");

        fclose( result);

        return -1;
    }// if

    // multiprocess needed here
    pid_t child_pid = fork();

    // set the time limit kill
    time_limit_kill = 0;

    if ( child_pid == -1)
    {
        // just in case
        perror("Fork failed");

        return -1;
    }// if
    else if ( child_pid != 0)
    {
        // parent process

        // has a maximum run time if time limit is not specified
        if ( time_limit == 0)
        {
            time_limit = MAX_TIME;
        }// if

        // need to write a watcher so that if there is an child that does not end, it gets killed

        sWatcher_Data data;

        data.child = child_pid;

        data.time = time_limit;
        
        pthread_t thread;

        // start watcher
        pthread_create( &thread, NULL, &watcher, ( void *) &data);

        siginfo_t child_info;

        // wait for the child process to end or stop
        if ( waitid( P_PID, child_pid, &child_info, WEXITED | WSTOPPED) == -1)
        {
            perror("Waitid failed\n");

            pthread_join( thread, NULL);

            return -1;
        }// if

        // stop the watcher thread
        pthread_cancel( thread);
        
        pthread_join( thread, NULL);
        
        struct rusage usage;

        // get child process usage
        getrusage( RUSAGE_CHILDREN, &usage);

        // determin the status of the result

        FILE *fresult = fopen( file_result, "w");

        double used_time = (( usage.ru_utime.tv_sec * 1000) + usage.ru_utime.tv_usec) / 1000.0;

        if ( child_info.si_code == CLD_EXITED)
        {
            // normally exited
            if ( usage.ru_maxrss > memory_limit)
            {
                fprintf( fresult, "MLE\nMemory used ( kb) %ld\n", usage.ru_maxrss);
            }// if
            else
            {
                fprintf( fresult, "Normal execution\nTime used ( sec) %lf, Memory used %ld\n", used_time, usage.ru_maxrss);
            }// else

            fprintf( fresult, "Exit status %d\n", child_info.si_status);
        }// if
        else
        {
            // something happened
            if ( child_info.si_status == SIGXFSZ)
            {
                fprintf( fresult, "OLE\nOutput limit %d\n", output_limit);
            }// if
            else if ( child_info.si_status == SIGKILL || child_info.si_status == SIGXCPU || time_limit_kill == 1)
            {
                fprintf( fresult, "TLE\nTime used ( sec) %lf\n", used_time);
            }// else if
            else if ( usage.ru_maxrss > memory_limit)
            {
                fprintf( fresult, "MLE\nMemory used ( kb) %ld\n", usage.ru_maxrss);
            }// else if
            else
            {
                fprintf( fresult, "RE\nRun time error\n");
            }// else

            fprintf( fresult, "%s\n", strsignal( child_info.si_status));
        }// else

        fclose( fresult);

    }// else if
    else
    {
        // child process

        // set the limits given

        // set the file io

        // set the seccomp rules

        // execute main

        char *arguments[] = { "./main", 0};

        execv( arguments[ 0], arguments);
    }// else

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

void *watcher( void *data)
{
    struct timespec time;

    time.tv_nsec = 0;

    time.tv_sec = (( sWatcher_Data *) data) -> time;

    nanosleep( &time, NULL);

    kill( (( sWatcher_Data *) data) -> child, SIGKILL);

    time_limit_kill = 1;

    pthread_exit( NULL);
}