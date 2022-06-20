//
// Created by Matthias on 06-Nov-18.
//

#ifndef FIAR_3D_CPP_GAME_H
#define FIAR_3D_CPP_GAME_H

//Determine here whether debug output should be on
// #define FULL_DEBUG_OUTPUT
// #define PRINT_BB

//#define WITHOUT_TRANSPOSITION_TABLE

// Choose here whether to terminate the game at a specified depth
// #define TERMINATE_AT_CERTAIN_DEPTH

//#define LIMIT_PLAYER_MOVES

//#define NO_PRUNING

#include "BoardState.h"

#include <ctime>

#ifdef FULL_DEBUG_OUTPUT
#include <iostream>
#endif

enum TableGameValue : uint8_t {
    INVALID,
    LOSS,
    DRAW_GUARANTEED,
    DRAW_OR_BETTER,
    WIN
};

#pragma pack(push)
#pragma pack(4)
struct TableEntry {
    CompactBoardState board;
    TableGameValue value;
    TableEntry();
};
#pragma pack(pop)

class Game {
public:
    //static const int TABLESIZE = 15485863;
    static const int TABLESIZE = 39916801;

#ifdef TERMINATE_AT_CERTAIN_DEPTH
    static uint8_t terminationDepth;
    static int terminationValue;
    static uint64_t terminationCount;
#endif

#ifdef LIMIT_PLAYER_MOVES
    static uint8_t firstPlayerMoveLimit;
    static uint8_t secondPlayerMoveLimit;
#endif

    static int iterationCount;

    static int computeEmptyGameValue();
    static int computeThreeLayerTestGameValue();
    static int computeTwoLayerTestGameValue();
    static int computeOneLayerTestGameValue();
    static int computeTrivialSecondPlayerVictory();
    static int computeCustomTestGameValue();
    static int computeGameValue(int initialBoard[64]);

    static int preset0[64];
    static int preset1[64];
    static int preset2[64];
    static int preset3[64];
    static int preset4[64];

    static int presetPlayed0_18[64];
    static int presetPlayed0_20[64];
    static int presetPlayed1_18[64];
    static int presetPlayed1_20[64];
    static int presetPlayed2_18[64];
    static int presetPlayed2_20[64];
    static int presetPlayed3_18[64];
    static int presetPlayed3_20[64];

    static int presetShowcase[64];

    static void initialize();
    static int tableFillCount();

    static std::string runTestSuite();
private:
    static TableEntry *transpositionTable;
    static int negaMax(BoardState& position, bool lowerBound, bool upperBound);
    static inline void saveCompactBoardStateInTable(CompactBoardState board, TableGameValue value);
};


#endif //FIAR_3D_CPP_GAME_H
