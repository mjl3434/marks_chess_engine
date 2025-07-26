#include "Common.h"

/**
 * @brief Constructs a Move object from string of algebraic notation
 * Assumptions:
 *     - The algebraic notation is valid, specifically:  The string is in the
 *       format of "nXmY" or "nXmYp" where n and m are a-h, and X and Y are
 *       1-8. If there is a promotion, the last character is one of 'q', 'r',
 *       'b', or 'n'. Castling is denoted like:  e1a1, e1e8, e8a8, e8h8.
 */
Move::Move(std::string algebraic_notation)
        : piece(Piece::EMPTY) ,
          captured_piece(Piece::EMPTY),
          promotion_chosen(Promotion::NONE)
{
    // A rank is a row (1-8), A file is a column (a-h)
    // Do ascii areithmetic to convert from a character to a number 1-8
    source_file = algebraic_notation[0] - 'a' + 1;
    source_rank = algebraic_notation[1] - '0';
    destination_file = algebraic_notation[2] - 'a' + 1;
    destination_rank = algebraic_notation[3] - '0';

    if (algebraic_notation.length() == 5) {
        if (algebraic_notation[4] == 'q') {
            promotion_chosen = Promotion::QUEEN;
        } else if (algebraic_notation[4] == 'r') {
            promotion_chosen = Promotion::ROOK;
        } else if (algebraic_notation[4] == 'b') {
            promotion_chosen = Promotion::BISHOP;
        } else if (algebraic_notation[4] == 'n') {
            promotion_chosen = Promotion::KNIGHT;
        }
    }
}

/**
 * @brief Default constructor for Move
 * Note: This default constructor is necessary because SearchResult class has
 * an empty move, and we need to be able to construct it empty and set it later.
 */
Move::Move()
    : source_rank(0),
      source_file(0),
      destination_rank(0),
      destination_file(0),
      piece(Piece::EMPTY),
      captured_piece(Piece::EMPTY),
      promotion_chosen(Promotion::NONE)
{
}