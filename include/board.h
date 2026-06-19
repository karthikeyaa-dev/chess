#ifndef BOARD_H
#define BOARD_H

#define SIZE 8

// Board representation
typedef struct {
    char squares[SIZE][SIZE];
} Board;

// Functions
void init_board(Board *board);
void print_board(Board *board);
char get_piece(Board *board, int x, int y);
void set_piece(Board *board, int x, int y, char piece);

// Test functions (only for development)
void test_board(void);

#endif
