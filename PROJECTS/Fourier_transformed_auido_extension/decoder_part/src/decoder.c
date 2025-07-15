#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dfta.h"

int decode_audio_file(const char* input_file, const char* output_file) {
    if (!input_file || !output_file) {
        return DFTA_ERROR_FILE_READ;
    }
    
    printf("Starting DFTA decoding...\n");
    printf("Input: %s\n", input_file);
    printf("Output: %s\n", output_file);
    
    // Read FTAE file
    SineWaveQueue* queue = NULL;
    AudioData audio_info = {0};
    
    int result = read_ftae_file(input_file, &queue, &audio_info);
    if (result != DFTA_SUCCESS) {
        return result;
    }
    
    printf("Synthesizing audio from %d components...\n", queue->count);
    
    // Synthesize audio from sine waves
    result = synthesize_audio_from_sinewaves(queue, &audio_info);
    if (result != DFTA_SUCCESS) {
        free_sinewave_queue(queue);
        free_audio_data(&audio_info);
        return result;
    }
    
    // Write WAV file
    result = write_wav_file(output_file, &audio_info);
    
    // Cleanup
    free_sinewave_queue(queue);
    free_audio_data(&audio_info);
    
    if (result == DFTA_SUCCESS) {
        printf("Decoding completed successfully!\n");
    }
    
    return result;
}

int synthesize_audio_from_sinewaves(SineWaveQueue* queue, AudioData* output_audio) {
    if (!queue || !output_audio || !output_audio->samples) {
        return DFTA_ERROR_MEMORY;
    }
    
    // Initialize output buffer to zero
    memset(output_audio->samples, 0, output_audio->sample_count * sizeof(float));
    
    SineWaveNode* current = queue->head;
    int components_processed = 0;
    
    while (current) {
        SineWave* wave = &current->wave;
        
        // Calculate sample range for this wave
        uint32_t start_sample = (uint32_t)(wave->start_time * output_audio->sample_rate);
        uint32_t duration_samples = (uint32_t)(wave->duration * output_audio->sample_rate);
        uint32_t end_sample = start_sample + duration_samples;
        
        // Clamp to output buffer bounds
        if (start_sample >= output_audio->sample_count) {
            current = current->next;
            continue;
        }
        
        end_sample = fmin(end_sample, output_audio->sample_count);
        
        // Convert stored values back to float
        float frequency = (float)wave->frequency;
        float amplitude = (float)wave->amplitude / 1000.0f;  // Unscale amplitude
        float phase_rad = (float)(wave->phase - 180) * M_PI / 180.0f;  // Convert back to radians
        
        // Generate sine wave samples
        for (uint32_t i = start_sample; i < end_sample; i++) {
            float t = (float)i / output_audio->sample_rate;
            float sample_value = amplitude * sinf(2.0f * M_PI * frequency * t + phase_rad);
            
            // Add to existing samples (allows multiple waves to combine)
            output_audio->samples[i] += sample_value;
        }
        
        components_processed++;
        current = current->next;
        
        // Progress indicator
        if (components_processed % 1000 == 0) {
            float progress = (float)components_processed / queue->count * 100;
            printf("Synthesis progress: %.1f%%\n", progress);
        }
    }
    
    // Normalize to prevent clipping
    float max_amplitude = 0.0f;
    for (uint32_t i = 0; i < output_audio->sample_count; i++) {
        float abs_sample = fabsf(output_audio->samples[i]);
        if (abs_sample > max_amplitude) {
            max_amplitude = abs_sample;
        }
    }
    
    if (max_amplitude > 1.0f) {
        float normalize_factor = 0.95f / max_amplitude;
        printf("Normalizing audio by factor %.3f\n", normalize_factor);
        
        for (uint32_t i = 0; i < output_audio->sample_count; i++) {
            output_audio->samples[i] *= normalize_factor;
        }
    }
    
    printf("Synthesis complete. Processed %d components\n", components_processed);
    return DFTA_SUCCESS;
}
