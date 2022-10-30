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

$(OBJ_CLIENT): $(C_FILES_CLIENT) | build
	$(CC) -g -c $< -o $@

$(OBJ_SERVER): $(C_FILES_SERVER) | build
	$(CC) -g -c $< -o $@



build:
	mkdir -p build
	mkdir -p bin
	mkdir -p db
	mkdir -p db/unreadMessages
	mkdir -p db/unreadNotifications
clean:
	rm -f $(OBJS) $(EXECS)
	rm -rf build
	rm -rf bin
	rm -rf db