
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

int write_ftae_file(const char* filename, SineWaveQueue* queue, 
                   const AudioData* original_audio, const EncodingConfig* config) {
    if (!filename || !queue || !original_audio || !config) {
        return DFTA_ERROR_FILE_WRITE;
    }
    
    FILE* file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: Cannot create output file %s\n", filename);
        return DFTA_ERROR_FILE_WRITE;
    }
    
    // Write FTAE header
    FTAEHeader header;
    memcpy(header.magic, "FTAE", 4);
    header.version = 1;
    header.sample_rate = original_audio->sample_rate;
    header.wave_count = queue->count;
    header.compression_level = config->compression_level;
    header.amplitude_threshold = config->amplitude_threshold;
    header.duration = (float)original_audio->sample_count / original_audio->sample_rate;
    memset(header.reserved, 0, sizeof(header.reserved));
    
    if (fwrite(&header, sizeof(FTAEHeader), 1, file) != 1) {
        fprintf(stderr, "Error: Failed to write FTAE header\n");
        fclose(file);
        return DFTA_ERROR_FILE_WRITE;
    }
    
    // Write SineWave data
    SineWaveNode* current = queue->head;
    uint32_t written_count = 0;
    
    while (current) {
        if (fwrite(&current->wave, sizeof(SineWave), 1, file) != 1) {
            fprintf(stderr, "Error: Failed to write SineWave data\n");
            fclose(file);
            return DFTA_ERROR_FILE_WRITE;
        }
        written_count++;
        current = current->next;
    }
    
    fclose(file);
    
    // Calculate compression statistics
    size_t original_size = original_audio->sample_count * sizeof(float);
    size_t compressed_size = sizeof(FTAEHeader) + (written_count * sizeof(SineWave));
    float compression_ratio = (float)original_size / compressed_size;
    
    printf("\nCompression Results:\n");
    printf("  Original size: %zu bytes\n", original_size);
    printf("  Compressed size: %zu bytes\n", compressed_size);
    printf("  Compression ratio: %.2fx\n", compression_ratio);
    printf("  SineWave components: %u\n", written_count);
    printf("  Space savings: %.1f%%\n", ((float)(original_size - compressed_size) / original_size) * 100);
    
    return DFTA_SUCCESS;
}
