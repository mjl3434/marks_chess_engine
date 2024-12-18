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
Game::getPieceAtSourceSquare(const Move& move)
{
    return board[move.source_rank][move.source_file].piece;
}