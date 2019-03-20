//
// Created by Matthias on 11-Nov-18.
//

#ifndef FIAR_3D_CPP_BOARDSTATE_H
#define FIAR_3D_CPP_BOARDSTATE_H

// Define here whether moves shall be sorted
//#define NO_MOVE_SORTING

#ifndef NO_MOVE_SORTING
//#define USE_SORTING_NETWORK
#endif

//#define COMPUTE_DETAILED_SCORES
#ifdef COMPUTE_DETAILED_SCORES
//#define USE_ENABLED_MOVE_FOR_SCORES
#endif

//#define NO_SYMMETRY_USAGE

#include <cstdint>
#include <array>
#include <string>

/**
 * The board is represented as a 64-bit uint. The 4 16-bit subparts each represent a layer in height, with 0 being on
 * the lowest layer. Whenever orientation of the board is relevant (e.g. to determine the symmetries), the board shall
 * be seen as follows (exemplified by the lowest layer, all other layers accordingly). Whenever coordinates are required,
 * they will also be seen according to the following board (height will be z).
 *   -----------> x
 * | 0  1  2  3
 * | 4  5  6  7
 * | 8  9  10 11
 * | 12 13 14 15
 * v
 * y
 */



enum Symmetry {
    ROTATION_90 = 0,        // clockwise
    ROTATION_180 = 1,       // clockwise
    ROTATION_270 = 2,       // clockwise
    AXIS_HORIZONTAL = 3,    // -
    AXIS_VERTICAL = 4,      // |
    AXIS_DIAGONAL_UL = 5,   // \ (starting upper left)
    AXIS_DIAGONAL_UR = 6,   // / (starting upper right)
    IDENTITY = 7
};

enum PointPositionRelativeToDimension {
    EDGE,
    INNER
};

//TODO check whether packing for this class and the enveloping struct is worth it and, if so, pack(1) or pack(2)
#pragma pack(push)
#pragma pack(4)
class CompactBoardState {
public:
    CompactBoardState(uint64_t activePlayerBoard, uint64_t opponentBoard);
    CompactBoardState();
    int hashKey(int tableSize);
    bool operator == (const CompactBoardState& other);
    uint64_t mainBoard;
    uint16_t topLayer;
};
#pragma pack(pop)

struct Move {
    uint64_t move;
    uint64_t updatedPlayerOpenings;
    int score = -100000000;
};

/**
 * In order to ignore memory allocation and deletion for objects on the heap, we just allocate 16 possible moves (the
 * maximum) on the stack and only use the moveCount first ones.
 */
struct MoveOrdering {
    int moveCount = 0;
    Move moves[16];
};

struct WinMasks {
    uint8_t maskCount;
    uint64_t masks[7];
};

class BoardState {
public:
    static const int WIDTH = 4;
    static const int LENGTH = 4;
    static const int HEIGHT = 4;

    static const int SCORE_WEIGHT_WIN_DIRECTION_POSSIBLE = 10000;
    static const int SCORE_WEIGHT_OPENING_CREATED = 100000;
    static const int SCORE_WEIGHT_BLOCKED_WIN_DIRECTION_POSSIBLE = 100;
    static const int SCORE_WEIGHT_BLOCKED_OPENING_CREATED = 1000;
    static const int SCORE_WEIGHT_ABOVE_OPPONENT_WIN_DIRECTION_POSSIBLE = -100;
    static const int SCORE_WEIGHT_ABOVE_OPPONENT_OPENING_CREATED = -1000;
    static const int SCORE_WEIGHT_ABOVE_OWN_WIN_DIRECTION_POSSIBLE = 1;
    static const int SCORE_WEIGHT_ABOVE_OWN_OPENING_CREATED = 10;

    static WinMasks openingUpdateMasks[64];
    static uint16_t symmetryLookupTables[65536][7];
    static unsigned shiftDirections[12];
    static uint64_t shiftMasks[12];
    static bool staticInitializationComplete;


    static unsigned getColumnFromPoint(uint64_t point);

    static std::string printBB(uint64_t board);
    static uint16_t rotateClockwiseBy90Degrees(uint16_t board);
    static uint16_t invertHorizontalAxis(uint16_t board);
    static uint16_t invertDiagonalULAxis(uint16_t board);
    static void initialize();

    uint64_t activePlayerBoard;
    uint64_t opponentBoard;
    uint64_t activePlayerOpenings;
    uint64_t opponentOpenings;
    uint64_t playableMoves;
    uint8_t movesPlayedCount;

    CompactBoardState symmetricBoards[8];
    CompactBoardState tableKey;
    bool symmetricBoardsInitialized;

    explicit BoardState(int* initialBoard);
    BoardState(BoardState &position, Move move);

    bool canWin();
    bool willDrawUnlessWin();
    MoveOrdering getMoves();
    void initializeSymmetricBoards();
    CompactBoardState getTableKey();

    std::string printCompleteBoardState();

private:
    uint64_t openings(uint64_t activePlayerBoard, uint64_t opponentBoard);
    static inline unsigned getPointFromCoordinates(unsigned x, unsigned y, unsigned z);
    static unsigned getIDFromPoint(uint64_t point);
    static inline void conditionalSwap(MoveOrdering &moves, unsigned first, unsigned second);
};


#endif //FIAR_3D_CPP_BOARDSTATE_H
