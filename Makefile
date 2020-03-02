
build: main.c
	gcc -march=native -g0 -s -Ofast -flto -fomit-frame-pointer main.c -o brainfuck
