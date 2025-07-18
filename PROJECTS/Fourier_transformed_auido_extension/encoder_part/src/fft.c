
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include "dfta.h"

void fft_radix2(double complex* data, int n, int inverse) {
    if (n <= 1) return;
    
    // Bit-reversal permutation
    int j = 0;
    for (int i = 1; i < n; i++) {
        int bit = n >> 1;
        while (j & bit) {
            j ^= bit;
            bit >>= 1;
        }
        j ^= bit;
        
        if (i < j) {
            double complex temp = data[i];
            data[i] = data[j];
            data[j] = temp;
        }
    }
    
    // FFT computation
    for (int len = 2; len <= n; len <<= 1) {
        double angle = (inverse ? 2.0 : -2.0) * M_PI / len;
        double complex wlen = cos(angle) + I * sin(angle);
        
        for (int i = 0; i < n; i += len) {
            double complex w = 1.0;
            
            for (int j = 0; j < len / 2; j++) {
                double complex u = data[i + j];
                double complex v = data[i + j + len / 2] * w;
                
                data[i + j] = u + v;
                data[i + j + len / 2] = u - v;
                
                w *= wlen;
            }
        }
    }
    
    // Normalize for inverse transform
    if (inverse) {
        for (int i = 0; i < n; i++) {
            data[i] /= n;
        }
    }
}

int next_power_of_2(int n) {
    if (n <= 1) return 1;
    
    int power = 1;
    while (power < n) {
        power <<= 1;
    }
    return power;
}

int adaptive_window_size(const float* samples, int start, int max_size, int sample_rate) {
    (void)sample_rate; // Suppress unused parameter warning
    
    if (!samples || max_size <= 0) return 1024;  // Default size
    
    int base_size = 1024;
    int min_size = 512;
    int actual_max = (max_size < 4096) ? max_size : 4096;
    
    if (actual_max < base_size) return actual_max;
    
    // Calculate signal complexity for a small window
    int test_size = (base_size < actual_max) ? base_size : actual_max;
    float complexity = calculate_signal_complexity(&samples[start], test_size);
    
    // Adjust window size based on complexity
    int adaptive_size;
    if (complexity > 0.5f) {
        // High complexity - use smaller window for better time resolution
        adaptive_size = base_size / 2;
    } else if (complexity < 0.1f) {
        // Low complexity - use larger window for better frequency resolution
        adaptive_size = base_size * 2;
    } else {
        adaptive_size = base_size;
    }
    
    // Ensure size is within bounds and power of 2
    adaptive_size = (min_size > adaptive_size) ? min_size : adaptive_size;
    adaptive_size = (adaptive_size > actual_max) ? actual_max : adaptive_size;
    return next_power_of_2(adaptive_size);
}
