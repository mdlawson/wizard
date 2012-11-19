#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//
// CONSTANTS
//

#define WIDTH 80
#define HEIGHT 30

#define MAX_BULLETS 20
#define BULLET_LIFE 30

#define MAX_MOBS 10

#define MAX_LOOT 3

#define SIGHT 12

#define STEP 20

// COLOURS

#define RED 10
#define GREEN 11
#define BLUE 12
#define YELLOW 13
#define MAGENTA 14
#define CYAN 15
#define RED_BG 20
#define GREEN_BG 21
#define BLUE_BG 22
#define YELLOW_BG 23
#define MAGENTA_BG 24
#define CYAN_BG 25
//
// GLOBALS
//

// Allows for easy access to directions
typedef enum {north,east,south,west} dir;

// Entity structure
typedef struct {
	int x;
	int y;
	int colour;
	char character;
	dir direction;
	int life;
	int attack;
} entity;

// Loot structure
typedef struct {
	int x;
	int y;
	int power;
} loot;

// Bullet structure
typedef struct {
	int x;
	int y;
	int colour;
	char character;
	dir direction;
	int life;
	int damage;
	int speed;
} bullet;

// Bullet pool
int nextBullet = 0;
bullet bullets[] = {[0 ... MAX_BULLETS-1] = {-1,-1,RED,'o',north,-1,1,1}};

// Mob pool
int nextMob = 0;
entity mobs[] = {[0 ... MAX_MOBS-1] = {-1,-1,RED_BG,'@',north, -1, 1}};

// Loot pool
int nextLoot = 0;
loot loots[] = {[0 ... MAX_LOOT-1] = {-1,-1,-1}};


// Map data
char map[HEIGHT][WIDTH] = {
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ','#','#','#','#','#','#','#','#','#','#','#','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#','#','#','#','#','#','#','#','#','#','+','+','+','+','+','+','+','+','+','+','+','+','+','#',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','#','#','#','#','#','#','#','#','+','#','#','#','#','#','#','#',' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','+','+','+','#',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ','#','#','#','#','#','#','#','#','+','#','#','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','+','+','+','#',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','#','+','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','#','#','#','#','+','#','#','#','#','#','#','#','#','#',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',' ',' ',' ',' ',' ',' ',' ',' ','#','#','#','#','+','#','#','#','#','#','#','#','#','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#','#','#','#','#','#','#','#','#','#','+','+','+','+','+','+','+','+','+','+','+','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#','#','#','#','#','#','#','#','#','#','#','#','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#','#','#','#','#','#','#','#','#','#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','#','#','#','#','#','#','#','#','#','#','+','#','#','#','#','#','#','#',' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','+','+','#','#','#','#','#','#','#','#','+','#','#','#','#','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ','#','#','#','#','#','#',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','#','#','#','#','#','#','#','#','#','#','#','#','#',' ',' ',' ',' ',' ',' ','#','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','#','#','#','#','#','#','#','#','#','#','#','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','#','#','+','#','#','#','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','#',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','+','+','+','+','+','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ','#','#','#','+','#','#',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','#',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','+','+','+','+','+','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','#',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','#','#','#','#','#','#','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','+','#','#','#','#','#','#','#','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','+','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',}
};

// Player definition
entity player = {10,4,BLUE_BG,' ', south, 20, 2};
int killCount = 0;

//
// MISC
//

// Function that returns a random integer between max and min
int randInt(int min, int max) { return (rand() % (max+1-min))+min; }

// Spawns a single mob at a random location, taken from free mobs in the mob pool.
void spawnMob() {
	int x,y,found = 0;
	while (!found) {
		x = randInt(0,WIDTH);
		y = randInt(0,HEIGHT);
		if (canMove(x,y,0)) {found = 1;}
	} 
	mobs[nextMob].x = x;
	mobs[nextMob].y = y;
	mobs[nextMob].life = 10;
	nextMob++;
	if (nextMob == MAX_MOBS) {
		nextMob = 0;
	}
}

// Spawns a loot at a random location, with random power
void spawnLoot() {
	int x,y,found = 0;
	while (!found) {
		x = randInt(0,WIDTH);
		y = randInt(0,HEIGHT);
		if (canMove(x,y,0)) {found = 1;}
	}
	loots[nextLoot].x = x;
	loots[nextLoot].y = y;
	loots[nextLoot].power = randInt(0,5);
	nextLoot++;
	if (nextLoot == MAX_LOOT) {
		nextLoot = 0;
	}
}

// Fires a bullet originating from the entity from, with speed and damage
void fireBullet(entity from, int speed){
	bullets[nextBullet].x = from.x;
	bullets[nextBullet].y = from.y;
	bullets[nextBullet].direction = from.direction;
	bullets[nextBullet].speed = speed;
	bullets[nextBullet].damage = from.attack;
	bullets[nextBullet].colour = from.colour-10;
	bullets[nextBullet].life = BULLET_LIFE;
	nextBullet++;
	if (nextBullet == MAX_BULLETS) {
		nextBullet = 0;
	}
}

// Tests if it is possible to move to a given location: Bullets pass though mobs so mobPassthough is set to 1
int canMove(int x, int y, int mobPassthough) {
	if (map[y][x] == '+') {
		if (!mobPassthough) {
			int i;
			for (i = 0; i < MAX_MOBS; i++) {
				if (mobs[i].life > -1 && x == mobs[i].x && y == mobs[i].y) {
					return 0;
				}
			}
		}	
		return 1;
	}	else {
		return 0;
	}
}

// Function to check if a square can be seen by an entity, using sight distance SIGHT
int canSee(entity from, int x, int y) {
	int dx = from.x - x;
	int dy = from.y - y;
	int d = sqrt(dx*dx+dy*dy);
	if (d > SIGHT) { return 0; } else { return 1; }
} 

//
// DRAWING
//

// Draws an entity
void drawBlock(entity obj){
	attron(COLOR_PAIR(obj.colour));
	mvaddch(obj.y,obj.x,obj.character);
	attroff(COLOR_PAIR(obj.colour));
}

// Draws a map, if SIGHT is non 0, only visible map portions are drawn
void drawMap(char data[HEIGHT][WIDTH]){
	int x,y;
	for (y = 0; y < HEIGHT; y++){
		for (x = 0; x < WIDTH; x++) {
			if (!SIGHT || (SIGHT && canSee(player,x,y))) {
				//attron(COLOR_PAIR(data[y][x]));
				mvaddch(y,x,data[y][x]);
				//attron(COLOR_PAIR(data[y][x]));	
			}
		}
	}
}

// Draws all the mobs, if SIGHT is non 0, only visible mobs are drawn
void drawMobs() {
	int i;
	for (i = 0; i < MAX_MOBS; i++) {
		if (mobs[i].life > -1) {
			if (!SIGHT || (SIGHT && canSee(player, mobs[i].x,mobs[i].y))) {
				drawBlock(mobs[i]);
			}
		}
	}
}

// Draws all the loots
void drawLoot(){
	int i;
	for (i = 0; i < MAX_LOOT; i++) {
		if (loots[i].power > -1) {
			if (!SIGHT || (SIGHT && canSee(player, loots[i].x, loots[i].y))) {
				attron(COLOR_PAIR(loots[i].power+10));
				mvaddch(loots[i].y,loots[i].x,'=');
				attroff(COLOR_PAIR(loots[i].power+10));
			}
		}
	}
}

// Draws all the bullets, if SIGHT is non 0, only visible bullets are drawn
void drawBullets(){
	int i;
	for (i = 0; i < MAX_BULLETS; i++){
		if (bullets[i].life > -1) {
			char c;
			// switch (bullets[i].direction) {
			// 	case north:
			// 	case south:
			// 		c = 'o';
			// 		break;
			// 	case east:
			// 	case west:
			// 		c = 'o';
			// 		break;
			// }
			if (!SIGHT || (SIGHT && canSee(player,bullets[i].x,bullets[i].y))) {
				attron(COLOR_PAIR(bullets[i].colour));
				mvaddch(bullets[i].y,bullets[i].x,bullets[i].character);
				attroff(COLOR_PAIR(bullets[i].colour));
			}
		}
	}
}

void drawUI() {
	mvprintw(0,0,"Life: %d",player.life);
	mvprintw(1,0,"Attack: %d",player.attack);
	mvprintw(2,0,"Kills: %d",killCount);
}

// 
// UPDATES
//

// Updates all the mobs
void updateMobs(){
	// 1/400 chance of spawing a mob each tick
	if (randInt(0,400) == 1) {
		spawnMob();
	}
	int i;
	for (i = 0; i < MAX_MOBS; i++) {
		if (mobs[i].life > -1) {
			int j;
			// If a live mob is on top of a live bullet, take damage
			for (j = 0; j < MAX_BULLETS; j++) {
				if (bullets[j].life > -1 && bullets[j].x == mobs[i].x && bullets[j].y == mobs[i].y) {
					mobs[i].life -= bullets[j].damage;
					bullets[j].life = -1;
					if (mobs[i].life < 0) { // mob just died.
						nextMob = i;
						killCount++;
					}
				}
			}
			// 4/50 chance of moving randomly each tick
			switch (randInt(0,50)) {
				case 0:
					mobs[i].direction = north;
					if (canMove(mobs[i].x,mobs[i].y-1,0)) { mobs[i].y--; }
					break;
				case 1:
					mobs[i].direction = south;
					if (canMove(mobs[i].x,mobs[i].y+1,0)) { mobs[i].y++; }
					break;
				case 2:
					mobs[i].direction = east;
					if (canMove(mobs[i].x+1,mobs[i].y,0)) { mobs[i].x++; }
					break;
				case 3:
					mobs[i].direction = west;
					if (canMove(mobs[i].x-1,mobs[i].y,0)) { mobs[i].x--; }
					break;
				default:
					break;
			}
			// 1/20 chance of attempting to attack each tick
			if (randInt(0,20) == 1) {
				// If we are on top of the player, hit them directly
				if (mobs[i].x == player.x && mobs[i].y == player.y) {
					player.life--;
				// else if we have line of sight with the player, turn and fire a bullet at them
				} else if (mobs[i].x == player.x) {
					if (player.y > mobs[i].y) { mobs[i].direction = south; } else { mobs[i].direction = north; }
					fireBullet(mobs[i],1);
				} else if (mobs[i].y == player.y) {
					if (player.x > mobs[i].x) { mobs[i].direction = east; } else { mobs[i].direction = west; }
					fireBullet(mobs[i],1);
				}
			}
		}
	}
}

// Updates all the loots
void updateLoot() {
	int i;
	if (randInt(0,800) == 1) {
		spawnLoot();
	}
	for (i = 0; i < MAX_LOOT; i++) {
		// handle loot pickup
		if (loots[i].power > -1 && player.x == loots[i].x && player.y == loots[i].y) {
			player.attack = loots[i].power;
			player.colour = loots[i].power+20;
			loots[i].power = -1;
			nextLoot = i;
		}
	}
}

// Updates all the bullets
void updateBullets() {
	int i;
	for (i = 0; i < MAX_BULLETS; i++) {
		if (bullets[i].life > -1) {
			// Move live bullets based on direction
			switch (bullets[i].direction) {
				case north:
					bullets[i].y-=bullets[i].speed;
					break;
				case south:
					bullets[i].y+=bullets[i].speed;
					break;
				case east:
					bullets[i].x+=bullets[i].speed;
					break;
				case west:
					bullets[i].x-=bullets[i].speed;
					break;
			}
			bullets[i].life--;
			// If we cant be here, die.
			if (!canMove(bullets[i].x,bullets[i].y,1)) { bullets[i].life = -1; }
			// if we die, mark bullet for recycling
			if (bullets[i].life == -1) {
				nextBullet = i;
			}
		}
	}
}

// Updates the player
void updatePlayer(){
	int i;
	for (i = 0; i < MAX_BULLETS; i++) {
		// Handle active bullet being on top of player -> Player takes damage
		if (bullets[i].life > -1 && bullets[i].x == player.x && bullets[i].y == player.y) {
			player.life -= bullets[i].damage;
			bullets[i].life = -1;
		}
	}
}

// Main render, calls all draw functions
void render(){
	clear();
	drawMap(map);
	drawBullets();
	drawLoot();
	drawBlock(player);
	drawMobs();
	drawUI();
	refresh();
}

// Main update, calls all update functions
void update(){
	updateBullets();
	updateMobs();
	updateLoot();
	updatePlayer();
}

// Main loop
void main(){

	// Seed random with time
	srand(time(NULL));

	initscr(); // init console
	cbreak();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	noecho();
	curs_set(0);
	start_color();

	// Colour pairs
	init_pair(1,COLOR_WHITE,COLOR_BLACK);
	init_pair(2,COLOR_BLACK,COLOR_WHITE);

	init_pair(RED,COLOR_RED,COLOR_BLACK);
	init_pair(GREEN,COLOR_GREEN,COLOR_BLACK);
	init_pair(BLUE,COLOR_BLUE,COLOR_BLACK);
	init_pair(CYAN,COLOR_CYAN,COLOR_BLACK);
	init_pair(MAGENTA,COLOR_MAGENTA,COLOR_BLACK);

	init_pair(RED_BG,COLOR_BLACK,COLOR_RED);
	init_pair(GREEN_BG,COLOR_BLACK,COLOR_GREEN);
	init_pair(BLUE_BG,COLOR_BLACK,COLOR_BLUE);
	init_pair(CYAN_BG,COLOR_BLACK,COLOR_CYAN);
	init_pair(MAGENTA_BG,COLOR_BLACK,COLOR_MAGENTA);

	int step = STEP*1000;
	int in;
	int running = 1;

	// Spawn one mob to get us started
	spawnMob();
	spawnLoot();

	while (running) {
		usleep(step);

		// Input
		in = getch();
		switch(in){
			case 'q':
				running = 0;
				break;
			case KEY_UP:
				player.direction = north;
				if (canMove(player.x,player.y-1,1)) { player.y--; }
				break;
			case KEY_DOWN:
				player.direction = south;
				if (canMove(player.x,player.y+1,1)) { player.y++; }
				break;
			case KEY_RIGHT:
				player.direction = east;
				if (canMove(player.x+1,player.y,1)) { player.x++; }
				break;
			case KEY_LEFT:
				player.direction = west;
				if (canMove(player.x-1,player.y,1)) { player.x--; }
				break;
			case 'z':
				fireBullet(player,1);
			default:
				break;
		}

		// Death
		if (player.life < 0) {
			clear();
			mvprintw(14,31,"### GAME OVER ###");
			refresh();
			usleep(5000000);
			running = 0;
		}
		update();
		render();
	}
	endwin();
	return;
}

