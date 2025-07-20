#include <Game.h>

#include <string>

Move::Move(std::string algebraic_notation)
        : algebraic(algebraic_notation),
          piece(Piece::EMPTY) {
    // A rank is a row (1-8)
    // A file is a column (a-h)
    source_file = algebraic[0] - 'a' + 1;
    source_rank = algebraic[1] - '0';
    destination_rank = algebraic[2] - 'a' + 1;
    destination_rank = algebraic[3] - '0';
}

Piece
ChessGame::getPieceAtSourceSquare(const Move& move)
{
    return game.board[move.source_rank][move.source_file].piece;
}

Piece
ChessGame::getPieceAtDestinationSquare(const Move& move)
{
    return game.board[move.source_rank][move.source_file].piece;
}

/**
 * @brief Checks if a move is legal, with the game board in it's current state
 */
bool
ChessGame::isLegalMove(const Move& move)
{
    if (move.piece == Piece::EMPTY) {
        return false;
    }

    // Is the piece moving in a valid way?

    switch (move.piece)
    {
        case Piece::WHITE_PAWN:
        case Piece::BLACK_PAWN:
            if (!isValidPawnMove(move)) {
                return false;
            }
            break;
        case Piece::WHITE_BISHOP:
        case Piece::BLACK_BISHOP:
            if (!isValidBishopMove(move)) {
                return false;
            }
            break;
        case Piece::WHITE_ROOK:
        case Piece::BLACK_ROOK:
            if (!isValidRookMove(move)) {
                return false;
            }
            break;
        case Piece::WHITE_KNIGHT:
        case Piece::BLACK_KNIGHT:
            if (!isValidKnightMove(move)) {
                return false;
            }
            break;
        case Piece::WHITE_QUEEN:
        case Piece::BLACK_QUEEN:
            if (!isValidQueenMove(move)) {
                return false;
            }
            break;
        case Piece::WHITE_KING:
        case Piece::BLACK_KING:
            if (!isValidKingMove(move)) {
                return false;
            }
            break;
    }

    if (kingIsInCheck()) {
        // Is the king out of check after the move is made?
    }
}

// Remember: For all of these, we've already checked that the algebreic 
// notation is valid, so we don't need to check that the destination
// is in bounds
bool
ChessGame::isValidPawnMove(const Move&)
{

}

bool
ChessGame::isValidBishopMove(const Move&)
{

}

bool
ChessGame::isValidKnightMove(const Move&)
{

}

bool
ChessGame::isValidRookMove(const Move&)
{

}

bool 
ChessGame::isValidQueenMove(const Move&)
{

}

bool 
ChessGame::isValidKingMove(const Move&)
{

}

