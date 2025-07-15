
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include "dfta.h"

void print_usage(const char* program_name) {
    printf("Dynamic Fourier Transform Audio Encoder (D-FTA)\n");
    printf("Usage: %s input.wav output.ftae [OPTIONS]\n\n", program_name);
    printf("Options:\n");
    printf("  --compression-level LEVEL    Compression level: low, medium, high (default: medium)\n");
    printf("  --amplitude-threshold FLOAT  Minimum amplitude threshold (default: 0.01)\n");
    printf("  --help                       Show this help message\n\n");
    printf("Examples:\n");
    printf("  %s audio.wav compressed.ftae --compression-level high\n", program_name);
}

int parse_compression_level(const char* level_str) {
    if (strcmp(level_str, "low") == 0) return COMPRESSION_LOW;
    if (strcmp(level_str, "medium") == 0) return COMPRESSION_MEDIUM;
    if (strcmp(level_str, "high") == 0) return COMPRESSION_HIGH;
    return -1;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }
    
    const char* input_file = argv[1];
    const char* output_file = argv[2];
    
    // Default settings
    EncodingConfig config = {
        .compression_level = COMPRESSION_MEDIUM,
        .amplitude_threshold = 0.01f,
        .frequency_min = 20.0f,
        .frequency_max = 20000.0f,
        .phase_tolerance = 0.1f,
        .similarity_threshold = 0.95f
    };
    
    // Parse command line options
    static struct option long_options[] = {
        {"compression-level", required_argument, 0, 'c'},
        {"amplitude-threshold", required_argument, 0, 'a'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    
    int opt;
    int option_index = 0;
    
    while ((opt = getopt_long(argc - 2, argv + 2, "c:a:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'c': {
                int level = parse_compression_level(optarg);
                if (level == -1) {
                    fprintf(stderr, "Error: Invalid compression level '%s'\n", optarg);
                    return 1;
                }
                config.compression_level = level;
                break;
            }
            case 'a':
                config.amplitude_threshold = atof(optarg);
                if (config.amplitude_threshold <= 0) {
                    fprintf(stderr, "Error: Amplitude threshold must be positive\n");
                    return 1;
                }
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
    
    printf("D-FTA Encoder - Starting compression...\n");
    printf("Input: %s\n", input_file);
    printf("Output: %s\n", output_file);
    printf("Compression Level: %s\n", 
           config.compression_level == COMPRESSION_LOW ? "Low" :
           config.compression_level == COMPRESSION_MEDIUM ? "Medium" : "High");
    printf("Amplitude Threshold: %.4f\n", config.amplitude_threshold);
    
    // Perform encoding
    int result = encode_audio_file(input_file, output_file, &config);
    
    if (result == 0) {
        printf("Encoding completed successfully!\n");
    } else {
        fprintf(stderr, "Encoding failed with error code: %d\n", result);
        return 1;
    }
    
    return 0;
}

