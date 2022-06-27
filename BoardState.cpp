//
// Created by Matthias on 11-Nov-18.
//

#include "BoardState.h"
#include <iostream>
// #include<bitset>

WinMasks BoardState::openingUpdateMasks[64] = {};
uint16_t BoardState::symmetryLookupTables[65536][7] = {};
unsigned BoardState::shiftDirections[12] = {1,
                                            WIDTH,
                                            WIDTH + 1,
                                            WIDTH - 1,
                                            WIDTH * LENGTH + 1,
                                            WIDTH * LENGTH - 1,
                                            WIDTH * (LENGTH + 1),
                                            WIDTH * (LENGTH - 1),
                                            WIDTH * (LENGTH + 1) + 1,
                                            WIDTH * (LENGTH - 1) - 1,
                                            WIDTH * (LENGTH + 1) - 1,
                                            WIDTH * (LENGTH - 1) + 1};
uint64_t BoardState::shiftMasks[12] = {uint64_t(0b0001000100010001000100010001000100010001000100010001000100010001),
                                       uint64_t(0b0000000000001111000000000000111100000000000011110000000000001111),
                                       uint64_t(0b0000000000000001000000000000000100000000000000010000000000000001),
                                       uint64_t(0b0000000000001000000000000000100000000000000010000000000000001000),
                                       uint64_t(0b0000000000000000000000000000000000000000000000000001000100010001),
                                       uint64_t(0b0000000000000000000000000000000000000000000000001000100010001000),
                                       uint64_t(0b0000000000000000000000000000000000000000000000000000000000001111),
                                       uint64_t(0b0000000000000000000000000000000000000000000000001111000000000000),
                                       uint64_t(0b0000000000000000000000000000000000000000000000000000000000000001),
                                       uint64_t(0b0000000000000000000000000000000000000000000000001000000000000000),
                                       uint64_t(0b0000000000000000000000000000000000000000000000000000000000001000),
                                       uint64_t(0b0000000000000000000000000000000000000000000000000001000000000000)};
bool BoardState::staticInitializationComplete = false;

bool BoardState::affected[64][64] = {};

uint64_t BoardState::activePlayerBoardHistory[64] = {};
uint64_t BoardState::opponentBoardHistory[64] = {};
uint64_t BoardState::activePlayerOpeningsHistory[64] {};
uint64_t BoardState::opponentOpeningsHistory[64] = {};
uint64_t BoardState::playableMovesHistory[64] = {};
MoveOrdering BoardState::moveOrderingHistory[64] = {};
uint64_t BoardState::moveHistory[64] = {};
uint8_t BoardState::movesPlayedCount = 0;

BoardState::BoardState(int *initialBoard) {
    //TODO assure this is a legal board: no draw, no wins, no floating pieces, number of moves for each player equal or different by one
    uint64_t board1 = 0;
    uint64_t board2 = 0;
    this->movesPlayedCount = 0;
    for(unsigned i = 0; i < 64; i++) {
        if (initialBoard[i] != 0) {
            uint64_t currStone = uint64_t(1) << i;
            this->movesPlayedCount++;
            if (initialBoard[i] == 1) {
                board1 |= currStone;
            }
            else {
                board2 |= currStone;
            }
        }
    }
    if (this->movesPlayedCount % 2 == 0) {
        this->activePlayerBoardHistory[movesPlayedCount] = board1;
        this->opponentBoardHistory[movesPlayedCount] = board2;
    }
    else {
        this->activePlayerBoardHistory[movesPlayedCount] = board2;
        this->opponentBoardHistory[movesPlayedCount] = board1;
    }
    this->activePlayerOpeningsHistory[movesPlayedCount] = openings(activePlayerBoardHistory[movesPlayedCount], opponentBoardHistory[movesPlayedCount]);
    this->opponentOpeningsHistory[movesPlayedCount] = openings(opponentBoardHistory[movesPlayedCount], activePlayerBoardHistory[movesPlayedCount]);

    // Compute the possible Moves
    uint64_t filled = activePlayerBoardHistory[movesPlayedCount] | opponentBoardHistory[movesPlayedCount];
    //Flip the board, so shift inserts filled positions (marked by 0)
    filled = ~filled;
    this->playableMovesHistory[movesPlayedCount] = filled ^ (filled << 16);

    // We create a list of all the moves and call them valid if they do not lead to an immediate loss.
    MoveOrdering moves = {};
    // Assume that we already checked that we cannot win immediately beforehand

    //get the list of all moves and whether they are valid or not
    // A move is invalid if it will never be played in this game.
    for (uint8_t i = 0; i < 16; ++i) {
        uint64_t currMove = playableMovesHistory[movesPlayedCount] & (uint64_t(0b0000000000000001000000000000000100000000000000010000000000000001) << i);
        moves.moves[i].score = 0;
        if ((currMove != 0)) {
            moves.moves[i].move = currMove;
            moves.moves[i].valid = true;
            Score(moves.moves[i]);
        }
        else {
            moves.moves[i].valid = false;
        }
    }
    moves.moveCount = 16;

    moveOrderingHistory[movesPlayedCount] = moves;

    this->symmetricBoardsInitialized = false;
}

// BoardState::BoardState(BoardState &position, Move move) {
//     //TODO either make sure that move is always legal or check here

//     // First update openings of previous opponent
//     this->activePlayerOpenings = position.opponentOpenings & (~(move.move));
//     this->opponentOpenings = move.updatedPlayerOpenings;
//     // Note that we do not need to subtract the current move here, because if it had been an opening, we would have won already
//     /*this->opponentOpenings = position.activePlayerOpenings;
//     // Add potential new openings for the player who just played
//     for (int i = 0; i < openingUpdateMasks[getIDFromPoint(move.move)].maskCount; i++) {
//         // First, check whether the opponent already has at least one stone of the line
//         if (((position.opponentBoard) & (openingUpdateMasks[getIDFromPoint(move.move)].masks[i])) == 0) {
//             // Check if there are two other stones on this line already, creating an opening
//             // (note that we cannot have 3 stones there, otherwise this current stone would have won
//             uint64_t currPotentialOpeningStones = (position.activePlayerBoard) & (openingUpdateMasks[getIDFromPoint(move.move)].masks[i]);
//             if (((currPotentialOpeningStones - 1) & currPotentialOpeningStones) != 0) {
//                 // Get the actual opening and add it to the mask
//                 this->opponentOpenings |= currPotentialOpeningStones ^ (openingUpdateMasks[getIDFromPoint(move.move)].masks[i]);
//             }
//         }

//     }
//     if (this->opponentOpenings != move.updatedPlayerOpenings) {
//         throw "Error";
//     }*/

//     this->activePlayerBoard = position.opponentBoard;
//     this->opponentBoard = position.activePlayerBoard | move.move;
//     this->movesPlayedCount = position.movesPlayedCount + 1;
//     this->symmetricBoardsInitialized = false;

//     // Compute the possible Moves
//     uint64_t filled = activePlayerBoard | opponentBoard;
//     //Flip the board, so shift inserts filled positions (marked by 0)
//     filled = ~filled;
//     this->playableMoves = filled ^ (filled << 16);
// }

void BoardState::MakeMove(Move move) {
    //TODO either make sure that move is always legal or check here

    // First update openings of previous opponent
    this->activePlayerOpeningsHistory[movesPlayedCount + 1] = opponentOpeningsHistory[movesPlayedCount] & (~(move.move));
    this->opponentOpeningsHistory[movesPlayedCount + 1] = move.updatedPlayerOpenings;
    // Note that we do not need to subtract the current move here, because if it had been an opening, we would have won already
    /*this->opponentOpenings = position.activePlayerOpenings;
    // Add potential new openings for the player who just played
    for (int i = 0; i < openingUpdateMasks[getIDFromPoint(move.move)].maskCount; i++) {
        // First, check whether the opponent already has at least one stone of the line
        if (((position.opponentBoard) & (openingUpdateMasks[getIDFromPoint(move.move)].masks[i])) == 0) {
            // Check if there are two other stones on this line already, creating an opening
            // (note that we cannot have 3 stones there, otherwise this current stone would have won
            uint64_t currPotentialOpeningStones = (position.activePlayerBoard) & (openingUpdateMasks[getIDFromPoint(move.move)].masks[i]);
            if (((currPotentialOpeningStones - 1) & currPotentialOpeningStones) != 0) {
                // Get the actual opening and add it to the mask
                this->opponentOpenings |= currPotentialOpeningStones ^ (openingUpdateMasks[getIDFromPoint(move.move)].masks[i]);
            }
        }

    }
    if (this->opponentOpenings != move.updatedPlayerOpenings) {
        throw "Error";
    }*/

    this->activePlayerBoardHistory[movesPlayedCount + 1] = opponentBoardHistory[movesPlayedCount];
    this->opponentBoardHistory[movesPlayedCount + 1] = activePlayerBoardHistory[movesPlayedCount] | move.move;
    this->movesPlayedCount = movesPlayedCount + 1;
    this->symmetricBoardsInitialized = false;

    // Compute the possible Moves
    uint64_t filled = activePlayerBoardHistory[movesPlayedCount] | opponentBoardHistory[movesPlayedCount];
    //Flip the board, so shift inserts filled positions (marked by 0)
    filled = ~filled;
    this->playableMovesHistory[movesPlayedCount] = filled ^ (filled << 16);

    this->moveHistory[movesPlayedCount] = move.move;

    MoveOrdering moves = {};

    for (uint8_t i = 0; i < 16; ++i) {
        uint64_t currMove = playableMovesHistory[movesPlayedCount] & (uint64_t(0b0000000000000001000000000000000100000000000000010000000000000001) << i);
        if ((currMove != 0)) {
            moves.moves[moves.moveCount].move = currMove;
            moves.moves[moves.moveCount].score = 0;
            if (affected[getIDFromPoint(move.move)][getIDFromPoint(currMove)]) {
                Score(moves.moves[moves.moveCount]);
            }
            else {
                moves.moves[moves.moveCount].score = moveOrderingHistory[movesPlayedCount - 1].moves[moves.moveCount].score;
            }
            move.updatedPlayerOpenings = activePlayerOpeningsHistory[movesPlayedCount];
            moves.moves[moves.moveCount].valid = true;
        }
        else {
            moves.moves[moves.moveCount].valid = false;
        }

        moves.moveCount++;  
    }

    moveOrderingHistory[movesPlayedCount] = moves;
}

void BoardState::Score(Move &move) {
    unsigned currID = getIDFromPoint(move.move);
    for (int j = 0; j < openingUpdateMasks[currID].maskCount; j++) {
        // First, check whether the opponent already has at least one stone of the line
        if ((opponentBoardHistory[movesPlayedCount] & (openingUpdateMasks[currID].masks[j])) == 0) {
            move.score += SCORE_WEIGHT_WIN_DIRECTION_POSSIBLE;
            // Check if there are two other stones on this line already, creating an opening
            // (note that we cannot have 3 stones there, otherwise this current stone would have won)
            uint64_t currPotentialOpeningStones = activePlayerBoardHistory[movesPlayedCount] & (openingUpdateMasks[currID].masks[j]);
            if (((currPotentialOpeningStones - 1) & currPotentialOpeningStones) != 0) {
                uint64_t createdOpening = currPotentialOpeningStones ^ (openingUpdateMasks[currID].masks[j]);
                // Check whether the opening is immediately playable and add the corresponding weight
                // Note that the second check is necessary because playableMoves does not take the current move into account, which might enable the immediate opening move
                if ((createdOpening & playableMovesHistory[movesPlayedCount]) != 0 || (((move.move) << 16) & createdOpening) != 0) {
                    move.score += SCORE_WEIGHT_PLAYABLE_OPENING_CREATED;
                }
                else {
                    move.score += SCORE_WEIGHT_FUTURE_OPENING_CREATED;
                }
                // Get the actual opening and add it to the mask
                move.updatedPlayerOpenings |= createdOpening;
            }
        }
    }
#ifdef COMPUTE_DETAILED_SCORES
    // Check the value of the current move for the opponent (since we would be blocking it)
    for (int j = 0; j < openingUpdateMasks[currID].maskCount; j++) {
        // First, check whether the active player already has at least one stone of the line
        if ((activePlayerBoardHistory[movesPlayedCount] & (openingUpdateMasks[currID].masks[j])) == 0) {
            move.score += SCORE_WEIGHT_BLOCKED_WIN_DIRECTION_POSSIBLE;
            // Check if there are two other stones on this line already, creating an opening
            // (note that we cannot have 3 stones there, otherwise this current stone would be an opponent's opening and this move would be forced)
            uint64_t currPotentialOpeningStones = opponentBoardHistory[movesPlayedCount] & (openingUpdateMasks[currID].masks[j]);
            if (((currPotentialOpeningStones - 1) & currPotentialOpeningStones) != 0) {
                move.score += SCORE_WEIGHT_BLOCKED_OPENING_CREATED;
            }
        }

    }
#ifdef USE_ENABLED_MOVE_FOR_SCORES
    currID += 16;
    if (currID < 64) {
        // Check the value of the current move for the opponent (since we would be blocking it)
        for (int j = 0; j < openingUpdateMasks[currID].maskCount; j++) {
            // First, check whether the active player already has at least one stone of the line
            if ((activePlayerBoardHistory[movesPlayedCount] & (openingUpdateMasks[currID].masks[j])) == 0) {
                move.score += SCORE_WEIGHT_ABOVE_OPPONENT_WIN_DIRECTION_POSSIBLE;
                // Check if there are two other stones on this line already, creating an opening
                // (note that we cannot have 3 stones there, otherwise this current stone would enable an opponent's opening and this move would be prevented)
                uint64_t currPotentialOpeningStones = opponentBoardHistory[movesPlayedCount] & (openingUpdateMasks[currID].masks[j]);
                if (((currPotentialOpeningStones - 1) & currPotentialOpeningStones) != 0) {
                    move.score += SCORE_WEIGHT_ABOVE_OPPONENT_OPENING_CREATED;
                }
            }

        }
        for (int j = 0; j < openingUpdateMasks[currID].maskCount; j++) {
            // First, check whether the opponent already has at least one stone of the line
            if ((opponentBoardHistory[movesPlayedCount] & (openingUpdateMasks[currID].masks[j])) == 0) {
                move.score += SCORE_WEIGHT_ABOVE_OWN_WIN_DIRECTION_POSSIBLE;
                // Check if there are two other stones on this line already, creating an opening
                uint64_t currPotentialOpeningStones = activePlayerBoardHistory[movesPlayedCount] & (openingUpdateMasks[currID].masks[j]);
                if (((currPotentialOpeningStones - 1) & currPotentialOpeningStones) != 0) {
                    move.score += SCORE_WEIGHT_ABOVE_OWN_OPENING_CREATED;
                }
            }

        }
    }
#endif
#endif
}

void BoardState::UndoMove() {
    movesPlayedCount--;
}

bool BoardState::canWin() {
    return ((playableMovesHistory[movesPlayedCount] & activePlayerOpeningsHistory[movesPlayedCount]) != uint64_t(0));
}

bool BoardState::willDrawUnlessWin() {
    return movesPlayedCount == 63;
}

uint64_t BoardState::openings(uint64_t activePlayerBoard, uint64_t opponentBoard) {
    uint64_t openings(0);
    for (int d = 0; d < 12; d++) {
        openings |= ((activePlayerBoard >> (3 * shiftDirections[d])) & (activePlayerBoard >> (2 * shiftDirections[d])) & (activePlayerBoard >> shiftDirections[d]) & shiftMasks[d])
                | ((activePlayerBoard >> (2 * shiftDirections[d])) & (activePlayerBoard >> shiftDirections[d]) & (shiftMasks[d] << shiftDirections[d]) & (activePlayerBoard << shiftDirections[d]))
                | ((activePlayerBoard >> shiftDirections[d]) & (shiftMasks[d] << (2 * shiftDirections[d])) & (activePlayerBoard << shiftDirections[d]) & (activePlayerBoard << (2 * shiftDirections[d])))
                | ((shiftMasks[d] << (3 * shiftDirections[d])) & (activePlayerBoard << shiftDirections[d]) & (activePlayerBoard << (2 * shiftDirections[d])) & (activePlayerBoard << (3 * shiftDirections[d])));
    }
    openings |= (activePlayerBoard << (WIDTH * LENGTH)) & (activePlayerBoard << (2 * WIDTH * LENGTH)) & (activePlayerBoard << (3 * WIDTH * LENGTH));

    // Exclude the openings where the opponent already has a stone
    return (openings & (~opponentBoard));
}

inline unsigned BoardState::getPointFromCoordinates(unsigned x, unsigned y, unsigned z) {
    return x + 4 * y + 16 * z;
}

uint16_t BoardState::rotateClockwiseBy90Degrees(uint16_t board) {
    uint16_t rot = uint16_t(0);
    for (unsigned i = 0; i < 16; i++) {
        if (((uint16_t(1) << i) & board) != 0) {
            rot |= uint16_t(1) << ((i % 4) * 4 + 3 - (i / 4));
        }
    }
    return rot;
}

uint16_t BoardState::invertHorizontalAxis(uint16_t board) {
    uint16_t inv = uint16_t(0);
    inv |= ((uint16_t(0b1111000000000000)) & board) >> 12;
    inv |= ((uint16_t(0b0000111100000000)) & board) >> 4;
    inv |= ((uint16_t(0b0000000011110000)) & board) << 4;
    inv |= ((uint16_t(0b0000000000001111)) & board) << 12;
    return inv;
}

uint16_t BoardState::invertDiagonalULAxis(uint16_t board) {
    uint16_t inv = uint16_t(0);
    for (unsigned i = 0; i < 16; i++) {
        if (((uint16_t(1) << i) & board) != 0) {
            inv |= uint16_t(1) << ((i % 4) * 4 + (i / 4));
        }
    }
    return inv;
}

void BoardState::initialize() {
    //TODO init symmetry lookup

    // Initialize the masks for updating openings
    // For those are essentially the same as the (soon defunct) WinMasks, the difference is the point in question not being set
    for (unsigned i = 0; i < 64; i++) {
        unsigned xCoordinate = i % 4;
        unsigned yCoordinate = (i % 16) / 4;
        unsigned zCoordinate = i / 16;
        PointPositionRelativeToDimension xPos = xCoordinate == 0 || xCoordinate == 3
                                                ? PointPositionRelativeToDimension::EDGE
                                                : PointPositionRelativeToDimension::INNER;
        PointPositionRelativeToDimension yPos = yCoordinate == 0 || yCoordinate == 3
                                                ? PointPositionRelativeToDimension::EDGE
                                                : PointPositionRelativeToDimension::INNER;
        PointPositionRelativeToDimension zPos = zCoordinate == 0 || zCoordinate == 3
                                                ? PointPositionRelativeToDimension::EDGE
                                                : PointPositionRelativeToDimension::INNER;

        for (int j = 0; j < 7; j++) {
            openingUpdateMasks[i].masks[j] = uint64_t(0);
        }
        openingUpdateMasks[i].maskCount = 3;

        for (unsigned j = 0; j < 4; j++) {
            // X-Directional Mask
            if (j != xCoordinate) {
                int point = getPointFromCoordinates(j, yCoordinate, zCoordinate);
                if (point != i) {
                    affected[i][point] = true;
                }
                openingUpdateMasks[i].masks[0] |= uint64_t(1) << point;
            }

            // Y-Directional Mask
            if (j != yCoordinate) {
                int point = getPointFromCoordinates(xCoordinate, j, zCoordinate);
                if (point != i) {
                    affected[i][point] = true;
                }
                openingUpdateMasks[i].masks[1] |= uint64_t(1) << point;
            }

            // Z-Directional Mask, generally unnecessary for zCoordinate != 3, but computing them here has no cost
            if (j != zCoordinate) {
                int point = getPointFromCoordinates(xCoordinate, yCoordinate, j);
                if (point != i) {
                    affected[i][point] = true;
                }
                openingUpdateMasks[i].masks[2] |= uint64_t(1) << point;
            }
        }

        // XY-Diagonal Mask
        if (xPos == yPos) {
            for (unsigned j = 0; j < 4; j++) {
                if (xCoordinate == yCoordinate) {
                    if (j != xCoordinate) {
                        int point = getPointFromCoordinates(j, j, zCoordinate);
                        if (point != i) {
                            affected[i][point] = true;
                        }
                        openingUpdateMasks[i].masks[openingUpdateMasks[i].maskCount] |= uint64_t(1) << point;
                    }
                }
                else {
                    if (j != xCoordinate) {
                        int point = getPointFromCoordinates(j, 3 - j, zCoordinate);
                        if (point != i) {
                            affected[i][point] = true;
                        }
                        openingUpdateMasks[i].masks[openingUpdateMasks[i].maskCount] |= uint64_t(1) << point;
                    }
                }
            }
            openingUpdateMasks[i].maskCount++;
        }

        // XZ-Diagonal Mask
        if (xPos == zPos) {
            for (unsigned j = 0; j < 4; j++) {
                if (xCoordinate == zCoordinate) {
                    if (j != xCoordinate) {
                        int point = getPointFromCoordinates(j, yCoordinate, j);
                        if (point != i) {
                            affected[i][point] = true;
                        }
                        openingUpdateMasks[i].masks[openingUpdateMasks[i].maskCount] |= uint64_t(1) << point;
                    }
                }
                else {
                    if (j != xCoordinate) {
                        int point = getPointFromCoordinates(j, yCoordinate, 3 - j);
                        if (point != i) {
                            affected[i][point] = true;
                        }
                        openingUpdateMasks[i].masks[openingUpdateMasks[i].maskCount] |= uint64_t(1) << point;
                    }
                }
            }
            openingUpdateMasks[i].maskCount++;
        }

        // YZ-Diagonal Mask
        if (yPos == zPos) {
            for (unsigned j = 0; j < 4; j++) {
                if (yCoordinate == zCoordinate) {
                    if (j != yCoordinate) {
                        int point = getPointFromCoordinates(xCoordinate, j, j);
                        if (point != i) {
                            affected[i][point] = true;
                        }
                        openingUpdateMasks[i].masks[openingUpdateMasks[i].maskCount] |= uint64_t(1) << point;
                    }
                }
                else {
                    if (j != yCoordinate) {
                        int point = getPointFromCoordinates(xCoordinate, j, 3 - j);
                        if (point != i) {
                            affected[i][point] = true;
                        }
                        openingUpdateMasks[i].masks[openingUpdateMasks[i].maskCount] |= uint64_t(1) << point;
                    }
                }
            }
            openingUpdateMasks[i].maskCount++;
        }

        // Full XYZ-Diagonal Mask
        if (xPos == yPos && yPos == zPos) {
            for (unsigned j = 0; j < 4; j++) {
                if (xCoordinate == yCoordinate) {
                    if (yCoordinate == zCoordinate) {
                        if (j != xCoordinate) {
                            int point = getPointFromCoordinates(j, j, j);
                            if (point != i) {
                                affected[i][point] = true;
                            }
                            openingUpdateMasks[i].masks[openingUpdateMasks[i].maskCount] |= uint64_t(1) << point;
                        }
                    }
                    else {
                        if (j != xCoordinate) {
                            int point = getPointFromCoordinates(j, j, 3 - j);
                            if (point != i) {
                                affected[i][point] = true;
                            }
                            openingUpdateMasks[i].masks[openingUpdateMasks[i].maskCount] |= uint64_t(1) << point;
                        }
                    }
                }
                else {
                    if (yCoordinate == zCoordinate) {
                        if (j != yCoordinate) {
                            int point = getPointFromCoordinates(3 - j, j, j);
                            if (point != i) {
                                affected[i][point] = true;
                            }
                            openingUpdateMasks[i].masks[openingUpdateMasks[i].maskCount] |= uint64_t(1) << point;
                        }
                    }
                    else {
                        if (j != xCoordinate) {
                            int point = getPointFromCoordinates(j, 3 - j, j);
                            if (point != i) {
                                affected[i][point] = true;
                            }
                            openingUpdateMasks[i].masks[openingUpdateMasks[i].maskCount] |= uint64_t(1) << point;
                        }
                    }
                }
            }
            openingUpdateMasks[i].maskCount++;
        }
    }

    // Initialize the symmetry lookup tables

    // First, do the last iteration of each loop (all 1) manually to avoid overflow
    for (int i = 0; i < 7; i++) {
        symmetryLookupTables[65535][i] = uint16_t(65535);
    }

    // Clockwise rotation by 90°, inversion at horizontal and diagonal(UL) axis
    for (uint16_t i = uint16_t(0); i < 65535; i++) {
        symmetryLookupTables[i][Symmetry::ROTATION_90] = rotateClockwiseBy90Degrees(i);
        symmetryLookupTables[i][Symmetry::AXIS_HORIZONTAL] = invertHorizontalAxis(i);
        symmetryLookupTables[i][Symmetry::AXIS_DIAGONAL_UL] = invertDiagonalULAxis(i);
    }

    // Clockwise rotation by 180° and 270°
    for (uint16_t i = uint16_t(0); i < 65535; i++) {
        uint16_t rot180 = symmetryLookupTables[symmetryLookupTables[i][Symmetry::ROTATION_90]][Symmetry::ROTATION_90];
        symmetryLookupTables[i][Symmetry::ROTATION_180] = rot180;
        symmetryLookupTables[i][Symmetry::ROTATION_270] = symmetryLookupTables[rot180][Symmetry::ROTATION_90];
    }

    // Inversion at vertical and diagonal(UL) axis
    for (uint16_t i = uint16_t(0); i < 65535; i++) {
        uint16_t rotated = symmetryLookupTables[i][Symmetry::ROTATION_90];
        symmetryLookupTables[i][Symmetry::AXIS_VERTICAL] = symmetryLookupTables[symmetryLookupTables[rotated][Symmetry::AXIS_HORIZONTAL]][Symmetry::ROTATION_270];
        symmetryLookupTables[i][Symmetry::AXIS_DIAGONAL_UR] = symmetryLookupTables[symmetryLookupTables[rotated][Symmetry::AXIS_DIAGONAL_UL]][Symmetry::ROTATION_270];
    }

    staticInitializationComplete = true;
}

MoveOrdering BoardState::getMoves() {
    MoveOrdering moves = {};
    // Assume that we already checked that we cannot win immediately beforehand

    // Compute opponents openings to see whether we have to make a certain move in order to block opponent's win
    uint64_t oppOpenings = opponentOpeningsHistory[movesPlayedCount];
    uint64_t oppPlayableOpenings = oppOpenings & playableMovesHistory[movesPlayedCount];
    if (oppPlayableOpenings != 0) {
        if ((((oppPlayableOpenings - 1) & oppPlayableOpenings) != 0) || (((oppPlayableOpenings << 16) & oppOpenings) != 0)) {
            // In this case the opponent either has more than one playable opening or the only opening is below another opening
            // In either case we are guaranteed to lose, so we return no moves, which will cause a loss in the Game loop
            return moves;
        }
        //Otherwise, play the one move that can block the opponent's win
        moves.moves[0].move = oppPlayableOpenings;
        moves.moveCount = 1;
    }
    else {
        for (uint8_t i = 0; i < 16; ++i) {
            if (moveOrderingHistory[movesPlayedCount].moves[i].valid && ((moveOrderingHistory[movesPlayedCount].moves[i].move << 16) & oppOpenings) == 0) {
                // moves.moves[moves.moveCount] = moveOrderingHistory[movesPlayedCount].moves[i];
                moves.moves[moves.moveCount].move = moveOrderingHistory[movesPlayedCount].moves[i].move;
                moves.moves[moves.moveCount].score = moveOrderingHistory[movesPlayedCount].moves[i].score;
                moves.moves[moves.moveCount].updatedPlayerOpenings = activePlayerOpeningsHistory[movesPlayedCount];
                moves.moves[moves.moveCount].valid = true;
                moves.moveCount++;
            }
        }
        // for (uint8_t i = 0; i < 16; ++i) {
        //     uint64_t currMove = playableMovesHistory[movesPlayedCount] & (uint64_t(0b0000000000000001000000000000000100000000000000010000000000000001) << i);
        //     if ((currMove != 0) && ((currMove << 16) & oppOpenings) == 0) {
        //         moves.moves[moves.moveCount].move = currMove;
        //         moves.moves[moves.moveCount].score = movesPlayedCount > 0? moveOrderingHistory[movesPlayedCount - 1].moves[moves.moveCount].score: 0;
        //         moves.moves[moves.moveCount].valid = true;
        //     }
        //     else {
        //         moves.moves[moves.moveCount].valid = false;
        //     }

        //     moves.moveCount++;  
        // }
    }

    // for (int i = 0; i < moves.moveCount; i++) {
    //     if (!moves.moves[i].valid) continue;

    //     moves.moves[i].updatedPlayerOpenings = activePlayerOpeningsHistory[movesPlayedCount];

    //     unsigned currID = getIDFromPoint(moves.moves[i].move);

    //     //see if this move was affected by the last move
    //     if (affected[getIDFromPoint(moveHistory[movesPlayedCount])][currID]) {
    //         moves.moves[i].score = 0;
    //         for (int j = 0; j < openingUpdateMasks[currID].maskCount; j++) {
    //             // First, check whether the opponent already has at least one stone of the line
    //             if ((opponentBoardHistory[movesPlayedCount] & (openingUpdateMasks[currID].masks[j])) == 0) {
    //                 moves.moves[i].score += SCORE_WEIGHT_WIN_DIRECTION_POSSIBLE;
    //                 // Check if there are two other stones on this line already, creating an opening
    //                 // (note that we cannot have 3 stones there, otherwise this current stone would have won)
    //                 uint64_t currPotentialOpeningStones = activePlayerBoardHistory[movesPlayedCount] & (openingUpdateMasks[currID].masks[j]);
    //                 if (((currPotentialOpeningStones - 1) & currPotentialOpeningStones) != 0) {
    //                     uint64_t createdOpening = currPotentialOpeningStones ^ (openingUpdateMasks[currID].masks[j]);
    //                     // Check whether the opening is immediately playable and add the corresponding weight
    //                     // Note that the second check is necessary because playableMoves does not take the current move into account, which might enable the immediate opening move
    //                     if ((createdOpening & playableMovesHistory[movesPlayedCount]) != 0 || (((moves.moves[i].move) << 16) & createdOpening) != 0) {
    //                         moves.moves[i].score += SCORE_WEIGHT_PLAYABLE_OPENING_CREATED;
    //                     }
    //                     else {
    //                         moves.moves[i].score += SCORE_WEIGHT_FUTURE_OPENING_CREATED;
    //                     }
    //                     // Get the actual opening and add it to the mask
    //                     moves.moves[i].updatedPlayerOpenings |= createdOpening;
    //                 }
    //             }
    //         }
    // #ifdef COMPUTE_DETAILED_SCORES
    //         // Check the value of the current move for the opponent (since we would be blocking it)
    //         for (int j = 0; j < openingUpdateMasks[currID].maskCount; j++) {
    //             // First, check whether the active player already has at least one stone of the line
    //             if ((activePlayerBoardHistory[movesPlayedCount] & (openingUpdateMasks[currID].masks[j])) == 0) {
    //                 moves.moves[i].score += SCORE_WEIGHT_BLOCKED_WIN_DIRECTION_POSSIBLE;
    //                 // Check if there are two other stones on this line already, creating an opening
    //                 // (note that we cannot have 3 stones there, otherwise this current stone would be an opponent's opening and this move would be forced)
    //                 uint64_t currPotentialOpeningStones = opponentBoardHistory[movesPlayedCount] & (openingUpdateMasks[currID].masks[j]);
    //                 if (((currPotentialOpeningStones - 1) & currPotentialOpeningStones) != 0) {
    //                     moves.moves[i].score += SCORE_WEIGHT_BLOCKED_OPENING_CREATED;
    //                 }
    //             }

    //         }
    // #ifdef USE_ENABLED_MOVE_FOR_SCORES
    //         currID += 16;
    //         if (currID < 64) {
    //             // Check the value of the current move for the opponent (since we would be blocking it)
    //             for (int j = 0; j < openingUpdateMasks[currID].maskCount; j++) {
    //                 // First, check whether the active player already has at least one stone of the line
    //                 if ((activePlayerBoardHistory[movesPlayedCount] & (openingUpdateMasks[currID].masks[j])) == 0) {
    //                     moves.moves[i].score += SCORE_WEIGHT_ABOVE_OPPONENT_WIN_DIRECTION_POSSIBLE;
    //                     // Check if there are two other stones on this line already, creating an opening
    //                     // (note that we cannot have 3 stones there, otherwise this current stone would enable an opponent's opening and this move would be prevented)
    //                     uint64_t currPotentialOpeningStones = opponentBoardHistory[movesPlayedCount] & (openingUpdateMasks[currID].masks[j]);
    //                     if (((currPotentialOpeningStones - 1) & currPotentialOpeningStones) != 0) {
    //                         moves.moves[i].score += SCORE_WEIGHT_ABOVE_OPPONENT_OPENING_CREATED;
    //                     }
    //                 }

    //             }
    //             for (int j = 0; j < openingUpdateMasks[currID].maskCount; j++) {
    //                 // First, check whether the opponent already has at least one stone of the line
    //                 if ((opponentBoardHistory[movesPlayedCount] & (openingUpdateMasks[currID].masks[j])) == 0) {
    //                     moves.moves[i].score += SCORE_WEIGHT_ABOVE_OWN_WIN_DIRECTION_POSSIBLE;
    //                     // Check if there are two other stones on this line already, creating an opening
    //                     uint64_t currPotentialOpeningStones = activePlayerBoardHistory[movesPlayedCount] & (openingUpdateMasks[currID].masks[j]);
    //                     if (((currPotentialOpeningStones - 1) & currPotentialOpeningStones) != 0) {
    //                         moves.moves[i].score += SCORE_WEIGHT_ABOVE_OWN_OPENING_CREATED;
    //                     }
    //                 }

    //             }
    //         }
    // #endif
    // #endif
    //         // std::cout << "loop 1 end " << i << std::endl;
    //     }
    // }

    moveOrderingHistory[movesPlayedCount] = moves;
    // std::cout << "scored moves" << std::endl;
#ifndef NO_MOVE_SORTING
#ifdef USE_SORTING_NETWORK
    // Sort the moves using Green's sorting network for 16 values
    for (unsigned i = 0; i < 16; i += 2) {
        conditionalSwap(moves, i, i + 1);
    }
    for (unsigned i = 0; i < 16; i += 4) {
        conditionalSwap(moves, i, i + 2);
        conditionalSwap(moves, i + 1, i + 3);
    }
    for (unsigned j = 0; j < 2; j++) {
        for (unsigned i = j * 8; i < j * 8 + 4; i++) {
            conditionalSwap(moves, i, i + 4);
        }
    }
    for (unsigned i = 0; i < 8; i++) {
        conditionalSwap(moves, i, i + 8);
    }
    conditionalSwap(moves, 5, 10);
    conditionalSwap(moves, 6, 9);
    conditionalSwap(moves, 1, 2);
    conditionalSwap(moves, 3, 12);
    conditionalSwap(moves, 4, 8);
    conditionalSwap(moves, 7, 11);
    conditionalSwap(moves, 13, 14);
    conditionalSwap(moves, 2, 8);
    conditionalSwap(moves, 7, 13);
    conditionalSwap(moves, 11, 14);
    conditionalSwap(moves, 1, 4);
    conditionalSwap(moves, 2, 4);
    conditionalSwap(moves, 11, 13);
    conditionalSwap(moves, 5, 6);
    conditionalSwap(moves, 9, 10);
    conditionalSwap(moves, 3, 8);
    conditionalSwap(moves, 7, 12);
    conditionalSwap(moves, 3, 5);
    conditionalSwap(moves, 6, 8);
    conditionalSwap(moves, 7, 9);
    conditionalSwap(moves, 10, 12);
    for (unsigned i = 3; i < 13; i += 2) {
        conditionalSwap(moves, i, i + 1);
    }
    conditionalSwap(moves, 6, 7);
    conditionalSwap(moves, 8, 9);

#else

    // std::cout << "sorting" << std::endl;
    // Sort the moves via insertion sort
    for (int i = 0; i < moves.moveCount; i++) {
        Move tmpMove = moves.moves[i];
        int j = i;
        for (; j > 0 && moves.moves[j - 1].score < tmpMove.score; j--) {
            moves.moves[j] = moves.moves[j - 1];
        }
        moves.moves[j] = tmpMove;
    }
#endif
#endif

    for (int i = 0; i < moves.moveCount; ++i) {
        std::cout << "move " << moves.moves[i].move << std::endl;
        std::cout << "score " << moves.moves[i].score << std::endl;
    }
    // std::cout << "returning" << std::endl;
    return moves;
}

//TODO include symmetry here, also save the key upon first computation and always return the result afterwards
#ifdef NO_SYMMETRY_USAGE
CompactBoardState BoardState::getTableKey() {
    return CompactBoardState(this->activePlayerBoardHistory[movesPlayedCount], this->opponentBoardHistory[movesPlayedCount]);
}
#else
CompactBoardState BoardState::getTableKey() {
    if (!symmetricBoardsInitialized) {
        initializeSymmetricBoards();
    }
    return tableKey[movesPlayedCount];
}
#endif

std::string BoardState::printBB(uint64_t board){
    std::string bbString = "";
    for (unsigned y = 0; y < 4; y++) {
        for (unsigned z = 0; z < 4; z++) {
            for (unsigned x = 0; x < 4; x++) {
                bbString += ((board & (uint64_t(1) << getPointFromCoordinates(x, y, z))) == uint64_t(0) ? "0 " : "1 ");
            }
            bbString += "      ";
        }
        bbString += "\n";
    }
    return bbString;
}

std::string BoardState::printCompleteBoardState() {
    std::string bbString = "";
    uint64_t  board1 = activePlayerBoardHistory[movesPlayedCount];
    uint64_t board2 = opponentBoardHistory[movesPlayedCount];
    if ((movesPlayedCount % 2) == 1) {
        board1 = opponentBoardHistory[movesPlayedCount];
        board2 = activePlayerBoardHistory[movesPlayedCount];
    }
    for (unsigned y = 0; y < 4; y++) {
        for (unsigned z = 0; z < 4; z++) {
            for (unsigned x = 0; x < 4; x++) {
                bbString += ((board1 & (uint64_t(1) << getPointFromCoordinates(x, y, z))) == uint64_t(0)
                        ? (board2 & (uint64_t(1) << getPointFromCoordinates(x, y, z))) == uint64_t(0)
                            ? "0 "
                            : "2 "
                        : (board2 & (uint64_t(1) << getPointFromCoordinates(x, y, z))) == uint64_t(0)
                            ? "1 "
                            : "3 ");
            }
            bbString += "      ";
        }
        bbString += "\n";
    }
    return bbString;
}

unsigned BoardState::getColumnFromPoint(uint64_t point) {
    switch (point) {
        case uint64_t(0b0000000000000000000000000000000000000000000000000000000000000001) :
        case uint64_t(0b0000000000000000000000000000000000000000000000010000000000000000) :
        case uint64_t(0b0000000000000000000000000000000100000000000000000000000000000000) :
        case uint64_t(0b0000000000000001000000000000000000000000000000000000000000000000) :
            return 0;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000000000000010) :
        case uint64_t(0b0000000000000000000000000000000000000000000000100000000000000000) :
        case uint64_t(0b0000000000000000000000000000001000000000000000000000000000000000) :
        case uint64_t(0b0000000000000010000000000000000000000000000000000000000000000000) :
            return 1;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000000000000100) :
        case uint64_t(0b0000000000000000000000000000000000000000000001000000000000000000) :
        case uint64_t(0b0000000000000000000000000000010000000000000000000000000000000000) :
        case uint64_t(0b0000000000000100000000000000000000000000000000000000000000000000) :
            return 2;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000000000001000) :
        case uint64_t(0b0000000000000000000000000000000000000000000010000000000000000000) :
        case uint64_t(0b0000000000000000000000000000100000000000000000000000000000000000) :
        case uint64_t(0b0000000000001000000000000000000000000000000000000000000000000000) :
            return 3;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000000000010000) :
        case uint64_t(0b0000000000000000000000000000000000000000000100000000000000000000) :
        case uint64_t(0b0000000000000000000000000001000000000000000000000000000000000000) :
        case uint64_t(0b0000000000010000000000000000000000000000000000000000000000000000) :
            return 4;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000000000100000) :
        case uint64_t(0b0000000000000000000000000000000000000000001000000000000000000000) :
        case uint64_t(0b0000000000000000000000000010000000000000000000000000000000000000) :
        case uint64_t(0b0000000000100000000000000000000000000000000000000000000000000000) :
            return 5;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000000001000000) :
        case uint64_t(0b0000000000000000000000000000000000000000010000000000000000000000) :
        case uint64_t(0b0000000000000000000000000100000000000000000000000000000000000000) :
        case uint64_t(0b0000000001000000000000000000000000000000000000000000000000000000) :
            return 6;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000000010000000) :
        case uint64_t(0b0000000000000000000000000000000000000000100000000000000000000000) :
        case uint64_t(0b0000000000000000000000001000000000000000000000000000000000000000) :
        case uint64_t(0b0000000010000000000000000000000000000000000000000000000000000000) :
            return 7;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000000100000000) :
        case uint64_t(0b0000000000000000000000000000000000000001000000000000000000000000) :
        case uint64_t(0b0000000000000000000000010000000000000000000000000000000000000000) :
        case uint64_t(0b0000000100000000000000000000000000000000000000000000000000000000) :
            return 8;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000001000000000) :
        case uint64_t(0b0000000000000000000000000000000000000010000000000000000000000000) :
        case uint64_t(0b0000000000000000000000100000000000000000000000000000000000000000) :
        case uint64_t(0b0000001000000000000000000000000000000000000000000000000000000000) :
            return 9;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000010000000000) :
        case uint64_t(0b0000000000000000000000000000000000000100000000000000000000000000) :
        case uint64_t(0b0000000000000000000001000000000000000000000000000000000000000000) :
        case uint64_t(0b0000010000000000000000000000000000000000000000000000000000000000) :
            return 10;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000100000000000) :
        case uint64_t(0b0000000000000000000000000000000000001000000000000000000000000000) :
        case uint64_t(0b0000000000000000000010000000000000000000000000000000000000000000) :
        case uint64_t(0b0000100000000000000000000000000000000000000000000000000000000000) :
            return 11;
        case uint64_t(0b0000000000000000000000000000000000000000000000000001000000000000) :
        case uint64_t(0b0000000000000000000000000000000000010000000000000000000000000000) :
        case uint64_t(0b0000000000000000000100000000000000000000000000000000000000000000) :
        case uint64_t(0b0001000000000000000000000000000000000000000000000000000000000000) :
            return 12;
        case uint64_t(0b0000000000000000000000000000000000000000000000000010000000000000) :
        case uint64_t(0b0000000000000000000000000000000000100000000000000000000000000000) :
        case uint64_t(0b0000000000000000001000000000000000000000000000000000000000000000) :
        case uint64_t(0b0010000000000000000000000000000000000000000000000000000000000000) :
            return 13;
        case uint64_t(0b0000000000000000000000000000000000000000000000000100000000000000) :
        case uint64_t(0b0000000000000000000000000000000001000000000000000000000000000000) :
        case uint64_t(0b0000000000000000010000000000000000000000000000000000000000000000) :
        case uint64_t(0b0100000000000000000000000000000000000000000000000000000000000000) :
            return 14;
        case uint64_t(0b0000000000000000000000000000000000000000000000001000000000000000) :
        case uint64_t(0b0000000000000000000000000000000010000000000000000000000000000000) :
        case uint64_t(0b0000000000000000100000000000000000000000000000000000000000000000) :
        case uint64_t(0b1000000000000000000000000000000000000000000000000000000000000000) :
            return 15;
        default: throw("Function getColumnFromPoint was called on a value that is not a point.");
    }
}

unsigned BoardState::getIDFromPoint(uint64_t point) {
    switch (point) {
        case uint64_t(0b0000000000000000000000000000000000000000000000000000000000000001) :
            return 0;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000000000000010) :
            return 1;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000000000000100) :
            return 2;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000000000001000) :
            return 3;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000000000010000) :
            return 4;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000000000100000) :
            return 5;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000000001000000) :
            return 6;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000000010000000) :
            return 7;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000000100000000) :
            return 8;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000001000000000) :
            return 9;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000010000000000) :
            return 10;
        case uint64_t(0b0000000000000000000000000000000000000000000000000000100000000000) :
            return 11;
        case uint64_t(0b0000000000000000000000000000000000000000000000000001000000000000) :
            return 12;
        case uint64_t(0b0000000000000000000000000000000000000000000000000010000000000000) :
            return 13;
        case uint64_t(0b0000000000000000000000000000000000000000000000000100000000000000) :
            return 14;
        case uint64_t(0b0000000000000000000000000000000000000000000000001000000000000000) :
            return 15;
        case uint64_t(0b0000000000000000000000000000000000000000000000010000000000000000) :
            return 16;
        case uint64_t(0b0000000000000000000000000000000000000000000000100000000000000000) :
            return 17;
        case uint64_t(0b0000000000000000000000000000000000000000000001000000000000000000) :
            return 18;
        case uint64_t(0b0000000000000000000000000000000000000000000010000000000000000000) :
            return 19;
        case uint64_t(0b0000000000000000000000000000000000000000000100000000000000000000) :
            return 20;
        case uint64_t(0b0000000000000000000000000000000000000000001000000000000000000000) :
            return 21;
        case uint64_t(0b0000000000000000000000000000000000000000010000000000000000000000) :
            return 22;
        case uint64_t(0b0000000000000000000000000000000000000000100000000000000000000000) :
            return 23;
        case uint64_t(0b0000000000000000000000000000000000000001000000000000000000000000) :
            return 24;
        case uint64_t(0b0000000000000000000000000000000000000010000000000000000000000000) :
            return 25;
        case uint64_t(0b0000000000000000000000000000000000000100000000000000000000000000) :
            return 26;
        case uint64_t(0b0000000000000000000000000000000000001000000000000000000000000000) :
            return 27;
        case uint64_t(0b0000000000000000000000000000000000010000000000000000000000000000) :
            return 28;
        case uint64_t(0b0000000000000000000000000000000000100000000000000000000000000000) :
            return 29;
        case uint64_t(0b0000000000000000000000000000000001000000000000000000000000000000) :
            return 30;
        case uint64_t(0b0000000000000000000000000000000010000000000000000000000000000000) :
            return 31;
        case uint64_t(0b0000000000000000000000000000000100000000000000000000000000000000) :
            return 32;
        case uint64_t(0b0000000000000000000000000000001000000000000000000000000000000000) :
            return 33;
        case uint64_t(0b0000000000000000000000000000010000000000000000000000000000000000) :
            return 34;
        case uint64_t(0b0000000000000000000000000000100000000000000000000000000000000000) :
            return 35;
        case uint64_t(0b0000000000000000000000000001000000000000000000000000000000000000) :
            return 36;
        case uint64_t(0b0000000000000000000000000010000000000000000000000000000000000000) :
            return 37;
        case uint64_t(0b0000000000000000000000000100000000000000000000000000000000000000) :
            return 38;
        case uint64_t(0b0000000000000000000000001000000000000000000000000000000000000000) :
            return 39;
        case uint64_t(0b0000000000000000000000010000000000000000000000000000000000000000) :
            return 40;
        case uint64_t(0b0000000000000000000000100000000000000000000000000000000000000000) :
            return 41;
        case uint64_t(0b0000000000000000000001000000000000000000000000000000000000000000) :
            return 42;
        case uint64_t(0b0000000000000000000010000000000000000000000000000000000000000000) :
            return 43;
        case uint64_t(0b0000000000000000000100000000000000000000000000000000000000000000) :
            return 44;
        case uint64_t(0b0000000000000000001000000000000000000000000000000000000000000000) :
            return 45;
        case uint64_t(0b0000000000000000010000000000000000000000000000000000000000000000) :
            return 46;
        case uint64_t(0b0000000000000000100000000000000000000000000000000000000000000000) :
            return 47;
        case uint64_t(0b0000000000000001000000000000000000000000000000000000000000000000) :
            return 48;
        case uint64_t(0b0000000000000010000000000000000000000000000000000000000000000000) :
            return 49;
        case uint64_t(0b0000000000000100000000000000000000000000000000000000000000000000) :
            return 50;
        case uint64_t(0b0000000000001000000000000000000000000000000000000000000000000000) :
            return 51;
        case uint64_t(0b0000000000010000000000000000000000000000000000000000000000000000) :
            return 52;
        case uint64_t(0b0000000000100000000000000000000000000000000000000000000000000000) :
            return 53;
        case uint64_t(0b0000000001000000000000000000000000000000000000000000000000000000) :
            return 54;
        case uint64_t(0b0000000010000000000000000000000000000000000000000000000000000000) :
            return 55;
        case uint64_t(0b0000000100000000000000000000000000000000000000000000000000000000) :
            return 56;
        case uint64_t(0b0000001000000000000000000000000000000000000000000000000000000000) :
            return 57;
        case uint64_t(0b0000010000000000000000000000000000000000000000000000000000000000) :
            return 58;
        case uint64_t(0b0000100000000000000000000000000000000000000000000000000000000000) :
            return 59;
        case uint64_t(0b0001000000000000000000000000000000000000000000000000000000000000) :
            return 60;
        case uint64_t(0b0010000000000000000000000000000000000000000000000000000000000000) :
            return 61;
        case uint64_t(0b0100000000000000000000000000000000000000000000000000000000000000) :
            return 62;
        case uint64_t(0b1000000000000000000000000000000000000000000000000000000000000000) :
            return 63;

        default: 
            // std::cout << "error" << std::endl;
            throw("Function getIDFromPoint was called on a value that is not a point.");
    }
}

inline void BoardState::conditionalSwap(MoveOrdering &moves, unsigned first, unsigned second) {
    if (moves.moves[second].score > moves.moves[first].score) {
        Move tmp = moves.moves[second];
        moves.moves[second] = moves.moves[first];
        moves.moves[first] = tmp;
    }
}

void BoardState::initializeSymmetricBoards() {
    if (!symmetricBoardsInitialized) {
        symmetricBoards[Symmetry::IDENTITY] = CompactBoardState(activePlayerBoardHistory[movesPlayedCount], opponentBoardHistory[movesPlayedCount]);
        tableKey[movesPlayedCount] = symmetricBoards[Symmetry::IDENTITY];
        for (int symmetryID = 0; symmetryID < 7; symmetryID++) {
            symmetricBoards[symmetryID] = CompactBoardState();
            for (unsigned layer = 0; layer < 4; layer++) {
                symmetricBoards[symmetryID].mainBoard |= (static_cast<uint64_t>(
                    symmetryLookupTables[
                        static_cast<uint16_t>(
                            ((symmetricBoards[Symmetry::IDENTITY].mainBoard) >> (layer * 16))
                            & uint64_t(0b0000000000000000000000000000000000000000000000001111111111111111)
                        )
                    ][symmetryID])) << layer * 16;
            }
            symmetricBoards[symmetryID].topLayer = symmetryLookupTables[symmetricBoards[Symmetry::IDENTITY].topLayer][symmetryID];
            if ((symmetricBoards[symmetryID].mainBoard < tableKey[movesPlayedCount].mainBoard)
                || ((symmetricBoards[symmetryID].mainBoard == tableKey[movesPlayedCount].mainBoard) && (symmetricBoards[symmetryID].topLayer < tableKey[movesPlayedCount].topLayer))) {
                tableKey[movesPlayedCount] = symmetricBoards[symmetryID];
            }
        }
    }
    symmetricBoardsInitialized = true;
}

CompactBoardState::CompactBoardState(uint64_t activePlayerBoard, uint64_t opponentBoard) {
    uint64_t filled = activePlayerBoard | opponentBoard;
    this->topLayer = static_cast<uint16_t> (filled >> 48);
    //Flip the board, so shift inserts filled positions (marked by 0)
    filled = ~filled;
    uint64_t lowestEmptySpots = filled ^ (filled << 16);
    this->mainBoard = activePlayerBoard | lowestEmptySpots;
}

CompactBoardState::CompactBoardState() {
    this->mainBoard = uint64_t(0);
    this->topLayer = uint16_t(0);
}

int CompactBoardState::hashKey(int tableSize) {
    return static_cast<int>(((mainBoard % tableSize) + (topLayer % tableSize)) % tableSize);
}

bool CompactBoardState::operator==(const CompactBoardState &other) {
    return (this->mainBoard == other.mainBoard) && (this->topLayer == other.topLayer);
}
