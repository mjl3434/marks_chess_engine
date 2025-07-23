#pragma once

// UCI keywords
enum class Keyword {
    UCI,
    DEBUG,
        ON,
        OFF,
    ISREADY,
    SETOPTION,
        NAME,
        VALUE,
    REGISTER,
        LATER,
        // NAME (duplicated from above, different meaning here)
        CODE,
    UCINEWGAME,
    POSITION,
        FEN,
        STARTPOS,
            MOVES,
    GO,
        SEARCHMOVES,
        PONDER,
        WTIME,
        BTIME,
        WINC,
        BINC,
        MOVESTOGO,
        DEPTH,
        NODES,
        MATE,
        MOVETIME,
        INFINITE,
    STOP,
    PONDERHIT,
    QUIT
};
