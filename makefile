cc = gcc
cflags = -lm -I./intr/

srcfiles = $(wildcard src/*.c)
objfiles = $(patsubst src/%.c, obj/%.o, $(srcfiles))

run : $(objfiles)
	$(cc) $^ -o run $(cflags)

obj/%.o : src/%.c
	@mkdir -p $(@D)
	$(cc) $< -o $@ -c $(cflags)

.PHONY : clean
clean:
	rm -f obj/* run

