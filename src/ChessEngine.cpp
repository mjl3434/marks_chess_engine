#include "ChessEngine.h"

#include <cctype>
#include <cstdint>
#include <iostream>
#include <list>
#include <regex>
#include <sstream>
#include <unordered_set>

#include "Application.h"
#include "ChessGame.h"
#include "HelperFunctions.h"
#include "UCICommand.h"
#include "Keyword.h"

ChessEngine::ChessEngine()
{
    printf("Created new ChessEngine\n");
}

void
ChessEngine::start()
{
    _engine_thread = std::thread(&ChessEngine::spin, this);
}

void
ChessEngine::stop() {
    std::unique_ptr<UCICommand> quit_command = std::make_unique<QuitCommand>();
    command_queue.enqueue(std::move(quit_command));
    if (_engine_thread.joinable()) {
        _engine_thread.join();
    }
}

void
ChessEngine::spin()
{
    while (true) {

        // Block until a command is available
        std::unique_ptr<UCICommand> command = command_queue.dequeue();

        // FIXME: Add these!

        // NOTE: In all long-running functions we need to periodically check the non-blocking call
        //
        //     std::optional<std::unique_ptr<UCICommand>> command = command_queue.try_dequeue();
        //
        // to see if there's a new command we need to handle.
        command->doCommand(*this);

    }
}


void
ChessEngine::doDebugCommand(DebugCommand& command)
{
    _debug_enabled = command.debug_enabled;
}

void
ChessEngine::doGoCommand(GoCommand& command)
{
    // Notes:
    // - Min-Max algorithm with alpha-beta pruning
    // - Time controls not implemented yet

    // Do we really need to parse out the arguments here? Seems like extra work
    //int max_depth = command.depth > 0 ? command.depth : DEFAULT_MAX_SEARCH_DEPTH;
    //int time_limit = command.movetime > 0 ? command.movetime : 10000; // Default to 10 seconds if not specified

    const GameState& game_state = _game->getLatestGameState();

    SearchResult result = findBestMove(game_state, command);
}

void
ChessEngine::doIsReadyCommand(IsReadyCommand& command)
{
    std::cout << "readyok\n";
}

void
ChessEngine::doPonderHitCommand(PonderHitCommand& command)
{
    // The opponent made the move we were thinking about
    // now what?

    // std::optional<std::unique_ptr<UCICommand>> command = command_queue.try_dequeue();
}

void
ChessEngine::doPositionCommand(PositionCommand& command)
{
    GameState initial_game_state;

    setUpBoardFromFen(command.fen, initial_game_state);

    while (command.moves.size() > 0) {
        Move current = command.moves.front();
        command.moves.pop_front();
        _game->doMove(current);
    }
}

void
ChessEngine::doQuitCommand(QuitCommand& command)
{
    // Clean up the engine
}

void
ChessEngine::doSetOptionCommand(SetOptionCommand& command)
{
    // Set the configuration option in the chess engine
    // For now we just print it out
    std::cout << "Set option: " << command.name << " to " << command.value << "\n";
    // In a real implementation we would store this in a map or similar structure
    // and actually use it to configure the engine's behavior.
}

void
ChessEngine::doStopCommand(StopCommand& command)
{
    // By the time we get here either the engine was not busy, or it stopped to
    // try_dequeue and found a stop command. Clean up whatever we were doing.
    // (But maybe that got done already after we found the stop command)
}


void
ChessEngine::doUciCommand(UciCommand& command)
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
ChessEngine::doUciNewGameCommand(UciNewGameCommand& command)
{
    printf("Started new game!\n");

    // Clear any internal state left over from a previous game
    _game.reset();

    // Don't set up the board until we get a "position" command
}

SearchResult
ChessEngine::findBestMove(const GameState& starting_state, const GoCommand& go_command) const
{
    SearchResult result;
    result.score = INT32_MIN;
    int max_depth = go_command.max_depth > 0 ? go_command.max_depth : DEFAULT_MAX_SEARCH_DEPTH;

    // Generate all legal moves
    std::list<Move> legal_moves = _game->_rules.generateLegalMovesForCurrentPlayer(starting_state);

    for (const Move& move : legal_moves) {

        // Temporarily make the move on a copy
        GameState search_state = starting_state;
        auto repetition_table = _game->getGamePositions();

        _game->tryMoveOnStateCopy(move, search_state);

        // Add the new position to the repetition table
        _game->addToRepetitionTable(search_state._game_state_hash, repetition_table);

        // Search deeper with alpha-beta
        int32_t score = minimax(search_state, repetition_table, max_depth - 1, INT32_MIN, INT32_MAX, false);

        // Remove the position from the repetition table (backtrack)
        _game->removeFromRepetitionTable(search_state._game_state_hash, repetition_table);

        if (score > result.score) {
            result.score = score;
            result.best_move = move;
        }
    }

    return result;
}

int32_t
ChessEngine::minimax(GameState game_state, position_hash_t& repetition_table,
    int depth, int alpha, int beta, bool maximizing) const
{
    Rules& rules = _game->_rules;

    GameResult result = rules.checkForGameEndings(game_state, repetition_table);

    if (result == GameResult::CHECKMATE) {
        if (maximizing) {
            return INT32_MAX;   // Checkmate is the best score possible
        } else {
            return INT32_MIN;   // Return the most extreme results
        }
    } else if (result == GameResult::STALEMATE ||
               result == GameResult::THREEFOLD ||
               result == GameResult::FIFTY_MOVE ||
               result == GameResult::INSUFFICIENT_MATERIAL) {
        return 0;   // Draw
    } else if (result == GameResult::NONE || depth == 0) {
        // Base case: reached search depth or terminal position
        return evaluatePosition(game_state, repetition_table);
    }

    std::list<Move> moves = _game->_rules.generateLegalMovesForCurrentPlayer(game_state);

    if (maximizing) {
        int32_t max_score = INT32_MIN;
        for (const Move& move : moves) {

            // Temporarily make the move on a copy 
            GameState new_state = game_state;
            _game->tryMoveOnStateCopy(move, new_state);

            // Add the new position to the repetition table
            _game->addToRepetitionTable(new_state._game_state_hash, repetition_table);

            // Keep recursing
            int score = minimax(new_state, repetition_table, depth - 1, alpha, beta, false);

            // Remove the position from the repetition table (backtrack)
            _game->removeFromRepetitionTable(new_state._game_state_hash, repetition_table);

            max_score = std::max(max_score, score);
            alpha = std::max(alpha, score);
            
            if (beta <= alpha) {
                break;  // Beta cutoff - prune remaining moves
            }
        }
        return max_score;
    } else {
        int32_t min_score = INT32_MAX;
        for (const Move& move : moves) {

            // Temporarily make the move on a copy 
            GameState new_state = game_state;  // Copy
            _game->tryMoveOnStateCopy(move, new_state);

            // Add the new position to the repetition table
            _game->addToRepetitionTable(new_state._game_state_hash, repetition_table);

            // Keep recursing
            int score = minimax(new_state, repetition_table, depth - 1, alpha, beta, true);

            // Remove the position from the repetition table (backtrack)
            _game->removeFromRepetitionTable(new_state._game_state_hash, repetition_table);

            min_score = std::min(min_score, score);
            beta = std::min(beta, score);
            
            if (beta <= alpha) {
                break;  // Alpha cutoff - prune remaining moves
            }
        }
        return min_score;
    }
}

int32_t
ChessEngine::evaluatePosition(const GameState& game_state, position_hash_t& repetition_table) const
{
    // FIXME: Implement this

    // For now, return a random score

    // First cut:

    // Add up the value of all the remaining material on the board for each player
    // and calculate the difference.

    // Subtract points for squares under attack
    
    // Subtract points for pinned pieces

    // Subtract points that are not guarded by other pieces


    return 0;
}

// Note: This must be called with a valid FEN string since no error checking is done
void
ChessEngine::setUpBoardFromFen(const std::string& fen, GameState& game_state) const
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
                game_state.board[row-1][column-1].piece = Piece::WHITE_PAWN;
                column++;
                break;
            case 'R':
                game_state.board[row-1][column-1].piece = Piece::WHITE_ROOK;
                column++;
                break;
            case 'B':
                game_state.board[row-1][column-1].piece = Piece::WHITE_BISHOP;
                column++;
                break;
            case 'N':
                game_state.board[row-1][column-1].piece = Piece::WHITE_KNIGHT;
                column++;
                break;
            case 'K':
                game_state.board[row-1][column-1].piece = Piece::WHITE_KING;
                column++;
                break;
            case 'Q':
                game_state.board[row-1][column-1].piece = Piece::WHITE_QUEEN;
                column++;
                break;
            case 'p': 
                game_state.board[row-1][column-1].piece = Piece::BLACK_PAWN;
                column++;
                break;
            case 'r':
                game_state.board[row-1][column-1].piece = Piece::BLACK_ROOK;
                column++;
                break;
            case 'b':
                game_state.board[row-1][column-1].piece = Piece::BLACK_BISHOP;
                column++;
                break;
            case 'n':
                game_state.board[row-1][column-1].piece = Piece::BLACK_KNIGHT;
                column++;
                break;
            case 'k':
                game_state.board[row-1][column-1].piece = Piece::BLACK_KING;
                column++;
                break;
            case 'q':
                game_state.board[row-1][column-1].piece = Piece::BLACK_QUEEN;
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
                    game_state.board[row-1][column-1].piece = Piece::EMPTY;
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
        game_state._current_player = Player::WHITE;
    } else if (whos_turn == "b") {
        game_state._current_player = Player::BLACK;
    }

    // Third field, castling availibility
    std::string castling = tokens.front();
    tokens.pop_front();

    game_state._white_kingside_castle_allowed = false;
    game_state._white_queenside_castle_allowed = false;
    game_state._black_kingside_castle_allowed = false;
    game_state._black_queenside_castle_allowed = false;

    for (const char c : castling) {
        switch (c)
        {
            case 'K':
                game_state._white_kingside_castle_allowed = true;
                break;
            case 'Q':
                game_state._white_queenside_castle_allowed = true;
                break;
            case 'k':
                game_state._black_kingside_castle_allowed = true;
                break;
            case 'q':
                game_state._black_queenside_castle_allowed = true;
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
        game_state._two_square_pawn_push_just_occured = true;
        toLower(en_passant_target_square);
        // Use subtraction to convert from a value in the ASCII range of
        // 97-104 for 'a'-'h' to, chess rank values of 1-8.
        game_state._en_passant_target_square_rank = en_passant_target_square[0] - 'a' + 1;
        game_state._en_passant_target_square_file = en_passant_target_square[1];
    } else {
        game_state._two_square_pawn_push_just_occured = false;
    }

    // Fifth field, halfmove clock
    std::string hmc = tokens.front();
    tokens.pop_front();

    int temp = 0;
    if (stringToInt(hmc,temp)) {
        game_state._halfmove_clock = temp;
    }

    // Sixth field, total moves
    std::string total_moves = tokens.front();
    tokens.pop_front();

    temp = 0;
    if (stringToInt(total_moves, temp)) {
        game_state._num_moves = temp;
    }
}

void
ChessEngine::printSupportedOptions(void) const
{
    // Once we support options we should print them here
    // option name Foo FooValue1 FooValue2 ...
    // option name Bar BarValue1 Baralue2 ...
}
