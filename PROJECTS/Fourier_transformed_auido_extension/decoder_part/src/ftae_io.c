

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "dfta.h"

// FTAE file format header
typedef struct {
    char magic[4];           // "FTAE"
    uint32_t version;        // Format version
    uint32_t sample_rate;    // Original sample rate
    uint32_t wave_count;     // Number of SineWave structs
    uint32_t compression_level; // Compression level used
    float amplitude_threshold;  // Amplitude threshold used
    float duration;          // Total duration in seconds
    uint32_t reserved[8];    // Reserved for future use
} FTAEHeader;

int read_ftae_file(const char* filename, SineWaveQueue** queue, AudioData* audio_info) {
    if (!filename || !queue || !audio_info) {
        return DFTA_ERROR_FILE_READ;
    }
    
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open FTAE file '%s'\n", filename);
        return DFTA_ERROR_FILE_READ;
    }
    
    // Read FTAE header
    FTAEHeader header;
    if (fread(&header, sizeof(FTAEHeader), 1, file) != 1) {
        fprintf(stderr, "Error: Failed to read FTAE header\n");
        fclose(file);
        return DFTA_ERROR_FILE_READ;
    }
    
    // Validate FTAE format
    if (strncmp(header.magic, "FTAE", 4) != 0) {
        fprintf(stderr, "Error: Invalid FTAE file format (not an FTAE file)\n");
        fclose(file);
        return DFTA_ERROR_FORMAT;
    }
    
    if (header.version != 1) {
        fprintf(stderr, "Error: Unsupported FTAE version %u (expected version 1)\n", header.version);
        fclose(file);
        return DFTA_ERROR_FORMAT;
    }
    
    printf("\nFTAE File Information:\n");
    printf("  Format Version: %u\n", header.version);
    printf("  Sample Rate: %u Hz\n", header.sample_rate);
    printf("  Duration: %.2f seconds\n", header.duration);
    printf("  Frequency Components: %u\n", header.wave_count);
    printf("  Compression Level: %u\n", header.compression_level);
    printf("  Amplitude Threshold: %.4f\n", header.amplitude_threshold);
    
    // Create SineWave queue
    *queue = create_sinewave_queue();
    if (!*queue) {
        fclose(file);
        return DFTA_ERROR_MEMORY;
    }
    
    // Read SineWave data
    printf("Loading frequency components...\n");
    for (uint32_t i = 0; i < header.wave_count; i++) {
        SineWave wave;
        if (fread(&wave, sizeof(SineWave), 1, file) != 1) {
            fprintf(stderr, "Error: Failed to read SineWave data at index %u\n", i);
            fclose(file);
            free_sinewave_queue(*queue);
            *queue = NULL;
            return DFTA_ERROR_FILE_READ;
        }
        
        enqueue_sinewave(*queue, &wave);
        
        // Progress indicator
        if ((i + 1) % 1000 == 0) {
            printf("  Loaded %u/%u components\n", i + 1, header.wave_count);
        }
    }
    
    fclose(file);
    
    // Setup audio info for reconstruction
    audio_info->sample_rate = header.sample_rate;
    audio_info->sample_count = (uint32_t)(header.duration * header.sample_rate);
    audio_info->channels = 1;  // FTAE format stores mono
    audio_info->bits_per_sample = 16;
    
    // Allocate memory for samples
    audio_info->samples = calloc(audio_info->sample_count, sizeof(float));
    if (!audio_info->samples) {
        free_sinewave_queue(*queue);
        *queue = NULL;
        return DFTA_ERROR_MEMORY;
    }
    
    printf("Successfully loaded %u frequency components\n", header.wave_count);
    return DFTA_SUCCESS;
}
