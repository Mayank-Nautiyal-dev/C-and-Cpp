
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dfta.h"

void print_usage(const char* program_name) {
    printf("Dynamic Fourier Transform Audio Decoder (D-FTA)\n");
    printf("Usage: %s input.ftae output.wav\n\n", program_name);
    printf("Description:\n");
    printf("  Converts compressed FTAE files back to WAV audio format\n\n");
    printf("Examples:\n");
    printf("  %s compressed.ftae restored.wav\n", program_name);
    printf("  %s music.ftae output.wav\n", program_name);
}

int main(int argc, char* argv[]) {
    // Check for help flag
    if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        print_usage(argv[0]);
        return 0;
    }
    
    // Check argument count
    if (argc != 3) {
        fprintf(stderr, "Error: Invalid number of arguments\n");
        print_usage(argv[0]);
        return 1;
    }
    
    const char* input_file = argv[1];
    const char* output_file = argv[2];
    
    // Validate file extensions
    const char* input_ext = strrchr(input_file, '.');
    const char* output_ext = strrchr(output_file, '.');
    
    if (!input_ext || strcmp(input_ext, ".ftae") != 0) {
        fprintf(stderr, "Warning: Input file should have .ftae extension\n");
    }
    
    if (!output_ext || strcmp(output_ext, ".wav") != 0) {
        fprintf(stderr, "Warning: Output file should have .wav extension\n");
    }
    
    printf("D-FTA Decoder v1.0\n");
    printf("==================\n");
    
    // Perform decoding
    int result = decode_audio_file(input_file, output_file);
    
    if (result == DFTA_SUCCESS) {
        printf("\n✓ Decoding completed successfully!\n");
        printf("Output file: %s\n", output_file);
    } else {
        fprintf(stderr, "\n✗ Decoding failed with error code: %d\n", result);
        
        switch (result) {
            case DFTA_ERROR_FILE_READ:
                fprintf(stderr, "Cannot read input file or file is corrupted\n");
                break;
            case DFTA_ERROR_FILE_WRITE:
                fprintf(stderr, "Cannot write output file\n");
                break;
            case DFTA_ERROR_MEMORY:
                fprintf(stderr, "Memory allocation failed\n");
                break;
            case DFTA_ERROR_FORMAT:
                fprintf(stderr, "Invalid file format\n");
                break;
            default:
                fprintf(stderr, "Unknown error occurred\n");
                break;
        }
        return 1;
    }
    
    return 0;
}
