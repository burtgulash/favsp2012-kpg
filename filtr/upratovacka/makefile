CC = gcc
CCFLAGS = -ansi -pedantic -Wall -rdynamic

LIB  = $$(pkg-config --libs --cflags gtk+-3.0)
EXEC = upratovac
SRC  = main.c callback.c filters.c


$(EXEC) : $(SRC)
	$(CC) $(CCFLAGS) -o $(EXEC) $^ $(LIB)

.PHONY: clean

clean :
	rm $(EXEC)
