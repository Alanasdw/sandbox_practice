#pragma once

#include <seccomp.h>
#include <stdint.h>
#include <stdio.h>

#include <sys/prctl.h>


int32_t add_rule( char *program_name);
