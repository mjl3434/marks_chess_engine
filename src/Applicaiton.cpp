#include "Application.h"

#include <iostream>
#include <string>
#include <functional>

void Application::run()
{
    std::string input;
    while (true) {
        std::getline(std::cin, input);
        if (uci.quitReceived(input)) {
            break;
        }
        std::optional<Command> result = uci.getCommand(input);
        if (result) {
            result.value()(chess_engine);
        }
    }
}



/*

Define Chess Engine Methods (will need to call either way)

void (std::list<std::pair<std::string, std::string>>);
void startNewGame();
void setUpPosition(std::string fen, std::list<Move>);
void startCalculating(
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
        int16_t mate
    );
void stopCalculating();
void ponderHit();
void quit();


actions:
    1. Tell chess engine debug on | off
    2. Send respnoses to UciCommand
    3. Ask chess engine to respond when ready
        - blocking call
    4. Set configuration options in chess engine
        - Need access to engine settings
        - Need to give name value pair of config options
    5. Start new game
        - Need access to game state
    6. Set up board position & play game moves
        - Need access to game board
        - Need to give list
    7. Calculate best move
        - Need access to game board
    8. quit

    -> uci
    <- id Name Mark's Chess Engine
    <- id Author Mark Larwill
    <- option name Foo FooValue1 FooValue2 ...
    <- option name Bar BarValue1 Baralue2 ...
    [ no action ]
    <- uciok

    -> debug on
    [ enable debug logs ]
    <- [ no response ]

    -> debug off
    [ disable debug logs ]
    <- [ no response ]

    -> isready
    [ finish any pending actions | if searching respond immediatly ]
    <- readyok

    -> setoption name [value] value [value]
    [ set configuration options ]
    <- [ no response ]

    -> ucinewgame
    [ reset current game ]
    <- [ no response ]

    -> position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR moves e2e4 e7e6
    [ set up a new game from fen or startpos ]
    [ execute moves if any ]
    <- [ no response ]

    -> go wtime 300000 btime 300000 movestogo 40
    [ start calculating on current position ]
    [ spit out info logs with calculation updates ]
    <- info depth 1 seldepth 4 multipv 1 score cp -29 nodes 114 nps 114000 hashfull 0 tbhits 0 time 1 pv e7e5
    <- info depth 2 seldepth 2 multipv 1 score cp -28 nodes 285 nps 142500 hashfull 0 tbhits 0 time 2 pv c7c5
    ...
    <- bestmove d7d5 ponder e4d5

    General gameplay:

 51 >Stockfish(1): position startpos moves e2e4
 52 >Stockfish(1): isready
 53 <Stockfish(1): readyok
 54 >Stockfish(1): go wtime 300000 btime 300000 movestogo 40
 55 <Stockfish(1): info string Available processors: 0-7
 56 <Stockfish(1): info string Using 1 thread
 57 <Stockfish(1): info string info string NNUE evaluation using nn-1c0000000000.nnue (133MiB, (22528, 3072, 15, 32, 1))
 58 <Stockfish(1): info string info string NNUE evaluation using nn-37f18f62d772.nnue (6MiB, (22528, 128, 15, 32, 1))
 59 <Stockfish(1): info depth 1 seldepth 4 multipv 1 score cp -29 nodes 114 nps 114000 hashfull 0 tbhits 0 time 1 pv e7e5
 60 <Stockfish(1): info depth 2 seldepth 2 multipv 1 score cp -28 nodes 285 nps 142500 hashfull 0 tbhits 0 time 2 pv c7c5
 61 <Stockfish(1): info depth 3 seldepth 2 multipv 1 score cp -16 nodes 543 nps 181000 hashfull 0 tbhits 0 time 3 pv e7e5
 62 <Stockfish(1): info depth 4 seldepth 2 multipv 1 score cp -16 nodes 745 nps 186250 hashfull 0 tbhits 0 time 4 pv e7e5
 63 <Stockfish(1): info depth 5 seldepth 5 multipv 1 score cp -13 nodes 1025 nps 205000 hashfull 0 tbhits 0 time 5 pv c7c5
 64 <Stockfish(1): info depth 6 seldepth 8 multipv 1 score cp -32 nodes 3406 nps 170300 hashfull 0 tbhits 0 time 20 pv e7e5 g1f3 b8c6 d2d4 e5d4 f3d4
 65 <Stockfish(1): info depth 7 seldepth 8 multipv 1 score cp -32 nodes 6131 nps 185787 hashfull 0 tbhits 0 time 33 pv e7e5 g1f3 b8c6 d2d4 e5d4 f3d4
 ...
 85 <Stockfish(1): bestmove d7d5 ponder e4d5

 86 >Stockfish(1): position startpos moves e2e4 d7d5 e4d5
 87 >Stockfish(1): isready
 88 <Stockfish(1): readyok
 89 >Stockfish(1): go wtime 297399 btime 282188 movestogo 39
 90 <Stockfish(1): info string Available processors: 0-7
 91 <Stockfish(1): info string Using 1 thread
 92 <Stockfish(1): info string info string NNUE evaluation using nn-1c0000000000.nnue (133MiB, (22528, 3072, 15, 32, 1))
 93 <Stockfish(1): info string info string NNUE evaluation using nn-37f18f62d772.nnue (6MiB, (22528, 128, 15, 32, 1))
 94 <Stockfish(1): info depth 1 seldepth 5 multipv 1 score cp -58 nodes 372 nps 372000 hashfull 0 tbhits 0 time 1 pv g8f6
 ...
 118 <Stockfish(1): bestmove g8f6 ponder d2d4

*/
