cc = gcc
cflags = -lm -I./intr/

srcfiles = $(wildcard src/*.c)
objfiles = $(patsubst src/%.c, obj/%.o, $(srcfiles))

dict_a : src/dict_agent.c obj/sqlite3.o obj/err_handler.o obj/cmptrie.o obj/bitsarr.o obj/rfd_utils.o 
	$(cc) $^ -o dict_a $(cflags) 

user_a : src/user_agent.c obj/sqlite3.o obj/err_handler.o
	$(cc) $^ -o user_a $(cflags)

simplest : obj/simplest.o obj/bitsarr.o obj/rfd_utils.o
	$(cc) $^ -o simplest $(cflags)

obj/%.o : src/%.c
	@mkdir -p $(@D)
	$(cc) $< -o $@ -c $(cflags)

.PHONY : clean
clean:
	rm -f obj/* cmptrie simplest logs

