// 
// Filename: main.c
// Created: 2018-02-13 18:49:32 +0100
// Author: Felix Nared
//

#include <ncurses.h>

#include "tetris.h"

int main(int argc, char *argv[])
{
  initscr();
  printw("Hello World!");
  refresh();
  getch();
  endwin();
  
  return 0;
}
