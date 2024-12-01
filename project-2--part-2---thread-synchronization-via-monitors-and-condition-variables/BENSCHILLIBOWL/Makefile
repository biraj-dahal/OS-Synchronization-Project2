# Define the compiler
CC=gcc

# Include necessary flags
CFLAGS=-I. -pthread -std=c99

# Define dependencies and object files
DEPS = BENSCHILLIBOWL.h
OBJ = BENSCHILLIBOWL.o main.o 

# Compile object files
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Link the object files into the final executable
main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
