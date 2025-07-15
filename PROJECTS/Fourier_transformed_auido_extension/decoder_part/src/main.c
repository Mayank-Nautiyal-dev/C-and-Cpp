
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dfta.h"

void print_usage(const char* program_name) {
    printf("Dynamic Fourier Transform Audio Decoder (D-FTA)\n");
    printf("Usage: %s input.ftae output.wav\n\n", program_name);
    printf("Examples:\n");
    printf("  %s compressed.ftae restored.wav\n", program_name);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Error: Decoder requires input and output files\n");
        print_usage(argv[0]);
        return 1;
    }
    
    const char* input_file = argv[1];
    const char* output_file = argv[2];
    
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_usage(argv[0]);
        return 0;
    }
    
    // Perform decoding
    int result = decode_audio_file(input_file, output_file);
    
    if (result == 0) {
        printf("Decoding completed successfully!\n");
    } else {
        fprintf(stderr, "Decoding failed with error code: %d\n", result);
        return 1;
    }
    
    return 0;
}


