#pragma once

// Piece type IDs (0..5)
constexpr int PVT_PAWN   = 0;
constexpr int PVT_KNIGHT = 1;
constexpr int PVT_BISHOP = 2;
constexpr int PVT_ROOK   = 3;
constexpr int PVT_QUEEN  = 4;
constexpr int PVT_KING   = 5;

// Colors
constexpr int PVT_WHITE = 0;
constexpr int PVT_BLACK = 1;

// Encoded piece constants (0..11)
constexpr int PVT_WHITE_PAWN   = (2 * PVT_PAWN   + PVT_WHITE);
constexpr int PVT_BLACK_PAWN   = (2 * PVT_PAWN   + PVT_BLACK);
constexpr int PVT_WHITE_KNIGHT = (2 * PVT_KNIGHT + PVT_WHITE);
constexpr int PVT_BLACK_KNIGHT = (2 * PVT_KNIGHT + PVT_BLACK);
constexpr int PVT_WHITE_BISHOP = (2 * PVT_BISHOP + PVT_WHITE);
constexpr int PVT_BLACK_BISHOP = (2 * PVT_BISHOP + PVT_BLACK);
constexpr int PVT_WHITE_ROOK   = (2 * PVT_ROOK   + PVT_WHITE);
constexpr int PVT_BLACK_ROOK   = (2 * PVT_ROOK   + PVT_BLACK);
constexpr int PVT_WHITE_QUEEN  = (2 * PVT_QUEEN  + PVT_WHITE);
constexpr int PVT_BLACK_QUEEN  = (2 * PVT_QUEEN  + PVT_BLACK);
constexpr int PVT_WHITE_KING   = (2 * PVT_KING   + PVT_WHITE);
constexpr int PVT_BLACK_KING   = (2 * PVT_KING   + PVT_BLACK);
constexpr int PVT_EMPTY        = (PVT_BLACK_KING + 1);

// small helpers
inline constexpr int PVT_PCOLOR(int p) { return (p & 1); }
inline constexpr int PVT_FLIP(int sq) { return (sq ^ 56); }
inline constexpr int PVT_OTHER(int side) { return (side ^ 1); }

#ifdef __cplusplus
extern "C" {
#endif

extern int side2move;    // set this to PVT_WHITE or PVT_BLACK before calling eval()
extern int board[64];    // fill with PVT_* piece constants defined above

// initialization and eval functions (defined in Pvt.cpp)
void init_tables();
int eval();

#ifdef __cplusplus
}
#endif