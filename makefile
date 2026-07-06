CC = gcc
OBJ = function.o FileSystem.o main.o

EXE = run
all: $(EXE)
.c.o: ; $(CC) -c $*.c

$(EXE): $(OBJ)
	$(CC) -o $@ $(OBJ)

clean:
	del -rf $(EXE) *.o *.d core

remake:
	make clean
	make

rerun:
	make remake
	./$(EXE)

