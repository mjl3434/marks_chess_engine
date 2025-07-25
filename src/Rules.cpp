#include "Rules.h"

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
    // 2. Moving two squares forward from the starting position to an unoccupied square
    // 3. Capturing an opponent's piece diagonally
    // 4. En passant capture into the en passant target square

    // In both cases 1. and 2. the pawn must stay in the same file and the square
    // directly ahead must be unoccupied.
    if (move.destination_file == move.source_file &&
        !isSquareOccupied(move.source_rank + forward_one_square, move.source_file)) {

        // Check for case 1: Moving one square forward
        if (move.destination_rank == move.source_rank + forward_one_square) {
            return true;
        }

        // Check for case 2: Moving two squares forward from the starting position
        if (move.source_rank == pawn_starting_rank &&
            move.destination_rank == move.source_rank + forward_two_squares &&
            !isSquareOccupied(move.source_rank + forward_two_squares, move.source_file)) {
            return true;
        }
    }

    // In both cases 3 and 4 the pawn must move diagonally to capture
    if (move.destination_rank == move.source_rank + forward_one_square &&
        (move.destination_file == move.source_file - 1 ||
         move.destination_file == move.source_file + 1)) {

        // If they're moving into an occupied square, it's a capture
        if (isSquareOccupied(move.destination_rank, move.destination_file) ||
            (two_square_pawn_push_just_occured &&
                move.destination_rank == en_passant_target_square_rank &&
                move.destination_file == en_passant_target_square_file)) {
            // If the opponent's pawn just moved two squares forward, and they're
            // moving into the en passant target square, it's a valid en passant capture
            return true;
        }
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
        if (isSquareOccupied(check_rank, check_file)) {
            return false;
        }
    }

    return true;
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
        if (isSquareOccupied(check_rank, check_file)) {
            return false;
        }
    }

    return true;
}

// FIXME: Note, the logic for both rooks, queens, and bishops doesn't consider that you can only
// capture an opponent's piece if it's in the destination square. I need to fix this.

bool 
Rules::isValidQueenMove(const Move& move, const GameState& game_state)
{
    if (isValidBishopMove(move) || isValidRookMove(move)) {
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
    // FIXME: Incorrect, it's not how UCI shows castling
    bool is_kingside_castle = (move.algebraic == "0-0");
    bool is_queenside_castle = (move.algebraic == "0-0-0");
    bool is_castling = is_kingside_castle || is_queenside_castle;
    Player player = (move.piece == Piece::WHITE_KING) ? Player::WHITE : Player::BLACK;

    // The King is complicated. He can move one square in any direction. He can
    // move to an empty square, or can capture, he can also castle. He may never
    // move into check.

    int8_t rank_diff = std::abs(move.destination_rank - move.source_rank);
    int8_t file_diff = std::abs(move.destination_file - move.source_file);

    // King can only move one square in any direction (except for castling, handled below)
    if (rank_diff > 1 || file_diff > 1) {
        return false;
    }


    // Castling logic is complicated. The king may only castle if all six conditions are met:
    // 1. The king has not moved yet
    // 2. The rook (which the king is castling with) has not moved yet
    // 3. The squares between the king and rook are unoccupied
    // 4. The squares the king moves through are not under attack
    // 5. The king is not currently in check
    // 6. The king does not end up in check after castling

    // Conditions 1 & 2 are tracked by the "castle_allowed" variables, which are updated in
    // the doMove() function, and reversed in undoMove().

    // First check if the king is castling
    if (is_castling) {
        if (is_kingside_castle) {
            // Kingside castling
            if ((move.piece == Piece::WHITE_KING && !white_kingside_castle_allowed) ||
                (move.piece == Piece::BLACK_KING && !black_kingside_castle_allowed)) {
                return false;
            }
        } else {
            // Queenside castling
            if (!((move.piece == Piece::WHITE_KING && !white_queenside_castle_allowed) ||
                  (move.piece == Piece::BLACK_KING && !black_queenside_castle_allowed))) {
                return false;
            }
        }
    }

    // Check if the squares between the king and rook are unoccupied, or if the squares the
    // king moves through are under attack
    if (is_kingside_castle) {
        // Kingside castling
        if (move.piece == Piece::WHITE_KING) {
            if ((isSquareOccupied(1, 6) || isSquareOccupied(1, 7)) || 
                (isSquareUnderAttack(1, 6, Player::WHITE) ||
                 isSquareUnderAttack(1, 7, Player::WHITE))) {
                return false;
            }
        } else {
            // Black kingside castling
            if (isSquareOccupied(8, 6) || isSquareOccupied(8, 7) ||
                (isSquareUnderAttack(8, 6, Player::BLACK) ||
                 isSquareUnderAttack(8, 7, Player::BLACK))) {
                return false;
            }
        }
    } else if (is_queenside_castle) {
        // Queenside castling
        if (move.piece == Piece::WHITE_KING) {
            if ((isSquareOccupied(1, 4) || isSquareOccupied(1, 3) || isSquareOccupied(1, 2)) ||
                (isSquareUnderAttack(1, 4, Player::WHITE) ||
                 isSquareUnderAttack(1, 3, Player::WHITE))) {
                return false;
            }
        } else {
            // Black queenside castling
            if ((isSquareOccupied(8, 4) || isSquareOccupied(8, 3) || isSquareOccupied(8, 2)) ||
                (isSquareUnderAttack(8, 4, Player::BLACK) ||
                 isSquareUnderAttack(8, 3, Player::BLACK))) {
                return false;
            }
        }
    }

    // FIXME: Move the king temporarily to the destination square
    bool king_in_check = kingIsInCheck();

    // FIXME: This is tricky because I need to be able to undo it
    if (is_kingside_castle) {
        doKingsideCastle(player);
    } else if (is_queenside_castle) {
        doQueensideCastle(player);
    } else {
        board[move.destination_rank][move.destination_file].piece = move.piece;
        board[move.source_rank][move.source_file].piece = Piece::EMPTY;
    }

    // FIXME: Move the king back to the source square
    if (king_in_check) {
        return false;
    }

    return true;
}

bool
Rules::isKingInCheck(const GameState& game_state) {
    // TBD
    return false;
}

bool
Rules::isCheckmate(const GameState& game_state) {
    // TBD
    return false;
}

bool
Rules::isStalemate(const GameState& game_state) {
    // TBD
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
    return game_state.board[rank - 1][file - 1].piece != Piece::EMPTY;
}

/*
bool
Rules::isWhite(Piece piece)
{
    return (piece >= Piece::WHITE_PAWN && piece <= Piece::WHITE_KING);
}

bool
Rules::isPawn(Piece piece)
{
    return (piece == Piece::WHITE_PAWN || piece == Piece::BLACK_PAWN);
}
*/