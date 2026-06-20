#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "board.h"
#include "moves.h"

// Maximum moves per position (safety limit)
#define MAX_MOVES 256

// Generate all pseudo-legal moves for a side
int generate_moves(Board *board, Move *moves, int side);

// Generate all legal moves (filters out moves that leave king in check)
int generate_legal_moves(Board *board, Move *moves, int side);

// Piece-specific generators (returns number of moves generated)
int gen_pawn_moves(Board *board, int x, int y, Move *moves);
int gen_knight_moves(Board *board, int x, int y, Move *moves);
int gen_bishop_moves(Board *board, int x, int y, Move *moves);
int gen_rook_moves(Board *board, int x, int y, Move *moves);
int gen_queen_moves(Board *board, int x, int y, Move *moves);
int gen_king_moves(Board *board, int x, int y, Move *moves);

// Test function
void test_movegen(void);

#endif
