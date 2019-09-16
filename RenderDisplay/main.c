/*
 * RenderDisplay.c
 *
 * Created: 15-Sep-19 5:17:23 PM
 * Author : Monmoy
 */ 

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

#include <stdlib.h>

// For Random Number
#include <time.h>
#include <stdlib.h>


volatile unsigned char background[16][16] = {
	"################",
	"#.F............#",
	"#....F......F..#",
	"#..#..#####..###",
	"#..#......#....#",
	"#..#.F....#.F..#",
	"#F.#####..###..#",
	"#.......F......#",
	"#..............#",
	"#.F...#F.####..#",
	"#.....#......F.#",
	"#..#.F#........#",
	"#..#..###.F##..#",
	"#.F....#.....F.#",
	"#......#.......#",
	"################"
};

unsigned char endScreen[16][16] = {
	"################",
	"................",
	"###.###.#.#.###.",
	"#...#.#.###.#...",
	"#.#.###.#.#.###.",
	"#.#.#.#.#.#.#...",
	"###.#.#.#.#.###.",
	"................",
	"................",
	"###.#.#.###.###.",
	"#.#.#.#.#...#.#.",
	"#.#.#.#.###.###.",
	"#.#.#.#.#...##..",
	"###.###.###.#.#.",
	"................",
	"################"
};

unsigned char winScreen[16][16] = {
	"################",
	"................",
	".#..#.####.#..#.",
	".#..#.#..#.#..#.",
	".####.#..#.#..#.",
	"..##..#..#.#..#.",
	"..##..####.####.",
	"................",
	"................",
	".#..#.###.#...#.",
	".#..#..#..##..#.",
	".#..#..#..#.#.#.",
	".####..#..#..##.",
	".#..#.###.#...#.",
	"................",
	"################"
};

unsigned char startScreen[16][16] = {
	".####.####.####.",
	".#..#.#..#.#....",
	".####.####.#....",
	".#....#..#.#....",
	".#....#..#.####.",
	"................",
	".####.####.####.",
	".#..#.#..#.#....",
	".####.####.#....",
	".#....#..#.#....",
	".#....#..#.####.",
	"................",
	".#..#.####.#..#.",
	".####.#..#.##.#.",
	".#..#.####.#.##.",
	".#..#.#..#.#..#.",
};

int ZERO = 0;
const int BLINK_RANGE = 128;
const int RENDER_DELAY_US = 25;
const int FRAME_RATE = 12;			/// RENDER_DELAY_US * FRAME_RATE should be ~250
const int BUZZER_DURATION = 5;

const int dx[4] = {-1, 0, 1,  0};
const int dy[4] = { 0, 1, 0, -1};
	
#define GHOST_COUNT 1	

volatile int foodBlinkState;
volatile int gameOver;
volatile int gameWin;
volatile int buzzerRemainingTime;

int px = 13, py = 8;
int gx[GHOST_COUNT] = {13};
int gy[GHOST_COUNT] = {5};


void reset() {
	PORTC |= 0b11000000;
}

void ledMatrixInit()
{
	
	/*``
	********** Common Anode ***********
	A - C0
	B - C1
	C - C2
	D - C3
	************* R ***************
	A - A0
	B - A1
	C - A2
	D - A3
	Control - C6
	************* G ***************
	A - A4
	B - A5
	C - A6
	D - A7
	Control - C7
	*/
	
	DDRA = 0b11111111;
	DDRC = 0b11001111;
	PORTC |= 0b11000000;
}

void controlInit() {
	// B0, B1
	DDRB &= 0b11111100;
}

void buzzerInit() {
	// B2
	DDRB |= 0b00000100;
}

void setBoardCommon(unsigned char row) {
	PORTC &= 0xF0;
	PORTC |= (0x0F & row);
}


void setBoardRed(unsigned char column) {
	PORTC &= 0b10111111;
	PORTA &= 0xF0;
	PORTA |= (0x0F & column);
}


void setBoardGreen(unsigned char column) {
	PORTC &= 0b01111111;
	PORTA &= 0x0F;
	PORTA |= (0xF0 & (column << 4));
}

volatile unsigned char board[16][16];
void makeBoard() {
	for (int i=0; i<16; i++)
		for (int j=0; j<16; j++)
			board[i][j] = background[i][j];
	
	board[px][py] = board[px+1][py] = board[px][py+1] = board[px+1][py+1] = 'P';
	
	for (int i=0; i<GHOST_COUNT; i++)
	board[gx[i]][gy[i]] = board[gx[i]+1][gy[i]] = board[gx[i]][gy[i]+1] = board[gx[i]+1][gy[i]+1] =  '1';
}

void displayBoard() {
	for (int i=0; i<16; i++) {
		setBoardCommon(i);
		for (int j=0; j<16; j++) {
			if (board[i][j] == '.')			continue;
			else if (board[i][j] == '#') {
				 setBoardRed(j);
			}
			else if (board[i][j] == '1') {
				setBoardGreen(j);
			}
			else if (board[i][j] == 'P') {
				setBoardGreen(j);
				setBoardRed(j);
			}
			else if (board[i][j] == 'F') {
				foodBlinkState = (foodBlinkState+1)%BLINK_RANGE;
				if (foodBlinkState<BLINK_RANGE/2)		continue;
				setBoardGreen(j);
				setBoardRed(j);
				
			}
			_delay_us(RENDER_DELAY_US);
			reset();
		}	
	}
	
}

int clash(int px, int py, int gx, int gy) {
	for (int i=0; i<2; i++)
		for (int j=0; j<2; j++)
			for (int k=0; k<2; k++)
				for (int l=0; l<2; l++)
					if (px+i==gx+k && py+j == gy+l)
						return 1;
	return 0;
}

void movePacman(int dir) {
	int nx = px + dx[dir];
	int ny = py + dy[dir];
	if (background[nx][ny] == '#' || background[nx+1][ny] == '#' ||
		background[nx][ny+1] == '#' || background[nx+1][ny+1] == '#')		return;
	
	for (int i=0; i<GHOST_COUNT; i++) {
		int ggx = gx[i];
		int ggy = gy[i];
		if (clash(px, py, ggx, ggy)) {
			gameOver = 1;
			return;
		}
	}
	
	px = nx;
	py = ny;
	
	if (background[nx][ny] == 'F' || background[nx][ny+1] == 'F' ||
	background[nx+1][ny] == 'F' || background[nx+1][ny+1] == 'F') {
		buzzerRemainingTime = BUZZER_DURATION;
	}
	
	if (background[nx][ny] == 'F')		background[nx][ny] = '.';
	if (background[nx+1][ny] == 'F')	background[nx+1][ny] = '.';
	if (background[nx][ny+1] == 'F')	background[nx][ny+1] = '.';
	if (background[nx+1][ny+1] == 'F')  background[nx+1][ny+1] = '.';
}

void buzz() {
	if (buzzerRemainingTime > 0)		PORTB |= 0b00000100;
	else								PORTB &= 0b11111011;
	if (buzzerRemainingTime) buzzerRemainingTime--;
}

/// 0 = nothing, 1 = blocked, 2 = Pacman Fucked
int ghostMoveResult(int id, int dir) {
	int cx = gx[id], cy = gy[id];
	int nx = cx + dx[dir];
	int ny = cy + dy[dir];
	
	if (background[nx][ny] == '#' || background[nx+1][ny] == '#' ||
		background[nx][ny+1] == '#' || background[nx+1][ny+1] == '#')		return 0;	
	
	if (clash(px, py, nx, ny))	return 2;
	return 1;
}

void ghostMove(int id, int dir) {
	int cx = gx[id], cy = gy[id];
	int nx = cx + dx[dir];
	int ny = cy + dy[dir];
		
	if (background[nx][ny] == '#' || background[nx+1][ny] == '#' ||
	background[nx][ny+1] == '#' || background[nx+1][ny+1] == '#')		return;
		
	if (clash(px, py, nx, ny))	{
		gameOver = 1;
		return ;
	}
	
	gx[id] = nx;
	gy[id] = ny;
}


void displayStartScreen()
{
	for (int i=0; i<16; i++) {
		setBoardCommon(i);
		for (int j=0; j<16; j++) {
			if (startScreen[i][j] == '.')			continue;
			else if (startScreen[i][j] == '#') {
				setBoardRed(j);
			}
			_delay_us(RENDER_DELAY_US);
			reset();
		}
	}
}

void displayGameOverScreen()
{
	for (int i=0; i<16; i++) {
		setBoardCommon(i);
		for (int j=0; j<16; j++) {
			if (endScreen[i][j] == '.')			continue;
			else if (endScreen[i][j] == '#') {
				setBoardRed(j);
			}
			_delay_us(RENDER_DELAY_US);
			reset();
		}
	}
}


void displayWinScreen()
{
	for (int i=0; i<16; i++) {
		setBoardCommon(i);
		for (int j=0; j<16; j++) {
			if (winScreen[i][j] == '.')			continue;
			else if (winScreen[i][j] == '#') {
				setBoardRed(j);
			}
			_delay_us(RENDER_DELAY_US);
			reset();
		}
	}
}


int isFoodLeft() {
	for (int i=0; i<16; i++) {
		for (int j=0; j<16; j++) {
			if (background[i][j] == 'F')			return 1;
		}
	}
	return 0;
}

int randomInt(int max)
{
	int r = rand();
	return (r % max);
}

int main(void)
{
	MCUCSR |= 1 << JTD;
	MCUCSR |= 1 << JTD;

	ledMatrixInit();
	controlInit();
	buzzerInit();
	
	//Random Init
	srand(time(NULL));   
    /* Replace with your application code */
	
	for (int i=0; i<200; i++)
	{
		displayStartScreen();
	}
	
	while(1) {
		int direction = PINB & 0x03;
		movePacman(direction);
		makeBoard();
		for (int i=0; i<FRAME_RATE; i++) {
			displayBoard();
			buzz();
		}
		if (!isFoodLeft())	gameWin = 1;
		if(gameOver || gameWin)		break;
	}
	
	while(gameOver) {
		displayGameOverScreen();
	}
	
	while(gameWin) {
		displayWinScreen();
	}	
}

