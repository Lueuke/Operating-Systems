# Set up the G++ Compiler set the excutable file name  
CC = g++
CFLAGS = -Wall -g -std=c++11 
TARGET = assignment_5

# Compile the .cpp files 
all: 
	$(CC) $(CFLAGS) scheduler.cpp -o $(TARGET)
# Set up clean to remove the excutable and make log if we run make clean 
clean:
	rm $(TARGET)
	rm make.log
