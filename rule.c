#include "rule.h"

int32_t add_rule( char *program_name)
{
    if ( prctl( PR_SET_NO_NEW_PRIVS, 1) == -1)
    {
        perror( "prctl failed");

        return 1;
    }// if
    
    scmp_filter_ctx ctx;

	ctx = seccomp_init( SCMP_ACT_ALLOW);

    // can only call execve( "./main")
    if ( seccomp_rule_add( ctx, SCMP_ACT_KILL, SCMP_SYS( execve), 1, SCMP_A0( SCMP_CMP_NE, ( scmp_datum_t) program_name)) != 0)
    {
        perror( "seccomp failed");

        return 1;
    }// if
    
    int32_t illegal_syscall[] = { SCMP_SYS( fork), SCMP_SYS( kill),
                                SCMP_SYS( clone), SCMP_SYS( open),
                                SCMP_SYS( openat), SCMP_SYS( vfork)};

    // add all rules
    for ( int32_t i = 0; i < 6 ; i++)
    {
        if ( seccomp_rule_add( ctx, SCMP_ACT_KILL, illegal_syscall[ i], 0) != 0)
        {
            perror( "seccomp failed");
        
            return 1;
        }// if
        
    }// for i
    
    if ( seccomp_load( ctx) != 0)
    {
        perror( "seccomp failed");
        
        return 1;
    }// if

    return 0;
}
