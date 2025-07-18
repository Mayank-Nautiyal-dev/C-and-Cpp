
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include "dfta.h"

void adjust_config_for_compression_level(EncodingConfig* config) {
    switch (config->compression_level) {
        case COMPRESSION_LOW:
            config->amplitude_threshold *= 0.5f;    // More lenient
            config->similarity_threshold = 0.98f;   // Less aggressive similarity filtering
            break;
        case COMPRESSION_MEDIUM:
            // Use default values
            break;
        case COMPRESSION_HIGH:
            config->amplitude_threshold *= 2.0f;    // More aggressive
            config->similarity_threshold = 0.90f;   // More aggressive similarity filtering
            break;
    }
}

int encode_audio_file(const char* input_file, const char* output_file, const EncodingConfig* config) {
    AudioData audio_data = {0};
    SineWaveQueue* wave_queue = NULL;
    int result = DFTA_SUCCESS;
    
    // Read input WAV file
    result = read_wav_file(input_file, &audio_data);
    if (result != DFTA_SUCCESS) {
        goto cleanup;
    }
    
    // Create SineWave queue
    wave_queue = create_sinewave_queue();
    if (!wave_queue) {
        result = DFTA_ERROR_MEMORY;
        goto cleanup;
    }
    
    // Make a copy of config to adjust for compression level
    EncodingConfig working_config = *config;
    adjust_config_for_compression_level(&working_config);
    
    printf("\nStarting FFT analysis with adaptive windowing...\n");
    
    // Process audio in overlapping windows
    int sample_pos = 0;
    int window_count = 0;
    const float overlap = 0.5f;  // 50% overlap
    
    while (sample_pos < (int)audio_data.sample_count) {
        // Determine adaptive window size
        int remaining_samples = audio_data.sample_count - sample_pos;
        int window_size = adaptive_window_size(audio_data.samples, sample_pos, 
                                             remaining_samples, audio_data.sample_rate);
        
        if (window_size < 64) break;  // Too small to process meaningfully
        
        // Ensure window size is power of 2
        window_size = next_power_of_2(window_size);
        if (window_size > remaining_samples) {
            window_size = next_power_of_2(remaining_samples / 2);
        }
        
        if (window_size < 64) break;
        
        // Prepare FFT input data
        double complex* fft_data = calloc(window_size, sizeof(double complex));
        if (!fft_data) {
            result = DFTA_ERROR_MEMORY;
            goto cleanup;
        }
        
        // Copy audio samples to FFT buffer with windowing
        for (int i = 0; i < window_size && (sample_pos + i) < (int)audio_data.sample_count; i++) {
            // Apply Hann window to reduce spectral leakage
            float window_func = 0.5f * (1.0f - cosf(2.0f * M_PI * i / (window_size - 1)));
            fft_data[i] = audio_data.samples[sample_pos + i] * window_func;
        }
        
        // Perform FFT
        fft_radix2(fft_data, window_size, 0);
        
        // Extract SineWave components
        float start_time = (float)sample_pos / audio_data.sample_rate;
        float duration = (float)window_size / audio_data.sample_rate;
        
        extract_sinewave_components(fft_data, window_size, (float)audio_data.sample_rate,
                                  start_time, duration, wave_queue);
        
        free(fft_data);
        
        // Move to next window with overlap
        sample_pos += (int)(window_size * (1.0f - overlap));
        window_count++;
        
        if (window_count % 100 == 0) {
            printf("  Processed %d windows, %d components so far\n", window_count, wave_queue->count);
        }
    }
    
    printf("FFT analysis complete. Generated %d raw components from %d windows\n", 
           wave_queue->count, window_count);
    
    // Apply filtering and optimization
    printf("\nApplying filters and optimizations...\n");
    
    int original_count = wave_queue->count;
    
    // 1. Frequency filtering (human audible range)
    apply_frequency_filtering(wave_queue, working_config.frequency_min, working_config.frequency_max);
    
    // 2. Amplitude filtering
    apply_amplitude_filtering(wave_queue, working_config.amplitude_threshold);
    
    // 3. Phase optimization
    apply_phase_optimization(wave_queue, working_config.phase_tolerance);
    
    // 4. Similarity filtering
    apply_similarity_filtering(wave_queue, working_config.similarity_threshold);
    
    printf("\nOptimization complete:\n");
    printf("  Original components: %d\n", original_count);
    printf("  Final components: %d\n", wave_queue->count);
    printf("  Reduction: %.1f%%\n", ((float)(original_count - wave_queue->count) / original_count) * 100);
    
    // Write output FTAE file
    printf("\nWriting compressed file...\n");
    result = write_ftae_file(output_file, wave_queue, &audio_data, &working_config);
    
cleanup:
    if (wave_queue) {
        free_sinewave_queue(wave_queue);
    }
    free_audio_data(&audio_data);
    
    return result;
}

float calculate_signal_complexity(const float* samples, int window_size) {
    if (!samples || window_size <= 0) return 1.0f;
    
    float energy = 0.0f;
    float zero_crossings = 0.0f;
    
    for (int i = 0; i < window_size; i++) {
        energy += samples[i] * samples[i];
        if (i > 0 && ((samples[i] >= 0) != (samples[i-1] >= 0))) {
            zero_crossings += 1.0f;
        }
    }
    
    energy /= window_size;
    zero_crossings /= window_size;
    
    // Normalize and combine metrics
    return sqrtf(energy) + (zero_crossings * 0.1f);
}

void extract_sinewave_components(const double complex* fft_data, int fft_size, 
                                float sample_rate, float start_time, float duration,
                                SineWaveQueue* queue) {
    if (!fft_data || !queue || fft_size <= 0) return;
    
    float freq_resolution = sample_rate / fft_size;
    int useful_bins = fft_size / 2;  // Only use positive frequencies
    
    for (int bin = 1; bin < useful_bins; bin++) {
        double magnitude = cabs(fft_data[bin]);
        double phase_rad = carg(fft_data[bin]);
        
        // Skip very low magnitude components
        if (magnitude < 0.001) continue;
        
        // Convert to our format
        SineWave wave;
        wave.frequency = (int)(bin * freq_resolution);
        wave.amplitude = (int)(magnitude * 1000);  // Scale for storage
        wave.phase = (int)((phase_rad * 180.0 / M_PI) + 180) % 360;  // Convert to degrees 0-359
        wave.start_time = start_time;
        wave.duration = duration;
        
        // Basic frequency limits
        if (wave.frequency >= 20 && wave.frequency <= 20000 && wave.amplitude > 0) {
            enqueue_sinewave(queue, &wave);
        }
    }
}
