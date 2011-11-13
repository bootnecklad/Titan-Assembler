# Makefile by bootnecklad, bootnecklad@gmail.com
# Tested on Windows XP

OBJ = main.o

assembler : $(OBJ)
	$(CC) -o $@ $^

clean :
	rm -f assembler $(OBJ)