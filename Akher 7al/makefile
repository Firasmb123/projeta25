CC = gcc
CFLAGS = -Wall -g
SDL_FLAGS = -lSDL -lSDL_image -lSDL_ttf

# Object files
OBJS = main.o fonction.o

# Output executable
TARGET = jeu

# Compile the program
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(SDL_FLAGS)  # <-- this line should start with a tab

# Compile main.o
main.o: main.c fonction.h
	$(CC) $(CFLAGS) -c main.c  # <-- this line should start with a tab

# Compile fonction.o
fonction.o: fonction.c fonction.h
	$(CC) $(CFLAGS) -c fonction.c  # <-- this line should start with a tab

# Clean up object files and executable
clean:
	rm -f $(OBJS) $(TARGET)  # <-- this line should start with a tab
