
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "dfta.h"

int write_wav_file(const char* filename, const AudioData* audio_data) {
    if (!filename || !audio_data || !audio_data->samples) {
        return DFTA_ERROR_FILE_WRITE;
    }
    
    FILE* file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: Cannot create output file %s\n", filename);
        return DFTA_ERROR_FILE_WRITE;
    }
    
    // Calculate sizes
    uint32_t bytes_per_sample = audio_data->bits_per_sample / 8;
    uint32_t data_size = audio_data->sample_count * bytes_per_sample * audio_data->channels;
    uint32_t file_size = sizeof(WAVHeader) - 8 + data_size;
    
    // Create WAV header
    WAVHeader header;
    memcpy(header.riff, "RIFF", 4);
    header.overall_size = file_size;
    memcpy(header.wave, "WAVE", 4);
    memcpy(header.fmt_chunk_marker, "fmt ", 4);
    header.length_of_fmt = 16;
    header.format_type = 1;  // PCM
    header.channels = audio_data->channels;
    header.sample_rate = audio_data->sample_rate;
    header.byterate = audio_data->sample_rate * audio_data->channels * bytes_per_sample;
    header.block_align = audio_data->channels * bytes_per_sample;
    header.bits_per_sample = audio_data->bits_per_sample;
    memcpy(header.data_chunk_header, "data", 4);
    header.data_size = data_size;
    
    // Write header
    if (fwrite(&header, sizeof(WAVHeader), 1, file) != 1) {
        fprintf(stderr, "Error: Failed to write WAV header\n");
        fclose(file);
        return DFTA_ERROR_FILE_WRITE;
    }
    
    // Convert and write audio data
    if (audio_data->bits_per_sample == 16) {
        int16_t* temp_buffer = malloc(data_size);
        if (!temp_buffer) {
            fclose(file);
            return DFTA_ERROR_MEMORY;
        }
        
        // Convert float samples to 16-bit integers
        for (uint32_t i = 0; i < audio_data->sample_count; i++) {
            // Clamp to [-1.0, 1.0] range
            float sample = audio_data->samples[i];
            if (sample > 1.0f) sample = 1.0f;
            if (sample < -1.0f) sample = -1.0f;
            
            // Convert to 16-bit integer
            int16_t sample_16 = (int16_t)(sample * 32767.0f);
            
            if (audio_data->channels == 1) {
                temp_buffer[i] = sample_16;
            } else {
                // Duplicate mono to stereo
                temp_buffer[i * 2] = sample_16;
                temp_buffer[i * 2 + 1] = sample_16;
            }
        }
        
        if (fwrite(temp_buffer, 1, data_size, file) != data_size) {
            fprintf(stderr, "Error: Failed to write audio data\n");
            free(temp_buffer);
            fclose(file);
            return DFTA_ERROR_FILE_WRITE;
        }
        
        free(temp_buffer);
    } else {
        fprintf(stderr, "Error: Only 16-bit output is supported\n");
        fclose(file);
        return DFTA_ERROR_FORMAT;
    }
    
    fclose(file);
    printf("Successfully wrote WAV file: %u samples at %u Hz\n", 
           audio_data->sample_count, audio_data->sample_rate);
    return DFTA_SUCCESS;
}

void free_audio_data(AudioData* audio_data) {
    if (audio_data && audio_data->samples) {
        free(audio_data->samples);
        audio_data->samples = NULL;
        audio_data->sample_count = 0;
    }
}
