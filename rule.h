#pragma once

#include <seccomp.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>

// #include <sys/prctl.h>
// #include <linux/seccomp.h>


int32_t add_rule( char *program_name);
