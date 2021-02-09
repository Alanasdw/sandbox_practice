#include "rule.h"

int32_t add_rule( char *program_name)
{
    // if ( prctl( PR_SET_SECCOMP, SECCOMP_MODE_STRICT) == -1)
    // {
    //     perror( "prctl failed");

    //     return 1;
    // }// if
    
    scmp_filter_ctx ctx;

	ctx = seccomp_init( SCMP_ACT_ALLOW);

    // can only call execve( "./main")
    if ( seccomp_rule_add( ctx, SCMP_ACT_KILL, SCMP_SYS( execve), 1, SCMP_A0( SCMP_CMP_NE, ( scmp_datum_t) program_name)) != 0)
    {
        perror( "seccomp execve failed");

        return 1;
    }// if
    
    int32_t illegal_syscall[] = { SCMP_SYS( fork), SCMP_SYS( kill),
                                SCMP_SYS( clone)/*, SCMP_SYS( open),
                                SCMP_SYS( openat)*/, SCMP_SYS( vfork)};

    // add all rules
    for ( int32_t i = 0; i < 4 ; i++)
    {
        if ( seccomp_rule_add( ctx, SCMP_ACT_KILL, illegal_syscall[ i], 0) != 0)
        {
            perror( "seccomp general failed");
        
            return 1;
        }// if
        
    }// for i

    // open/openat need to have their own rules
    if ( seccomp_rule_add( ctx, SCMP_ACT_KILL, SCMP_SYS( open), 1, SCMP_CMP( 1, SCMP_CMP_MASKED_EQ, O_WRONLY, O_WRONLY)) != 0 ||
        seccomp_rule_add( ctx, SCMP_ACT_KILL, SCMP_SYS( open), 1, SCMP_CMP( 1, SCMP_CMP_MASKED_EQ, O_RDWR, O_RDWR)) != 0 ||
        seccomp_rule_add( ctx, SCMP_ACT_KILL, SCMP_SYS( openat), 1, SCMP_CMP( 1, SCMP_CMP_MASKED_EQ, O_WRONLY, O_WRONLY)) != 0 ||
        seccomp_rule_add( ctx, SCMP_ACT_KILL, SCMP_SYS( openat), 1, SCMP_CMP( 1, SCMP_CMP_MASKED_EQ, O_RDWR, O_RDWR)) != 0)
    {
        perror( "seccomp open/openat failed");

        return 1;
    }// if

    if ( seccomp_load( ctx) != 0)
    {
        perror( "seccomp load failed");
        
        return 1;
    }// if

    return 0;
}
