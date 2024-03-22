# Set up the G++ Compiler set the excutable file name  
CC = g++
CFLAGS = -Wall -g -std=c++11 -pthread
TARGET = assignment_4

# Compile the .cpp files 
all: 
	$(CC) $(CFLAGS) Thread.cpp -o $(TARGET)
# Set up clean to remove the excutable if we run make clean 
clean:
	rm $(TARGET)
