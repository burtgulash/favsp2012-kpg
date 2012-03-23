CC = gcc
CCFLAGS = -ansi -pedantic -Wall -rdynamic

LIB  = $$(pkg-config --libs --cflags gtk+-3.0)
EXEC = upravovac
SRC  = main.c callback.c blur.c edge_detect.c gray_scale.c sharpen.c


$(EXEC) : $(SRC)
	$(CC) $(CCFLAGS) -o $(EXEC) $^ $(LIB)

.PHONY: clean

clean :
	rm $(EXEC)
