
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "dfta.h"

int decode_audio_file(const char* input_file, const char* output_file) {
    if (!input_file || !output_file) {
        return DFTA_ERROR_FILE_READ;
    }
    
    printf("Input:  %s\n", input_file);
    printf("Output: %s\n", output_file);
    printf("\nStarting decompression...\n");
    
    SineWaveQueue* wave_queue = NULL;
    AudioData audio_info = {0};
    int result = DFTA_SUCCESS;
    
    // Read FTAE file
    printf("Reading FTAE file...\n");
    result = read_ftae_file(input_file, &wave_queue, &audio_info);
    if (result != DFTA_SUCCESS) {
        fprintf(stderr, "Error: Failed to read FTAE file\n");
        goto cleanup;
    }
    
    printf("Loaded %d frequency components\n", wave_queue->count);
    printf("Audio properties: %u Hz, %.2f seconds\n", 
           audio_info.sample_rate, 
           (float)audio_info.sample_count / audio_info.sample_rate);
    
    // Synthesize audio from SineWave components
    printf("\nSynthesizing audio...\n");
    result = synthesize_audio_from_sinewaves(wave_queue, &audio_info);
    if (result != DFTA_SUCCESS) {
        fprintf(stderr, "Error: Failed to synthesize audio\n");
        goto cleanup;
    }
    
    // Write output WAV file
    printf("Writing WAV file...\n");
    result = write_wav_file(output_file, &audio_info);
    if (result != DFTA_SUCCESS) {
        fprintf(stderr, "Error: Failed to write WAV file\n");
        goto cleanup;
    }
    
    printf("Successfully reconstructed %u samples at %u Hz\n", 
           audio_info.sample_count, audio_info.sample_rate);
    
cleanup:
    if (wave_queue) {
        free_sinewave_queue(wave_queue);
    }
    free_audio_data(&audio_info);
    
    return result;
}

int synthesize_audio_from_sinewaves(SineWaveQueue* queue, AudioData* output_audio) {
    if (!queue || !output_audio || queue->count == 0) {
        return DFTA_ERROR_MEMORY;
    }
    
    // Initialize output buffer with zeros
    memset(output_audio->samples, 0, output_audio->sample_count * sizeof(float));
    
    printf("Processing %d frequency components...\n", queue->count);
    
    SineWaveNode* current = queue->head;
    int component_count = 0;
    
    while (current) {
        SineWave* wave = &current->wave;
        
        // Calculate sample indices for this wave's duration
        int start_sample = (int)(wave->start_time * output_audio->sample_rate);
        int duration_samples = (int)(wave->duration * output_audio->sample_rate);
        int end_sample = start_sample + duration_samples;
        
        // Clamp to audio bounds
        if (start_sample < 0) start_sample = 0;
        if (end_sample > (int)output_audio->sample_count) {
            end_sample = output_audio->sample_count;
        }
        
        // Convert scaled amplitude back to float
        float amplitude = (float)wave->amplitude / 1000.0f;
        
        // Convert phase from degrees to radians
        float phase_rad = (float)wave->phase * M_PI / 180.0f;
        
        // Generate sine wave and add to output
        for (int i = start_sample; i < end_sample; i++) {
            float t = (float)i / output_audio->sample_rate;
            float sample_value = amplitude * sinf(2.0f * M_PI * wave->frequency * t + phase_rad);
            
            // Add to existing signal (additive synthesis)
            output_audio->samples[i] += sample_value;
        }
        
        component_count++;
        if (component_count % 500 == 0) {
            printf("  Progress: %d/%d components (%.1f%%)\n", 
                   component_count, queue->count, 
                   (float)component_count / queue->count * 100);
        }
        
        current = current->next;
    }
    
    // Normalize the output to prevent clipping
    float max_amplitude = 0.0f;
    for (uint32_t i = 0; i < output_audio->sample_count; i++) {
        float abs_sample = fabsf(output_audio->samples[i]);
        if (abs_sample > max_amplitude) {
            max_amplitude = abs_sample;
        }
    }
    
    if (max_amplitude > 1.0f) {
        printf("Normalizing audio (peak: %.3f)\n", max_amplitude);
        float scale_factor = 0.95f / max_amplitude;  // Leave some headroom
        for (uint32_t i = 0; i < output_audio->sample_count; i++) {
            output_audio->samples[i] *= scale_factor;
        }
    }
    
    printf("Audio synthesis complete!\n");
    return DFTA_SUCCESS;
}
