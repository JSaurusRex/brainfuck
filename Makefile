
build: main.c
	# gcc -ggdb sw_main.c -o brainfuck
	gcc -g0 -s -O2 main.c -o brainfuck -pipe -fomit-frame-pointer
	# gcc -O2 sw_main.c -o brainfuck_sw -pipe
	# gcc -march=native -g0 -s -O2 -flto fp_main.c -o brainfuck_fp -pipe
	# gcc -g0 -s -O3 main.c -o brainfuck
