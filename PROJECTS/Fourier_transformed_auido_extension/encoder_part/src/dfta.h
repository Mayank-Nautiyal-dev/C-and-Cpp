 
#ifndef DFTA_H
#define DFTA_H

#include <stdint.h>
#include <complex.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Compression levels
#define COMPRESSION_LOW    0
#define COMPRESSION_MEDIUM 1
#define COMPRESSION_HIGH   2

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

// Encoding configuration
typedef struct {
    int compression_level;
    float amplitude_threshold;
    float frequency_min;
    float frequency_max;
    float phase_tolerance;
    float similarity_threshold;
} EncodingConfig;

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

// Function declarations - ENCODER ONLY
int encode_audio_file(const char* input_file, const char* output_file, const EncodingConfig* config);
int read_wav_file(const char* filename, AudioData* audio_data);
int write_ftae_file(const char* filename, SineWaveQueue* queue, const AudioData* original_audio, const EncodingConfig* config);
void free_audio_data(AudioData* audio_data);

// FFT functions
void fft_radix2(double complex* data, int n, int inverse);
int next_power_of_2(int n);
int adaptive_window_size(const float* samples, int start, int max_size, int sample_rate);

// SineWave queue functions
SineWaveQueue* create_sinewave_queue(void);
void enqueue_sinewave(SineWaveQueue* queue, const SineWave* wave);
void free_sinewave_queue(SineWaveQueue* queue);

// Filtering and optimization functions
void apply_frequency_filtering(SineWaveQueue* queue, float min_freq, float max_freq);
void apply_amplitude_filtering(SineWaveQueue* queue, float threshold);
void apply_phase_optimization(SineWaveQueue* queue, float tolerance);
void apply_similarity_filtering(SineWaveQueue* queue, float threshold);

// Analysis functions
float calculate_signal_complexity(const float* samples, int window_size);
void extract_sinewave_components(const double complex* fft_data, int fft_size, 
                                float sample_rate, float start_time, float duration,
                                SineWaveQueue* queue);

#endif // DFTA_H
