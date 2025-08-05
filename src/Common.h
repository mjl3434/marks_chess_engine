#pragma once

#include <string>
#include <unordered_map>

using position_hash_t = std::unordered_map<std::size_t, int>;

enum class Promotion
{
    NONE = 0,
    QUEEN,
    ROOK,
    BISHOP,
    KNIGHT
};

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

struct Square
{
  public:
    Piece piece;
};

// Note: Dumb data class, should contain what happened, but no game logic.
struct Move
{
public:
    Move(std::string algebraic_notation);
    Move();
    int8_t source_rank; // (row)
    int8_t source_file; // (column)
    int8_t destination_rank; // (row)
    int8_t destination_file; // (column)

    // The fields below are not set in the constructor
    Piece piece;
    Piece captured_piece;
    Promotion promotion_chosen;

    // Note: Optionally include special flags about, castling, en passant, promotion
};
