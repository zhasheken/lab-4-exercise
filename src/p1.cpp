#include <cstdint>
#include <random>
#include <iostream>
#include <cstddef>
#include <cassert>
#include "timer.h"
constexpr size_t SIZE = 4000;


int main() {
    uint64_t (*a)[SIZE] = new uint64_t[SIZE][SIZE];
    uint64_t (*b)[SIZE] = new uint64_t[SIZE][SIZE];
    Timer timer;
    std::mt19937_64 rng(0);

    // fill first array in row major order
    rng.seed(0);
    for (size_t i = 0; i < SIZE; i++){
        for (size_t j = 0; j < SIZE; j++){
            a[i][j] = rng();
        }
    }

    // fill second array in column major order
    rng.seed(0);
    for (size_t i = 0; i < SIZE; i++){
        for (size_t j = 0; j < SIZE; j++){
            b[j][i] = rng();
        }
    }
    
    // sum row major array, timed
    uint64_t row_major_sum = 0;
    timer.restart();
    for (size_t i = 0; i < SIZE; i++){
        for (size_t j = 0; j < SIZE; j++){
            row_major_sum += a[i][j];
        }
    }
    uint64_t row_major_time = timer.click<Timer::Micros>();

    // sum column major array, timed
    uint64_t col_major_sum = 0;
    timer.restart();
    for (size_t i = 0; i < SIZE; i++){
        for (size_t j = 0; j < SIZE; j++){
            col_major_sum += b[j][i];
        }
    }
    uint64_t col_major_time = timer.click<Timer::Micros>();
    assert(row_major_sum == col_major_sum);


    // print
    std::cout << row_major_time << " " << row_major_sum << std::endl;
    std::cout << col_major_time << " " << col_major_sum << std::endl;

    // free
    delete[] a;
    delete[] b;
    return 0;
}
