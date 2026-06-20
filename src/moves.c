#include "moves.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

static int file_to_x(char file) {
    return tolower(file) - 'a';
}


static int rank_to_y(char rank) {
    return '8' - rank;
}

// Parse input like "e2 e4"
int parse_move(const char *input, Move *move) {
    if (!input || !move) return 0;
    
    // Expected format: "e2 e4" (5 characters including space)
    // Example: e2 e4 -> x1=4, y1=6, x2=4, y2=4
    if (strlen(input) < 5) return 0;
    
    // Parse first position (e2)
    char file1 = input[0];
    char rank1 = input[1];
    
    // Parse second position (e4)
    char file2 = input[3];
    char rank2 = input[4];
    
    // Validate files (a-h)
    if (file1 < 'a' || file1 > 'h' || file2 < 'a' || file2 > 'h') {
        return 0;
    }
    
    // Validate ranks (1-8)
    if (rank1 < '1' || rank1 > '8' || rank2 < '1' || rank2 > '8') {
        return 0;
    }
    
    move->x1 = file_to_x(file1);
    move->y1 = rank_to_y(rank1);
    move->x2 = file_to_x(file2);
    move->y2 = rank_to_y(rank2);
    
    return 1;  // Success
}

// Execute move (simple: move piece from (x1,y1) to (x2,y2))
void apply_move(Board *board, Move move) {
    if (!board) return;
    
    // Get piece from source
    char piece = get_piece(board, move.x1, move.y1);
    if (piece == ' ') return;  // No piece to move
    
    // Move piece to destination (capture if occupied)
    set_piece(board, move.x2, move.y2, piece);
    set_piece(board, move.x1, move.y1, ' ');
}

// Optional: test move parsing
void test_moves() {
    Move move;
    
    printf("Testing parse_move:\n");
    
    if (parse_move("e2 e4", &move)) {
        printf("e2 e4 -> (%d,%d) to (%d,%d)\n", 
               move.x1, move.y1, move.x2, move.y2);
        // Should print: e2 e4 -> (4,6) to (4,4)
    }
    
    if (parse_move("g1 f3", &move)) {
        printf("g1 f3 -> (%d,%d) to (%d,%d)\n", 
               move.x1, move.y1, move.x2, move.y2);
        // Should print: g1 f3 -> (6,7) to (5,5)
    }
    
    // Invalid test
    if (!parse_move("i9 j0", &move)) {
        printf("'i9 j0' correctly rejected\n");
    }
}
