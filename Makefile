# Makefile by bootnecklad, bootnecklad@gmail.com
# Tested on Windows XP

OBJ = main.o

assmbler : $(OBJ)
	$(CC) -o $@ $^

clean :
	rm -f assembler $(OBJ)