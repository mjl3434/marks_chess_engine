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
    int8_t source_rank; // (row)
    int8_t source_file; // (column)
    int8_t destination_rank; // (row)
    int8_t destination_file; // (column)

private:
    Piece piece;
    Piece captured_piece;
    //std::unique_ptr<SideEffects> side_effects = nullptr;
};

/*
struct SideEffects
{
  public:
    bool white_kingside_rook_moved = false;
    bool white_queenside_rook_moved = false;
    bool black_kingside_rook_moved = false;
    bool black_queenside_rook_moved = false;
    bool white_king_moved = false;
    bool black_king_moved = false;
    bool pawn_moved_two_squares = false;
    bool white_kingside_castle_changed = false;
    bool white_queenside_castle_changed = false;
    bool black_kingside_castle_changed = false;
    bool black_queenside_castle_changed = false;
    //bool draw_from_threefold_repetition = false;
    //bool draw_from_fifty_move_rule = false;
    //bool draw_from_insufficient_material = false;
    //bool draw_from_stalemate = false;
};
*/


struct ChessGame
{
public:

    void doMove(Move&);
    void undoMove(Move&);
    /*
    Piece getPieceAtSourceSquare(const Move&);
    Piece getPieceAtDestinationSquare(const Move&);
    void doKingsideCastle(Player player);
    void doQueensideCastle(Player player);
    bool isLegalMove(const Move&);
    bool isValidPawnMove(const Move&);
    bool isValidBishopMove(const Move&);
    bool isValidKnightMove(const Move&);
    bool isValidRookMove(const Move&);
    bool isValidQueenMove(const Move&);
    bool isValidKingMove(const Move&);
    bool kingIsInCheck(Player player);
    bool isSquareOccupied(int8_t rank, int8_t file);
    bool isSquareUnderAttack(int8_t rank, int8_t file, Player player);
    bool isWhite(Piece piece);
    bool isPawn(Piece piece);
    */

    Square board[8][8]; // [num_rows][num_columns]
    Player current_player;

    // These fields are needed by games started with FEN, where we don't have
    // the full moves list and can't determine some of the game state, however
    // they're still useful regardless so we always keep them updated.

    int halfmove_clock = 0; // For calculating fifty-move rule
    int num_moves = 0; // Num prior full moves from game started w/o moves list
    uint8_t en_passant_target_square_rank = 0;
    uint8_t en_passant_target_square_file = 0;
    bool two_square_pawn_push_just_occured = false;
    bool white_kingside_castle_allowed = true;
    bool white_queenside_castle_allowed = true;
    bool black_kingside_castle_allowed = true;
    bool black_queenside_castle_allowed = true;


    // With the full moves list, you don't need an of the FEN variables above
    std::list<Move> moves;
};
