
#ifndef DFTA_H
#define DFTA_H

#include <stdint.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Error codes
#define DFTA_SUCCESS           0
#define DFTA_ERROR_FILE_READ   1
#define DFTA_ERROR_FILE_WRITE  2
#define DFTA_ERROR_MEMORY      3
#define DFTA_ERROR_FORMAT      4

// SineWave structure for storing frequency components
typedef struct {
    int phase;           // Phase in degrees (0-359)
    int amplitude;       // Amplitude (scaled integer)
    int frequency;       // Frequency in Hz
    float start_time;    // Start time in seconds
    float duration;      // Duration in seconds
} SineWave;

// WAV file header structure
typedef struct {
    char riff[4];
    uint32_t overall_size;
    char wave[4];
    char fmt_chunk_marker[4];
    uint32_t length_of_fmt;
    uint16_t format_type;
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t byterate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char data_chunk_header[4];
    uint32_t data_size;
} WAVHeader;

// Audio data structure
typedef struct {
    float* samples;
    uint32_t sample_count;
    uint32_t sample_rate;
    uint16_t channels;
    uint16_t bits_per_sample;
} AudioData;

// SineWave queue for managing frequency components
typedef struct SineWaveNode {
    SineWave wave;
    struct SineWaveNode* next;
} SineWaveNode;

typedef struct {
    SineWaveNode* head;
    SineWaveNode* tail;
    int count;
} SineWaveQueue;

// Function declarations - DECODER ONLY
int decode_audio_file(const char* input_file, const char* output_file);
int read_ftae_file(const char* filename, SineWaveQueue** queue, AudioData* audio_info);
int write_wav_file(const char* filename, const AudioData* audio_data);
void free_audio_data(AudioData* audio_data);

// SineWave queue functions
SineWaveQueue* create_sinewave_queue(void);
void enqueue_sinewave(SineWaveQueue* queue, const SineWave* wave);
void free_sinewave_queue(SineWaveQueue* queue);

// Synthesis functions
int synthesize_audio_from_sinewaves(SineWaveQueue* queue, AudioData* output_audio);

#endif // DFTA_H
