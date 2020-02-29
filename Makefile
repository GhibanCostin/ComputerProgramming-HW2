n=10

build: main.c game_logic.c
	gcc -g -Wall $^ -o trivia -lncurses
run: trivia
	./trivia matematica.txt istorie.txt literatura.txt

top: rankings.txt
	sort -gr $^ | head -n $(n) $^ && echo

.PHONY: clean
clean:
	rm trivia
