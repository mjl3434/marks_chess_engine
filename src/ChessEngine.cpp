#include "ChessEngine.h"

#include <cctype>
#include <cstdint>
#include <iostream>
#include <list>
#include <regex>
#include <sstream>
#include <unordered_set>

#include "HelperFunctions.h"

ChessEngine::ChessEngine()
{
    printf("Created new ChessEngine\n");
}

void
ChessEngine::startNewGame()
{
    printf("Started new game!\n");
    game_in_progress = true;

    // Clear any internal state left over from a previous game

    // Don't set up the board until we get a "position" command
}

bool
ChessEngine::isValidFen(const std::string& unvalidated_fen_string)
{
    // If we were clever, we could both validate the FEN string, and extract it's data into an
    // object all in one go. However, the logic here to validate is already complicated enough
    // to understand, so for now we will keep it separate. The validation logic is an
    // efficient single pass, and doing two passes one to validate, another to extract only
    // adds negligible overhead.

    // From Wikipedia:
    // 
    // A FEN record contains six fields, each separated by a space. The fields are as follows:
    // 
    // 1. Piece placement data: Each rank is described, starting with rank 8 and ending with rank 1,
    // with a "/" between each one; within each rank, the contents of the squares are described in
    // order from the a-file to the h-file. Each piece is identified by a single letter taken from
    // the standard English names in algebraic notation (pawn = "P", knight = "N", bishop = "B",
    // rook = "R", queen = "Q" and king = "K"). White pieces are designated using uppercase letters
    // ("PNBRQK"), while black pieces use lowercase letters ("pnbrqk"). A set of one or more
    // consecutive empty squares within a rank is denoted by a digit from "1" to "8", corresponding
    // to the number of squares.
    // 
    // 2. Active color: "w" means that White is to move; "b" means that Black is to move.
    // 
    // 3. Castling availability: If neither side has the ability to castle, this field uses the
    // character "-". Otherwise, this field contains one or more letters: "K" if White can castle
    // kingside, "Q" if White can castle queenside, "k" if Black can castle kingside, and "q" if
    // Black can castle queenside. A situation that temporarily prevents castling does not prevent
    // the use of this notation.
    //
    // 4. En passant target square: This is a square over which a pawn has just passed while moving
    // two squares; it is given in algebraic notation. If there is no en passant target square, this
    // field uses the character "-". This is recorded regardless of whether there is a pawn in
    // position to capture en passant.[6] An updated version of the spec has since made it so the
    // target square is recorded only if a legal en passant capture is possible, but the old version
    // of the standard is the one most commonly used.
    // 
    // 5. Halfmove clock: The number of halfmoves since the last capture or pawn advance, used for
    // the fifty-move rule.[9]
    //
    // 6. Fullmove number: The number of the full moves. It starts at 1 and is incremented after
    // Black's move.

    std::list<std::string> tokens;
    std::istringstream stream(unvalidated_fen_string);
    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
    }
    
    // There are exactly 6 fields in FEN notation, no more, no less
    if (tokens.size() != 6) {
        return false;
    }

    std::string board = tokens.front();
    tokens.pop_front();

    const uint8_t MAX_MAJOR_PIECE = 1;
    const uint8_t MAX_MINOR_PIECE = 2;
    const uint8_t MAX_PAWNS = 8;

    uint8_t white_rooks = 0, white_bishops = 0, white_knights = 0;
    uint8_t white_queens = 0, white_kings = 0, white_pawns = 0;
    uint8_t black_rooks = 0, black_bishops = 0, black_knights = 0;
    uint8_t black_queens = 0, black_kings = 0, black_pawns = 0;
    uint8_t empty_squares_in_row = 0, pieces_in_row = 0;
    uint8_t empty_squares_on_board = 0, pieces_on_board = 0;
    uint8_t total_rows = 0;

    // Begin validating field 1

    // Example FEN string (subset) for the initial board position:
    // rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR

    if (board[0] != '/') {
        total_rows++;
    } else {
        return false;
    }

    for (char c : board) {
        switch (c)
        {
            case 'P': pieces_in_row++; white_pawns++;  break;
            case 'R': pieces_in_row++; white_rooks++; break;
            case 'B': pieces_in_row++; white_bishops++; break;
            case 'N': pieces_in_row++; white_knights++; break;
            case 'K': pieces_in_row++; white_kings++; break;
            case 'Q': pieces_in_row++; white_queens++; break;
            case 'p': pieces_in_row++; black_pawns++; break;
            case 'r': pieces_in_row++; black_rooks++; break;
            case 'b': pieces_in_row++; black_bishops++; break;
            case 'n': pieces_in_row++; black_knights++; break;
            case 'k': pieces_in_row++; black_kings++; break;
            case 'q': pieces_in_row++; black_queens++; break;
            case '1': empty_squares_in_row += 1; break; 
            case '2': empty_squares_in_row += 2; break; 
            case '3': empty_squares_in_row += 3; break; 
            case '4': empty_squares_in_row += 4; break; 
            case '5': empty_squares_in_row += 5; break; 
            case '6': empty_squares_in_row += 6; break; 
            case '7': empty_squares_in_row += 7; break; 
            case '8': empty_squares_in_row += 8; break;
            case '/':
                // End of a row, update our totals
                empty_squares_on_board += empty_squares_in_row;
                pieces_on_board += pieces_in_row;
                total_rows++;
                //  Sanity check the current row
                if (pieces_in_row + empty_squares_in_row != 8) {
                    return false;
                }
                // Reset row counts
                pieces_in_row = 0;
                empty_squares_in_row = 0;
                break;
            default:
                return false;
                break;
        }
    }
    if (total_rows != 8 ||
        pieces_on_board + empty_squares_on_board != 64 ||
        white_pawns > MAX_PAWNS ||
        white_bishops > MAX_MINOR_PIECE ||
        white_knights > MAX_MINOR_PIECE ||
        white_rooks > MAX_MINOR_PIECE ||
        white_queens > MAX_MAJOR_PIECE ||
        white_kings > MAX_MAJOR_PIECE ||
        black_pawns > MAX_PAWNS ||
        black_bishops > MAX_MINOR_PIECE ||
        black_knights > MAX_MINOR_PIECE ||
        black_rooks > MAX_MINOR_PIECE ||
        black_queens > MAX_MAJOR_PIECE ||
        black_kings > MAX_MAJOR_PIECE) {
        return false;
    }

    // Begin validating field 2
    std::string active_color = tokens.front();
    tokens.pop_front();
    toLower(active_color);
    if (!(active_color == "w" || active_color == "b")) {
        return false;
    }

    // Begin validating field 3
    std::string castling_status = tokens.front();
    tokens.pop_front();
    if (castling_status != "-") {

        const std::string validChars = "KQkq";
        std::unordered_set<char> found;

        for (char c : castling_status) {
            // If we encountered an invalid character
            if (validChars.find(c) == std::string::npos) {
                return false;
            }
            // Each character can only occur once
            if (found.count(c) > 0) {
                return false;
            }
            found.insert(c);
        }

    } // else we have a "-" and the field is valid

    // Begin validating field 4
    std::string en_passant_target_square = tokens.front();
    tokens.pop_front();

    // The only valid squares are on ranks 3 for white and 6 for black
    if (en_passant_target_square != "-") {
        toLower(en_passant_target_square);
        // If it's not a valid square (a3-h3, or a6-h6), bail
        if (!((en_passant_target_square[0] >= 'a' && en_passant_target_square[0] <= 'h') &&
              (en_passant_target_square[1] == '3' || en_passant_target_square[1] == '6')))
        return false;
    }

    // Begin validating field 5
    std::string halfmove_clock = tokens.front();
    tokens.pop_front();

    int halfmove_count = 0;
    if (!convertStringToInt(halfmove_clock, &halfmove_count)) {
        // Converstion failed, field is invalid
        return false;
    } else {
        // Valid values are 0-100
        if (halfmove_count < 0 || halfmove_count > 100) {
            return false;
        }
    }

    // Begin validating field 6
    std::string fullmove_number = tokens.front();
    tokens.pop_front();

    int fullmove_count = 0;
    if (!convertStringToInt(fullmove_number, &fullmove_count)) {
        // Converstion failed, field is invalid
        return false;
    } else {
        // Valid values are > 0
        if (fullmove_count < 0) {
            return false;
        }
    }

    // All fields validated
    return true;
}

// Note this only checks if the move is valid algebric notation, not whether
// the move is legal and makes sense.
bool
ChessEngine::isValidAlgebraicNotation(const std::string& input)
{
    std::string move = input; 

    // Ignore case
    toLower(move);

    // e.g.: a2a4
    if ((move[0] >= 'a' && move[0] <= 'h') &&
        (move[1] >= '1' && move[1] <= '8') &&
        (move[2] >= 'a' && move[2] <= 'h') &&
        (move[3] >= '1' && move[3] <= '8')) {
        return true;
    }
    return false;
}

// Note: This must be called with a valid FEN string since no error checking is done
void
ChessEngine::setUpBoardFromFen(const std::string& fen)
{
    std::list<std::string> tokens;
    std::istringstream stream(fen);
    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
    }

    // First field, the board. E.g.:
    // rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR

    uint8_t row = 8;
    uint8_t column = 1;
    int num_blank_spaces = 0;

    std::string board_layout = tokens.front();
    tokens.pop_front();

    for (const char c : board_layout) {
        switch (c)
        {
            case 'P': 
                game.board[row-1][column-1].piece = Piece::WHITE_PAWN;
                column++;
                break;
            case 'R':
                game.board[row-1][column-1].piece = Piece::WHITE_ROOK;
                column++;
                break;
            case 'B':
                game.board[row-1][column-1].piece = Piece::WHITE_BISHOP;
                column++;
                break;
            case 'N':
                game.board[row-1][column-1].piece = Piece::WHITE_KNIGHT;
                column++;
                break;
            case 'K':
                game.board[row-1][column-1].piece = Piece::WHITE_KING;
                column++;
                break;
            case 'Q':
                game.board[row-1][column-1].piece = Piece::WHITE_QUEEN;
                column++;
                break;
            case 'p': 
                game.board[row-1][column-1].piece = Piece::BLACK_PAWN;
                column++;
                break;
            case 'r':
                game.board[row-1][column-1].piece = Piece::BLACK_ROOK;
                column++;
                break;
            case 'b':
                game.board[row-1][column-1].piece = Piece::BLACK_BISHOP;
                column++;
                break;
            case 'n':
                game.board[row-1][column-1].piece = Piece::BLACK_KNIGHT;
                column++;
                break;
            case 'k':
                game.board[row-1][column-1].piece = Piece::BLACK_KING;
                column++;
                break;
            case 'q':
                game.board[row-1][column-1].piece = Piece::BLACK_QUEEN;
                column++;
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                num_blank_spaces = atoi(&c);
                for (int i = 0; i < num_blank_spaces; i++) {
                    game.board[row-1][column-1].piece = Piece::EMPTY;
                    column++;
                }
                break;
            case '/':
                row -= 1;
                break;
        }
    }

    // Second field, current player turn

    std::string whos_turn = tokens.front();
    tokens.pop_front();
    toLower(whos_turn);

    if (whos_turn == "w") {
        game.current_players_turn = Player::WHITE;
    } else if (whos_turn == "b") {
        game.current_players_turn = Player::BLACK;
    }

    // Third field, castling availibility
    std::string castling = tokens.front();
    tokens.pop_front();

    game.white_kingside_castle_allowed = false;
    game.white_queenside_castle_allowed = false;
    game.black_kingside_castle_allowed = false;
    game.black_queenside_castle_allowed = false;

    for (const char c : castling) {
        switch (c)
        {
            case 'K':
                game.white_kingside_castle_allowed = true;
                break;
            case 'Q':
                game.white_queenside_castle_allowed = true;
                break;
            case 'k':
                game.black_kingside_castle_allowed = true;
                break;
            case 'q':
                game.black_queenside_castle_allowed = true;
                break;
            case '-':
            default:
                break;
        }
    }

    // Fourth field
    std::string en_passant_target_square = tokens.front();
    tokens.pop_front();

    // Begin validating field 4

    // The only valid squares are on ranks 3 for white and 6 for black
    if (en_passant_target_square != "-") {
        game.two_square_pawn_push_just_occured = true;
        toLower(en_passant_target_square);
        // Use subtraction to convert from a value in the ASCII range of
        // 97-104 for 'a'-'h' to, chess rank values of 1-8.
        game.en_passant_target_square_rank = en_passant_target_square[0] - 'a' + 1;
        game.en_passant_target_square_file = en_passant_target_square[1];
    } else {
        game.two_square_pawn_push_just_occured = false;
    }

    // Fifth field, halfmove clock
    std::string hmc = tokens.front();
    tokens.pop_front();

    int temp = 0;
    if (convertStringToInt(hmc, &temp)) {
        game.halfmove_clock = temp;
    }

    // Sixth field, total moves
    std::string total_moves = tokens.front();
    tokens.pop_front();

    temp = 0;
    if (convertStringToInt(total_moves, &temp)) {
        game.num_moves = temp;
    }
}

void ChessEngine::setUpBoardInStartPosition()
{
    // rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR
    game.board[0][0].piece = Piece::WHITE_ROOK;
    game.board[0][1].piece = Piece::WHITE_KNIGHT;
    game.board[0][2].piece = Piece::WHITE_BISHOP;
    game.board[0][3].piece = Piece::WHITE_QUEEN;
    game.board[0][4].piece = Piece::WHITE_KING;
    game.board[0][5].piece = Piece::WHITE_BISHOP;
    game.board[0][6].piece = Piece::WHITE_KNIGHT;
    game.board[0][7].piece = Piece::WHITE_ROOK;
    game.board[1][0].piece = Piece::WHITE_PAWN;
    game.board[1][1].piece = Piece::WHITE_PAWN;
    game.board[1][2].piece = Piece::WHITE_PAWN;
    game.board[1][3].piece = Piece::WHITE_PAWN;
    game.board[1][4].piece = Piece::WHITE_PAWN;
    game.board[1][5].piece = Piece::WHITE_PAWN;
    game.board[1][6].piece = Piece::WHITE_PAWN;
    game.board[1][7].piece = Piece::WHITE_PAWN;

    for (int row = 2; row <= 5; row++) {
        for (int column = 0; column < 8; column++) {
            game.board[row][column].piece = Piece::EMPTY;
        }
    }

    game.board[6][0].piece = Piece::BLACK_PAWN;
    game.board[6][1].piece = Piece::BLACK_PAWN;
    game.board[6][2].piece = Piece::BLACK_PAWN;
    game.board[6][3].piece = Piece::BLACK_PAWN;
    game.board[6][4].piece = Piece::BLACK_PAWN;
    game.board[6][5].piece = Piece::BLACK_PAWN;
    game.board[6][6].piece = Piece::BLACK_PAWN;
    game.board[6][7].piece = Piece::BLACK_PAWN;
    game.board[7][0].piece = Piece::BLACK_ROOK;
    game.board[7][1].piece = Piece::BLACK_KNIGHT;
    game.board[7][2].piece = Piece::BLACK_BISHOP;
    game.board[7][3].piece = Piece::BLACK_QUEEN;
    game.board[7][4].piece = Piece::BLACK_KING;
    game.board[7][5].piece = Piece::BLACK_BISHOP;
    game.board[7][6].piece = Piece::BLACK_KNIGHT;
    game.board[7][7].piece = Piece::BLACK_ROOK;
}