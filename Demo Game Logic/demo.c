#include<stdio.h>
#include "demo.h"

int main() {
    State cur = init();
    print(cur);

    while(1) {
        char c;
        int x;
        scanf(" %c", &c);

        if (c=='s') x = 0;
        else if (c=='d') x = 1;
        else if (c=='w') x = 2;
        else if (c=='a') x = 3;

        cur = move(cur, x);
        print(cur);
    }
}
