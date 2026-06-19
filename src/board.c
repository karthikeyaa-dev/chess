#include "board.h"
#include <stdio.h>
#include <string.h>

void init_board(Board *board) {
    memset(board->squares, ' ', SIZE * SIZE);

    char black_pieces[] = {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'};
    for (int x = 0; x < SIZE; x++) {
        board->squares[0][x] = black_pieces[x]; 
        board->squares[1][x] = 'p';              
    }

    for (int x = 0; x < SIZE; x++) {
        board->squares[6][x] = 'P';               
        board->squares[7][x] = black_pieces[x] - 32; 
    }
}

void print_board(Board *board) {
    printf("\n  a b c d e f g h\n");
    printf("  -----------------\n");
    
    for (int y = 0; y < SIZE; y++) {
        printf("%d ", 8 - y);  
        for (int x = 0; x < SIZE; x++) {
            char piece = board->squares[y][x];
            printf("|%c", piece == ' ' ? ' ' : piece);
        }
        printf("|\n");
        printf("  -----------------\n");
    }
    printf("\n");
}

char get_piece(Board *board, int x, int y) {
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) {
        return '\0';
    }
    return board->squares[y][x];
}

void set_piece(Board *board, int x, int y, char piece) {
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) {
        return;
    }
    board->squares[y][x] = piece;
}

void test_board() {
    Board board;
    init_board(&board);
    print_board(&board);
}
