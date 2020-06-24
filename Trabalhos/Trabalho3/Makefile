all:
	gcc -o exec src/*.c -I./include

debug:
	gcc -o exec src/*.c -I./include -g -fsanitize=address -Wall

run:
	@./exec