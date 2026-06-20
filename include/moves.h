#ifndef MOVES_H
#define MOVES_H

#include "board.h"

// A move from (x1,y1) → (x2,y2)
typedef struct {
    int x1, y1;
    int x2, y2;
} Move;

// Parse input like "e2 e4"
int parse_move(const char *input, Move *move);

// Execute move (no rule checking yet)
void apply_move(Board *board, Move move);

// Test functions (only for development)
void test_moves(void);
void test_board(void);

#endif
