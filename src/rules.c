#include "rules.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

// Helper: Get piece color
int get_piece_color(char piece) {
    if (piece == ' ') return -1;
    return isupper(piece) ? WHITE : BLACK;
}

// Helper: Check if piece is enemy
int is_enemy(char piece, int turn) {
    int color = get_piece_color(piece);
    if (color == -1) return 0;
    return color != turn;
}

// Helper: Check if position is on board
int is_on_board(int x, int y) {
    return x >= 0 && x < SIZE && y >= 0 && y < SIZE;
}

// Validate pawn move
static int valid_pawn_move(Board *board, Move move, int turn) {
    char piece = get_piece(board, move.x1, move.y1);
    char target = get_piece(board, move.x2, move.y2);
    int dy = move.y2 - move.y1;
    int dx = abs(move.x2 - move.x1);
    int direction = (turn == WHITE) ? -1 : 1;
    int start_row = (turn == WHITE) ? 6 : 1;
    
    // Must move in correct direction
    if (dy * direction <= 0) return 0;
    
    // Forward move (no capture)
    if (dx == 0 && target == ' ') {
        // Single step
        if (dy == direction) return 1;
        // Double step from starting position
        if (dy == 2 * direction && move.y1 == start_row && 
            get_piece(board, move.x1, move.y1 + direction) == ' ') {
            return 1;
        }
        return 0;
    }
    
    // Capture diagonally
    if (dx == 1 && dy == direction && is_enemy(target, turn)) {
        return 1;
    }
    
    return 0;
}

// Validate knight move
static int valid_knight_move(Board *board, Move move, int turn) {
    int dx = abs(move.x2 - move.x1);
    int dy = abs(move.y2 - move.y1);
    char target = get_piece(board, move.x2, move.y2);
    
    // Knight moves in L-shape
    if ((dx == 2 && dy == 1) || (dx == 1 && dy == 2)) {
        // Can capture enemy or move to empty
        if (target == ' ' || is_enemy(target, turn)) {
            return 1;
        }
    }
    return 0;
}

// Validate bishop move (FIXED)
static int valid_bishop_move(Board *board, Move move, int turn) {
    int dx = move.x2 - move.x1;
    int dy = move.y2 - move.y1;
    char target = get_piece(board, move.x2, move.y2);
    
    // Must move diagonally (abs(dx) must equal abs(dy))
    if (abs(dx) != abs(dy) || (dx == 0 && dy == 0)) {
        return 0;
    }
    
    // Check if path is clear
    int step_x = (dx > 0) ? 1 : -1;
    int step_y = (dy > 0) ? 1 : -1;
    
    int x = move.x1 + step_x;
    int y = move.y1 + step_y;
    while (x != move.x2 && y != move.y2) {
        if (get_piece(board, x, y) != ' ') {
            return 0; // Path blocked
        }
        x += step_x;
        y += step_y;
    }
    
    // Destination must be empty or enemy piece
    if (target == ' ' || is_enemy(target, turn)) {
        return 1;
    }
    
    return 0;
}

// Validate rook move
static int valid_rook_move(Board *board, Move move, int turn) {
    int dx = move.x2 - move.x1;
    int dy = move.y2 - move.y1;
    char target = get_piece(board, move.x2, move.y2);
    
    // Must move horizontally or vertically
    if ((dx != 0 && dy != 0) || (dx == 0 && dy == 0)) return 0;
    
    // Check if path is clear
    int step_x = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
    int step_y = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;
    
    int x = move.x1 + step_x;
    int y = move.y1 + step_y;
    while (x != move.x2 || y != move.y2) {
        if (get_piece(board, x, y) != ' ') return 0;
        x += step_x;
        y += step_y;
    }
    
    // Destination must be empty or enemy
    return (target == ' ' || is_enemy(target, turn));
}

// Validate queen move
static int valid_queen_move(Board *board, Move move, int turn) {
    // Queen combines bishop and rook moves
    return valid_bishop_move(board, move, turn) || 
           valid_rook_move(board, move, turn);
}

// Validate king move
static int valid_king_move(Board *board, Move move, int turn) {
    int dx = abs(move.x2 - move.x1);
    int dy = abs(move.y2 - move.y1);
    char target = get_piece(board, move.x2, move.y2);
    
    // King moves one square in any direction
    if (dx <= 1 && dy <= 1 && (dx || dy)) {
        return (target == ' ' || is_enemy(target, turn));
    }
    
    // TODO: Castling
    return 0;
}

// Main validation function
int is_valid_move(Board *board, Move move, int turn) {
    char piece = get_piece(board, move.x1, move.y1);
    int color = get_piece_color(piece);
    
    // No piece or wrong color
    if (piece == ' ' || color != turn) return 0;
    
    // Check if destination has own piece
    char target = get_piece(board, move.x2, move.y2);
    if (get_piece_color(target) == turn) return 0;
    
    // Validate based on piece type
    char lower_piece = tolower(piece);
    int valid = 0;
    
    switch (lower_piece) {
        case PAWN:   valid = valid_pawn_move(board, move, turn); break;
        case KNIGHT: valid = valid_knight_move(board, move, turn); break;
        case BISHOP: valid = valid_bishop_move(board, move, turn); break;
        case ROOK:   valid = valid_rook_move(board, move, turn); break;
        case QUEEN:  valid = valid_queen_move(board, move, turn); break;
        case KING:   valid = valid_king_move(board, move, turn); break;
        default: return 0;
    }
    
    if (!valid) return 0;
    
    // Simulate the move to check if it leaves king in check
    Board temp = *board;
    apply_move(&temp, move);
    return !is_in_check(&temp, turn);
}

// Check if a square is attacked by the given color
int is_attacked(Board *board, int x, int y, int by_color) {
    // Check all enemy pieces
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            char piece = board->squares[j][i];
            if (get_piece_color(piece) == by_color) {
                Move attack_move = {i, j, x, y};
                // Temporarily disable king check verification
                // Just check if piece can move to the square
                char lower_piece = tolower(piece);
                int valid = 0;
                
                switch (lower_piece) {
                    case PAWN:   valid = valid_pawn_move(board, attack_move, by_color); break;
                    case KNIGHT: valid = valid_knight_move(board, attack_move, by_color); break;
                    case BISHOP: valid = valid_bishop_move(board, attack_move, by_color); break;
                    case ROOK:   valid = valid_rook_move(board, attack_move, by_color); break;
                    case QUEEN:  valid = valid_queen_move(board, attack_move, by_color); break;
                    case KING:   valid = valid_king_move(board, attack_move, by_color); break;
                }
                
                if (valid) return 1;
            }
        }
    }
    return 0;
}

// Check if king is in check
int is_in_check(Board *board, int turn) {
    // Find king
    char king = (turn == WHITE) ? 'K' : 'k';
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board->squares[j][i] == king) {
                // Check if king's position is attacked by enemy
                return is_attacked(board, i, j, !turn);
            }
        }
    }
    return 0; // King not found (shouldn't happen)
}

// Check if checkmate
int is_checkmate(Board *board, int turn) {
    if (!is_in_check(board, turn)) return 0;
    
    // Try all possible moves
    for (int x1 = 0; x1 < SIZE; x1++) {
        for (int y1 = 0; y1 < SIZE; y1++) {
            for (int x2 = 0; x2 < SIZE; x2++) {
                for (int y2 = 0; y2 < SIZE; y2++) {
                    Move move = {x1, y1, x2, y2};
                    if (is_valid_move(board, move, turn)) {
                        return 0; // Found a valid move
                    }
                }
            }
        }
    }
    return 1; // No valid moves
}

// Check if stalemate
int is_stalemate(Board *board, int turn) {
    if (is_in_check(board, turn)) return 0;
    
    // Try all possible moves
    for (int x1 = 0; x1 < SIZE; x1++) {
        for (int y1 = 0; y1 < SIZE; y1++) {
            for (int x2 = 0; x2 < SIZE; x2++) {
                for (int y2 = 0; y2 < SIZE; y2++) {
                    Move move = {x1, y1, x2, y2};
                    if (is_valid_move(board, move, turn)) {
                        return 0; // Found a valid move
                    }
                }
            }
        }
    }
    return 1; // No valid moves
}

// Test rules
void test_rules(void) {
    Board board;
    init_board(&board);
    
    printf("\n===== RULES TEST =====\n");
    
    // Test valid moves
    Move moves[] = {
        {4, 6, 4, 4},  // e2 e4 (pawn)
        {6, 7, 5, 5},  // g1 f3 (knight)
        {3, 7, 2, 6},  // d1 d2? (invalid - queen can't move like that)
        {4, 7, 4, 5},  // e1 e3? (invalid - king can't move that far)
    };
    
    int num_moves = sizeof(moves) / sizeof(moves[0]);
    for (int i = 0; i < num_moves; i++) {
        int valid = is_valid_move(&board, moves[i], WHITE);
        printf("Move (%d,%d)->(%d,%d) is %s\n", 
               moves[i].x1, moves[i].y1, moves[i].x2, moves[i].y2,
               valid ? "VALID" : "INVALID");
    }
    
    printf("White in check: %s\n", is_in_check(&board, WHITE) ? "YES" : "NO");
    printf("Checkmate: %s\n", is_checkmate(&board, WHITE) ? "YES" : "NO");
    printf("Stalemate: %s\n", is_stalemate(&board, WHITE) ? "YES" : "NO");
}

// Test bishop moves
void test_bishop_moves(void) {
    Board board;
    init_board(&board);
    
    printf("\n===== TESTING BISHOP MOVES =====\n");
    print_board(&board);
    
    // Test bishop at c1 (white bishop)
    Move moves[] = {
        {2, 7, 3, 6},  // c1 to d2 (diagonal)
        {2, 7, 4, 5},  // c1 to e3 (diagonal)
        {2, 7, 1, 6},  // c1 to b2 (diagonal)
        {2, 7, 0, 5},  // c1 to a3 (diagonal)
        {2, 7, 3, 5},  // c1 to d3 (invalid - not diagonal)
        {2, 7, 2, 5},  // c1 to c3 (invalid - vertical)
    };
    
    for (int i = 0; i < 6; i++) {
        int valid = is_valid_move(&board, moves[i], WHITE);
        printf("Move (%d,%d)->(%d,%d) is %s\n", 
               moves[i].x1, moves[i].y1, moves[i].x2, moves[i].y2,
               valid ? "VALID ✓" : "INVALID ✗");
    }
}
