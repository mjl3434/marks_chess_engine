#include "Rules.h"

#include "ChessGame.h"

Rules::Rules(const ChessGame& game)
    : parent_game(game) {}

// Note: For this entire class whenever we're given a Move, we assume that the
// algebraic notation has been checked, and is valid. This means we don't need
// to check that the destination is in bounds.

bool
Rules::destinationContainsSamePlayer(const Move& move, const GameState& state) const
{
    Player player = isWhite(move.piece) ? Player::WHITE : Player::BLACK;
    return isSquareOccupiedBy(move.destination_rank, move.destination_file, state, player);
}

bool
Rules::isBlack(const Piece& piece) const
{
    return (piece >= Piece::BLACK_PAWN && piece <= Piece::BLACK_KING);
}

bool
Rules::isWhite(const Piece& piece) const
{
    return (piece >= Piece::WHITE_PAWN && piece <= Piece::WHITE_KING);
}

bool
Rules::isCheckmate(const GameState& game_state) const
{
    // Checkmate occurs when the king is under attack and it cannot eliminate all
    // threats in one move by doing the following:
    // - Capture the attacker(s)
    // - Block the attacker(s) with another piece (if queen/rook/bishop)
    // - Move out of the way to a safe square

    Player player = game_state._current_player;
    Player opponent = player == Player::WHITE ? Player::BLACK : Player::WHITE;

    // If king is not under attack it's definitly not checkmate
    if (!isKingInCheck(game_state)) {
        return false;
    }

    // The king is in check, try and find ways out of check
    std::list<Move> legal_moves = generateLegalMovesForCurrentPlayer(game_state);
    for (const Move& move : legal_moves) {
        // If the move is legal and it does not leave the king in check
        GameState game_state_copy = game_state;
        parent_game.tryMoveOnStateCopy(move, game_state_copy);
        if (!isKingInCheckAfterMove(move, game_state_copy)) {
            return false; // Found a legal move that escapes check
        }
    }

    // No legal moves escape check
    return true;
}

bool
Rules::isDrawByFiftyMoveRule(const GameState& state) const
{
    // The half move clock is updated after each move by updateGameState.
    // Here, all we have to do is check if the 50 moves (100 half moves)
    // has been reached.
    if (state._halfmove_clock >= 100) {
        return true;
    }
    return false;
}

bool 
Rules::isDrawByInsufficientMaterial(const GameState& state) const
{
    // The following cases result in a draw by insufficient material
    // - King vs King
    // - King and Bishop vs King
    // - King and Knight vs King
    // - King and Bishop vs King and Bishop (bishops on same color)

    uint8_t white_bishops = 0, black_bishops = 0;
    uint8_t white_knights = 0, black_knights = 0;
    uint8_t white_rooks = 0, black_rooks = 0;
    uint8_t white_queens = 0, black_queens = 0;
    uint8_t white_pawns = 0, black_pawns = 0;
    uint8_t white_bishops_light = 0, white_bishops_dark = 0;
    uint8_t black_bishops_light = 0, black_bishops_dark = 0;

    for (int rank = 1; rank <= 8; ++rank) {
        for (int file = 1; file <= 8; ++file) {
            Piece p = state.board[rank-1][file-1].piece;
            int square_color = (rank + file) % 2; // 0 = dark, 1 = light
            switch (p) {
                case Piece::WHITE_BISHOP:
                {
                    white_bishops++;
                    if (square_color == 1) white_bishops_light++; 
                    else white_bishops_dark++;
                    break;
                }
                case Piece::BLACK_BISHOP:
                {
                    black_bishops++;
                    if (square_color == 1) black_bishops_light++;
                    else black_bishops_dark++;
                    break;
                }
                case Piece::WHITE_KNIGHT: white_knights++; break;
                case Piece::BLACK_KNIGHT: black_knights++; break;
                case Piece::WHITE_ROOK: white_rooks++; break;
                case Piece::BLACK_ROOK: black_rooks++; break;
                case Piece::WHITE_QUEEN: white_queens++; break;
                case Piece::BLACK_QUEEN: black_queens++; break;
                case Piece::WHITE_PAWN: white_pawns++; break;
                case Piece::BLACK_PAWN: black_pawns++; break;
                default: break;
            }
        }
    }

    // In all the cases there must be no rooks or queens
    if (white_rooks == 0 && black_rooks == 0 && white_queens == 0 && black_queens == 0) {

        // Only kings are left on the board
        if (white_bishops == 0 && white_knights == 0 &&
            black_bishops == 0 && black_knights == 0) {
            return true;
        }

        // King and minor piece vs king
        if ((white_bishops + white_knights == 1 && black_bishops + black_knights == 0) ||
            (black_bishops + black_knights == 1 && white_bishops + white_knights == 0)) {
            return true;
        }

        // King and Bishop vs King and Bishop (bishops on same color)
        if (white_knights == 0 && black_knights == 0 && white_bishops == 1 && black_bishops == 1) {
            if ((white_bishops_light == 1 && black_bishops_light == 1) ||
                (white_bishops_dark == 1 && black_bishops_dark == 1)) {
                return true;
            }
        }
    }

    return false;
}

bool
Rules::isKingInCheck(const GameState& game_state) const
{
    Piece king = game_state._current_player == Player::WHITE ? Piece::WHITE_KING : Piece::BLACK_KING;
    Player opponent = game_state._current_player == Player::WHITE ? Player::BLACK : Player::WHITE;

    // First locate the current player's king
    for (int8_t r = 1; r <= 8; r++) {
        for (int8_t f = 1; f <= 8; f++) {
            if (game_state.board[r-1][f-1].piece == king) {
                return isSquareUnderAttackBy(r, f, game_state, opponent);
            }
        }
    }
    return false;
}

bool
Rules::isKingInCheckAfterMove(const Move& move, const GameState& game_state) const
{
    GameState copy = game_state;
    Player current_player_before_move;
    Player opponent_before_move;
    Piece king_before_move;

    if (game_state._current_player == Player::WHITE) {
        current_player_before_move = Player::WHITE;
        opponent_before_move = Player::BLACK;
        king_before_move = Piece::WHITE_KING;
    } else {
        current_player_before_move = Player::BLACK;
        opponent_before_move = Player::WHITE;
        king_before_move = Piece::BLACK_KING;
    }

    // Try the move on a copy of the game state
    parent_game.tryMoveOnStateCopy(move, copy);

    // After we try the move, the current player is updated, and players are reversed.
    // Just ignore this and use the players and pieces from before the move.

    // Find the previous player's king
    for (int8_t r = 1; r <= 8; r++) {
        for (int8_t f = 1; f <= 8; f++) {
            if (copy.board[r-1][f-1].piece == king_before_move) {
                // Check if the king is being attacked after moving there
                if (isSquareUnderAttackBy(r, f, copy, opponent_before_move)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool
Rules::isKingSideCastle(const Move& move) const
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
Rules::isQueenSideCastle(const Move& move) const
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

/**
 * @brief Checks if a move is legal, with the game board in it's current state
 * @param[in] move - The move to check if it's legal
 * @param[in] game_state - The current game state, before the move is made
 * @return true if the move is legal, false otherwise
 */
bool
Rules::isLegalMove(const Move& move, const GameState& game_state) const
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

bool
Rules::isSameColor(const Piece& piece1, const Piece& piece2) const
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

/**
 * @brief Checks if a square is occupied by a piece
 * Assumes:
 *     - Rank and file are in bounds
 */
bool
Rules::isSquareOccupied(int8_t rank, int8_t file, const GameState& game_state) const
{
    // Check if the square is occupied
    return game_state.board[rank-1][file-1].piece != Piece::EMPTY;
}

bool
Rules::isSquareOccupiedBy(int8_t rank, int8_t file, const GameState& state, Player player) const
{
    // Check if the square is occupied by a piece of the specified player
    Piece piece = state.board[rank-1][file-1].piece;
    if (player == Player::WHITE) {
        return piece == Piece::WHITE_PAWN || piece == Piece::WHITE_BISHOP ||
               piece == Piece::WHITE_ROOK || piece == Piece::WHITE_KNIGHT ||
               piece == Piece::WHITE_QUEEN || piece == Piece::WHITE_KING;
    } else {
        return piece == Piece::BLACK_PAWN || piece == Piece::BLACK_BISHOP ||
               piece == Piece::BLACK_ROOK || piece == Piece::BLACK_KNIGHT ||
               piece == Piece::BLACK_QUEEN || piece == Piece::BLACK_KING;
    }
}

bool
Rules::isSquareUnderAttackBy(int8_t rank, int8_t file, const GameState& state, Player opponent) const
{
    // First check all the knights
    if (isSquareUnderAttackByKnight(rank, file, state, opponent)) {
        return true;
    }

    // Then check all the rooks and queens horizontally and vertically
    if (isSquareUnderAttackByRookOrQueen(rank, file, state, opponent)) {
        return true;
    }

    // Then check all the bishops and queens diagonally
    if (isSquareUnderAttackByBishopOrQueen(rank, file, state, opponent)) {
        return true;
    }

    // Then check for pawn attacks
    if (isSquareUnderAttackByPawn(rank, file, state, opponent)) {
        return true;
    }

    // Finally check for king attacks
    if (isSquareUnderAttackByKing(rank, file, state, opponent)) {
        return true;
    }

    return false;
}

bool
Rules::isSquareUnderAttackByBishopOrQueen(int8_t rank, int8_t file, const GameState& state,
        Player opponent) const
{
    Piece opposing_bishop, opposing_queen;
    if (opponent == Player::BLACK) {
        opposing_bishop = Piece::BLACK_BISHOP;
        opposing_queen = Piece::BLACK_QUEEN;
    } else {
        opposing_bishop = Piece::WHITE_BISHOP;
        opposing_queen = Piece::WHITE_QUEEN;
    }

    // Check the +x, +y diagonal
    for (int8_t r = rank + 1, f = file + 1; r <= 8 && f <= 8; r++, f++) {
        Piece piece = state.board[r-1][f-1].piece;
        if (piece == opposing_bishop || piece == opposing_queen) {
            return true;
        }
        if (piece != Piece::EMPTY) {
            break; // Path blocked
        }
    }

    // Check the +x, -y diagonal
    for (int8_t r = rank + 1, f = file - 1; r <= 8 && f >= 1; r++, f--) {
        Piece piece = state.board[r-1][f-1].piece;
        if (piece == opposing_bishop || piece == opposing_queen) {
            return true;
        }
        if (piece != Piece::EMPTY) {
            break; // Path blocked
        }
    }

    // Check the -x, -y diagonal
    for (int8_t r = rank - 1, f = file - 1; r >= 1 && f >= 1; r--, f--) {
        Piece piece = state.board[r-1][f-1].piece;
        if (piece == opposing_bishop || piece == opposing_queen) {
            return true;
        }
        if (piece != Piece::EMPTY) {
            break; // Path blocked
        }
    }

    // Check the -x, +y diagonal
    for (int8_t r = rank - 1, f = file + 1; r >= 1 && f <= 8; r--, f++) {
        Piece piece = state.board[r-1][f-1].piece;
        if (piece == opposing_bishop || piece == opposing_queen) {
            return true;
        }
        if (piece != Piece::EMPTY) {
            break; // Path blocked
        }
    }

    return false;
}

bool
Rules::isSquareUnderAttackByKing(int8_t rank, int8_t file, const GameState& state,
        Player opponent) const
{
    Piece opposing_king;
    if (opponent == Player::BLACK) {
        opposing_king = Piece::BLACK_KING;
    } else {
        opposing_king = Piece::WHITE_KING;
    }

    // Check all 8 squares around the given square for an opposing king
    for (int8_t r = -1; r <= 1; r++) {
        for (int8_t f = -1; f <= 1; f++) {

            if (r == 0 && f == 0) continue; // Skip the center square

            int8_t check_rank = rank + r;
            int8_t check_file = file + f;
            
            // Check bounds
            if (check_rank >= 1 && check_rank <= 8 && check_file >= 1 && check_file <= 8) {
                if (state.board[check_rank-1][check_file-1].piece == opposing_king) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool
Rules::isSquareUnderAttackByKnight(int8_t rank, int8_t file, const GameState& state,
        Player opponent) const
{
    Piece opposing_knight;
    if (opponent == Player::WHITE) {
        opposing_knight = Piece::WHITE_KNIGHT;
    } else {
        opposing_knight = Piece::BLACK_KNIGHT;
    }

    // Check all knight positions

    // Check for knight at 1 o'clock
    if (rank + 2 <= 8 && file + 1 <= 8) {
        if (state.board[rank+2-1][file+1-1].piece == opposing_knight) {
            return true;
        }
    }
    // Check for knight at 2 o'clock
    if (rank + 1 <= 8 && file + 2 <= 8) {
        if (state.board[rank+1-1][file+2-1].piece == opposing_knight) {
            return true;
        }
    }
    // Check for knight at 4 o'clock
    if (rank - 1 >= 1 && file + 2 <= 8) {
        if (state.board[rank-1-1][file+2-1].piece == opposing_knight) {
            return true;
        }
    }
    // Check for knight at 5 o'clock
    if (rank - 2 >= 1 && file + 1 <= 8) {
        if (state.board[rank-2-1][file+1-1].piece == opposing_knight) {
            return true;
        }
    }
    // Check for knight at 7 o'clock
    if (rank - 2 >= 1 && file - 1 >= 1) {
        if (state.board[rank-2-1][file-1-1].piece == opposing_knight) {
            return true;
        }
    }
    // Check for knight at 8 o'clock
    if (rank - 1 >= 1 && file - 2 >= 1) {
        if (state.board[rank-1-1][file-2-1].piece == opposing_knight) {
            return true;
        }
    }
    // Check for knight at 10 o'clock
    if (rank + 1 <= 8 && file - 2 >= 1) {
        if (state.board[rank+1-1][file-2-1].piece == opposing_knight) {
            return true;
        }
    }
    // Check for knight at 11 o'clock
    if (rank + 2 <= 8 && file - 1 >= 1) {
        if (state.board[rank+2-1][file-1-1].piece == opposing_knight) {
            return true;
        }
    }
    return false;
}

bool
Rules::isSquareUnderAttackByPawn(int8_t rank, int8_t file, const GameState& state,
        Player opponent) const
{
    int8_t attack_direction;
    Piece opposing_pawn;
    if (opponent == Player::BLACK) {
        opposing_pawn = Piece::BLACK_PAWN;
        attack_direction = 1;  // Black pawns attack from higher ranks
    } else {
        opposing_pawn = Piece::WHITE_PAWN;
        attack_direction = -1; // White pawns attack from lower ranks
    }

    int8_t pawn_rank = rank + attack_direction;
    
    // Check bounds and diagonals where opposing pawns could be
    if (pawn_rank >= 1 && pawn_rank <= 8) {
        // Left diagonal
        if (file - 1 >= 1) {
            if (state.board[pawn_rank-1][file-1-1].piece == opposing_pawn) {
                return true;
            }
        }
        // Right diagonal
        if (file + 1 <= 8) {
            if (state.board[pawn_rank-1][file+1-1].piece == opposing_pawn) {
                return true;
            }
        }
    }
    return false;
}

bool
Rules::isSquareUnderAttackByRookOrQueen(int8_t rank, int8_t file, const GameState& state,
        Player opponent) const
{
    Piece opposing_rook, opposing_queen;
    if (opponent == Player::BLACK) {
        opposing_rook = Piece::BLACK_ROOK;
        opposing_queen = Piece::BLACK_QUEEN;
    } else {
        opposing_rook = Piece::WHITE_ROOK;
        opposing_queen = Piece::WHITE_QUEEN;
    }

    // Check in the +x direction
    for (int8_t f = file + 1; f <= 8; f++) {
        Piece piece = state.board[rank-1][f-1].piece;
        if (piece == opposing_rook || piece == opposing_queen) {
            return true;
        }
        if (piece != Piece::EMPTY) {
            break; // Path blocked
        }
    }

    // Check in the -x direction
    for (int8_t f = file - 1; f >= 1; f--) {
        Piece piece = state.board[rank-1][f-1].piece;
        if (piece == opposing_rook || piece == opposing_queen) {
            return true;
        }
        if (piece != Piece::EMPTY) {
            break; // Path blocked
        }
    }

    // Check in the +y direction
    for (int8_t r = rank + 1; r <= 8; r++) {
        Piece piece = state.board[r-1][file-1].piece;
        if (piece == opposing_rook || piece == opposing_queen) {
            return true;
        }
        if (piece != Piece::EMPTY) {
            break; // Path blocked
        }
    }

    // Check in the -y direction
    for (int8_t r = rank - 1; r >= 1; r--) {
        Piece piece = state.board[r-1][file-1].piece;
        if (piece == opposing_rook || piece == opposing_queen) {
            return true;
        }
        if (piece != Piece::EMPTY) {
            break; // Path blocked
        }
    }

    return false;
}

bool
Rules::isStalemate(const GameState& game_state) const
{
    // A stalemate occurs when the king is not in check and current player has no legal moves
    if (isKingInCheck(game_state)) {
        return false; // If the king is in check, it's not stalemate (it might be a checkmate)
    }

    std::list<Move> legal_moves = generateLegalMovesForCurrentPlayer(game_state);
    if (legal_moves.empty()) {
        // No legal moves available, it's stalemate
        return true;
    }

    return false;
}

bool
Rules::isValidBishopMove(const Move& move, const GameState& game_state) const
{
    int8_t squares_moved_horizontally = std::abs(move.destination_file - move.source_file);
    int8_t squares_moved_vertically = std::abs(move.destination_rank - move.source_rank);

    if (squares_moved_horizontally != squares_moved_vertically) {
        return false;
    }

    int8_t squares_moved = squares_moved_horizontally;

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

/**
 * @brief Checks if a move is a valid king move
 * Assumes:
 *    - Piece is a king
 *    - The castling variables are up to date
 */
bool 
Rules::isValidKingMove(const Move& move, const GameState& game_state) const
{
    bool is_kingside_castle = isKingSideCastle(move);
    bool is_queenside_castle = isQueenSideCastle(move);
    bool is_castling = is_kingside_castle || is_queenside_castle;
    Player opponent = game_state._current_player == Player::WHITE ? Player::BLACK : Player::WHITE;  

    // The King is complicated. He can move one square in any direction. He can
    // move to an empty square, or can capture, he can also castle. He may never
    // move into check.

    int8_t rank_diff = std::abs(move.destination_rank - move.source_rank);
    int8_t file_diff = std::abs(move.destination_file - move.source_file);

    // King can only move one square in any direction (except for castling, handled below)
    if (!is_castling && (rank_diff > 1 || file_diff > 1)) {
        return false;
    }
    // If the king moves, he (obviously) can't stay in the same place
    if (rank_diff == 0 && file_diff == 0) {
        return false;
    }
    // Check if destination square has a piece of the same color (can't capture own pieces)
    if (destinationContainsSamePlayer(move, game_state)) {
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
            if (isSquareUnderAttackBy(king_rank, 5, game_state, opponent) || // King currently in check
                isSquareUnderAttackBy(king_rank, 6, game_state, opponent) || // f-file under attack
                isSquareUnderAttackBy(king_rank, 7, game_state, opponent)) { // g-file under attack
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
            if (isSquareUnderAttackBy(king_rank, 5, game_state, opponent) || // King currently in check
                isSquareUnderAttackBy(king_rank, 4, game_state, opponent) || // d-file under attack
                isSquareUnderAttackBy(king_rank, 3, game_state, opponent)) { // c-file under attack
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
Rules::isValidKnightMove(const Move& move, const GameState& game_state) const
{
    Player player = move.piece == Piece::WHITE_KNIGHT ? Player::WHITE : Player::BLACK;
    Player opponent = player == Player::WHITE ? Player::BLACK : Player::WHITE;
    int8_t rank_diff = std::abs(move.destination_rank - move.source_rank);
    int8_t file_diff = std::abs(move.destination_file - move.source_file);

    // Check that the move is an L-shape: two squares in one direction and one square in the other
    if ((rank_diff == 2 && file_diff == 1) || (rank_diff == 1 && file_diff == 2)) {
        // Check that the destination is empty or an opposing piece
        if (isSquareOccupied(move.destination_rank, move.destination_file, game_state)) {
            // You can only capture your opponent's pieces, not your own
            if (isSquareOccupiedBy(move.destination_rank, move.destination_file,
                                    game_state, opponent)) {
                return true;
            }
        } else {
            // You can always move to an empty square
            return true;
        }
    }
    return false;
}

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
Rules::isValidPawnMove(const Move& move, const GameState& game_state) const
{
    int8_t forward_one_square = move.piece == Piece::WHITE_PAWN ? 1 : -1;
    int8_t forward_two_squares = move.piece == Piece::WHITE_PAWN ? 2 : -2;
    int8_t pawn_starting_rank = move.piece == Piece::WHITE_PAWN ? 2 : 7;
    int8_t en_passant_source_rank = move.piece == Piece::WHITE_PAWN ? 5 : 4;
    Player opponent = move.piece == Piece::WHITE_PAWN ? Player::BLACK : Player::WHITE;

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
        if (isSquareOccupiedBy(move.destination_rank, move.destination_file, game_state, opponent)) {
            return true;
        }

        if (move.source_rank == en_passant_source_rank &&
            game_state._two_square_pawn_push_just_occured &&
            move.destination_rank == game_state._en_passant_target_square_rank &&
            move.destination_file == game_state._en_passant_target_square_file) {
            // If the opponent's pawn just moved two squares forward, and they're
            // moving into the en passant target square, it's a valid en passant capture
            return true;
        }
    }
    return false;
}

bool 
Rules::isValidQueenMove(const Move& move, const GameState& game_state) const
{
    if (isValidBishopMove(move, game_state) || isValidRookMove(move, game_state)) {
        return true;
    }
    return false;
}

bool
Rules::isValidRookMove(const Move& move, const GameState& game_state) const
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
    if (destinationContainsSamePlayer(move, game_state)) {
        return false;
    }

    return true;
}

GameResult
Rules::checkForGameEndings(const GameState& game_state, position_hash_t& repetition_table) const
{
    if (isCheckmate(game_state)) {
        return GameResult::CHECKMATE;
    } else if (isStalemate(game_state)) {
        return GameResult::STALEMATE;
    } else if (isDrawByFiftyMoveRule(game_state)) {
        return GameResult::FIFTY_MOVE;
    } else if (parent_game.isDrawByThreefoldRepetition(game_state, repetition_table)) {
        return GameResult::THREEFOLD;
    } else if (isDrawByInsufficientMaterial(game_state)) {
        return GameResult::INSUFFICIENT_MATERIAL;
    }
    return GameResult::NONE;
}

std::list<Move>
Rules::generateLegalMovesForCurrentPlayer(const GameState& game_state) const
{
    Player player = game_state._current_player;
    std::list<Move> legal_moves;

    // For each square on the board
    for (uint8_t r = 1; r <= 8; r++) {
        for (uint8_t f = 1; f <= 8; f++) {
            // Get the current piece
            Piece current_piece = game_state.board[r-1][f-1].piece;

            if (current_piece == Piece::EMPTY) {
                continue; // Skip empty squares
            }
            if (player == Player::WHITE && isBlack(current_piece) ||
                player == Player::BLACK && isWhite(current_piece)) {
                continue; // Skip opponent's pieces
            }

            // Get all the legal moves for this piece
            std::list<Move> legal_moves_for_piece =
                    generateLegalMovesForPieceAt(r, f, game_state, current_piece);

            // And add it to the larger list
            if (!legal_moves_for_piece.empty()) {
                legal_moves.insert(legal_moves.end(), legal_moves_for_piece.begin(),
                    legal_moves_for_piece.end());
            }

        }
    }
    debugLog("returning legal moves of size %zu\n", legal_moves.size());
    return legal_moves;
}

std::list<Move>
Rules::generateLegalMovesForPieceAt(uint8_t rank, uint8_t file, const GameState& game_state,
        const Piece& piece) const
{
    std::list<Move> possible_moves;
    std::list<Move> legal_moves_for_piece;

    switch (piece)
    {
        case Piece::WHITE_PAWN:
        case Piece::BLACK_PAWN:
            possible_moves = generatePossibleMovesForPawnAt(rank, file, game_state, piece);
            break;
        case Piece::WHITE_BISHOP:
        case Piece::BLACK_BISHOP:
            possible_moves = generatePossibleMovesForBishopAt(rank, file, game_state, piece);
            break;
        case Piece::WHITE_KNIGHT:
        case Piece::BLACK_KNIGHT:
            possible_moves = generatePossibleMovesForKnightAt(rank, file, game_state, piece);
            break;
        case Piece::WHITE_ROOK:
        case Piece::BLACK_ROOK:
            possible_moves = generatePossibleMovesForRookAt(rank, file, game_state, piece);
            break;
        case Piece::WHITE_QUEEN:
        case Piece::BLACK_QUEEN:
            possible_moves = generatePossibleMovesForQueenAt(rank, file, game_state, piece);
            break;
        case Piece::WHITE_KING:
        case Piece::BLACK_KING:
            possible_moves = generatePossibleMovesForKingAt(rank, file, game_state, piece);
            break;
        case Piece::EMPTY:
            break;
    }

    // Generate all possible moves for the piece
    for (const Move& move : possible_moves) {
        // Check if the move is legal
        if (isLegalMove(move, game_state)) {
            legal_moves_for_piece.push_back(move);
        }
    }

    return legal_moves_for_piece;
}

std::list<Move>
Rules::generatePossibleMovesForPawnAt(uint8_t rank, uint8_t file,
        const GameState& game_state, const Piece& piece) const
{
    int8_t direction, start_rank, en_passant_rank;
    Player player, opponent;
    std::list<Move> moves;

    if (piece == Piece::WHITE_PAWN) {
        direction = 1;
        start_rank = 2;
        en_passant_rank = 5;
        player = Player::WHITE;
        opponent = Player::BLACK;
    } else {
        direction = -1;
        start_rank = 7;
        en_passant_rank = 4;
        player = Player::BLACK;
        opponent = Player::WHITE;
    }

    // Forward one square
    int8_t forward_one_rank = rank + direction;
    if (forward_one_rank >= 1 && forward_one_rank <= 8) {
        if (!isSquareOccupied(forward_one_rank, file, game_state)) {
            Move m;
            m.piece = piece;
            m.source_rank = rank;
            m.source_file = file;
            m.destination_rank = forward_one_rank;
            m.destination_file = file;
            moves.push_back(m);
        }
    }

    // Forward two squares from starting position
    if (rank == start_rank) {
        int8_t forward_two_ranks = rank + 2 * direction;
        if (!isSquareOccupied(forward_one_rank, file, game_state) &&
            !isSquareOccupied(forward_two_ranks, file, game_state)) {
            Move m2;
            m2.piece = piece;
            m2.source_rank = rank;
            m2.source_file = file;
            m2.destination_rank = forward_two_ranks;
            m2.destination_file = file;
            moves.push_back(m2);
        }
    }

    // Captures (diagonal left and right)
    for (int8_t df = -1; df <= 1; df += 2) {
        int8_t dest_file = file + df;
        if (forward_one_rank >= 1 && forward_one_rank <= 8 && dest_file >= 1 && dest_file <= 8) {
            if (isSquareOccupiedBy(forward_one_rank, dest_file, game_state, opponent)) {
                Move m;
                m.piece = piece;
                m.source_rank = rank;
                m.source_file = file;
                m.destination_rank = forward_one_rank;
                m.destination_file = dest_file;
                moves.push_back(m);
            }
        }
    }

    // En passant
    if (rank == en_passant_rank && game_state._two_square_pawn_push_just_occured) {
        // Only one en passant target square exists, check that square
        if (forward_one_rank == game_state._en_passant_target_square_rank &&
            std::abs(file - game_state._en_passant_target_square_file) == 1) {
            int8_t dest_file = game_state._en_passant_target_square_file;
            Move m;
            m.piece = piece;
            m.source_rank = rank;
            m.source_file = file;
            m.destination_rank = forward_one_rank;
            m.destination_file = dest_file;
            moves.push_back(m);
        }
    }

    return moves;
}

std::list<Move>
Rules::generatePossibleMovesForBishopAt(uint8_t rank, uint8_t file,
        const GameState& game_state, const Piece& piece) const
{
    std::list<Move> moves;
    Player player = isWhite(piece) ? Player::WHITE : Player::BLACK;
    Player opponent = player == Player::WHITE ? Player::BLACK : Player::WHITE;

    // Directions: (+1,+1), (+1,-1), (-1,+1), (-1,-1)
    int8_t directions[4][2] = { {1,1}, {1,-1}, {-1,1}, {-1,-1} };
    for (int d = 0; d < 4; ++d) {
        int8_t dr = directions[d][0];
        int8_t df = directions[d][1];
        uint8_t r = rank;
        uint8_t f = file;
        while (true) {
            r += dr;
            f += df;
            if (r < 1 || r > 8 || f < 1 || f > 8) break;
            Piece dest_piece = game_state.board[r-1][f-1].piece;
            if (dest_piece == Piece::EMPTY) {
                Move m;
                m.piece = piece;
                m.source_rank = rank;
                m.source_file = file;
                m.destination_rank = r;
                m.destination_file = f;
                moves.push_back(m);
            } else {
                if (!isSameColor(piece, dest_piece)) {
                    Move m;
                    m.piece = piece;
                    m.source_rank = rank;
                    m.source_file = file;
                    m.destination_rank = r;
                    m.destination_file = f;
                    moves.push_back(m);
                }
                break; // Blocked by any piece
            }
        }
    }
    return moves;
}

std::list<Move>
Rules::generatePossibleMovesForKnightAt(uint8_t rank, uint8_t file,
        const GameState& game_state, const Piece& piece) const
{
    std::list<Move> moves;
    Player player = isWhite(piece) ? Player::WHITE : Player::BLACK;

    // All 8 possible knight moves (L-shapes)
    int8_t offsets[8][2] = {
        { 2,  1}, { 1,  2}, {-1,  2}, {-2,  1},
        {-2, -1}, {-1, -2}, { 1, -2}, { 2, -1}
    };

    for (int i = 0; i < 8; ++i) {
        int8_t r = rank + offsets[i][0];
        int8_t f = file + offsets[i][1];
        if (r >= 1 && r <= 8 && f >= 1 && f <= 8) {
            Piece dest_piece = game_state.board[r-1][f-1].piece;
            if (dest_piece == Piece::EMPTY || !isSameColor(piece, dest_piece)) {
                Move m;
                m.piece = piece;
                m.source_rank = rank;
                m.source_file = file;
                m.destination_rank = r;
                m.destination_file = f;
                moves.push_back(m);
            }
        }
    }
    return moves;
}

std::list<Move>
Rules::generatePossibleMovesForRookAt(uint8_t rank, uint8_t file,
        const GameState& game_state, const Piece& piece) const
{
    std::list<Move> moves;
    Player player = isWhite(piece) ? Player::WHITE : Player::BLACK;
    Player opponent = player == Player::WHITE ? Player::BLACK : Player::WHITE;

    // Directions: (0,+1), (0,-1), (+1,0), (-1,0)
    int8_t directions[4][2] = { {0,1}, {0,-1}, {1,0}, {-1,0} };
    for (int d = 0; d < 4; ++d) {
        int8_t dr = directions[d][0];
        int8_t df = directions[d][1];
        uint8_t r = rank;
        uint8_t f = file;
        while (true) {
            r += dr;
            f += df;
            if (r < 1 || r > 8 || f < 1 || f > 8) break;
            Piece dest_piece = game_state.board[r-1][f-1].piece;
            if (dest_piece == Piece::EMPTY) {
                Move m;
                m.piece = piece;
                m.source_rank = rank;
                m.source_file = file;
                m.destination_rank = r;
                m.destination_file = f;
                moves.push_back(m);
            } else {
                if (!isSameColor(piece, dest_piece)) {
                    Move m;
                    m.piece = piece;
                    m.source_rank = rank;
                    m.source_file = file;
                    m.destination_rank = r;
                    m.destination_file = f;
                    moves.push_back(m);
                }
                break; // Blocked by any piece
            }
        }
    }
    return moves;
}

std::list<Move>
Rules::generatePossibleMovesForQueenAt(uint8_t rank, uint8_t file,
        const GameState& game_state, const Piece& piece) const
{
    // Add the possible moves for a rook
    std::list<Move> moves = generatePossibleMovesForRookAt(rank, file, game_state, piece);

    // And the possible moves for a bishop
    std::list<Move> bishop_moves = generatePossibleMovesForBishopAt(rank, file, game_state, piece);

    // Combine the rook moves and the bishop moves into one list
    moves.splice(moves.end(), bishop_moves);

    return moves;
}

std::list<Move>
Rules::generatePossibleMovesForKingAt(uint8_t rank, uint8_t file,
        const GameState& game_state, const Piece& piece) const
{
    std::list<Move> moves;
    Player player = isWhite(piece) ? Player::WHITE : Player::BLACK;

    // All 8 possible king moves (one square in any direction)
    int8_t offsets[8][2] = {
        { 1, 0}, { 1,  1}, { 0,  1}, {-1, 1},
        {-1, 0}, {-1, -1}, { 0, -1}, {1, -1}
    };

    for (int i = 0; i < 8; ++i) {
        int8_t r = rank + offsets[i][0];
        int8_t f = file + offsets[i][1];
        if (r >= 1 && r <= 8 && f >= 1 && f <= 8) {
            Piece dest_piece = game_state.board[r-1][f-1].piece;
            if (dest_piece == Piece::EMPTY || !isSameColor(piece, dest_piece)) {
                Move m;
                m.piece = piece;
                m.source_rank = rank;
                m.source_file = file;
                m.destination_rank = r;
                m.destination_file = f;
                moves.push_back(m);
            }
        }
    }

    // Only consider castling if king is on its original square
    if ((piece == Piece::WHITE_KING && rank == 1 && file == 5) ||
        (piece == Piece::BLACK_KING && rank == 8 && file == 5)) {
        if (piece == Piece::WHITE_KING && game_state._white_kingside_castle_allowed ||
            piece == Piece::BLACK_KING && game_state._black_kingside_castle_allowed) {
            // Construct kingside castling move
            Move kingside;
            kingside.piece = piece;
            kingside.source_rank = rank;
            kingside.source_file = file;
            kingside.destination_rank = rank;
            kingside.destination_file = 7;
            if (isValidKingMove(kingside, game_state)) {
                moves.push_back(kingside);
            }
        }

        if (piece == Piece::WHITE_KING && game_state._white_queenside_castle_allowed ||
            piece == Piece::BLACK_KING && game_state._black_queenside_castle_allowed) {
            // Construct queenside castling move
            Move queenside;
            queenside.piece = piece;
            queenside.source_rank = rank;
            queenside.source_file = file;
            queenside.destination_rank = rank;
            queenside.destination_file = 3;
            if (isValidKingMove(queenside, game_state)) {
                moves.push_back(queenside);
            }
        }
    }

    return moves;
}