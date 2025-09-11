#include "Common.h"

// DEBUG FUNCTIONS
#if 0
#include <iostream>
#include <iomanip>

const char* printPiece(const Piece& piece)
{
    switch (piece)
    {
        case Piece::EMPTY:
            return "empty";
        case Piece::WHITE_PAWN:
            return "white pawn";
        case Piece::WHITE_BISHOP:
            return "white bishop";
        case Piece::WHITE_KNIGHT:
            return "white knight";
        case Piece::WHITE_ROOK:
            return "white rook";
        case Piece::WHITE_QUEEN:
            return "white queen";
        case Piece::WHITE_KING:
            return "white king";
        case Piece::BLACK_PAWN:
            return "black pawn";
        case Piece::BLACK_BISHOP:
            return "black bishop";
        case Piece::BLACK_KNIGHT:
            return "black knight";
        case Piece::BLACK_ROOK:
            return "black rook";
        case Piece::BLACK_QUEEN:
            return "black queen";
        case Piece::BLACK_KING:
            return "black king";
        default:
            return "unknown piece";
    }
}

// Helper function to print the board state
void printBoard(const GameState& state) {
    // Piece to char mapping
    auto pieceToChar = [](Piece p) -> char {
        switch (p) {
            case Piece::WHITE_PAWN:   return 'P';
            case Piece::WHITE_KNIGHT: return 'N';
            case Piece::WHITE_BISHOP: return 'B';
            case Piece::WHITE_ROOK:   return 'R';
            case Piece::WHITE_QUEEN:  return 'Q';
            case Piece::WHITE_KING:   return 'K';
            case Piece::BLACK_PAWN:   return 'p';
            case Piece::BLACK_KNIGHT: return 'n';
            case Piece::BLACK_BISHOP: return 'b';
            case Piece::BLACK_ROOK:   return 'r';
            case Piece::BLACK_QUEEN:  return 'q';
            case Piece::BLACK_KING:   return 'k';
            case Piece::EMPTY:        return '.';
            default:                  return '?';
        }
    };
    std::cerr << "\n  a b c d e f g h\n";
    for (int rank = 8; rank >= 1; --rank) {
        std::cerr << rank << ' ';
        for (int file = 1; file <= 8; ++file) {
            char c = pieceToChar(state.board[rank-1][file-1].piece);
            std::cerr << c << ' ';
        }
        std::cerr << rank << '\n';
    }
    std::cerr << "  a b c d e f g h\n";
}
#endif
// DEBUG FUNCTIONS END

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

std::string
Move::toString() const
{
    std::string result;
    result += static_cast<char>('a' + source_file - 1);
    result += static_cast<char>('1' + source_rank - 1);
    result += static_cast<char>('a' + destination_file - 1);
    result += static_cast<char>('1' + destination_rank - 1);

    if (promotion_chosen == Promotion::QUEEN) {
        result += 'q';
    } else if (promotion_chosen == Promotion::ROOK) {
        result += 'r';
    } else if (promotion_chosen == Promotion::BISHOP) {
        result += 'b';
    } else if (promotion_chosen == Promotion::KNIGHT) {
        result += 'n';
    }

    return result;
}

bool debug_enabled = false;

void debugLog(const char* format, ...)
{
    if (!debug_enabled)
        return;

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}