
build: main.c
	# gcc -ggdb main.c -o brainfuck -g3 -pg
	gcc -g0 -s -Ofast -flto main.c -o brainfuck -pipe
	# gcc -g0 -s -Ofast -flto old_main.c -o oldBrainfuck -pipe
	# gcc -g0 -s -O3 main.c -o brainfuck -pipe
	# clang -g0 -s -O3 main.c -o brainfuck_clang -flto -std=c17
	# gcc -O2 sw_main.c -o brainfuck_sw -pipe
	# gcc -march=native -g0 -s -O2 -flto fp_main.c -o brainfuck_fp -pipe
	# gcc -g0 -s -O3 main.c -o brainfuck
