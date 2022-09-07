#__________________________________________________________
# VARIABLES START HERE
#
# CURRENT ASSIGMENT
ASG_NICKNAME = HW21
BASE_NAME = huffman
#
# SOURCE FILENAMES
SRC_C = $(BASE_NAME).c
TEST_C = test_$(SRC_C) frequencies.c priority_queue.c bit_writer.c
SRC_H = $(BASE_NAME).h miniunit.h clog.h frequencies.h priority_queue.h bit_writer.h
SUBMIT_FILES = $(SRC_C) $(TEST_C) $(SRC_H) Makefile result1.txt result2.txt result3.txt result4.txt result5.txt result6.txt test.txt test1.txt test2.txt test3.txt test4.txt test5.txt test6.txt test_table.txt
#
# EXECUTABLES
EXECUTABLE = test_$(BASE_NAME)
EXECUTABLE_GCOV = $(EXECUTABLE)_gcov
#
# SYSTEM
SHELL = /bin/bash
CC = gcc
CFLAGS = -g -std=c11 -Wall -Wshadow -Wvla -Werror -pedantic
CFLAGS_GCOV = $(CFLAGS) -fprofile-arcs -ftest-coverage
#__________________________________________________________
# RULES START HERE.
$(EXECUTABLE): $(SRC_C) $(TEST_C) $(SRC_H)
	$(CC) -o $(EXECUTABLE) $(SRC_C) $(TEST_C) $(CFLAGS)

test: $(EXECUTABLE)
	./$(EXECUTABLE)

submit: $(SUBMIT_FILES)
	264submit $(ASG_NICKNAME) $(SUBMIT_FILES)

pretest: submit
	264test $(ASG_NICKNAME)

coverage: $(SRC_C) $(TEST_C)
	$(CC) -o $(EXECUTABLE_GCOV) $(SRC_C) $(TEST_C) $(CFLAGS_GCOV) 
	./$(EXECUTABLE_GCOV)
	gcov -f $(SRC_C)

clean: 
	rm -f $(EXECUTABLE) $(EXECUTABLE_GCOV) *.c.gcov *.gcda *.gcno

.PHONY: test submit pretest coverage clean 
