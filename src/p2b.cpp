#include <cstdint>
#include <random>
#include <iostream>
#include <cassert>
#include <cstddef>
#include "timer.h"
#include "p2a.h"
constexpr size_t SIZE = 256;


int main() {
    Timer timer;
    std::mt19937_64 rng(0);

    // A: Standard (row-major) order, as an array of Z arrays of Y arrays of X uint64_ts.
    uint64_t (*A)[SIZE][SIZE] = new uint64_t[SIZE][SIZE][SIZE];

    // B: 1D array of length Z * Y * X
    uint64_t *B = new uint64_t[SIZE * SIZE * SIZE];

    // fill A in row major order
    rng.seed(0);
    for (size_t z = 0; z < SIZE; z++) {
        for (size_t y = 0; y < SIZE; y++) {
            for (size_t x = 0; x < SIZE; x++) {
                A[z][y][x] = rng();
            }
        }
    }

    // copy A into B in Morton order so that they represent the same array with different memory layouts
    for (size_t z = 0; z < SIZE; z++) {
        for (size_t y = 0; y < SIZE; y++) {
            for (size_t x = 0; x < SIZE; x++) {
                B[morton3d(x, y, z)] = A[z][y][x];
            }
        }
    }

    assert(A[0][0][0] == B[morton3d(0, 0, 0)]);
    assert(A[12][204][156] == B[morton3d(156, 204, 12)]);

    // ------- P2B -------
    // kernels
    const size_t kernel_dim = 4;
    uint64_t Ka[kernel_dim][kernel_dim][kernel_dim];
    uint64_t Kb[kernel_dim * kernel_dim * kernel_dim];
    // K(x,y,z) = x + y + z, 0 <= x,y,z < 4
    for (size_t kz = 0; kz < kernel_dim; kz++) {
        for (size_t ky = 0; ky < kernel_dim; ky++) {
            for (size_t kx = 0; kx < kernel_dim; kx++) {
                const uint64_t v = kx + ky + kz;
                // Ka in row major order
                Ka[kz][ky][kx] = v;
                // Kb in Morton order
                Kb[morton3d(kx, ky, kz)] = v;
            }
        }
    }

    size_t const stride = 4;
    size_t const output_dim = (SIZE - kernel_dim) / stride + 1; // 64
    uint64_t (*outputA)[output_dim][output_dim] = new uint64_t[output_dim][output_dim][output_dim];
    uint64_t *outputB = new uint64_t[output_dim * output_dim * output_dim];

    // convolve Ka over A
    timer.restart();

    // iterate over output array
    for (size_t outputz = 0; outputz < output_dim; outputz++) {
        for (size_t outputy = 0; outputy < output_dim; outputy++) {
            for (size_t outputx = 0; outputx < output_dim; outputx++) {
                uint64_t sum = 0;
                // iterate over kernel
                for (size_t kz = 0; kz < kernel_dim; kz++) {
                    for (size_t ky = 0; ky < kernel_dim; ky++) {
                        for (size_t kx = 0; kx < kernel_dim; kx++) {
                            // dot product of kernel and window of A, with stride
                            sum += A[stride*outputz+kz][stride*outputy+ky][stride*outputx+kx] * Ka[kz][ky][kx];
                        }
                    }
                }
                outputA[outputz][outputy][outputx] = sum;
            }
        }
    }
    uint64_t row_major_conv_time = timer.click<Timer::Micros>();

    // convolve Kb over B
    timer.restart();
    for (size_t i = 0; i < output_dim * output_dim * output_dim; i++) {
        // starts at window i
        const size_t window_start = kernel_dim * kernel_dim * kernel_dim * i;
        uint64_t sum = 0;
        // Kb has the same Morton layout as the window, so j index works for both
        for (size_t j = 0; j < kernel_dim*kernel_dim*kernel_dim; j++) {
            sum += B[window_start + j] * Kb[j];
        }
        outputB[i] = sum;
    }
    uint64_t morton_conv_time = timer.click<Timer::Micros>();

    // verify that the outputs are the same
    for (size_t outputz = 0; outputz < output_dim; outputz++) {
        for (size_t outputy = 0; outputy < output_dim; outputy++) {
            for (size_t outputx = 0; outputx < output_dim; outputx++) {
                assert(outputA[outputz][outputy][outputx] ==
                       outputB[morton3d(outputx, outputy, outputz)]);
            }
        }
    }

    // print time in microseconds
    std::cout << row_major_conv_time << std::endl;
    std::cout << morton_conv_time << std::endl;

    // free
    delete[] A;
    delete[] B;
    delete[] outputA;
    delete[] outputB;

    return 0;
}