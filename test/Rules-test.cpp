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

    // Add more test cases here...

    return true;
}

bool test_isKingInCheckAfterMove(const Move& move, const GameState& game_state)
{

}