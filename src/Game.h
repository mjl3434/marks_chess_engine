#pragma once

#include <cstdint>
#include <list>
#include <string>

enum class Piece
{
    EMPTY = 0,
    WHITE_PAWN,
    WHITE_BISHOP,
    WHITE_KNIGHT,
    WHITE_ROOK,
    WHITE_QUEEN,
    WHITE_KING,
    BLACK_PAWN,
    BLACK_BISHOP,
    BLACK_KNIGHT,
    BLACK_ROOK,
    BLACK_QUEEN,
    BLACK_KING
};

enum class Player 
{
    WHITE,
    BLACK
};

class Square
{
  public:
    Piece piece;
};

class Move
{
  public:
    Move(std::string algebraic_notation)
        : algebraic(algebraic_notation),
          piece(Piece::EMPTY),
          source_rank(-1),
          source_file(-1),
          destination_rank(-1),
          destination_file(-1) {}
    std::string algebraic;
    Piece piece;
    int8_t source_rank; // (row)
    int8_t source_file; // (column)
    int8_t destination_rank; // (row)
    int8_t destination_file; // (column)
};

class Game
{
  public:
    Square board[8][8]; // [num_rows][num_columns]
    Player current_players_turn;
    bool white_kingside_castle_allowed = true;
    bool white_queenside_castle_allowed = true;
    bool black_kingside_castle_allowed = true;
    bool black_queenside_castle_allowed = true;

    // These fields are needed by games started with FEN, where we don't have
    // the full moves list and can't determine some of the game state.
    int halfmove_clock = 0;
    int num_moves = 0; // Num prior full moves from game started w/o moves list
    bool two_square_pawn_push_just_occured = false;
    uint8_t en_passant_target_square_rank = 0;
    uint8_t en_passant_target_square_file = 0;

    // With the full moves list, you don't need an of the FEN variables above
    std::list<Move> moves;
};