CC = gcc
CFLAGS = -Wall -g
LIB = -pthread -lseccomp
OUTPUT_OPTION = -MMD -MP -o $@

SOURCE = sandbox.c compile.c rule.c
OBJS = $(SOURCE:.c=.o)
DEPS = $(SOURCE:.c=.d)
-include ${DEPS}

sandbox: ${OBJS}
	${CC} ${OBJS} ${LIB} -o sandbox

test:
	# sandbox program.c file_in file_out file_err file_result time_limit memory_limit output_limit process_limit
	./sandbox test.c file_input.txt file_output.txt file_error.txt file_result.txt 0 2500 1024 1

help:
	-@echo "make help: display this help message"
	-@echo "make sandbox: compile the sandbox.c"
	-@echo "make test: run the sandbox with the arguments"
	-@echo "make clean: clear the directory free of garbage"

clean:
	rm *.d
	rm *.o
	rm sandbox
	-@rm -f main