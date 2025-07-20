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
ChessEngine::setDebug(bool enabled)
{
    debug_enabled = enabled;
}

void
ChessEngine::doUciCommand(void)
{
    std::cout << "id Name" << engine_name << "\n";
    std::cout << "id Author" << author << "\n";

    /*
  1 <Stockfish(0): info string Using 8 threads
  2 <Stockfish(0): No such option: Contempt
  3 <Stockfish(0): info string Found 145 WDL and 0 DTZ tablebase files (up to 5-man).
  4 <Stockfish(0): id name Stockfish dev-20241122-b7f17346
  5 <Stockfish(0): id author the Stockfish developers (see AUTHORS file)
  6 <Stockfish(0): option name Debug Log File type string default <empty>
  7 <Stockfish(0): option name NumaPolicy type string default auto
  8 <Stockfish(0): option name Threads type spin default 1 min 1 max 1024
  9 <Stockfish(0): option name Hash type spin default 16 min 1 max 33554432
 10 <Stockfish(0): option name Clear Hash type button
 11 <Stockfish(0): option name Ponder type check default false
 12 <Stockfish(0): option name MultiPV type spin default 1 min 1 max 256
 13 <Stockfish(0): option name Skill Level type spin default 20 min 0 max 20
 14 <Stockfish(0): option name Move Overhead type spin default 10 min 0 max 5000
 15 <Stockfish(0): option name nodestime type spin default 0 min 0 max 10000
 16 <Stockfish(0): option name UCI_Chess960 type check default false
 17 <Stockfish(0): option name UCI_LimitStrength type check default false
 18 <Stockfish(0): option name UCI_Elo type spin default 1320 min 1320 max 3190
 19 <Stockfish(0): option name UCI_ShowWDL type check default false
 20 <Stockfish(0): option name SyzygyPath type string default <empty>
 21 <Stockfish(0): option name SyzygyProbeDepth type spin default 1 min 1 max 100
 22 <Stockfish(0): option name Syzygy50MoveRule type check default true
 23 <Stockfish(0): option name SyzygyProbeLimit type spin default 7 min 0 max 7
 24 <Stockfish(0): option name EvalFile type string default nn-1c0000000000.nnue
 25 <Stockfish(0): option name EvalFileSmall type string default nn-37f18f62d772.nnue
    */
    printSupportedOptions();

    std::cout << "uciok\n";
}

void
ChessEngine::printSupportedOptions(void)
{
    // Once we support options we should print them here
    // option name Foo FooValue1 FooValue2 ...
    // option name Bar BarValue1 Baralue2 ...
}

void
ChessEngine::respondWhenReady(void)
{
    std::cout << "readyok\n";
}

void
ChessEngine::setConfigurationOption(const std::string& name, const std::string& value)
{
    // Once we support config options we would set them here
}

void
ChessEngine::startNewGame(void)
{
    printf("Started new game!\n");

    // Clear any internal state left over from a previous game
    game.reset();

    // Don't set up the board until we get a "position" command

    // rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
}

void
ChessEngine::setUpPosition(std::string fen, std::list<Move> moves)
{
    setUpBoardFromFen(fen);

    while (moves.size() > 0) {
        Move current = moves.front();
        moves.pop_front();
        playMove(current);
    }
}

void
ChessEngine::startCalculating(
                std::list<Move> searchMoves,
                bool ponder,
                bool infinite,
                int32_t movetime,
                int32_t wtime,
                int32_t btime,
                int32_t winc,
                int32_t binc,
                int16_t movestogo,
                int16_t nodes,
                int16_t mate)
{

}

void
ChessEngine::playMove(const Move& move)
{
    // The protocol does not specify what to do if we get a move which is
    // illegal or doesn't make sense. If this happens we will print an
    // error and then return without doing anything (no-op)
    Piece piece_moved = game->getPieceAtSourceSquare(move);
    if (piece_moved == Piece::EMPTY) {
        fprintf(stderr, "Error: No piece to move at %s\n", move.algebraic.c_str());
        return;
    }

    // Check if move is legal
    if (!game->isLegalMove(move)) {
        fprintf(stderr, "Error: Illegal move in this context: %s", move.algebraic.c_str());
    }
}

void
ChessEngine::stopCalculating(void)
{
}

void
ChessEngine::ponderHit(void)
{
}

void
ChessEngine::quit(void)
{
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
                game->board[row-1][column-1].piece = Piece::WHITE_PAWN;
                column++;
                break;
            case 'R':
                game->board[row-1][column-1].piece = Piece::WHITE_ROOK;
                column++;
                break;
            case 'B':
                game->board[row-1][column-1].piece = Piece::WHITE_BISHOP;
                column++;
                break;
            case 'N':
                game->board[row-1][column-1].piece = Piece::WHITE_KNIGHT;
                column++;
                break;
            case 'K':
                game->board[row-1][column-1].piece = Piece::WHITE_KING;
                column++;
                break;
            case 'Q':
                game->board[row-1][column-1].piece = Piece::WHITE_QUEEN;
                column++;
                break;
            case 'p': 
                game->board[row-1][column-1].piece = Piece::BLACK_PAWN;
                column++;
                break;
            case 'r':
                game->board[row-1][column-1].piece = Piece::BLACK_ROOK;
                column++;
                break;
            case 'b':
                game->board[row-1][column-1].piece = Piece::BLACK_BISHOP;
                column++;
                break;
            case 'n':
                game->board[row-1][column-1].piece = Piece::BLACK_KNIGHT;
                column++;
                break;
            case 'k':
                game->board[row-1][column-1].piece = Piece::BLACK_KING;
                column++;
                break;
            case 'q':
                game->board[row-1][column-1].piece = Piece::BLACK_QUEEN;
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
                    game->board[row-1][column-1].piece = Piece::EMPTY;
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
        game->current_player = Player::WHITE;
    } else if (whos_turn == "b") {
        game->current_player = Player::BLACK;
    }

    // Third field, castling availibility
    std::string castling = tokens.front();
    tokens.pop_front();

    game->white_kingside_castle_allowed = false;
    game->white_queenside_castle_allowed = false;
    game->black_kingside_castle_allowed = false;
    game->black_queenside_castle_allowed = false;

    for (const char c : castling) {
        switch (c)
        {
            case 'K':
                game->white_kingside_castle_allowed = true;
                break;
            case 'Q':
                game->white_queenside_castle_allowed = true;
                break;
            case 'k':
                game->black_kingside_castle_allowed = true;
                break;
            case 'q':
                game->black_queenside_castle_allowed = true;
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
        game->two_square_pawn_push_just_occured = true;
        toLower(en_passant_target_square);
        // Use subtraction to convert from a value in the ASCII range of
        // 97-104 for 'a'-'h' to, chess rank values of 1-8.
        game->en_passant_target_square_rank = en_passant_target_square[0] - 'a' + 1;
        game->en_passant_target_square_file = en_passant_target_square[1];
    } else {
        game->two_square_pawn_push_just_occured = false;
    }

    // Fifth field, halfmove clock
    std::string hmc = tokens.front();
    tokens.pop_front();

    int temp = 0;
    if (stringToInt(hmc,temp)) {
        game->halfmove_clock = temp;
    }

    // Sixth field, total moves
    std::string total_moves = tokens.front();
    tokens.pop_front();

    temp = 0;
    if (stringToInt(total_moves, temp)) {
        game->num_moves = temp;
    }
}
