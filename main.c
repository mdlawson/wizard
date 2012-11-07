#include <ncurses.h>
#include <unistd.h>

typedef struct {
	int x;
	int y;
} block;

block player = {0,0};

void main(){
	initscr();
	cbreak();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	noecho();
	curs_set(0);
	int step = 17*1000;
	int in;
	int running = 1;
	while (running) {
		usleep(step);
		in = getch();
		switch(in){
			case 'q':
				running = 0;
				break;
			case KEY_UP:
				player.y--;
				break;
			case KEY_DOWN:
				player.y++;
				break;
			case KEY_RIGHT:
				player.x++;
				break;
			case KEY_LEFT:
				player.x--;
				break;
			default:
				break;
		}
		update();
		render();
	}
	endwin();
	return;
}

void render(){
	clear();
	mvaddch(player.y,player.x,'#');
	refresh();
}

void update(){

}

