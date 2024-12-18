#include "UniversalChessInterface.h"

#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>

#include "ChessEngine.h"
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

    if (move.length() != 4) {
        return false;
    }

    // e.g.: a2a4
    if ((move[0] >= 'a' && move[0] <= 'h') &&
        (move[1] >= '1' && move[1] <= '8') &&
        (move[2] >= 'a' && move[2] <= 'h') &&
        (move[3] >= '1' && move[3] <= '8')) {
        return true;
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
std::optional<Command>
UniversalChessInterface::getCommand(const std::string& input)
{
    std::optional<Command> command_to_return = std::nullopt;
    std::stringstream ss(input);
    std::list<std::string> tokens;
    std::string token;

    while (ss >> token) {
        tokens.push_back(token);
    }

    // Get the first token fron the list
    token = popFrontLowercase(tokens);

    // Make sure it's a valid keyword
    auto it = valid_keywords.find(token);
    if (it == valid_keywords.end()) {
        return std::nullopt;
    }

    switch (it->second)
    {
        case Keyword::DEBUG:
        {
            // Call isValidDebugCommand to check if the command syntax is valid,
            // and if so extract all of the relevant data to output arguments.
            // Next, create a lambda function that captures the local
            // variables by value, and returns the whole lambda inside a
            // std::optional, so that we can call the relevant chess engine
            // method later. This separates parsing of a UCI command, from
            // execution in the chess engine, and allows us to potentially que
            // up multiple engine commands, later if necssary by queuing up the
            // lambdas.
            bool enabled;
            if (isValidDebugCommand(tokens, enabled)) {
                Command debug_command = [enabled](ChessEngine& engine) {
                    engine.setDebug(enabled);
                };
                command_to_return.value() = debug_command;
            }
            break;
        }
        case Keyword::UCI:
        {
            // Be forgiving and ignore any trailing garbage after "uci"
            Command uci_command = [](ChessEngine& engine) {
                engine.doUciCommand();
            };
            command_to_return.value() = uci_command;
            break;
        }
        case Keyword::ISREADY:
        {
            // Be forgiving and ignore any trailing garbage after "isready"
            Command ready_okay_command = [](ChessEngine& engine) {
                engine.respondWhenReady();
            };
            command_to_return.value() = ready_okay_command;
            break;
        }
        case Keyword::UCINEWGAME:
        {
            // Be forgiving and ignore any trailing garbage after "ucinewgame"
            Command new_game_command = [](ChessEngine& engine) {
                engine.startNewGame();
            };
            command_to_return.value() = new_game_command;
            break;
        }
        case Keyword::SETOPTION:
        {
            std::string name, value;
            if (isValidSetoptionCommand(input, name, value)) {
                Command setoption_command = [name, value](ChessEngine& engine) {
                    engine.setConfigurationOption(name, value);
                };
                command_to_return.value() = setoption_command;
            }
            break;
        }  
        /*
        // FIXME: Add this, not quite sure what it's for
        case Keyword::REGISTER:
            if (!isValidRegisterCommand(tokens)) {
                return false;
            }
            break;
        */ 
        case Keyword::POSITION:
        {
            std::string fen;
            std::list<Move> moves;
            if (isValidPositionCommand(tokens, fen, moves)) {
                Command position_command = [fen, moves](ChessEngine& engine) {
                    engine.setUpPosition(fen, moves);
                };
                command_to_return.value() = position_command;

            }
            break;
        }
        case Keyword::GO:
        {
            std::list<Move> restrict_search;
            bool ponder, infinite;
            int movetime, wtime, btime, winc, binc, movestogo, depth, nodes, mate;
            if (isValidGoCommand(tokens, restrict_search, ponder, infinite,
                    movetime, wtime, btime, winc, binc, movestogo, depth, nodes,
                    mate)) {
                Command go_command = [=](ChessEngine& engine) {
                    engine.startCalculating(restrict_search, ponder, infinite,
                            movetime, wtime, btime, winc, binc, movestogo,
                            nodes, mate);
                };
                command_to_return.value() = go_command;
            }
            break;
        }
        case Keyword::STOP:
        {
            if (isValidNoArgCommand(tokens)) {
                Command stop_command = [](ChessEngine& engine) {
                    engine.stopCalculating();
                };
            }
            break;
        }
        case Keyword::PONDERHIT:
            if (isValidNoArgCommand(tokens)) {
                Command stop_command = [](ChessEngine& engine) {
                    engine.ponderHit();
                };
            }
            break;
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
UniversalChessInterface::isValidDebugCommand(std::list<std::string>& tokens, bool& enabled)
{
    // The only token remaining should be "on" or "off"
    if (tokens.size() != 1) {
        return false;
    }
    std::string value = tokens.front();
    toLower(value);
    if (value == "on") {
        enabled = true;
    } else if (value == "off") {
        enabled = false;
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
    // Noting comes after the stop
    if (tokens.size() != 0) {
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
 * @param[out] option_name - The parsed option name is stored here
 * @param[out] option_value - The parsed option value is stored here
 * @return True for valid command, false for invalid
 */
bool
UniversalChessInterface::isValidSetoptionCommand(const std::string& input,
        std::string& option_name, std::string& option_value)
{
    // Expect an option string like 
    // "setoption name Style value Risky\n"
    // and capture "Style" and "Risky"

    std::regex pattern(R"(setoption\s+name\s+(\S+)\s+value\s+(\S+)\n)");
    std::smatch matches;

    if (std::regex_match(input, matches, pattern)) {
        std::cout << "Match successful!" << std::endl;
        std::cout << "Captured name: " << matches[1] << std::endl;
        option_name = matches[1];
        std::cout << "Captured value: " << matches[2] << std::endl;
        option_value = matches[2];
    } else {
        std::cout << "No match!" << std::endl;
        return false;
    }

 
    return true;
}

bool
UniversalChessInterface::isValidPositionCommand(std::list<std::string>& tokens,
        std::string& output_fen, std::list<Move>& output_moves)
{
    // position [fen <fenstring> | startpos ]  moves <move1> .... <movei>
	// set up the position described in fenstring on the internal board and
	// play the moves on the internal chess board.
	// if the game was played  from the start position the string "startpos" will be sent
	// Note: no "new" command is needed. However, if this position is from a different game than
	// the last position sent to the engine, the GUI should have sent a "ucinewgame" inbetween. 

    const std::string startpos_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    // Get next token after "position"
    std::string token = popFrontLowercase(tokens);

    // Get the correct FEN string to return
    if (token == "startpos") {
        output_fen = startpos_fen;
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
        output_fen = fen;
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
            output_moves.push_back(Move(token));
        }
    }

    return true;
}

bool
UniversalChessInterface::isValidGoCommand(std::list<std::string>& tokens,
        std::list<Move> restrict_search, bool ponder, bool infinite,
        int movetime, int wtime, int btime, int winc, int binc,
        int movestogo, int depth, int nodes, int mate)
{
    // Initialize output arguments to default values, -1 for "not set"
    ponder = false; infinite = false;
    movetime = -1; wtime = -1; btime = -1; winc = -1; binc = -1;
    movestogo = -1; depth = -1; nodes = -1; mate = -1;

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
                ponder = true;
                break;
            case Keyword::INFINITE:
                infinite = true;
                break;
            case Keyword::WTIME:
                token = popFrontLowercase(tokens);
                if (!stringToInt(token, wtime)) {
                    return false;
                }
                break;
            case Keyword::BTIME:
                token = popFrontLowercase(tokens);
                if (!stringToInt(token, btime)) {
                    return false;
                }
                break;
            case Keyword::WINC:
                token = popFrontLowercase(tokens);
                if (!stringToInt(token, winc)) {
                    return false;
                }
                break;
            case Keyword::BINC:
                token = popFrontLowercase(tokens);
                if (!stringToInt(token, binc)) {
                    return false;
                }
                break;
            case Keyword::MOVESTOGO:
                token = popFrontLowercase(tokens);
                if (!stringToInt(token, movestogo)) {
                    return false;
                }
                break;
            case Keyword::DEPTH:
                token = popFrontLowercase(tokens);
                if (!stringToInt(token, depth)) {
                    return false;
                }
                break;
            case Keyword::NODES:
                token = popFrontLowercase(tokens);
                if (!stringToInt(token, nodes)) {
                    return false;
                }
                break;
            case Keyword::MATE:
                token = popFrontLowercase(tokens);
                if (!stringToInt(token, mate)) {
                    return false;
                }
                break;
            case Keyword::MOVETIME:
                token = popFrontLowercase(tokens);
                if (!stringToInt(token, movetime)) {
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
                        restrict_search.push_back(Move(token));
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