#ifndef libs
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <time.h>
    #include <ncurses.h>
    #include "game_logic.h"
    #define libs
#endif

void rank_player(int score, char *player_name){
    FILE *rank = fopen("rankings.txt","a");
    fprintf(rank,"\n%d %s",score,player_name); 
    fclose(rank);
}

void chg_player_name(char **name, WINDOW *menu){
    int i; //contor
    int y = getmaxy(menu), x = getmaxx(menu); //coordonatele maxime

    //curatam eventualul mesaj de eroare
    for(i = 0; i < x - 2; i++){
            mvwaddch(menu, y - 2, i + 1, ' ');
    }

    char *new_name; //nu mai mult de 14 caractere
    new_name = (char*)malloc(30); //fixam o limita rezonabila pentru a nu iesi din memorie
    curs_set(1); //facem cursorul vizibil

    wmove(menu, y - 2, 1); //mutam cursorul la final de fereastra
    echo(); //permitem utilizatorului sa vada ce tasteaza
    //citim input-ul utilizatorului
    wscanw(menu,"%s",new_name);
    if((strlen(new_name) < 15) && (strlen(new_name) >= 3)){
        //curatam vechiul nume
        for(i = 0; i < strlen(*name); i++){
            mvwaddch(menu, y - 3, x / 2 + 5 + i, ' ');
        }
        *name = realloc(*name, strlen(new_name) + 1); //realocam memoria pentru nume
        strcpy(*name,new_name);
    }
    else if(strlen(new_name) >= 15){
        //s-au introdus prea multe caractere
        mvwaddstr(menu, y - 2, 1, "Alege maxim 14 caractere");
        return;
    }
    else{
        //s-au introdus prea putine caractere
        mvwaddstr(menu, y - 2, 1, "Alege macar 3 caractere");
        return;
    }
    noecho();
    curs_set(0); //scapam de cursor
    
    //curatam input-ul utilizatorului de pe ecran
    for(i = 0; i < strlen(new_name); i++){
        mvwaddch(menu, y - 2, i + 1, ' ');
    }
    mvwprintw(menu, y - 3, x / 2 + 5, "%s", *name);
    
    free(new_name);
    wrefresh(menu);
}

void play(WINDOW *menu, WINDOW *game, FILE **intrebari, int argc, char **menu_items, char **player_name){
    int i = 0; //contor
    static int flag_resume = 0;
    box(menu,0,0); //margini in jurul meniului
    keypad(menu,true); //pentru sagetele
    curs_set(0); //mascam cursorul

    //coordonate utile pentru pozitionare
    int menu_y_max, menu_x_max, menu_y_beg, menu_x_beg;
    getmaxyx(menu,menu_y_max,menu_x_max);
    getbegyx(menu, menu_y_beg, menu_x_beg);

    //titlu
    wattron(menu,COLOR_PAIR(1) | A_STANDOUT);
    mvwprintw(menu, 1, menu_x_max/2 - 7, "----TRIVIA----");
    wattroff(menu, A_STANDOUT | COLOR_PAIR(1));

    //afisam numele jucatorului
    mvwprintw(menu, menu_y_max - 3, menu_x_max / 2 - 3, "Player: %s", *player_name);
    wrefresh(menu);

    int sel = 0; //variabila ce retine ce element din meniu e selectat
    //cream meniul jocului
    for(i = 0; i < 4; i++){
        if(i == sel){
            wattron(menu, COLOR_PAIR(2) | A_REVERSE);
            mvwaddstr(menu, menu_y_max / 2 - 2 + 2 * i, menu_x_max / 2 - 3, menu_items[i]);
            wattroff(menu, A_REVERSE | COLOR_PAIR(2));
        }
        else{
            mvwaddstr(menu, menu_y_max / 2 - 2 + 2 * i, menu_x_max / 2 - 3, menu_items[i]);
        }
    }

    wrefresh(menu);
    int c = wgetch(menu); //user input
    while(1){
        while(c == KEY_DOWN || c == KEY_UP){
            if(c == KEY_DOWN){
                if(sel != 3){
                    sel++;
                }
                else{
                    sel = 0; //daca suntem la ultima selectie revenim la prima
                }
            }
            if(c == KEY_UP){
                if(sel > 0){
                    sel--;
                }
                else{
                    sel = 3; //daca suntem la prima selectie revenim la ultima
                }
            }

            //redesenam meniul
            for(i = 0; i < 4; i++){
                if(i == sel){
                    wattron(menu, COLOR_PAIR(2) | A_REVERSE);
                    mvwaddstr(menu, menu_y_max / 2 - 2 + 2 * i, menu_x_max / 2 - 3, menu_items[i]);
                    wattroff(menu, A_REVERSE | COLOR_PAIR(2));
                }
                else{
                    mvwaddstr(menu, menu_y_max / 2 - 2 + 2 * i, menu_x_max / 2 - 3, menu_items[i]);
                }
            }
            wrefresh(menu);
            c = wgetch(menu);
        }
    
        if(c == 10){
            //KEY_ENTER nu functioneaza for some reason
            switch(sel){
                case 0: 
                    start_game("new", game, intrebari, argc, *player_name, &flag_resume);
                    //redesenam meniul static (fara butoane)
                    box(menu,0,0);
                    wattron(menu,COLOR_PAIR(1) | A_STANDOUT);
                    mvwprintw(menu, 1, menu_x_max/2 - 7, "----TRIVIA----");
                    wattroff(menu, A_STANDOUT | COLOR_PAIR(1));
                    mvwprintw(menu, menu_y_max - 3, menu_x_max / 2 - 3, "Player: %s", *player_name);
                    wrefresh(menu);
                    c = wgetch(menu);   
                    continue;
                case 1:
                    if(flag_resume){
                        start_game("resume", game, intrebari, argc, *player_name, &flag_resume);
                        //redesenam meniul static (fara butoane)
                        box(menu,0,0);
                        wattron(menu,COLOR_PAIR(1) | A_STANDOUT);
                        mvwprintw(menu, 1, menu_x_max/2 - 7, "----TRIVIA----");
                        wattroff(menu, A_STANDOUT | COLOR_PAIR(1));
                        mvwprintw(menu, menu_y_max - 3, menu_x_max / 2 - 3, "Player: %s", *player_name);
                        wrefresh(menu);
                    } 
                    c = wgetch(menu);  
                    continue;
                case 2:
                    chg_player_name(player_name, menu);
                    c = wgetch(menu);
                    continue;
                case 3:
                    endwin();
                    return;
            }
        }
        else{
            c = wgetch(menu);
        }
    }
}

void start_game(const char *mode, WINDOW *game, FILE **intrebari, int argc, char *player_name, int *flag_resume){
    int i = 0, j = 0; //contoare
    static int score = 0;
    static int flag_opt[3] = { 0 }; //care verifica daca o var ajut a fost sau nu folosita
    char *options[] = {"skip one", "fifty-fifty", "ask a friend"};
    static int nrf = 1; //numarul fisierului de intrebari
    static int count = 0; //cate fisiere au ramas fara intrebari
    static int wrong_ans = 0, correct_ans = 0;
    static char prev_quest[100], ans1[30], ans2[30], ans3[30], ans4[30];
    static int corect[4] = { 0 }; //vector valoare de adevar raspunsuri
    static int cor;

    int y_max, x_max, y_beg, x_beg;
    getmaxyx(stdscr, y_max, x_max);
    getbegyx(stdscr, y_beg, x_beg);
    box(game,0,0);
    keypad(game,true); //pentru sagetele
    curs_set(0); //mascam cursorul
    //coordonate utile pentru pozitionare
    int game_y_max, game_x_max, game_y_beg, game_x_beg;
    getmaxyx(game,game_y_max,game_x_max);
    getbegyx(game, game_y_beg, game_x_beg);

    //titlu
    wattron(game,COLOR_PAIR(1) | A_STANDOUT);
    mvwprintw(game, 1, game_x_max/2 - 7, "----TRIVIA----");
    wattroff(game, A_STANDOUT | COLOR_PAIR(1));
    wrefresh(game);
    int sel = 0; //selectia raspunsului
    char aux[200];
    char *quest, **answ = calloc(4,sizeof(char*));
    if(strcmp(mode,"new") == 0){
        //incepem joc nou; reinitializam valorile statice
        score = 0;
        for(i = 0; i < 3; i++){
            flag_opt[i] = 0;
        }
        nrf = 1;
        count = 0;
        wrong_ans = correct_ans = 0;
        for(i = 0; i < 4; i++){
            corect[i] = 0;
        }
        //ne asiguram ca suntem la inceputul fisierului
        for(i = 1; i < argc; i++){
            fseek(intrebari[i],0,SEEK_SET);
        }
        //citim si afisam prima intrebare
        fgets(aux, 200, intrebari[nrf]);
        aux[strlen(aux) - 1] = '\0'; //scapam de ultimul \n citit
        while(feof(intrebari[nrf])){
            //verificam daca ne mai avem intrebari
            if(nrf == argc - 1){
                nrf = 1;
            }
            else{
                nrf++;
            }
            count++;
            if(count == argc - 1){
                //jocul s-a teminat; toate intrebarile au fost parcurse
                wclear(game);
                mvwprintw(game,game_y_max / 2, game_x_max / 2 - 29,
                "Congrats! You got %d points. You got %d right and %d wrong Press any key to exit.",score,correct_ans,wrong_ans);
                rank_player(score,player_name);
                *flag_resume = 0;
                wrefresh(game);
                wgetch(game);
                wclear(game);
                return;
            }
            fgets(aux, 200, intrebari[nrf]);
            aux[strlen(aux) - 1] = '\0';
        }
        quest = strdup(aux);
        mvwaddstr(game, game_y_max / 3, game_x_max / 2 - strlen(quest) / 2, quest);
        for(i = 0; i < 4; i++){
            fscanf(intrebari[nrf],"%s",aux);
            answ[i] = strdup(aux);
            if(i == sel){
                wattron(game, COLOR_PAIR(2) | A_REVERSE);
                mvwaddstr(game, game_y_max / 3 + 2 * i + 4, game_x_max / 2, answ[i]);
                wattroff(game, COLOR_PAIR(2) | A_REVERSE);
            }
            else{
                mvwaddstr(game, game_y_max / 3 + 2 * i + 4, game_x_max / 2, answ[i]);
            }
        }
        fscanf(intrebari[nrf],"%d",&cor);
        corect[cor] = 1; //flag pt raspunsul corect
    }
    else if(strcmp(mode,"resume") == 0){
        quest = strdup(prev_quest);
        answ[0] = strdup(ans1);
        answ[1] = strdup(ans2);
        answ[2] = strdup(ans3);
        answ[3] = strdup(ans4);
        mvwaddstr(game, game_y_max / 3, game_x_max / 2 - strlen(quest) / 2, quest);
        for(i = 0; i < 4; i++){
            if(i == sel){
                wattron(game, COLOR_PAIR(2) | A_REVERSE);
                mvwaddstr(game, game_y_max / 3 + 2 * i + 4, game_x_max / 2, answ[i]);
                wattroff(game, COLOR_PAIR(2) | A_REVERSE);
            }
            else{
                mvwaddstr(game, game_y_max / 3 + 2 * i + 4, game_x_max / 2, answ[i]);
            }
        }
    }
    mvwprintw(game,4,4,"score: %d  ",score);

    int c = wgetch(game);
    while(1){
        //afisam timpul curent la apasarea unei taste
        time_t raw_curr_time;
        struct tm *curr_time;
        time(&raw_curr_time);
        curr_time = localtime(&raw_curr_time);
        mvwprintw(game,6,4,"score: %s",asctime(curr_time));

        while(c == KEY_DOWN || c == KEY_UP){
            time(&raw_curr_time);
            curr_time = localtime(&raw_curr_time);
            mvwprintw(game,6,4,"time: %s",asctime(curr_time));
            if(c == KEY_DOWN){
                if(sel != 3){
                    sel++;
                }
                else{
                    sel = 0; 
                }
            }
            if(c == KEY_UP){
                if(sel > 0){
                    sel--;
                }
                else{
                    sel = 3; 
                }
            }

            for(i = 0; i < 4; i++){
                if(i == sel){
                    wattron(game, COLOR_PAIR(2) | A_REVERSE);
                    mvwaddstr(game, game_y_max / 3 + 2 * i + 4, game_x_max / 2, answ[i]);
                    wattroff(game, COLOR_PAIR(2) | A_REVERSE);
                }
                else{
                    mvwaddstr(game, game_y_max / 3 + 2 * i + 4, game_x_max / 2, answ[i]);
                }
            }
            wrefresh(game);
            c = wgetch(game);
        }
        if(c == 10){
            if(corect[sel] == 1){
                score += 10;
                correct_ans++;
                wattron(game,COLOR_PAIR(3));
                mvwprintw(game,4,4,"score: %d  ",score);
                wattroff(game,COLOR_PAIR(3));
            }
            else{
                if(score > 5){
                    score -= 5;
                    wrong_ans++;
                    wattron(game,COLOR_PAIR(4));
                    mvwprintw(game,4,4,"score: %d  ",score);
                    wattroff(game,COLOR_PAIR(4));
                }
                else{
                    score = 0;
                    wrong_ans++;
                    wattron(game,COLOR_PAIR(4));
                    mvwprintw(game,4,4,"score: %d  ",score);
                    wattroff(game,COLOR_PAIR(4));
                }       
            }
            fgets(aux,200,intrebari[nrf]);//citire falsa pt \n
            //permutam intre fisierele de intrebari
            if(nrf == argc - 1){
                nrf = 1;
            }
            else{
                nrf++;
            }
            for(j = 0; j < strlen(quest); j++){
                mvwaddch(game, game_y_max / 3, game_x_max / 2 - strlen(quest) / 2 + j, ' ');
            }
            fgets(aux,200,intrebari[nrf]);
            aux[strlen(aux) - 1] = '\0';
            count = 0;
            while(feof(intrebari[nrf])){
                //verificam daca ne mai avem intrebari
                if(nrf == argc - 1){
                    nrf = 1;
                }
                else{
                    nrf++;
                }
                count++;
                if(count == argc - 1){
                    wclear(game);
                    mvwprintw(game,game_y_max / 2, game_x_max / 2 - 29,
                    "Congrats! You got %d points. You got %d right and %d wrong Press any key to exit.",score,correct_ans,wrong_ans);
                    rank_player(score,player_name);
                    *flag_resume = 0;
                    wrefresh(game);
                    wgetch(game);
                    wclear(game);
                    return;
                }
                fgets(aux, 200, intrebari[nrf]);
                aux[strlen(aux) - 1] = '\0';
            }
            quest = realloc(quest, strlen(aux)+1);
            strcpy(quest, aux);
            mvwaddstr(game, game_y_max / 3, game_x_max / 2 - strlen(quest) / 2, quest);
            sel = 0;
            for(i = 0; i < 4; i++){
                for(j = 0; j < strlen(answ[i]); j++){
                    mvwaddch(game, game_y_max / 3 + 2 * i + 4, game_x_max / 2 + j, ' ');
                }
                fscanf(intrebari[nrf],"%s",aux);
                answ[i] = strdup(aux);
                if(i == sel){
                    wattron(game, COLOR_PAIR(2) | A_REVERSE);
                    mvwaddstr(game, game_y_max / 3 + 2 * i + 4, game_x_max / 2, answ[i]);
                    wattroff(game, COLOR_PAIR(2) | A_REVERSE);
                }
                else{
                    mvwaddstr(game, game_y_max / 3 + 2 * i + 4, game_x_max / 2, answ[i]);
                }
            }
            fscanf(intrebari[nrf],"%d",&cor);
            for(i = 0; i < 4; i++){
                corect[i] = 0;
            }
            corect[cor] = 1;
        }
        
        if(c == 'h'){
            //h pentru "help"
            int h_sel = 0;

            //afisam variantele ajutatoare
            for(i = 0; i < 3; i++){
                if(flag_opt[i] == 0){
                    if(i == h_sel){
                        wattron(game,COLOR_PAIR(2) | A_REVERSE);
                        mvwaddstr(game,game_y_max - 3,game_x_max / 2 - 13 + 13 * i,options[i]);
                        wattroff(game, COLOR_PAIR(2) | A_REVERSE);
                    }
                    else{
                        mvwaddstr(game,game_y_max - 3,game_x_max / 2 - 13 + 13 * i,options[i]);
                    }
                }
                else{
                    for(j = 0; j < strlen(options[i]); j++){
                        mvwaddch(game,game_y_max - 3,game_x_max / 2 - 13 + 13 * i + j,' ');
                    }
                }
            }
            wrefresh(game);
            c = wgetch(game);
            while(c == KEY_LEFT || c == KEY_RIGHT){
                time(&raw_curr_time);
                curr_time = localtime(&raw_curr_time);
                mvwprintw(game,6,4,"time: %s",asctime(curr_time));
                if(c == KEY_RIGHT){
                    if(h_sel != 2){
                        h_sel++;
                        if(flag_opt[h_sel] == 1){
                            h_sel++;
                        }
                    }
                    else{
                        h_sel = 0; 
                    }
                }
                if(c == KEY_LEFT){
                    if(h_sel > 0){
                        h_sel--;
                        if(flag_opt[h_sel] == 1){
                            h_sel--;
                        }
                    }
                    else{
                        h_sel = 2; 
                    }
                }
                for(i = 0; i < 3; i++){
                    if(flag_opt[i] == 0){
                        if(i == h_sel){
                            wattron(game,COLOR_PAIR(2) | A_REVERSE);
                            mvwaddstr(game,game_y_max - 3,game_x_max / 2 - 13 + 13 * i,options[i]);
                            wattroff(game, COLOR_PAIR(2) | A_REVERSE);
                        }
                        else{
                            mvwaddstr(game,game_y_max - 3,game_x_max / 2 - 13 + 13 * i,options[i]);
                        }
                    }
                    else{
                        for(j = 0; j < strlen(options[i]); j++){
                            mvwaddch(game,game_y_max - 3,game_x_max / 2 - 13 + 13 * i + j,' ');
                        }
                    }
                }
                wrefresh(game);
                c = wgetch(game);
            }
            if((c == 'h') || (c == KEY_UP) || (c == KEY_DOWN) || (c == 10)){
                if(c == 10){
                    switch(h_sel){
                        case 0:
                            //skip one question
                            if(flag_opt[h_sel] == 0){
                                fgets(aux,200,intrebari[nrf]);//citire falsa pt \n
                                //permutam intre fisierele de intrebari
                                if(nrf == argc - 1){
                                    nrf = 1;
                                }
                                else{
                                    nrf++;
                                }
                                for(j = 0; j < strlen(quest); j++){
                                    mvwaddch(game, game_y_max / 3, game_x_max / 2 - strlen(quest) / 2 + j, ' ');
                                }
                                fgets(aux,200,intrebari[nrf]);
                                aux[strlen(aux) - 1] = '\0';
                                while(feof(intrebari[nrf])){
                                    //verificam daca ne mai avem intrebari
                                    if(nrf == argc - 1){
                                        nrf = 1;
                                    }
                                    else{
                                        nrf++;
                                    }
                                    count++;
                                    if(count == argc - 1){
                                        wclear(game);
                                        mvwprintw(game,game_y_max / 2, game_x_max / 2 - 29,
                                        "Congrats! You got %d points. You got %d right and %d wrong Press any key to exit.",score,correct_ans,wrong_ans);
                                        rank_player(score,player_name);
                                        *flag_resume = 0;
                                        wrefresh(game);
                                        wgetch(game);
                                        wclear(game);
                                        return;
                                    }
                                    fgets(aux, 200, intrebari[nrf]);
                                    aux[strlen(aux) - 1] = '\0';
                                }
                                quest = realloc(quest, strlen(aux)+1);
                                strcpy(quest, aux);
                                mvwaddstr(game, game_y_max / 3, game_x_max / 2 - strlen(quest) / 2, quest);
                                sel = 0;
                                for(i = 0; i < 4; i++){
                                    for(j = 0; j < strlen(answ[i]); j++){
                                        mvwaddch(game, game_y_max / 3 + 2 * i + 4, game_x_max / 2 + j, ' ');
                                    }
                                    fscanf(intrebari[nrf],"%s",aux);
                                    answ[i] = strdup(aux);
                                    if(i == sel){
                                        wattron(game, COLOR_PAIR(2) | A_REVERSE);
                                        mvwaddstr(game, game_y_max / 3 + 2 * i + 4, game_x_max / 2, answ[i]);
                                        wattroff(game, COLOR_PAIR(2) | A_REVERSE);
                                    }
                                    else{
                                        mvwaddstr(game, game_y_max / 3 + 2 * i + 4, game_x_max / 2, answ[i]);
                                    }
                                }
                                fscanf(intrebari[nrf],"%d",&cor);
                                for(i = 0; i < 4; i++){
                                    corect[i] = 0;
                                }
                                corect[cor] = 1;
                                flag_opt[h_sel] = 1; //dezactivam optiunea
                            }
                            break;
                        case 1:
                            //fifty-fifty
                            if(flag_opt[h_sel] == 0){
                                int r = rand() % 5;
                                i = 0;
                                while(i < 2){
                                    if(corect[r] == 0){
                                        /*wattron(game,COLOR_PAIR(4));
                                        mvwaddstr(game, game_y_max / 3 + 2 * r + 4, game_x_max / 2 - 2, "x");
                                        wattroff(game,COLOR_PAIR(4));
                                        */
                                        for(j = 0; j < strlen(answ[r]); j++){
                                            mvwaddstr(game, game_y_max / 3 + 2 * r + 4, game_x_max / 2 + j, " ");
                                        }
                                        strcpy(answ[r]," ");
                                        r = rand() % 5;
                                        i++;
                                    }
                                    else{
                                        r = rand() % 5;
                                    }
                                }
                                flag_opt[h_sel] = 1;
                            }
                            break;
                        case 2:
                            //ask a friend
                            if(flag_opt[h_sel] == 0){
                                system("firefox google.com 2>/dev/null &");
                                flag_opt[h_sel] = 1;
                            }
                            break;
                    }
                }
                //ascundem variantele
                for(i = 1; i < game_x_max - 1; i++){
                    mvwaddch(game,game_y_max - 3,i,' ');
                }
            }
        }

        if(c == 'q'){
            *flag_resume = 1;
            strcpy(prev_quest,quest); //retinem intrebarea la care am ramas
            //retinem raspunsurile aferente
            strcpy(ans1,answ[0]);
            strcpy(ans2,answ[1]);
            strcpy(ans3,answ[2]);
            strcpy(ans4,answ[3]);
            free(quest);
            free(*answ);
            wclear(game);
            wrefresh(game);
            return;
        }
        else{
            c = wgetch(game);
        }
    }
    free(quest);
    free(*answ);
}