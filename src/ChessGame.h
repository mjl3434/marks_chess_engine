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

struct Square
{
  public:
    Piece piece;
};

struct Move
{
  public:
    Move(std::string algebraic_notation);
    std::string algebraic;
    Piece piece;
    Piece captured_piece;
    int8_t source_rank; // (row)
    int8_t source_file; // (column)
    int8_t destination_rank; // (row)
    int8_t destination_file; // (column)
};

struct ChessGame
{
  public:

    Piece getPieceAtSourceSquare(const Move&);
    Piece getPieceAtDestinationSquare(const Move&);
    bool isLegalMove(const Move&);
    bool isValidPawnMove(const Move&);
    bool isValidBishopMove(const Move&);
    bool isValidKnightMove(const Move&);
    bool isValidRookMove(const Move&);
    bool isValidQueenMove(const Move&);
    bool isValidKingMove(const Move&);
    bool kingIsInCheck();

    Square board[8][8]; // [num_rows][num_columns]
    Player current_player;
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