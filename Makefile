
build: main.c
	# gcc -ggdb main.c -o brainfuck
	gcc -march=native -g0 -s -Ofast -flto -fomit-frame-pointer main.c -o brainfuck
