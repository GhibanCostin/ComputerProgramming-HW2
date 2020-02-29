#ifndef libs
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <time.h>
    #include <ncurses.h>
    #include "game_logic.h"
    #define libs
#endif

int main(int argc, char const *argv[])
{
    if(argc == 1){
        printf("[Eroare]: Nu s-au dat argumente in linia de comanda");
        return 1;
    }
    srand(time(0)); //seed pentru random
    int i = 0, j = 0; //contoare
    char *menu_items[] = {"New Game", "Resume Game", "Change Player Name", "Quit"};
    char *player_name;
    player_name = (char*)malloc(6);
    strcpy(player_name,"Guest");

    FILE **intrebari = malloc((argc - 1) * sizeof(FILE*));
    for(i = 1; i < argc; i++){
        intrebari[i] = fopen(argv[i],"r");
    }

    //initializam ncurses stdscr
    initscr();
    raw();
    start_color();
    init_pair(1,COLOR_BLACK,COLOR_CYAN); //culoare pentru titlu
    init_pair(2,COLOR_WHITE,COLOR_BLACK); //culoare pentru selectii
    init_pair(3,COLOR_GREEN,COLOR_BLACK); //culoare rasp corect
    init_pair(4,COLOR_RED,COLOR_BLACK); //culoare rasp gresit
    noecho();

    //obtinem coordonatele de inceput si maxime
    int y_max, x_max, y_beg, x_beg;
    getmaxyx(stdscr, y_max, x_max);
    getbegyx(stdscr, y_beg, x_beg);

    //ferestre diferite pentru meniu si joc
    WINDOW *menu, *game; 
    int menu_height = (y_max - y_beg) / 2, menu_width = (x_max - x_beg) / 2;
    int menu_start_y = (y_max - y_beg) / 4, menu_start_x = (x_max - x_beg) / 4;
    menu = newwin(menu_height, menu_width, menu_start_y, menu_start_x);

    int game_height = (y_max - y_beg) / 2 + y_max / 4, game_width = (x_max - x_beg) / 2 + x_max / 4;
    int game_start_y = (y_max - y_beg) / 8, game_start_x = (x_max - x_beg) / 8;
    game = newwin(game_height, game_width, game_start_y, game_start_x);
    refresh();

    //initializam meniul si jocul cu toate functionalitatile lor
    play(menu, game, intrebari, argc, menu_items, &player_name);

    //inchidem fisierele cu intrebari
    for(i = 1; i < argc; i++){
        fclose(intrebari[i]);
    }

    //eliberam memoria alocata dinamic
    free(player_name);
    free(intrebari);
    return 0;
}
