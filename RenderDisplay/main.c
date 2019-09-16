/*
 * RenderDisplay.c
 *
 * Created: 15-Sep-19 5:17:23 PM
 * Author : Monmoy
 */ 

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

volatile const int BLINK_RANGE = 128;
volatile int foodBlinkState;
volatile int dx[4] = {-1, 0, 1,  0};
volatile int dy[4] = { 0, 1, 0, -1};
	

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

volatile unsigned char board[16][16];
#define GHOST_COUNT 1
int px = 13, py = 8;

const int gx[GHOST_COUNT] = {13};
const int gy[GHOST_COUNT] = {5};

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
	reset();
		
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

const int RENDER_DELAY = 30;


void MakeBoard() {
	for (int i=0; i<16; i++)
		for (int j=0; j<16; j++)
			board[i][j] = background[i][j];
	
	board[px][py] = board[px+1][py] = board[px][py+1] = board[px+1][py+1] = 'P';
	
	for (int i=0; i<GHOST_COUNT; i++)
	board[gx[i]][gy[i]] = board[gx[i]+1][gy[i]] = board[gx[i]][gy[i]+1] = board[gx[i]+1][gy[i]+1] =  '1';
}

void DisplayBoard() {
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
			_delay_us(RENDER_DELAY);
			reset();
		}	
	}
	
}

void MovePacmac(int dir) {
	int nx = px + 
}

int main(void)
{
	MCUCSR |= 1 << JTD;
	MCUCSR |= 1 << JTD;

	ledMatrixInit();

    /* Replace with your application code */
	
	while(1) {
		MakeBoard();
		DisplayBoard();
	}																			
}

