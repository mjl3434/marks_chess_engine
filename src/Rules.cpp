#include "Rules.h"

#include "ChessGame.h"

Rules::Rules(const ChessGame& game)
    : parent_game(game) {}

/**
 * @brief Checks if a move is legal, with the game board in it's current state
 * @param[in] move - The move to check if it's legal
 * @param[in] game_state - The current game state, before the move is made
 * @return true if the move is legal, false otherwise
 */
bool
Rules::isLegalMove(const Move& move, const GameState& game_state)
{
    if (move.piece == Piece::EMPTY) {
        return false;
    }

    switch (move.piece)
    {
        case Piece::WHITE_PAWN:
        case Piece::BLACK_PAWN:
            if (!isValidPawnMove(move, game_state)) {
                return false;
            }
            break;
        case Piece::WHITE_BISHOP:
        case Piece::BLACK_BISHOP:
            if (!isValidBishopMove(move, game_state)) {
                return false;
            }
            break;
        case Piece::WHITE_ROOK:
        case Piece::BLACK_ROOK:
            if (!isValidRookMove(move, game_state)) {
                return false;
            }
            break;
        case Piece::WHITE_KNIGHT:
        case Piece::BLACK_KNIGHT:
            if (!isValidKnightMove(move, game_state)) {
                return false;
            }
            break;
        case Piece::WHITE_QUEEN:
        case Piece::BLACK_QUEEN:
            if (!isValidQueenMove(move, game_state)) {
                return false;
            }
            break;
        case Piece::WHITE_KING:
        case Piece::BLACK_KING:
            if (!isValidKingMove(move, game_state)) {
                return false;
            }
            break;
        case Piece::EMPTY:
            return false;
    }

    if (isKingInCheckAfterMove(move, game_state)) {
        return false;
    }

    return true;
}

// Remember: For all of these, we've already checked that the algebreic 
// notation is valid, so we don't need to check that the destination
// is in bounds

/**
 * @brief Checks if a pawn move is valid. 
 * Assumes:
 *     - Piece is a pawn
 *     - Destination square is in bounds
 *     - Variable two_square_pawn_push_just_occured is up to date
 *     - Variable en_passant_target_square_rank is up to date
 *     - Variable en_passant_target_square_file is up to date
 */
bool
Rules::isValidPawnMove(const Move& move, const GameState& game_state)
{
    int8_t forward_one_square = move.piece == Piece::WHITE_PAWN ? 1 : -1;
    int8_t forward_two_squares = move.piece == Piece::WHITE_PAWN ? 2 : -2;
    int8_t pawn_starting_rank = move.piece == Piece::WHITE_PAWN ? 2 : 7;

    // The pawn is a complicated piece. Valid pawn moves are:
    // 1. Moving one square forward to an unoccupied square
    // 2. Moving two squares forward from the starting position through two unoccupied squares
    // 3. Capturing an opponent's piece diagonally one square
    // 4. En passant capture into the en passant target square

    // In both cases 1. and 2. the pawn must stay in the same file and the square
    // directly ahead must be unoccupied.
    if (move.destination_file == move.source_file &&
        !isSquareOccupied(move.source_rank + forward_one_square, move.source_file, game_state)) {

        // Check for case 1: Moving one square forward
        if (move.destination_rank == move.source_rank + forward_one_square) {
            return true;
        }

        // Check for case 2: Moving two squares forward from the starting position
        if (move.source_rank == pawn_starting_rank &&
            move.destination_rank == move.source_rank + forward_two_squares &&
            !isSquareOccupied(move.source_rank + forward_two_squares, move.source_file, game_state)) {
            return true;
        }
    }

    // In both cases 3 and 4 the pawn must move diagonally to capture
    if (move.destination_rank == move.source_rank + forward_one_square &&
        (move.destination_file == move.source_file - 1 ||
         move.destination_file == move.source_file + 1)) {

        // If they're moving into an occupied square, it's a capture
        if (isSquareOccupied(move.destination_rank, move.destination_file, game_state) ||
            (game_state._two_square_pawn_push_just_occured &&
             move.destination_rank == game_state._en_passant_target_square_rank &&
             move.destination_file == game_state._en_passant_target_square_file)) {
            // If the opponent's pawn just moved two squares forward, and they're
            // moving into the en passant target square, it's a valid en passant capture
            return true;
        }
    }

    return false;
}

bool
Rules::isValidKnightMove(const Move& move, const GameState& game_state)
{
    int8_t rank_diff = std::abs(move.destination_rank - move.source_rank);
    int8_t file_diff = std::abs(move.destination_file - move.source_file);

    // Check that the move is an L-shape: two squares in one direction and one square in the other
    if ((rank_diff == 2 && file_diff == 1) || (rank_diff == 1 && file_diff == 2)) {
        return true;
    }
    return false;
}

bool
Rules::isValidBishopMove(const Move& move, const GameState& game_state)
{
    int8_t squares_moved_horizontally = move.destination_file - move.source_file;
    int8_t squares_moved_vertically = move.destination_rank - move.source_rank;

    if (squares_moved_horizontally != squares_moved_vertically) {
        return false;
    }

    int8_t squares_moved = std::abs(squares_moved_horizontally);

    // Calculate direction increments for both file and rank
    int8_t rank_direction = (move.destination_rank > move.source_rank) ? 1 : -1;
    int8_t file_direction = (move.destination_file > move.source_file) ? 1 : -1;

    // Check all squares along the diagonal path (excluding destination)
    for (int i = 1; i < squares_moved; i++) {
        int check_rank = move.source_rank + (i * rank_direction);
        int check_file = move.source_file + (i * file_direction);
        if (isSquareOccupied(check_rank, check_file, game_state)) {
            return false;
        }
    }

    // Check if destination square has a piece of the same color (can't capture own pieces)
    if (isSquareOccupied(move.destination_rank, move.destination_file, game_state)) {
        Piece destination_piece = game_state.board[move.destination_rank-1][move.destination_file-1].piece;
        if (isSameColor(move.piece, destination_piece)) {
            return false;
        }
    }

    return true;
}

bool
Rules::isValidRookMove(const Move& move, const GameState& game_state)
{
    // Rook must move either horizontally or vertically
    if (!((move.source_file == move.destination_file && move.source_rank != move.destination_rank) ||
        (move.source_rank == move.destination_rank && move.source_file != move.destination_file))) {
        return false;
    }

    // Calculate direction increments (one will be 0, the other will be 1 or -1)
    int8_t rank_direction = 0;
    int8_t file_direction = 0;
    int8_t squares_moved = 0;

    if (move.source_file != move.destination_file) {
        // Moving horizontally
        file_direction = (move.destination_file > move.source_file) ? 1 : -1;
        squares_moved = std::abs(move.destination_file - move.source_file);
    } else {
        // Moving vertically
        rank_direction = (move.destination_rank > move.source_rank) ? 1 : -1;
        squares_moved = std::abs(move.destination_rank - move.source_rank);
    }

    // Check all squares along the path (excluding destination)
    for (int i = 1; i < squares_moved; i++) {
        int8_t check_rank = move.source_rank + (i * rank_direction);
        int8_t check_file = move.source_file + (i * file_direction);
        if (isSquareOccupied(check_rank, check_file, game_state)) {
            return false;
        }
    }

    // Check if destination square has a piece of the same color (can't capture own pieces)
    if (isSquareOccupied(move.destination_rank, move.destination_file, game_state)) {
        Piece destination_piece = game_state.board[move.destination_rank-1][move.destination_file-1].piece;
        if (isSameColor(move.piece, destination_piece)) {
            return false;
        }
    }

    return true;
}

bool 
Rules::isValidQueenMove(const Move& move, const GameState& game_state)
{
    if (isValidBishopMove(move, game_state) || isValidRookMove(move, game_state)) {
        return true;
    }
    return false;
}

/**
 * @brief Checks if a move is a valid king move
 * Assumes:
 *    - Piece is a king
 *    - The castling variables are up to date
 */
bool 
Rules::isValidKingMove(const Move& move, const GameState& game_state)
{
    bool is_kingside_castle = isKingSideCastle(move);
    bool is_queenside_castle = isQueenSideCastle(move);
    bool is_castling = is_kingside_castle || is_queenside_castle;

    // The King is complicated. He can move one square in any direction. He can
    // move to an empty square, or can capture, he can also castle. He may never
    // move into check.

    int8_t rank_diff = std::abs(move.destination_rank - move.source_rank);
    int8_t file_diff = std::abs(move.destination_file - move.source_file);

    // King can only move one square in any direction (except for castling, handled below)
    if (rank_diff > 1 || file_diff > 1) {
        return false;
    }
    // If the king moves, he (obviously) can't stay in the same place
    if (rank_diff == 0 && file_diff == 0) {
        return false;
    }

    // Castling logic is complicated. The king may only castle if all six conditions are met:
    //
    // 1. The king has not moved yet
    // 2. The rook (which the king is castling with) has not moved yet
    // 3. The squares between the king and rook are unoccupied
    // 4. The squares the king moves in are not under attack
    // 5. The king is not currently in check
    // 6. The king does not end up in check after castling
    //
    // Rather than check all six conditions, we update the castling rights
    // flags when a move is made. These "right" amount to checking conditions
    // 1 and 2.

    // First check if the king is castling
    if (is_castling) {
        // Determine which player and rank we're working with
        bool is_white = (move.piece == Piece::WHITE_KING);
        int8_t king_rank = is_white ? 1 : 8;
        if (is_kingside_castle) {
            // Check castling rights (conditions 1 and 2)
            bool castle_allowed = is_white ? game_state._white_kingside_castle_allowed 
                                           : game_state._black_kingside_castle_allowed;
            if (!castle_allowed) {
                return false;
            }
            // Check condition 3: squares between king and rook are unoccupied
            if (isSquareOccupied(king_rank, 6, game_state) ||
                isSquareOccupied(king_rank, 7, game_state)) {
                return false;
            }
            // Check conditions 4 and 5: king not in check and doesn't move through check
            if (isSquareUnderAttack(king_rank, 5, game_state) || // King currently in check
                isSquareUnderAttack(king_rank, 6, game_state) || // f-file under attack
                isSquareUnderAttack(king_rank, 7, game_state)) { // g-file under attack
                return false;
            }
        } else if (is_queenside_castle) {
            // Check castling rights (conditions 1 and 2)
            bool castle_allowed = is_white ? game_state._white_queenside_castle_allowed
                                           : game_state._black_queenside_castle_allowed;
            if (!castle_allowed) {
                return false;
            }
            // Check condition 3: squares between king and rook are unoccupied
            if (isSquareOccupied(king_rank, 2, game_state) ||
                isSquareOccupied(king_rank, 3, game_state) ||
                isSquareOccupied(king_rank, 4, game_state)) {
                return false;
            }
            // Check conditions 4 and 5: king not in check and doesn't move through check
            if (isSquareUnderAttack(king_rank, 5, game_state) || // King currently in check
                isSquareUnderAttack(king_rank, 4, game_state) || // d-file under attack
                isSquareUnderAttack(king_rank, 3, game_state)) { // c-file under attack
                return false;
            }
        }
    }

    // Check if king is in check after the move
    if (isKingInCheckAfterMove(move, game_state)) {
        return false;
    }

    return true;
}

bool
Rules::isKingInCheck(const GameState& game_state) {
    // FIXME: implement this
    return false;
}

bool
Rules::isCheckmate(const GameState& game_state) {
    // FIXME: implement this
    return false;
}

bool
Rules::isStalemate(const GameState& game_state) {
    // FIXME: implement this
    return false;
}

bool 
Rules::isDrawByInsufficientMaterial(const GameState& state)
{
    // FIXME: implement this
    return false;
}

bool
Rules::isGameOver(const GameState& game_state) {
    if (isCheckmate(game_state) ||
        isStalemate(game_state) ||
        parent_game.isDrawByFiftyMoveRule(game_state) || 
        parent_game.isDrawByThreefoldRepetition(game_state) ||
        isDrawByInsufficientMaterial(game_state)) {
        return true;
    }
    return false;
}

/**
 * @brief Checks if a square is occupied by a piece
 * Assumes:
 *     - Rank and file are in bounds
 */
bool
Rules::isSquareOccupied(int8_t rank, int8_t file, const GameState& game_state)
{
    // Check if the square is occupied
    return game_state.board[rank-1][file-1].piece != Piece::EMPTY;
}

bool
Rules::isSquareUnderAttack(int8_t rank, int8_t file, const GameState& state)
{
    // FIXME: Implement this
    return false;
}

bool
Rules::isKingSideCastle(const Move& move)
{
    // Kingside castling: King moves 2 squares to the right from starting position
    return (move.piece == Piece::WHITE_KING &&
            move.source_file == 5 &&
            move.source_rank == 1 && 
            move.destination_file == 7 &&
            move.destination_rank == 1) ||
           (move.piece == Piece::BLACK_KING &&
            move.source_file == 5 &&
            move.source_rank == 8 && 
            move.destination_file == 7 &&
            move.destination_rank == 8);
}


bool
Rules::isQueenSideCastle(const Move& move)
{
    // Queenside castling: King moves 2 squares to the left from starting position
    return (move.piece == Piece::WHITE_KING &&
            move.source_file == 5 &&
            move.source_rank == 1 && 
            move.destination_file == 3 &&
            move.destination_rank == 1) ||
           (move.piece == Piece::BLACK_KING &&
            move.source_file == 5 &&
            move.source_rank == 8 &&
            move.destination_file == 3 &&
            move.destination_rank == 8);
}

bool
Rules::isKingInCheckAfterMove(const Move& move, const GameState& game_state)
{
    GameState copy = game_state;
    parent_game.tryMoveOnStateCopy(move, copy);
    if (isSquareUnderAttack(move.destination_rank, move.destination_file, copy)) {
        return true;
    }
    return false;
}

bool
Rules::isSameColor(const Piece& piece1, const Piece& piece2)
{
    if (piece1 == Piece::EMPTY || piece2 == Piece::EMPTY) {
        return false;
    }
    if ((isWhite(piece1) && isWhite(piece2)) ||
        (isBlack(piece1) && isBlack(piece2))) {
        return true;
    }
    return false;
}

bool
Rules::isWhite(const Piece& piece)
{
    return (piece >= Piece::WHITE_PAWN && piece <= Piece::WHITE_KING);
}

bool
Rules::isBlack(const Piece& piece)
{
    return (piece >= Piece::BLACK_PAWN && piece <= Piece::BLACK_KING);
}