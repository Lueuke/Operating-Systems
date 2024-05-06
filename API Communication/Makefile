# Set up the G++ Compiler set the excutable file name  
CC = g++
CFLAGS = -Wall -g -std=c++11
TARGET = assignment_3 
LDFLAGS = -lcurl

# Compile the .cpp files 
all: 
	$(CC) $(CFLAGS) client.cpp -o $(TARGET) $(LDFLAGS)
# Set up clean to remove the excutable if we run make clean 
clean:
	rm $(TARGET)
