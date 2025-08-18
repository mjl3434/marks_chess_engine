#include "UniversalChessInterface.h"

#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>

#include "HelperFunctions.h"

bool
UniversalChessInterface::isValidFen(const std::string& unvalidated_fen_string)
{
    // If we were clever, we could both validate the FEN string, and extract it's data into an
    // object all in one go. However, the logic here to validate is already complicated enough so
    // we will keep it separate. The validation logic is an efficient single pass, and doing two
    // passes one to validate, another to extract only adds negligible overhead.

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
    if (!stringToInt(halfmove_clock, halfmove_count)) {
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
    if (!stringToInt(fullmove_number, fullmove_count)) {
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
UniversalChessInterface::isValidAlgebraicNotation(const std::string& input)
{
    std::string move = input; 
    toLower(move);

    if (move.length() != 4 && move.length() != 5) {
        return false;
    }

    // e.g.: a2a4
    if ((move[0] >= 'a' && move[0] <= 'h') &&
        (move[1] >= '1' && move[1] <= '8') &&
        (move[2] >= 'a' && move[2] <= 'h') &&
        (move[3] >= '1' && move[3] <= '8')) {

        // Promotion, e.g.: e7e8q
        if (move.length() == 5 && 
            (move[4] == 'q' || move[4] == 'r' ||
             move[4] == 'n' || move[4] == 'b')) {
            return true;
        }

        // Regular move
        if (move.length() == 4) {
            return true;
        }
    }
    return false;
}

bool
UniversalChessInterface::quitReceived(const std::string& input)
{
    if (input.length() >= 4) {
        std::string substring = input.substr(0, 4);
        toLower(substring);
        if (substring == "quit") {
            return true;
        }
    }
    return false;
}


// Parse the UCI input string, and if it's valid create a command object which
// contains all of the relevant data, parsed into useful fields
std::unique_ptr<UCICommand>
UniversalChessInterface::getCommand(const std::string& input)
{
    std::unique_ptr<UCICommand> command_to_return = nullptr;
    std::stringstream ss(input);
    std::list<std::string> tokens;
    std::string token;

    // Tokenize the input string to remove whitespace and simplify parsing
    while (ss >> token) {
        tokens.push_back(token);
    }

    // Get the first token from the list
    token = atFrontLowercase(tokens);

    // Make sure it's a valid keyword
    auto it = valid_keywords.find(token);
    if (it == valid_keywords.end()) {
        return nullptr;
    }

    switch (it->second)
    {
        case Keyword::DEBUG:
        {
            auto debug_command = std::make_unique<DebugCommand>(tokens);
            if (isValidDebugCommand(tokens, *debug_command)) {
                command_to_return = std::move(debug_command);
            } else {
                return nullptr; // Invalid debug command
            }
            break;
        }
        case Keyword::UCI:
        {
            auto uci_command = std::make_unique<UciCommand>(tokens);
            if (isValidNoArgCommand(tokens)) {
                command_to_return = std::move(uci_command);
            } else {
                return nullptr; // Invalid UCI command
            }
            break;
        }
        case Keyword::ISREADY:
        {
            auto is_ready_command = std::make_unique<IsReadyCommand>(tokens);
            if (isValidNoArgCommand(tokens)) {
                command_to_return = std::move(is_ready_command);
            } else {
                return nullptr; // Invalid isready command
            }
            break;
        }
        case Keyword::UCINEWGAME:
        {
            auto uci_new_game_command = std::make_unique<UciNewGameCommand>(tokens);
            if (isValidNoArgCommand(tokens)) {
                command_to_return = std::move(uci_new_game_command);
            } else {
                return nullptr; // Invalid ucinewgame command
            }
            break;
        }
        case Keyword::SETOPTION:
        {
            auto set_option_command = std::make_unique<SetOptionCommand>(tokens);
            if (isValidSetoptionCommand(input, *set_option_command)) {
                command_to_return = std::move(set_option_command);
            } else {
                return nullptr; // Invalid setoption command
            }
            break;
        }  
        case Keyword::REGISTER:
        {
            // Since this is free software we have no need to register a software license
            break;
        }
        case Keyword::POSITION:
        {
            auto position_command = std::make_unique<PositionCommand>(tokens);
            if (isValidPositionCommand(tokens, *position_command)) {
                command_to_return = std::move(position_command);
            } else {
                return nullptr; // Invalid position command
            }
            break;
        }
        case Keyword::GO:
        {
            auto go_command = std::make_unique<GoCommand>(tokens);
            if (isValidGoCommand(tokens, *go_command)) {
                command_to_return = std::move(go_command);
            } else {
                return nullptr; // Invalid go command
            }
            break;
        }
        case Keyword::STOP:
        {
            auto stop_command = std::make_unique<StopCommand>(tokens);
            if (isValidNoArgCommand(tokens)) {
                command_to_return = std::move(stop_command);
            } else {
                return nullptr; // Invalid stop command
            }
            break;
        }
        case Keyword::PONDERHIT:
        {
            auto ponder_hit_command = std::make_unique<PonderHitCommand>(tokens);
            if (isValidNoArgCommand(tokens)) {
                command_to_return = std::move(ponder_hit_command);
            } else {
                return nullptr; // Invalid ponderhit command
            }
            break;
        }
        default:
            break;
    }

    return command_to_return;
}

/**
 * @brief Validates a debug command, and returns the value
 * 
 * If the return value is false, all output arguments should be treated as
 * undefined and ignored.
 * 
 * @param[in] input - The whole UCI string
 * @param[out] enabled - True for debug enabled, false for disabled
 * @return True for valid command, false for invalid
 */
bool
UniversalChessInterface::isValidDebugCommand(const std::list<std::string>& tokens, DebugCommand& debug_command)
{
    // The only token after "debug" should be "on" or "off"
    if (tokens.size() != 2) {
        return false;
    }
    std::string value = tokens.back();
    toLower(value);
    if (value == "on") {
        debug_command.debug_enabled = true; 
    } else if (value == "off") {
        debug_command.debug_enabled = false; 
    } else {
        // Unknown value, invalid command
        return false;
    }
    return true;
}

/*
 stop and ponderhit all have no arguments
*/
bool
UniversalChessInterface::isValidNoArgCommand(std::list<std::string>& tokens)
{
    // Nothing comes after the command
    if (tokens.size() != 1) {
        return false;
    }
    return true;
}

/**
 * @brief Validates a setoption command, and returns option name and value
 * 
 * If the return value is false, all output arguments should be treated as
 * undefined and ignored.
 * 
 * @param[in] input - The whole UCI string
 * @param[out] set_option_command - Object containing the parsed data
 * @return True for valid command, false for invalid
 */
bool
UniversalChessInterface::isValidSetoptionCommand(const std::string& input, SetOptionCommand& set_option_command)
{
    // Expect an option string like 
    // "setoption name Style value Risky\n"
    // and capture "Style" and "Risky"

    std::regex pattern(R"(setoption\s+name\s+(\S+)\s+value\s+(\S+)\n)");
    std::smatch matches;

    if (std::regex_match(input, matches, pattern)) {
        std::cout << "Match successful!" << std::endl;
        std::cout << "Captured name: " << matches[1] << std::endl;
        set_option_command.name = matches[1];
        std::cout << "Captured value: " << matches[2] << std::endl;
        set_option_command.value = matches[2];
    } else {
        std::cout << "No match!" << std::endl;
        return false;
    }

    return true;
}

bool
UniversalChessInterface::isValidPositionCommand(std::list<std::string>& tokens, PositionCommand& position_command)
{
    // position [fen <fenstring> | startpos ]  moves <move1> .... <movei>
	// set up the position described in fenstring on the internal board and
	// play the moves on the internal chess board.
	// if the game was played  from the start position the string "startpos" will be sent
	// Note: no "new" command is needed. However, if this position is from a different game than
	// the last position sent to the engine, the GUI should have sent a "ucinewgame" inbetween. 

    const std::string startpos_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    // Get next token after "position"
    tokens.pop_front();
    std::string token = popFrontLowercase(tokens);

    // Get the correct FEN string to return
    if (token == "startpos") {
        position_command.fen = startpos_fen;
        if (tokens.size() == 0) {
            // We've reached the end of a "position startpos" command
            return true;
        }
    } else if (token == "fen") {
        // Extract the FEN string from the next 6 tokens
        std::string fen;
        if (tokens.size() < 6) {
            return false;
        }
        for (int i = 0; i < 6; i++) {
            token = tokens.front();
            tokens.pop_front();
            fen += token;
            if (i < 5) {
                fen += " ";
            }
        }
        if (!isValidFen(fen)) {
            return false;
        }
        position_command.fen = fen;
    } else {
        return false;
    }

    // It's legal to have no moves to play and return here
    if (tokens.size() == 0) {
        return true;
    }

    // Extract the moves token
    token = popFrontLowercase(tokens);
    if (token != "moves") {
        return false;
    }

    while (tokens.size() > 0) {
        token = popFrontLowercase(tokens);
        if (!isValidAlgebraicNotation(token)) {
            return false;
        } else {
            // Add our move to the moves list
            position_command.moves.push_back(Move(token));
        }
    }

    return true;
}

bool
UniversalChessInterface::isValidGoCommand(std::list<std::string>& tokens, GoCommand& go_command)
{
    // * go
    //     start calculating on the current position set up with the "position" command.
    //     There are a number of commands that can follow this command, all will be sent in the same string.
    //     If one command is not sent its value should be interpreted as it would not influence the search.
    //     * searchmoves <move1> .... <movei>
    //         restrict search to this moves only
    //         Example: After "position startpos" and "go infinite searchmoves e2e4 d2d4"
    //         the engine should only search the two moves e2e4 and d2d4 in the initial position.
    //     * ponder
    //         start searching in pondering mode.
    //         Do not exit the search in ponder mode, even if it's mate!
    //         This means that the last move sent in in the position string is the ponder move.
    //         The engine can do what it wants to do, but after a "ponderhit" command
    //         it should execute the suggested move to ponder on. This means that the ponder move sent by
    //         the GUI can be interpreted as a recommendation about which move to ponder. However, if the
    //         engine decides to ponder on a different move, it should not display any mainlines as they are
    //         likely to be misinterpreted by the GUI because the GUI expects the engine to ponder
    //         on the suggested move.
    //     * wtime <x>
    //         white has x msec left on the clock
    //     * btime <x>
    //         black has x msec left on the clock
    //     * winc <x>
    //         white increment per move in mseconds if x > 0
    //     * binc <x>
    //         black increment per move in mseconds if x > 0
    //     * movestogo <x>
    //         there are x moves to the next time control,
    //         this will only be sent if x > 0,
    //         if you don't get this and get the wtime and btime it's sudden death
    //     * depth <x>
    //         search x plies only.
    //     * nodes <x>
    //         search x nodes only,
    //     * mate <x>
    //         search for a mate in x moves
    //     * movetime <x>
    //         search exactly x mseconds
    //     * infinite
    //         search until the "stop" command. Do not exit the search without being told so in this mode!

    // Discard the first token, which should be "go"
    tokens.pop_front();

    while (tokens.size() > 0) {
        std::string token = popFrontLowercase(tokens);
        auto it = valid_keywords.find(token);
        if (it == valid_keywords.end()) {
            return false;
        }

        // Check after popping that there are additional arguments in the
        // token list for commands that require them.
        if (it->second != Keyword::PONDER &&
            it->second != Keyword::INFINITE && 
            tokens.size() == 0) {
            return false;
        }

        switch (it->second) {
            case Keyword::PONDER:
                go_command.ponder = true;
                break;
            case Keyword::INFINITE:
                go_command.infinite = true;
                break;
            case Keyword::WTIME:
                token = popFrontLowercase(tokens);
                if (!stringToInt(token, go_command.wtime_ms)) {
                    return false;
                }
                break;
            case Keyword::BTIME:
                token = popFrontLowercase(tokens);
                if (!stringToInt(token, go_command.btime_ms)) {
                    return false;
                }
                break;
            case Keyword::WINC:
                token = popFrontLowercase(tokens);
                if (!stringToInt(token, go_command.winc_ms)) {
                    return false;
                }
                break;
            case Keyword::BINC:
                token = popFrontLowercase(tokens);
                if (!stringToInt(token, go_command.binc_ms)) {
                    return false;
                }
                break;
            case Keyword::MOVESTOGO:
                token = popFrontLowercase(tokens);
                if (!stringToInt(token, go_command.movestogo)) {
                    return false;
                }
                break;
            case Keyword::DEPTH:
                token = popFrontLowercase(tokens);
                if (!stringToInt(token, go_command.max_depth)) {
                    return false;
                }
                break;
            case Keyword::NODES:
                token = popFrontLowercase(tokens);
                if (!stringToInt(token, go_command.max_nodes)) {
                    return false;
                }
                break;
            case Keyword::MATE:
                token = popFrontLowercase(tokens);
                if (!stringToInt(token, go_command.mate_in_x)) {
                    return false;
                }
                break;
            case Keyword::MOVETIME:
                token = popFrontLowercase(tokens);
                if (!stringToInt(token, go_command.max_movetime_ms)) {
                    return false;
                }
                break;
            case Keyword::SEARCHMOVES:
                // The logic here is a little complicated because we allow for
                // the case where the UCI command is something like this:
                // 
                // go searchmoves e2e4 d2d4 infinite
                // 
                // So we my have to pop from the list of UCI strings, and if
                // we determine it's a keyword and not a move in algebraic
                // notation, we have to put it back in the front of the list.
                while (tokens.size() > 0) {
                    token = popFrontLowercase(tokens);
                    if (isValidAlgebraicNotation(token)) {
                        go_command.search_moves.push_back(Move(token));
                    } else {
                        // Perhaps there's another keyword
                        auto it = valid_keywords.find(token);
                        if (it != valid_keywords.end()) {
                            // Add it back to the list of tokens
                            tokens.push_front(token);
                            break;
                        } else {
                            // There was garbage after the last move
                            return false;
                        }
                    }
                }
                break;
            default:
                return false;
        }
    }
    return true;
}