#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "map.h"

typedef enum {north,east,south,west} dir;

int randInt(int min, int max) { return (rand() % (max+1-min))+min; }

void split(char data[HEIGHT][WIDTH], int x1, int y1, int x2, int y2, int depth) {
  int divider = 0;
  if (depth == 0 || x2 - x1 <= BUFFER || y2 - y1 <= BUFFER) {
    int y,x;
    for (y = y1; y < y2; y++) {
      for (x = x1; x < x2; x++) {
        data[y][x] = '+';
      }
    }
  } else {
    depth--;
    if (randInt(0,1)) {
      divider = randInt(x1+BUFFER,x2-BUFFER);
      split(data,x1,y1,x1+divider,y2,depth);
      split(data,x1+divider+1,y1,x2,y2,depth);
    } else {
      divider = randInt(y1+BUFFER,y2-BUFFER);
      split(data,x1,y1,x2,y1+divider,depth);
      split(data,x1,y1+divider+1,x2,y2,depth);
    }
  }
}

void genMap(char data[HEIGHT][WIDTH], int depth) {
  srand(time(NULL));
  int y,x;
  for (y = 0; y < HEIGHT; y++) {
    for (x = 0; x < WIDTH; x++) {
      data[y][x] = ' ';
    }
  }
  split(data,1,1,WIDTH-1,HEIGHT-1,depth);
}

void main() {
  char map[HEIGHT][WIDTH];
  genMap(map,4);
  int y, x;
  for (y = 0; y < HEIGHT; y++) {
    for (x = 0; x < WIDTH; x++) {
      printf("%c", map[y][x]);
    }
    printf("\n");
  }
}