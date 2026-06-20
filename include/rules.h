#ifndef RULES_H
#define RULES_H

#include "board.h"
#include "moves.h"

// Piece types
#define PAWN   'p'
#define KNIGHT 'n'
#define BISHOP 'b'
#define ROOK   'r'
#define QUEEN  'q'
#define KING   'k'

// Colors
#define WHITE 0
#define BLACK 1

// Helper functions (exposed for movegen)
int get_piece_color(char piece);
int is_enemy(char piece, int turn);
int is_on_board(int x, int y);

// Check if a move is valid for a piece
int is_valid_move(Board *board, Move move, int turn);

// Check if a position is under attack by the given color
int is_attacked(Board *board, int x, int y, int by_color);

// Check if king is in check
int is_in_check(Board *board, int turn);

// Check if the game is in checkmate
int is_checkmate(Board *board, int turn);

// Check if the game is in stalemate
int is_stalemate(Board *board, int turn);

// Test functions
void test_rules(void);
void test_bishop_moves(void);  // Add this

#endif
