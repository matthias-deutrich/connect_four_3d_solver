#include <iostream>
#include <ctime>
#include "Game.h"

int main() {
    std::cout << "Starting initialization." << std::endl;
    Game::initialize();
    std::cout << "Initialization complete. Starting Computation." << std::endl;
    clock_t time;
    time = clock();
    int result = Game::computeGameValue(Game::presetPlayed0_20);
    //int result = Game::computeEmptyGameValue();
    double computationTime = (clock() - time) / (double) CLOCKS_PER_SEC;
    double timePerIteration = computationTime / Game::iterationCount;
    double kPosPerS = Game::iterationCount / computationTime / 1000;
    std::cout << "Computation complete. Result is " << result << ". Computed in " << computationTime << " seconds using " << Game::iterationCount << " iterations." << std::endl;
    std::cout << "Time per iteration: " << timePerIteration << "      K pos/s: " << kPosPerS << std::endl;
    std::cout << "Of the " << Game::TABLESIZE << " possible table entries, " << Game::tableFillCount() << " were used in the end." << std::endl;
    system("pause");
#ifdef TERMINATE_AT_CERTAIN_DEPTH
    std::cout << "Game was terminated at depth " << static_cast<int>(Game::terminationDepth) << " " << Game::terminationCount << " times." << std::endl;
#endif
    return 0;
}

/*int main() {
    std::cout << "Size of struct \"TableEntry\": " << sizeof(TableEntry) << std::endl;
    std::cout << "Size of class \"CompactBoardState\": " << sizeof(CompactBoardState) << std::endl;
    std::cout << "Size of type \"uint64_t\": " << sizeof(uint64_t) << std::endl;
    std::cout << "Size of type \"uint16_t\": " << sizeof(uint16_t) << std::endl;
    std::cout << "Should be 14: " << BoardState::getColumnFromPoint(uint64_t(0b0100000000000000000000000000000000000000000000000000000000000000)) << std::endl;
}*/