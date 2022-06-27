#include <iostream>
#include <ctime>
#include "Game.h"

int main() {
    std::cout << "Starting initialization." << std::endl;
    Game::initialize();
    // std::cout << "   ";
	// for (int col = 0; col < 64; col++) {
    //     if (col < 10) std::cout << " " << col << " ";
    //     else std::cout << col << " ";
	// }
	// std::cout << std::endl;
	
	// for (int row = 0; row < 64; row++) {
    //     if (row < 10) std::cout << " " << row << " ";
    //     else std::cout << " " << row;

	// 	for (int col = 0; col < 64; col++) {

	// 		if (BoardState::affected[row][col] == false) {
	// 			std::cout << "   ";
	// 		}
	// 		else std::cout << " * ";
	// 	}
	// 	std::cout << std::endl;
	// }
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
    std::cout << "Winning positions found " << Game::WinningPosition << std::endl;
    std::cout << "Draw positions found " << Game::TrueDrawPosition << std::endl;
    std::cout << "Loss positions found " << Game::LossPositionInTable << std::endl;
    std::cout << "Draw positions found in table " << Game::DrawPositionInTable << std::endl;
    std::cout << "Winning positions found in table " << Game::WinPositionInTable << std::endl;
    std::cout << "Draw or better positions found in table " << Game::DrawOrBetterPositionInTable << std::endl;
    std::cout << "forcing draw positions found " << Game::CanforceDraw << std::endl;
    for (int const i: Game::WinMoves) {
        std::cout << "Killer moves " << i << std::endl;
    }
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