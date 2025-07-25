#include "Application.h"


int main(int argc, char* argv[])
{
    Application marks_chess_engine;
    marks_chess_engine.run();
    return 0;
}

/*

Core Design Principles
1. Separate "What Happened" from "How to Execute"

- Current GameState- Move history- makeMove(Move) -> returns new GameState- undoMove() -> restores previous GameState
Move class (keep it simple):

- from/to squares- piece type- captured piece- special flags (castle type, promotion piece, en passant)
State Management Strategy
Option 1: Immutable GameState (Recommended)
Each makeMove() creates a new GameState
Keep a stack of previous states
undoMove() just pops the stack
No complex undo logic needed

Let each class do one thing well
For Minimax Search
This design is perfect for search because:

Copy GameState before making a move
Make move, evaluate position
Restore previous GameState (or let it go out of scope)
No complex undo logic needed in search
Would you like me to elaborate on any of these design patterns, or discuss how this would work with your specific UCI interface?

*/