#ifndef DEMO_H
#define DEMO_H

#include<string.h>

char start[8][8] = {
"........",
".....xx.",
".xxx..x.",
"......x.",
"..x.x...",
"..x..xx.",
"..xx..x.",
"........"
};

void assert(int b) {
    if (!b) 1/0;
}

typedef struct Coord {
    int x, y;
} Coord;

struct Coord newPosition(Coord old, int dir) {
    Coord ans;
    ans.x = old.x;
    ans.y = old.y;

    if (dir == 0)    ans.x++;
    else if (dir == 1)    ans.y++;
    else if (dir == 2)    ans.x--;
    else if (dir == 3)    ans.y--;
    return ans;
}

typedef struct State {
    char s[9][9];
    char bg[9][9];
    int ghostdir;
    Coord Pacman, Ghost;
    int status;

} State;


State setChar(State cur, Coord c, char ch) {
    cur.s[c.x][c.y] = ch;
    return cur;
}

char getChar(State cur, Coord c) {
    if (c.x < 0 && c.x >=8 && c.y < 0 && c.y >= 8)  return -1;
    return cur.s[c.x][c.y];
}

char getBGChar(State cur, Coord c) {
    if (c.x < 0 && c.x >=8 && c.y < 0 && c.y >= 8)  return -1;
    return cur.bg[c.x][c.y];
}

State setBGChar(State cur, Coord c, char ch) {
    cur.bg[c.x][c.y] = ch;
    return cur;
}


State init() {
    State ans;
    ans.ghostdir = 0;
    ans.status = 0;

    memset(ans.s, 0, sizeof ans.s);
    memset(ans.bg, 0, sizeof ans.bg);

    for (int i=0; i<8; i++)
        for (int j=0; j<8; j++)
            ans.s[i][j] = ans.bg[i][j] = start[i][j];

    ans.Pacman.x = ans.Pacman.y = 0;
    ans.Ghost.x = ans.Ghost.y = 7;
    ans = setChar(ans, ans.Pacman, 'P');
    ans = setChar(ans, ans.Ghost, 'G');
    ans = setBGChar(ans, ans.Pacman, ' ');
    return ans;
}

int win(State cur) {
    for (int i=0; i<8; i++) {
        for (int j=0; j<8; j++) {
            Coord c;
            c.x = i;
            c.y = j;
            if (getBGChar(cur, c) == '.')   return 0;
        }
    }
    return 1;
}

State move(State cur, int dir) {
    if (cur.status)     return cur;
    assert(getChar(cur, cur.Pacman) == 'P');
    assert(getChar(cur, cur.Ghost)  == 'G');

    Coord nw = newPosition(cur.Pacman, dir);
    if (getChar(cur, nw) == 'G') {
        cur = setChar(cur, cur.Pacman, ' ');
        cur.status = -1;
        return cur;
    }
    if (getChar(cur, nw) == '.' || getChar(cur, nw) == ' ') {
        cur = setChar(cur, cur.Pacman, ' ');
        cur.Pacman = nw;
        cur = setChar(cur, cur.Pacman, 'P');
        cur = setBGChar(cur, cur.Pacman, ' ');
    }

    while(1) {
        int nwdir = cur.ghostdir;
        if (rand()%2)            nwdir = nwdir;
        else if (rand()%5 == 0)  nwdir = (nwdir+2)%4;
        else if (rand()%2)       nwdir = (nwdir+3)%4;
        else                     nwdir = (nwdir+1)%4;

        Coord nw = newPosition(cur.Ghost, nwdir);
        if (getChar(cur, nw) != '.' && getChar(cur, nw) != ' ')    continue;


        char prv =  getChar(cur, nw);

        cur.ghostdir = nwdir;

        cur = setChar(cur, cur.Ghost, getBGChar(cur, cur.Ghost));
        cur.Ghost = nw;
        cur = setChar(cur, cur.Ghost, 'G');
        if (prv == 'P') cur.status = -1;
        break;
    }
    if (win(cur))   cur.status = 1;
    return cur;
}

void print(State cur) {
    if (cur.status == 1)        printf("You Won");
    if (cur.status == -1)       printf("You Lost");
    for (int i=0; i<8; i++) {
        for (int j=0; j<8; j++) {
            printf("%c ", cur.s[i][j]);
        }
        printf("\n");
    }
}

#endif
