#include <dlx.h>

#include <chrono>
#include <iostream>
#include <ratio>

int main() {
    const DLX<3>::Board puzzle1 = {{
        {0, 2, 9, 0, 0, 0, 4, 0, 0},
        {0, 0, 0, 5, 0, 0, 1, 0, 0},
        {0, 4, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 4, 2, 0, 0, 5},
        {6, 0, 0, 0, 0, 0, 0, 7, 0},
        {5, 0, 0, 0, 0, 0, 0, 0, 0},
        {7, 0, 0, 3, 0, 0, 0, 0, 5},
        {0, 1, 0, 0, 9, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 6, 0}
    }};

    const DLX<4>::Board puzzle2 = {{
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16},
        {0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 0},
        {0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 0, 0},
        {0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0},
        {0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 7, 0, 0, 10, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 8, 9, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 9, 8, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 10, 0, 0, 7, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0},
        {0, 0, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0},
        {0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0},
        {0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0},
        {16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
    }};

    std::cout << "Original puzzle:\n";
    DLX<3>::printBoard(puzzle1);

    auto start1 = std::chrono::high_resolution_clock::now();
    auto solution1 = DLX<3>::solve(puzzle1);
    auto end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration1 = end1 - start1;

    std::cout << "Solution:\n";
    DLX<3>::printBoard(solution1);
    std::cout << "Time taken: " << duration1.count() << " ms\n";
    std::cout << "\n" << std::string(30, '=') << "\n\n";

    std::cout << "Original puzzle:\n";
    DLX<4>::printBoard(puzzle2);

    auto start2 = std::chrono::high_resolution_clock::now();
    auto solution2 = DLX<4>::solve(puzzle2);
    auto end2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration2 = end2 - start2;

    std::cout << "Solution:\n";
    DLX<4>::printBoard(solution2);
    std::cout << "Time taken: " << duration2.count() << " ms\n";
    std::cout << "\n" << std::string(30, '=') << "\n\n";

    return 0;
}
