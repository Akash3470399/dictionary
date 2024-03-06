cc = clang 
cflags = -I./intr/

srcfiles = $(wildcard src/*.c)
objfiles = $(patsubst src/%.c, obj/%.o, $(srcfiles))

dict_a : obj/sqlite3.o obj/err_handler.o obj/bitsarr.o obj/cmptrie.o src/dict_agent.c 
	$(cc) $^ -o dict_a $(cflags) -lpthread 

user_a : src/user_agent.c obj/sqlite3.o obj/err_handler.o
	$(cc) $^ -o user_a $(cflags) -lpthread

cmptrie : src/cmptrie.c obj/bitsarr.o main.c
	$(cc) $^ -o cmptrie $(cflags) 

simplest : obj/simplest.o obj/bitsarr.o 
	$(cc) $^ -o simplest $(cflags) -lm

obj/%.o : src/%.c
	@mkdir -p $(@D)
	$(cc) $< -o $@ -c $(cflags)

.PHONY : clean
clean:
	rm -f dict_a user_a simplest logs comm.db

