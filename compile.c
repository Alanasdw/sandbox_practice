#include "compile.h"

int32_t compile( char *name)
{
    int32_t status = 0;

    char *gcc = "gcc ";

    char *arguments = " -lm -O2 -w -std=c11 -pipe -DONLINE_JUDGE -o main";

    char *full = calloc( strlen( gcc) + strlen( arguments) + strlen( name) + 1, sizeof( char));

    strcpy( full, gcc);

    strcat( full, name);

    strcat( full, arguments);

    printf("%s)\n", full);

    status = system( full);

    // free the space that is used
    free( full);

    full = NULL;

    return status;
}
