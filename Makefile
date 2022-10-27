#Compiler flags
CC=gcc

#Project flags
C_FILES_SERVER = $(wildcard src/Server/*.c)
C_FILES_CLIENT = $(wildcard src/Client/*.c)
OBJ_SERVER = $(patsubst src/Server/%.c, build/%.o, $(C_FILES_SERVER))
OBJ_CLIENT = $(patsubst src/Client/%.c, build/%.o, $(C_FILES_CLIENT))
EXEC_SERVER = $(patsubst src/Server/%.c, bin/%, $(C_FILES_SERVER))
EXEC_CLIENT = $(patsubst src/Client/%.c, bin/%, $(C_FILES_CLIENT))

all: $(EXEC_SERVER) $(EXEC_CLIENT)

$(EXEC_SERVER): $(OBJ_SERVER)
	$(CC) -g -o $@ $^

$(EXEC_CLIENT): $(OBJ_CLIENT)
	$(CC) -g -o $@ $^

$(OBJ_SERVER): $(C_FILES_SERVER) | build
	$(CC) -c $< -o $@

$(OBJ_CLIENT): $(C_FILES_CLIENT) | build
	$(CC) -c $< -o $@

build:
	mkdir -p build
	mkdir -p bin

clean:
	rm -f $(OBJS) $(EXECS)
	rm -rf build
	rm -rf bin