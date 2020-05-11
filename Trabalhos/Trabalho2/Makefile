all:
	gcc -o exec src/*.c -I./include

debug:
	gcc -o exec src/binarioNaTela.c src/registro.c src/main.c -I./include -g -fsanitize=address

run:
	@./exec