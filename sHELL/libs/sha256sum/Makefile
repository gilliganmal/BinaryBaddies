
CC=x86_64-w64-mingw32-gcc
.PHONY: clean build 
NAME=sha256sum
CFLAGS=-Wall -fPIC -shared -nostdlib -ffreestanding -s 
LDFLAGS=-lkernel32 -lbcrypt
all: build dist 

build:
	$(CC) $(CFLAGS) $(NAME).c  -e DllMain  $(LDFLAGS) -o $(NAME).dll


dist: build 
	cp $(NAME).dll ../../dist/

clean:
	-rm *.exe 
	-rm *.dll 