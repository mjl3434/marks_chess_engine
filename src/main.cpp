#include "Application.h"


int main(int argc, char* argv[])
{
    Application marks_chess_engine;
    marks_chess_engine.run();
    return 0;
}

/*

You're absolutely right - the Move class and game state management is the heart of the complexity, and it can spiral out of control quickly if not designed carefully. Here's my advice on separation of concerns and clean design:

Core Design Principles
1. Separate "What Happened" from "How to Execute"
Move should be a data class - just describe what happened
Don't put game logic inside Move - it should be dumb data
Let other classes handle the execution and validation
2. Game State vs Move Data
Move should contain:

Source/destination squares
Piece type that moved
Captured piece (if any)
Special move flags (castle, en passant, promotion)
Maybe algebraic notation for UCI
Move should NOT contain:

Side effects or state changes
Validation logic
Undo information
3. Suggested Class Architecture
GameState class (separate from ChessGame):


- Board position- Current player- Castling rights- En passant target- Halfmove clock- Fullmove counter
Rules class (your instinct was right):


- isLegalMove(GameState, Move)- generateLegalMoves(GameState)- isInCheck(GameState, Player)- isCheckmate/Stalemate/Draw
ChessGame class:


- Current GameState- Move history- makeMove(Move) -> returns new GameState- undoMove() -> restores previous GameState
Move class (keep it simple):


- from/to squares- piece type- captured piece- special flags (castle type, promotion piece, en passant)
State Management Strategy
Option 1: Immutable GameState (Recommended)
Each makeMove() creates a new GameState
Keep a stack of previous states
undoMove() just pops the stack
No complex undo logic needed
Option 2: Delta/Undo Information
Store "undo information" with each move
More memory efficient but more complex
Handling Special Cases
Castling:
Store as a special move type in Move
Rules class validates castling legality
GameState tracks castling rights simply
En Passant:
Flag in Move when it's an en passant capture
GameState tracks the en passant target square
Reset en passant target after each move (unless pawn moved 2)
50-Move Rule:
GameState tracks halfmove clock
Rules class checks for draw conditions
Reset clock on pawn moves or captures
Benefits of This Approach
Move stays simple - just data, no behavior
Rules is stateless - pure functions, easy to test
GameState is focused - just holds position info
Easy to debug - each class has one responsibility
Easy to extend - add new rules without touching existing code
Search-friendly - can easily copy/restore game states
Key Insight: Don't Try to Be Too Clever
The complexity you encountered comes from trying to make Move do everything. Instead:

Keep data structures simple
Use composition over inheritance
Favor immutability where possible
Let each class do one thing well
For Minimax Search
This design is perfect for search because:

Copy GameState before making a move
Make move, evaluate position
Restore previous GameState (or let it go out of scope)
No complex undo logic needed in search
Would you like me to elaborate on any of these design patterns, or discuss how this would work with your specific UCI interface?

*/