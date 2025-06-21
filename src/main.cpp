#include <dlx.h>

#include <iostream>

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

    std::cout << "Original puzzle:\n";
    DLX<3>::printBoard(puzzle1);

    return 0;
}
