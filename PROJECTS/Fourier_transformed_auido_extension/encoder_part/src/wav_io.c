
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "dfta.h"

int read_wav_file(const char* filename, AudioData* audio_data) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        return DFTA_ERROR_FILE_READ;
    }
    
    WAVHeader header;
    size_t read_count = fread(&header, sizeof(WAVHeader), 1, file);
    if (read_count != 1) {
        fprintf(stderr, "Error: Cannot read WAV header\n");
        fclose(file);
        return DFTA_ERROR_FILE_READ;
    }
    
    // Validate WAV format
    if (strncmp(header.riff, "RIFF", 4) != 0 || 
        strncmp(header.wave, "WAVE", 4) != 0) {
        fprintf(stderr, "Error: Invalid WAV file format\n");
        fclose(file);
        return DFTA_ERROR_FORMAT;
    }
    
    // Only support PCM format for now
    if (header.format_type != 1) {
        fprintf(stderr, "Error: Only PCM WAV files are supported\n");
        fclose(file);
        return DFTA_ERROR_FORMAT;
    }
    
    printf("WAV File Info:\n");
    printf("  Sample Rate: %u Hz\n", header.sample_rate);
    printf("  Channels: %u\n", header.channels);
    printf("  Bits per Sample: %u\n", header.bits_per_sample);
    printf("  Data Size: %u bytes\n", header.data_size);
    
    // Calculate number of samples
    uint32_t bytes_per_sample = header.bits_per_sample / 8;
    uint32_t total_samples = header.data_size / (bytes_per_sample * header.channels);
    
    // Allocate memory for samples
    audio_data->samples = malloc(total_samples * sizeof(float));
    if (!audio_data->samples) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return DFTA_ERROR_MEMORY;
    }
    
    // Read and convert samples to float
    if (header.bits_per_sample == 16) {
        int16_t* temp_buffer = malloc(header.data_size);
        if (!temp_buffer) {
            free(audio_data->samples);
            fclose(file);
            return DFTA_ERROR_MEMORY;
        }
        
        fread(temp_buffer, 1, header.data_size, file);
        
        // Convert 16-bit samples to float and mix channels if stereo
        for (uint32_t i = 0; i < total_samples; i++) {
            if (header.channels == 1) {
                audio_data->samples[i] = temp_buffer[i] / 32768.0f;
            } else {
                // Mix stereo to mono
                float left = temp_buffer[i * 2] / 32768.0f;
                float right = temp_buffer[i * 2 + 1] / 32768.0f;
                audio_data->samples[i] = (left + right) / 2.0f;
            }
        }
        
        free(temp_buffer);
    } else {
        fprintf(stderr, "Error: Only 16-bit WAV files are supported\n");
        free(audio_data->samples);
        fclose(file);
        return DFTA_ERROR_FORMAT;
    }
    
    audio_data->sample_count = total_samples;
    audio_data->sample_rate = header.sample_rate;
    audio_data->channels = header.channels;
    audio_data->bits_per_sample = header.bits_per_sample;
    
    fclose(file);
    printf("Successfully loaded %u samples\n", total_samples);
    return DFTA_SUCCESS;
}

void free_audio_data(AudioData* audio_data) {
    if (audio_data && audio_data->samples) {
        free(audio_data->samples);
        audio_data->samples = NULL;
        audio_data->sample_count = 0;
    }
}


