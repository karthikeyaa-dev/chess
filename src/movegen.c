#include "movegen.h"
#include "rules.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

// Generate all pseudo-legal moves for a side (no check validation)
int generate_moves(Board *board, Move *moves, int side) {
    int count = 0;
    
    // Loop through all squares
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            char piece = board->squares[y][x];
            int color = get_piece_color(piece);
            
            // Only generate moves for pieces of the given side
            if (color != side) continue;
            
            char lower = tolower(piece);
            int generated = 0;
            
            switch (lower) {
                case PAWN:
                    generated = gen_pawn_moves(board, x, y, moves + count);
                    break;
                case KNIGHT:
                    generated = gen_knight_moves(board, x, y, moves + count);
                    break;
                case BISHOP:
                    generated = gen_bishop_moves(board, x, y, moves + count);
                    break;
                case ROOK:
                    generated = gen_rook_moves(board, x, y, moves + count);
                    break;
                case QUEEN:
                    generated = gen_queen_moves(board, x, y, moves + count);
                    break;
                case KING:
                    generated = gen_king_moves(board, x, y, moves + count);
                    break;
                default:
                    continue;
            }
            
            count += generated;
        }
    }
    
    return count;
}

// Generate legal moves using rules.c for validation
int generate_legal_moves(Board *board, Move *moves, int side) {
    Move pseudo_legal[MAX_MOVES];
    int num_pseudo = generate_moves(board, pseudo_legal, side);
    int count = 0;
    
    // Use is_valid_move from rules.c to filter illegal moves
    for (int i = 0; i < num_pseudo; i++) {
        if (is_valid_move(board, pseudo_legal[i], side)) {
            moves[count++] = pseudo_legal[i];
        }
    }
    
    return count;
}

// Generate pawn moves
int gen_pawn_moves(Board *board, int x, int y, Move *moves) {
    int count = 0;
    char piece = board->squares[y][x];
    int color = get_piece_color(piece);
    int direction = (color == WHITE) ? -1 : 1;
    int start_row = (color == WHITE) ? 6 : 1;
    
    // Forward move
    int new_y = y + direction;
    if (new_y >= 0 && new_y < SIZE) {
        if (board->squares[new_y][x] == ' ') {
            // Single step
            moves[count++] = (Move){x, y, x, new_y};
            
            // Double step from starting position
            int double_y = y + 2 * direction;
            if (y == start_row && board->squares[double_y][x] == ' ') {
                moves[count++] = (Move){x, y, x, double_y};
            }
        }
    }
    
    // Captures (diagonal)
    int capture_x[] = {x - 1, x + 1};
    for (int i = 0; i < 2; i++) {
        int cx = capture_x[i];
        if (cx < 0 || cx >= SIZE) continue;
        
        int cy = y + direction;
        if (cy < 0 || cy >= SIZE) continue;
        
        char target = board->squares[cy][cx];
        if (is_enemy(target, color)) {
            moves[count++] = (Move){x, y, cx, cy};
        }
    }
    
    // TODO: En passant (needs game state)
    // TODO: Pawn promotion (need to generate all promotion options)
    
    return count;
}

// Generate knight moves
int gen_knight_moves(Board *board, int x, int y, Move *moves) {
    int count = 0;
    char piece = board->squares[y][x];
    int color = get_piece_color(piece);
    
    // All knight move offsets
    int offsets[8][2] = {
        {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
        {1, -2}, {1, 2}, {2, -1}, {2, 1}
    };
    
    for (int i = 0; i < 8; i++) {
        int nx = x + offsets[i][0];
        int ny = y + offsets[i][1];
        
        if (!is_on_board(nx, ny)) continue;
        
        char target = board->squares[ny][nx];
        // Can move to empty square or capture enemy
        if (target == ' ' || is_enemy(target, color)) {
            moves[count++] = (Move){x, y, nx, ny};
        }
    }
    
    return count;
}

// Generate bishop moves
int gen_bishop_moves(Board *board, int x, int y, Move *moves) {
    int count = 0;
    char piece = board->squares[y][x];
    int color = get_piece_color(piece);
    
    // Diagonal directions
    int dirs[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    
    for (int d = 0; d < 4; d++) {
        int nx = x + dirs[d][0];
        int ny = y + dirs[d][1];
        
        while (is_on_board(nx, ny)) {
            char target = board->squares[ny][nx];
            
            if (target == ' ') {
                moves[count++] = (Move){x, y, nx, ny};
            } else if (is_enemy(target, color)) {
                moves[count++] = (Move){x, y, nx, ny};
                break; // Can't move past captured piece
            } else {
                break; // Blocked by own piece
            }
            
            nx += dirs[d][0];
            ny += dirs[d][1];
        }
    }
    
    return count;
}

// Generate rook moves
int gen_rook_moves(Board *board, int x, int y, Move *moves) {
    int count = 0;
    char piece = board->squares[y][x];
    int color = get_piece_color(piece);
    
    // Horizontal and vertical directions
    int dirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    
    for (int d = 0; d < 4; d++) {
        int nx = x + dirs[d][0];
        int ny = y + dirs[d][1];
        
        while (is_on_board(nx, ny)) {
            char target = board->squares[ny][nx];
            
            if (target == ' ') {
                moves[count++] = (Move){x, y, nx, ny};
            } else if (is_enemy(target, color)) {
                moves[count++] = (Move){x, y, nx, ny};
                break;
            } else {
                break;
            }
            
            nx += dirs[d][0];
            ny += dirs[d][1];
        }
    }
    
    return count;
}

// Generate queen moves
int gen_queen_moves(Board *board, int x, int y, Move *moves) {
    // Queen = bishop + rook
    int count = 0;
    count += gen_bishop_moves(board, x, y, moves);
    count += gen_rook_moves(board, x, y, moves + count);
    return count;
}

// Generate king moves
int gen_king_moves(Board *board, int x, int y, Move *moves) {
    int count = 0;
    char piece = board->squares[y][x];
    int color = get_piece_color(piece);
    
    // All 8 adjacent squares
    int offsets[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1},  {1, 0},  {1, 1}
    };
    
    for (int i = 0; i < 8; i++) {
        int nx = x + offsets[i][0];
        int ny = y + offsets[i][1];
        
        if (!is_on_board(nx, ny)) continue;
        
        char target = board->squares[ny][nx];
        if (target == ' ' || is_enemy(target, color)) {
            moves[count++] = (Move){x, y, nx, ny};
        }
    }
    
    // TODO: Castling
    
    return count;
}

// Test move generation
void test_movegen(void) {
    Board board;
    init_board(&board);
    
    printf("\n===== MOVEGEN TEST =====\n");
    
    Move moves[MAX_MOVES];
    int num_moves = generate_moves(&board, moves, WHITE);
    
    printf("White has %d pseudo-legal moves\n", num_moves);
    
    // Show first 10 moves
    int show = (num_moves < 10) ? num_moves : 10;
    printf("First %d moves:\n", show);
    for (int i = 0; i < show; i++) {
        printf("  %d: (%d,%d) -> (%d,%d)\n", 
               i+1, moves[i].x1, moves[i].y1, moves[i].x2, moves[i].y2);
    }
    
    // Count legal moves (using rules.c validation)
    Move legal_moves[MAX_MOVES];
    int num_legal = generate_legal_moves(&board, legal_moves, WHITE);
    printf("\nWhite has %d legal moves\n", num_legal);
    
    // Verify that is_valid_move from rules.c is being used
    printf("\nVerifying with rules.c:\n");
    Move test_move = {4, 6, 4, 4};  // e2 e4
    int valid = is_valid_move(&board, test_move, WHITE);
    printf("  e2 e4 is %s\n", valid ? "LEGAL" : "ILLEGAL");
    
    // Test an illegal move
    test_move = (Move){4, 7, 4, 5};  // e1 e3 (king moves too far)
    valid = is_valid_move(&board, test_move, WHITE);
    printf("  e1 e3 is %s\n", valid ? "LEGAL" : "ILLEGAL");
}
