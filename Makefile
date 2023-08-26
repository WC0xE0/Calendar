# Makefile for Project #4 (Calendar)

CC = gcc
CFLAGS = -ansi -Wall -g -O0 -Wwrite-strings -Wshadow \
         -pedantic-errors -fstack-protector-all -Wextra
PROGS = public01 public02 public03 public04 public05 public06 \
        student_tests

.PHONY: all clean

# First target (default target for make)
all: $(PROGS)

student_tests: student_tests.o calendar.o my_memory_checker_216.o
    $(CC) -o student_tests student_tests.o calendar.o \
    my_memory_checker_216.o

public%: public%.o calendar.o my_memory_checker_216.o
    $(CC) -o $@ $^

public%.o: public%.c calendar.h event.h my_memory_checker_216.h
    $(CC) $(CFLAGS) -c $<

student_tests.o: student_tests.c calendar.h event.h \
    my_memory_checker_216.h
    $(CC) $(CFLAGS) -c student_tests.c

calendar.o: calendar.c calendar.h event.h
    $(CC) $(CFLAGS) -c calendar.c

my_memory_checker_216.o: my_memory_checker_216.c my_memory_checker_216.h
    $(CC) $(CFLAGS) -c my_memory_checker_216.c

clean:
    rm -f *.o $(PROGS) a.out
