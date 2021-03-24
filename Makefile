CC=gcc

main_source := $(wildcard src/*.c)
main_headers := $(wildcard src/*.h)
data := $(wildcard data/*)

.PHONY: all
all: bin/ogldev.exe bin/data

bin/ogldev.exe: inter/main.o
	$(CC) -g inter/main.o extern/lib/libcimgui.a -o bin/ogldev.exe -lstdc++ -pthread -lmingw32 -Lextern\lib -lassimp -lminizip -lz -lSDL2main -lSDL2 -lopengl32 -lglew32 -lm

inter/main.o: $(main_source) $(main_headers)
	$(CC) -g -c src/main.c -o inter/main.o -Iextern/include

bin/data: $(data)
	cp -r data/* bin/

.PHONY: clean
clean:
	rm -f inter/main.o
	rm -f bin/ogldev.exe
	rm -f bin/shaders/*
