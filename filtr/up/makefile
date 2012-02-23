CFLAGS = -rdynamic -Wall 

LIB  = $$(pkg-config --libs --cflags gtk+-3.0)
EXEC = upratovacka
GUI  = up.glade
SRC  = up.c

$(EXEC) : $(SRC)
	gcc $(CFLAGS) $^ $(LIB) -o $@

.PHONY: clean

clean:
	rm *.o $(EXEC)
