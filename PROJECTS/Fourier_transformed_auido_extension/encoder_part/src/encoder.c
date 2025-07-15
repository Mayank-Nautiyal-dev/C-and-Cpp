
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "dfta.h"

// Default encoding configuration
static const EncodingConfig default_config = {
    .compression_level = COMPRESSION_MEDIUM,
    .amplitude_threshold = 0.01f,
    .frequency_min = 20.0f,
    .frequency_max = 20000.0f,
    .phase_tolerance = 0.1f,
    .similarity_threshold = 0.8f
};

int encode_audio_file(const char* input_file, const char* output_file, const EncodingConfig* config) {
    if (!input_file || !output_file) {
        return DFTA_ERROR_FILE_READ;
    }
    
    if (!config) {
        config = &default_config;
    }
    
    printf("Starting DFTA encoding...\n");
    printf("Input: %s\n", input_file);
    printf("Output: %s\n", output_file);
    
    // Read WAV file
    AudioData audio_data = {0};
    int result = read_wav_file(input_file, &audio_data);
    if (result != DFTA_SUCCESS) {
        return result;
    }
    
    // Create SineWave queue for storing components
    SineWaveQueue* queue = create_sinewave_queue();
    if (!queue) {
        free_audio_data(&audio_data);
        return DFTA_ERROR_MEMORY;
    }
    
    // Process audio in overlapping windows
    int window_size = 2048;  // Base window size
    int hop_size = window_size / 4;  // 75% overlap
    int fft_size = next_power_of_2(window_size * 2);
    
    printf("Processing audio with window size: %d, FFT size: %d\n", window_size, fft_size);
    
    double complex* fft_buffer = malloc(fft_size * sizeof(double complex));
    if (!fft_buffer) {
        free_sinewave_queue(queue);
        free_audio_data(&audio_data);
        return DFTA_ERROR_MEMORY;
    }
    
    int windows_processed = 0;
    
    for (uint32_t pos = 0; pos < audio_data.sample_count - window_size; pos += hop_size) {
        // Adaptive window sizing based on signal complexity
        int adaptive_size = adaptive_window_size(audio_data.samples + pos, 
                                                window_size, 
                                                fmaxf(window_size, audio_data.sample_count - pos),
                                                audio_data.sample_rate);
        
        if (adaptive_size > fft_size / 2) {
            adaptive_size = fft_size / 2;
        }
        
        // Clear FFT buffer
        memset(fft_buffer, 0, fft_size * sizeof(double complex));
        
        // Apply Hanning window and copy to FFT buffer
        for (int i = 0; i < adaptive_size && (pos + i) < audio_data.sample_count; i++) {
            double window_coeff = 0.5 * (1.0 - cos(2.0 * M_PI * i / (adaptive_size - 1)));
            fft_buffer[i] = audio_data.samples[pos + i] * window_coeff;
        }
        
        // Perform FFT
        fft_radix2(fft_buffer, fft_size, 0);
        
        // Extract sine wave components
        float start_time = (float)pos / audio_data.sample_rate;
        float duration = (float)adaptive_size / audio_data.sample_rate;
        
        extract_sinewave_components(fft_buffer, fft_size, 
                                  audio_data.sample_rate, 
                                  start_time, duration, queue);
        
        windows_processed++;
        
        // Progress indicator
        if (windows_processed % 100 == 0) {
            float progress = (float)pos / audio_data.sample_count * 100;
            printf("Progress: %.1f%%, Components found: %d\n", progress, queue->count);
        }
    }
    
    free(fft_buffer);
    
    printf("Analysis complete. Found %d raw components\n", queue->count);
    
    // Apply filtering based on compression level
    switch (config->compression_level) {
        case COMPRESSION_HIGH:
            apply_amplitude_filtering(queue, config->amplitude_threshold * 2.0f);
            apply_frequency_filtering(queue, config->frequency_min * 2.0f, config->frequency_max * 0.8f);
            apply_similarity_filtering(queue, config->similarity_threshold * 0.8f);
            apply_phase_optimization(queue, config->phase_tolerance);
            break;
            
        case COMPRESSION_MEDIUM:
            apply_amplitude_filtering(queue, config->amplitude_threshold);
            apply_frequency_filtering(queue, config->frequency_min, config->frequency_max);
            apply_similarity_filtering(queue, config->similarity_threshold);
            break;
            
        case COMPRESSION_LOW:
            apply_amplitude_filtering(queue, config->amplitude_threshold * 0.5f);
            apply_frequency_filtering(queue, config->frequency_min, config->frequency_max);
            break;
    }
    
    printf("After filtering: %d components remain\n", queue->count);
    
    // Write FTAE file
    result = write_ftae_file(output_file, queue, &audio_data, config);
    
    // Cleanup
    free_sinewave_queue(queue);
    free_audio_data(&audio_data);
    
    if (result == DFTA_SUCCESS) {
        printf("Encoding completed successfully!\n");
    }
    
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
