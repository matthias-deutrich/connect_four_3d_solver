//
// Created by Matthias on 06-Nov-18.
//



#include "Game.h"
// #include <iostream>



int Game::iterationCount = 0;

#ifdef TERMINATE_AT_CERTAIN_DEPTH
uint8_t Game::terminationDepth = 20;
int Game::terminationValue = 0;
uint64_t Game::terminationCount = 0;
#endif

#ifdef LIMIT_PLAYER_MOVES
uint8_t Game::firstPlayerMoveLimit = 4;
uint8_t Game::secondPlayerMoveLimit = 5;
#endif

// Both 0 and 1 were created with only simple move ordering

int Game::preset0[64] = {1, 0, 1, 2, 1, 2, 1, 1,
                         2, 2, 1, 2, 1, 0, 2, 2,
                         1, 0, 0, 0, 0, 1, 0, 0,
                         0, 2, 0, 0, 0, 0, 0, 0,
                         1, 0, 0, 0, 0, 0, 0, 0,
                         0, 2, 0, 0, 0, 0, 0, 0,
                         2, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0};

//Stats: Computation complete. Result is -1. Computed in 541.134 seconds using 1695190417 iterations.
// Time per iteration: 3.19217e-07      K pos/s: 3132.66
// 18 pieces, ID 0
int Game::preset1[64] = {1, 0, 1, 2, 1, 2, 1, 1,
                         2, 2, 1, 2, 1, 0, 2, 2,
                         1, 0, 0, 0, 0, 0, 0, 0,
                         0, 2, 0, 0, 0, 0, 0, 0,
                         1, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         2, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0};

//Stats: Computation complete. Result is -1. Computed in 623.104 seconds using 1695190417 iterations.
int Game::preset2[64] = {1, 0, 1, 2, 1, 2, 1, 1,
                         2, 2, 1, 2, 1, 0, 2, 2,
                         1, 0, 0, 0, 0, 0, 0, 0,
                         0, 2, 0, 0, 0, 0, 0, 0,
                         1, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         2, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0};


// complete move ordering, 10000, 100000, 100, 1000 -100, -1000, 1, 10
//Stats with full ordering: Computation complete. Result is -1. Computed in 386.604 seconds using 895049146 iterations.
//Stats with limited ordering: Computation complete. Result is -1. Computed in 278.761 seconds using 863582120 iterations. Time per iteration: 3.22796e-07      K pos/s: 3097.93
//20 pieces, ID 1
int Game::preset3[64] = {1, 1, 0, 2, 1, 1, 2, 1,
                         2, 1, 2, 2, 1, 2, 2, 2,
                         1, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 2, 0, 0, 0, 0, 0,
                         1, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 1, 0, 0, 0, 0, 0,
                         2, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0};

int Game::preset4[64] = {
    1, 1, 0, 0,
    0, 2, 0, 0,
    0, 0, 2, 0,
    0, 2, 0, 2,
    
    1, 1, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    
    1, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    
    2, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
};

//Stats: Computation complete. Result is -1. Computed in 236.504 seconds using 747492067 iterations. Time per iteration: 3.16397e-07      K pos/s: 3160.59
// ID 3
int Game::presetPlayed0_18[64] = {2, 0, 0, 1, 1, 1, 2, 2,
                                  2, 1, 1, 1, 2, 2, 0, 1,
                                  0, 0, 0, 0, 0, 2, 2, 0,
                                  0, 0, 2, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 1, 0, 0,
                                  0, 0, 1, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0};

//Stats: Computation complete. Result is -1. Computed in 2.828 seconds using 9381899 iterations. 8698257.
int Game::presetPlayed0_20[64] = {2, 0, 0, 1, 1, 1, 2, 2,
                                  2, 1, 1, 1, 2, 2, 0, 1,
                                  0, 0, 0, 0, 0, 2, 2, 1,
                                  0, 0, 2, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 1, 2, 0,
                                  0, 0, 1, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0};

//Stats: Computation complete. Result is 1. Computed in 0 seconds using 10 iterations. Time per iteration: 0      K pos/s: inf
int Game::presetPlayed1_18[64] = {2, 2, 1, 2, 0, 2, 2, 0,
                                  0, 1, 0, 0, 1, 2, 1, 1,
                                  0, 0, 0, 2, 0, 2, 1, 0,
                                  0, 1, 0, 0, 1, 1, 0, 0,
                                  0, 0, 0, 0, 0, 0, 2, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0};

int Game::presetPlayed1_20[64] = {2, 2, 1, 2, 0, 2, 2, 0,
                                  0, 1, 0, 0, 1, 2, 1, 1,
                                  0, 0, 0, 2, 0, 2, 1, 0,
                                  0, 1, 0, 0, 1, 1, 1, 2,
                                  0, 0, 0, 0, 0, 0, 2, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0};

//Stats: Computation complete. Result is -1. Computed in 69.578 seconds using 217215817 iterations. Time per iteration: 3.20317e-07      K pos/s: 3121.9
int Game::presetPlayed2_18[64] = {
                                  2, 0, 0, 0, 
                                  1, 1, 1, 2,
                                  0, 2, 1, 0, 
                                  2, 1, 2, 1,

                                  0, 0, 0, 0, 
                                  0, 0, 2, 0,
                                  0, 2, 0, 0, 
                                  1, 2, 1, 1,

                                  0, 0, 0, 0, 
                                  0, 0, 2, 0,
                                  0, 0, 0, 0, 
                                  0, 0, 0, 0,

                                  0, 0, 0, 0, 
                                  0, 0, 0, 0,
                                  0, 0, 0, 0, 
                                  0, 0, 0, 0};

int Game::presetPlayed2_20[64] = {2, 0, 0, 2, 1, 1, 1, 2,
                                  0, 2, 1, 0, 2, 1, 2, 1,
                                  0, 0, 0, 0, 0, 0, 2, 0,
                                  0, 2, 0, 0, 1, 2, 1, 1,
                                  0, 0, 0, 0, 0, 0, 2, 0,
                                  0, 0, 0, 0, 0, 1, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0};

int Game::presetPlayed3_18[64] = {2, 2, 0, 1, 0, 1, 1, 0,
                                  0, 2, 0, 0, 1, 1, 2, 2,
                                  0, 1, 0, 0, 0, 2, 0, 0,
                                  0, 1, 0, 0, 0, 0, 0, 2,
                                  0, 0, 0, 0, 0, 1, 0, 0,
                                  0, 2, 0, 0, 0, 0, 0, 1,
                                  0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 2};

//Stats: Computation complete. Result is -1. Computed in 578.58 seconds using 1842889844 iterations. Time per iteration: 3.13953e-07      K pos/s: 3185.19
int Game::presetPlayed3_20[64] = {2, 2, 0, 1, 0, 1, 1, 0,
                                  0, 2, 0, 0, 1, 1, 2, 2,
                                  2, 1, 0, 0, 0, 2, 0, 0,
                                  0, 1, 0, 0, 0, 0, 1, 2,
                                  0, 0, 0, 0, 0, 1, 0, 0,
                                  0, 2, 0, 0, 0, 0, 0, 1,
                                  0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 2};

//Stats: Computation complete. Result is 1. Computed in 473.558 seconds using 1531790400 iterations. Time per iteration: 3.09153e-07      K pos/s: 3234.64
int Game::presetShowcase[64] = {1, 0, 1, 2, 0, 2, 2, 0,
                                0, 1, 0, 0, 2, 0, 2, 1,
                                2, 0, 1, 0, 0, 2, 0, 0,
                                0, 2, 0, 0, 2, 0, 0, 1,
                                0, 0, 1, 0, 0, 1, 0, 0,
                                0, 0, 0, 0, 1, 0, 0, 0,
                                0, 0, 2, 0, 0, 0, 0, 0,
                                0, 0, 0, 0, 1, 0, 0, 0};

//TableEntry Game::transpositionTable[Game::TABLESIZE] = {};
TableEntry *Game::transpositionTable = nullptr;

int Game::computeEmptyGameValue() {
    int emptyBoard[64] = {0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0};
    return Game::computeGameValue(emptyBoard);
}

int Game::computeThreeLayerTestGameValue() {
    int testBoard[64] = {1, 2, 2, 1, 2, 1, 1, 2,
                         1, 2, 2, 1, 2, 1, 1, 2,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0};
    return Game::computeGameValue(testBoard);
}

int Game::computeTwoLayerTestGameValue() {
    int testBoard[64] = {1, 2, 2, 1, 2, 1, 1, 2,
                         1, 2, 2, 1, 2, 1, 1, 2,
                         1, 2, 2, 1, 2, 1, 1, 2,
                         1, 2, 2, 1, 2, 1, 1, 2,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0};
    return Game::computeGameValue(testBoard);
}

int Game::computeOneLayerTestGameValue() {
    int testBoard[64] = {1, 2, 2, 1, 2, 1, 1, 2,
                         1, 2, 2, 1, 2, 1, 1, 2,
                         1, 2, 2, 1, 2, 1, 1, 2,
                         1, 2, 2, 1, 2, 1, 1, 2,
                         2, 0, 0, 2, 0, 0, 0, 0,
                         0, 0, 0, 0, 1, 0, 0, 1,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0};
    return Game::computeGameValue(testBoard);
}

/*int Game::computeCustomTestGameValue() {
    int testBoard[64] = {1, 2, 2, 1, 2, 1, 1, 2,
                         1, 2, 2, 1, 2, 1, 1, 2,
                         2, 1, 1, 2, 1, 2, 2, 1,
                         2, 1, 1, 2, 1, 2, 2, 1,
                         0, 2, 1, 0, 1, 1, 1, 2,
                         2, 2, 2, 1, 0, 1, 2, 0,
                         0, 0, 0, 0, 0, 2, 2, 0,
                         0, 1, 1, 0, 0, 0, 0, 0};
    return Game::computeGameValue(testBoard);
}*/

/*int Game::computeCustomTestGameValue() {
    int testBoard[64] = {1, 2, 2, 1, 2, 1, 1, 2,
                         1, 2, 2, 1, 2, 1, 1, 2,
                         2, 1, 1, 2, 1, 2, 2, 1,
                         2, 1, 1, 2, 1, 2, 2, 1,
                         0, 0, 0, 0, 1, 1, 1, 2,
                         2, 2, 2, 1, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 2, 2, 0,
                         0, 1, 1, 0, 0, 0, 0, 0};
    return Game::computeGameValue(testBoard);
}*/

//Pretty good game for explicit testing, hard enough to not be trivial, but still close to doable
/*int Game::computeCustomTestGameValue() {
    int testBoard[64] = {1, 2, 2, 1, 2, 1, 1, 2,
                         1, 2, 2, 1, 2, 1, 1, 2,
                         2, 1, 1, 2, 1, 2, 2, 1,
                         2, 1, 1, 2, 1, 2, 2, 1,
                         0, 0, 0, 0, 1, 1, 1, 2,
                         2, 2, 2, 1, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 2, 2, 0,
                         0, 1, 1, 0, 0, 0, 0, 0};
    return Game::computeGameValue(testBoard);
}*/

/*int Game::computeCustomTestGameValue() {
    int testBoard[64] = {1, 2, 2, 1, 2, 1, 1, 2,
                         1, 2, 2, 1, 2, 1, 1, 2,
                         0, 2, 2, 1, 2, 1, 1, 2,
                         1, 2, 2, 1, 2, 1, 1, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0};
    return Game::computeGameValue(testBoard);
}*/

int Game::computeTrivialSecondPlayerVictory(){
    int testBoard[64] = {1, 0, 0, 1, 1, 2, 2, 1,
                         1, 0, 0, 1, 2, 1, 1, 2,
                         0, 0, 0, 0, 0, 2, 2, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 2, 2, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0};
    return Game::computeGameValue(testBoard);
}

/*int Game::computeCustomTestGameValue() {
    int testBoard[64] = {1, 2, 2, 1, 2, 1, 1, 2,
                         1, 2, 2, 1, 2, 1, 1, 2,
                         2, 1, 1, 2, 1, 2, 2, 1,
                         2, 1, 1, 2, 1, 2, 2, 1,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0};
    return Game::computeGameValue(testBoard);
}*/

// This board is solvable now, which is quite the progress
// ID := 10
int Game::computeCustomTestGameValue() {
    int testBoard[64] = {1, 2, 2, 1, 2, 1, 1, 2,
                         1, 2, 2, 1, 2, 1, 1, 2,
                         0, 0, 0, 0, 1, 2, 2, 1,
                         2, 1, 1, 2, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0};
    return Game::computeGameValue(testBoard);
}

// ID := 11
/*int Game::computeCustomTestGameValue() {
    int testBoard[64] = {1, 2, 2, 1, 2, 1, 1, 2,
                         1, 2, 2, 1, 2, 1, 1, 2,
                         1, 0, 0, 1, 2, 0, 0, 2,
                         2, 0, 0, 2, 1, 0, 0, 1,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0};
    return Game::computeGameValue(testBoard);
}*/

int Game::computeGameValue(int *initialBoard) {
    if (!BoardState::staticInitializationComplete) {
        BoardState::initialize();
    }

    BoardState inputBoard(initialBoard);
    int result = negaMax(inputBoard, false, false);
    return result;
}

int Game::negaMax(BoardState &position, bool lowerBound, bool upperBound) {
    iterationCount++;
    // std::cout << "Negamax" << std::endl;

#ifdef FULL_DEBUG_OUTPUT
#ifdef PRINT_BB
    std::cout << "Computing the following position at depth : " << std::to_string(position.movesPlayedCount) << " :\n" << position.printCompleteBoardState() << std::endl;
    //std::cout << "Openings of active player:\n" << BoardState::printBB(position.activePlayerOpenings) << std::endl;
#endif
#endif

    if (position.canWin()) {
    // std::cout << "can win" << std::endl;
#ifdef FULL_DEBUG_OUTPUT
        std::cout << "Position at depth " << std::to_string(position.movesPlayedCount) << " can win immediately." << std::endl;
#endif
        return 1;
    }
    if (position.willDrawUnlessWin()) {
    // std::cout << "will draw" << std::endl;
#ifdef FULL_DEBUG_OUTPUT
        std::cout << "Position at depth " << std::to_string(position.movesPlayedCount) << " cannot win and will draw." << std::endl;
#endif
        return 0;
    }

#ifdef TERMINATE_AT_CERTAIN_DEPTH
    if (position.movesPlayedCount >= terminationDepth) {
        terminationCount++;
        return terminationValue;
    }
#endif
    
    // std::cout << "checking transposition" << std::endl;
    //Check for value in transposition table
#ifndef WITHOUT_TRANSPOSITION_TABLE
    if (transpositionTable[position.getTableKey().hashKey(TABLESIZE)].value != INVALID
        && position.getTableKey() == transpositionTable[position.getTableKey().hashKey(TABLESIZE)].board) {

        switch (transpositionTable[position.getTableKey().hashKey(TABLESIZE)].value) {
            case LOSS :
#ifdef FULL_DEBUG_OUTPUT
                std::cout << "Found position at depth " << std::to_string(position.movesPlayedCount) << " in table, result is LOSS." << std::endl;
#endif
                return -1;
            case DRAW_GUARANTEED :
#ifdef FULL_DEBUG_OUTPUT
                std::cout << "Found position at depth " << std::to_string(position.movesPlayedCount) << " in table, result is DRAW_GUARANTEED." << std::endl;
#endif
                return 0;
            case WIN :
#ifdef FULL_DEBUG_OUTPUT
                std::cout << "Found position at depth " << std::to_string(position.movesPlayedCount) << " in table, result is WIN." << std::endl;
#endif
                return 1;
            case DRAW_OR_BETTER :
#ifdef FULL_DEBUG_OUTPUT
                std::cout << "Found position at depth " << std::to_string(position.movesPlayedCount) << " in table, result is DRAW_OR_BETTER." << std::endl;
#endif
#ifndef NO_PRUNING
                if (upperBound) {
                    return 0;
                }
#endif
                lowerBound = true;
                break;
            default :
#ifdef FULL_DEBUG_OUTPUT
                std::cout << "Found position at depth " << std::to_string(position.movesPlayedCount) << " in table, but key is invalid, something went wrong." << std::endl;
#endif
                break;
        }
    }
#ifdef FULL_DEBUG_OUTPUT
    else if (transpositionTable[position.getTableKey().hashKey(TABLESIZE)].value != INVALID) {
        std::cout << "Different doard with same hashkey as position at depth " << std::to_string(position.movesPlayedCount) << " was found." << std::endl;
    }
#endif
#endif

#ifdef FULL_DEBUG_OUTPUT
    std::cout << "Position at depth " << std::to_string(position.movesPlayedCount) << " cannot compute immediately and will call children (Lowerbound: " << lowerBound << ", Upperbound: " << upperBound << ")." << std::endl;
#endif

    // std::cout << "getting moves" << std::endl;
    MoveOrdering moves = position.getMoves();
    
    // std::cout << "got moves" << std::endl;
#ifdef LIMIT_PLAYER_MOVES
    // Check whose turn it is
    if (position.movesPlayedCount % 2 == 0) {
        // Then it is the first player's turn
        if (moves.moveCount > firstPlayerMoveLimit) {
            moves.moveCount = firstPlayerMoveLimit;
        }
    }
    else {
        if (moves.moveCount > secondPlayerMoveLimit) {
            moves.moveCount = secondPlayerMoveLimit;
        }
    }
#endif

    int value = -1;
    for (int i = 0; i < moves.moveCount; i++) {

        // BoardState boardAfterMove(position, moves.moves[i]);
        // std::cout << "working" << std::endl;
        position.MakeMove(moves.moves[i]);
        int currVal = -negaMax(position, upperBound, lowerBound);
        // std::cout << "still working1" << std::endl;
        position.UndoMove();
        // std::cout << "still working" << std::endl;
        //TODO save to table, be careful to remember the difference between boards with value of exactly 0 (all following moves were computed) and at least/most 0 (pruned)
        if (currVal == 1) {
#ifdef FULL_DEBUG_OUTPUT
            std::cout << "Position at depth " << std::to_string(position.movesPlayedCount) << " will win because a child will win." << std::endl;
#endif
            saveCompactBoardStateInTable(position.getTableKey(), TableGameValue::WIN);
            return 1;
        }
        else if (currVal == 0) {
#ifndef NO_PRUNING
            if (upperBound) {
#ifdef FULL_DEBUG_OUTPUT
                std::cout << "Position at depth " << std::to_string(position.movesPlayedCount) << " can force draw and is terminated because of upper bound." << std::endl;
#endif
                saveCompactBoardStateInTable(position.getTableKey(), TableGameValue::DRAW_OR_BETTER);
                return 0;
            }
            else {
                value = 0;
                lowerBound = true;
            }
#else
            value = 0;
            lowerBound = true;
#endif
        }
    }

#ifdef FULL_DEBUG_OUTPUT
    std::cout << "Position at depth " << std::to_string(position.movesPlayedCount) << " found value " << std::to_string(value) << " by computing all its children." << std::endl;
#endif

    saveCompactBoardStateInTable(position.getTableKey(), value == 0 ? TableGameValue::DRAW_GUARANTEED : TableGameValue::LOSS);
    return value;
}

void Game::saveCompactBoardStateInTable(CompactBoardState board, TableGameValue value) {
    transpositionTable[board.hashKey(TABLESIZE)].board = board;
    transpositionTable[board.hashKey(TABLESIZE)].value = value;
}

void Game::initialize() {
    BoardState::initialize();
    transpositionTable = new TableEntry[Game::TABLESIZE];
}

int Game::tableFillCount() {
    int count = 0;
    /*for (auto &i : transpositionTable) {
        if (i.value != TableGameValue::INVALID) {
            count++;
        }
    }*/
    for (int i = 0; i < Game::TABLESIZE; i++) {
        if (Game::transpositionTable[i].value != TableGameValue::INVALID) {
            count++;
        }
    }
    return count;
}

std::string Game::runTestSuite() {
    clock_t time;
    time = clock();

    int testBoard[64] = {1, 2, 2, 1, 2, 1, 1, 2,
                         1, 2, 2, 1, 2, 1, 1, 2,
                         2, 1, 1, 2, 1, 2, 2, 1,
                         2, 1, 1, 2, 1, 2, 2, 1,
                         0, 0, 0, 0, 1, 1, 1, 2,
                         2, 2, 2, 1, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 2, 2, 0,
                         0, 1, 1, 0, 0, 0, 0, 0};
    if (computeGameValue(testBoard) != 1) {
        return "Algorithm failed test 0.";
    }

    return "Test successful, time required: " + std::to_string((clock() - time) / (double) CLOCKS_PER_SEC) + " seconds.";
}

TableEntry::TableEntry() {
    board = CompactBoardState();
    value = TableGameValue::INVALID;
}
