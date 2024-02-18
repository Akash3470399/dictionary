cc = gcc
cflags = -lm -I./intr/ -g

srcfiles = $(wildcard src/*.c)
objfiles = $(patsubst src/%.c, obj/%.o, $(srcfiles))


cmptrie : obj/cmptrie.o obj/bitsarr.o obj/rfd_utils.o
	$(cc) $^ main.c -o cmptrie $(cflags)

simplest : obj/simplest.o obj/bitsarr.o obj/rfd_utils.o
	$(cc) $^ -o simplest $(cflags)

obj/%.o : src/%.c
	@mkdir -p $(@D)
	$(cc) $< -o $@ -c $(cflags)

.PHONY : clean
clean:
	rm -f obj/* cmptrie simplest

