#include "Rules-test.h"

#include <cassert>

#include "ChessGame.h"
#include "GameState.h"
#include "Rules.h"

bool
test_destinationContainsSamePlayer(const Rules& rules)
{
    GameState game_state;
    Move move;

    // Put two white pieces on the same file
    game_state.board[2-1][1-1].piece = Piece::WHITE_PAWN;
    game_state.board[4-1][1-1].piece = Piece::WHITE_PAWN;

    // Move one into the other
    move.piece = Piece::WHITE_PAWN;
    move.source_rank = 2-1;
    move.source_file = 1-1;
    move.destination_rank = 4-1;
    move.destination_file = 1-1;

    // Test if the destination contains the same player's piece
    assert(rules.destinationContainsSamePlayer(move, game_state));

    return true;
}

bool
run_all_rules_tests()
{
    ChessGame chess_game;

    test_destinationContainsSamePlayer(chess_game._rules);

    test_isKingInCheckAfterMove(chess_game._rules);

    return true;
}

bool
test_isKingInCheckAfterMove(const Rules& rules)
{
    GameState game_state;

    // Place black king on e8, white rook on e1 (black king is in check)
    game_state.board[8-1][5-1].piece = Piece::BLACK_KING; // e8
    game_state.board[1-1][5-1].piece = Piece::WHITE_ROOK; // e1
    game_state._current_player = Player::BLACK;

    // Try moving a random black pawn (should NOT resolve check)
    Move pawn_move;
    pawn_move.piece = Piece::BLACK_PAWN;
    game_state.board[7-1][4-1].piece = Piece::BLACK_PAWN; // d7
    pawn_move.source_rank = 7;
    pawn_move.source_file = 4;
    pawn_move.destination_rank = 6;
    pawn_move.destination_file = 4;
    assert(rules.isKingInCheckAfterMove(pawn_move, game_state) == true);

    // Try moving the king out of check (should resolve check)
    Move king_move;
    king_move.piece = Piece::BLACK_KING;
    king_move.source_rank = 8;
    king_move.source_file = 5;
    king_move.destination_rank = 8;
    king_move.destination_file = 4; // Move to d8
    assert(rules.isKingInCheckAfterMove(king_move, game_state) == false);

    // Try blocking the check with a pawn (should resolve check)
    Move block_move;
    block_move.piece = Piece::BLACK_PAWN;
    block_move.source_rank = 7;
    block_move.source_file = 5;
    block_move.destination_rank = 6;
    block_move.destination_file = 5; // Move to e7
    game_state.board[7-1][5-1].piece = Piece::BLACK_PAWN; // e7
    assert(rules.isKingInCheckAfterMove(block_move, game_state) == false);

    return true;
}