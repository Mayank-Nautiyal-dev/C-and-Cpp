
# D-FTA Decoder

## Overview

The D-FTA Decoder is the counterpart to the D-FTA Encoder, responsible for reconstructing audio from the compressed FTAE (Fourier Transform Audio Enhanced) format back to standard WAV files. It performs additive synthesis using the stored sine wave components to recreate the original audio signal with high fidelity.

## Architecture and Components

### Core Components

#### 1. **main.c** - Command Line Interface
- **Purpose**: Handles user interaction and program flow control
- **Key Features**:
  - Simple two-argument interface (input.ftae output.wav)
  - File extension validation and warnings
  - Comprehensive error reporting with descriptive messages
  - Help system with usage examples

#### 2. **decoder.c** - Core Decoding Engine
- **Purpose**: Orchestrates the entire decoding process
- **Key Functions**:
  - `decode_audio_file()`: Main decoding pipeline
  - `synthesize_audio_from_sinewaves()`: Additive synthesis engine
  - Audio normalization and clipping prevention
  - Progress reporting for large files

#### 3. **ftae_io.c** - FTAE File Processing
- **Purpose**: Reads and validates compressed FTAE input files
- **Key Functions**:
  - `read_ftae_file()`: Comprehensive FTAE file parser
  - Format validation and version checking
  - Metadata extraction and audio parameter setup
  - Progressive loading with status updates

#### 4. **wav_io.c** - WAV File Generation
- **Purpose**: Creates standard WAV output files
- **Key Functions**:
  - `write_wav_file()`: Complete WAV file generation
  - PCM format conversion (float to 16-bit)
  - Mono to stereo expansion capability
  - Proper WAV header construction

#### 5. **sinewave_queue.c** - Data Structure Management
- **Purpose**: Manages sine wave component storage and iteration
- **Key Functions**:
  - `create_sinewave_queue()`: Queue initialization
  - `enqueue_sinewave()`: Component loading from file
  - `free_sinewave_queue()`: Memory cleanup
  - Linked list traversal for synthesis

#### 6. **dfta.h** - Decoder Definitions
- **Purpose**: Contains decoder-specific structures and function declarations
- **Key Structures**:
  - `SineWave`: Individual frequency component
  - `AudioData`: Reconstructed audio information
  - `SineWaveQueue`: Component storage structure
  - `FTAEHeader`: File format header structure

## Decoding Process Flow

### Phase 1: File Input and Validation
1. **Command Line Parsing**: Validate input parameters and file paths
2. **FTAE File Opening**: Open and validate file accessibility
3. **Header Reading**: Parse FTAE header and extract metadata
4. **Format Validation**: Verify magic number, version, and integrity
5. **Parameter Setup**: Configure output audio parameters

### Phase 2: Component Loading
1. **Memory Allocation**: Prepare data structures for component storage
2. **Progressive Loading**: Read sine wave components with progress tracking
3. **Queue Construction**: Build linked list of frequency components
4. **Validation**: Verify component integrity and reasonable values
5. **Statistics Display**: Show loaded component count and file information

### Phase 3: Audio Buffer Preparation  
1. **Sample Buffer Allocation**: Allocate float array for reconstructed audio
2. **Buffer Initialization**: Zero-initialize output buffer
3. **Parameter Configuration**: Set sample rate, channels, and bit depth
4. **Memory Management**: Ensure sufficient memory for reconstruction

### Phase 4: Additive Synthesis
1. **Component Iteration**: Process each sine wave component
2. **Time Window Calculation**: Determine active sample range for each component
3. **Sine Wave Generation**: Generate samples using frequency, amplitude, and phase
4. **Additive Combination**: Sum all components into output buffer
5. **Progress Monitoring**: Track synthesis progress for user feedback

### Phase 5: Post-Processing and Output
1. **Peak Detection**: Find maximum amplitude in reconstructed signal
2. **Normalization**: Scale audio to prevent clipping while preserving dynamics
3. **Format Conversion**: Convert float samples to 16-bit PCM
4. **WAV File Writing**: Generate complete WAV file with proper header
5. **Cleanup**: Free all allocated memory and close files

## Technical Details

### FTAE File Format Structure
```c
typedef struct {
    char magic[4];           // "FTAE" identifier
    uint32_t version;        // Format version (currently 1)
    uint32_t sample_rate;    // Original sample rate
    uint32_t wave_count;     // Number of sine wave components
    uint32_t compression_level; // Compression level used
    float amplitude_threshold;  // Amplitude threshold used
    float duration;          // Total duration in seconds
    uint32_t reserved[8];    // Reserved for future use
} FTAEHeader;
```

### Sine Wave Component Structure
```c
typedef struct {
    int phase;           // Phase in degrees (0-359)
    int amplitude;       // Amplitude (scaled integer * 1000)
    int frequency;       // Frequency in Hz
    float start_time;    // Start time in seconds
    float duration;      // Duration in seconds
} SineWave;
```

### Additive Synthesis Algorithm
```c
// For each sine wave component
for (int i = start_sample; i < end_sample; i++) {
    float t = (float)i / sample_rate;
    float sample_value = amplitude * sin(2π * frequency * t + phase);
    output_samples[i] += sample_value;  // Additive synthesis
}
```

### Normalization Strategy
- **Peak Detection**: Find absolute maximum amplitude across all samples
- **Scaling Decision**: Apply normalization only if peak exceeds 1.0
- **Headroom Preservation**: Scale to 0.95 maximum to prevent digital clipping
- **Dynamic Range**: Maintains relative amplitude relationships

## Synthesis Process Details

### 1. Component Processing
Each sine wave component is processed independently:
- **Time Bounds**: Calculate start and end sample indices from timing information
- **Amplitude Scaling**: Convert integer amplitude back to float (÷1000)
- **Phase Conversion**: Convert degrees to radians for sine function
- **Sample Generation**: Generate sine wave samples for the specified duration

### 2. Temporal Accuracy
- **Sample-Perfect Timing**: Uses floating-point time calculations for precision
- **Boundary Handling**: Properly clips components to audio buffer bounds
- **Overlap Management**: Multiple components can overlap in time domain

### 3. Frequency Accuracy
- **Integer Frequency Storage**: Maintains exact frequency values from encoding
- **Phase Coherence**: Preserves phase relationships between components
- **Harmonic Structure**: Reconstructs complex harmonic relationships

### 4. Quality Preservation
- **No Additional Filtering**: Preserves all encoded frequency information
- **Bit-Accurate Reconstruction**: Uses same mathematical formulations as encoder
- **Dynamic Range**: Maintains amplitude relationships through careful scaling

## Audio Format Support

### Input Format (FTAE)
- **Magic Number**: "FTAE" file identifier
- **Version Support**: Currently version 1
- **Endianness**: Little-endian for cross-platform compatibility
- **Component Limit**: Theoretical limit of 4.3 billion components

### Output Format (WAV)
- **Format**: 16-bit PCM WAV
- **Channels**: Mono (can be expanded to stereo)
- **Sample Rates**: Supports all standard rates (8kHz to 192kHz)
- **Bit Depth**: 16-bit signed integers
- **Compatibility**: Standard WAV format playable on all audio software

## Usage Examples

### Basic Usage
```bash
# Simple decoding
./dfta_decode compressed.ftae restored.wav

# Decode with explicit paths
./dfta_decode /path/to/input.ftae /path/to/output.wav
```

### Batch Processing
```bash
# Process all FTAE files in directory
for file in *.ftae; do
    ./dfta_decode "$file" "${file%.ftae}_restored.wav"
done

# Process with subdirectories
find . -name "*.ftae" -exec ./dfta_decode {} {}_restored.wav \;
```

### Integration Examples
```bash
# Encode and decode pipeline
./encoder/dfta_encode original.wav temp.ftae --compression-level medium
./decoder/dfta_decode temp.ftae reconstructed.wav

# Quality comparison
ffmpeg -i original.wav -i reconstructed.wav -filter_complex "[0:a][1:a]adelay=0|0[a1];[a1]asplit[a1][a2];[a1][a2]adifference" difference.wav
```

## Performance Characteristics

### Memory Usage
- **Component Storage**: Linear with number of sine wave components
- **Output Buffer**: Sample count * sizeof(float) for reconstruction
- **Peak Usage**: Approximately 2x final WAV file size during processing
- **Optimization**: Sequential processing minimizes memory footprint

### Processing Speed
- **Synthesis Speed**: Linear with total sample count and component count
- **I/O Performance**: Efficient buffered file operations
- **Progress Tracking**: Minimal overhead for user feedback
- **Scalability**: Handles files from seconds to hours in length

### Quality Metrics
- **Frequency Response**: Exact reconstruction of encoded frequencies
- **Phase Accuracy**: Preserves phase relationships within encoding precision
- **Amplitude Fidelity**: Maintains dynamic range with normalization protection
- **Temporal Precision**: Sample-accurate timing reconstruction

## Error Handling and Diagnostics

### File Format Errors
```bash
# Invalid FTAE file
Error: Invalid FTAE file format (not an FTAE file)

# Version mismatch
Error: Unsupported FTAE version 2 (expected version 1)

# Corrupted data
Error: Failed to read SineWave data at index 1234
```

### Memory Errors
```bash
# Insufficient memory
Error: Memory allocation failed

# Large file handling
Warning: Large file detected, processing may take several minutes
```

### Output Errors
```bash
# Permission issues
Error: Cannot create output file 'readonly.wav'

# Disk space
Error: Failed to write audio data (disk full?)
```

### Quality Warnings
```bash
# Normalization applied
Normalizing audio (peak: 2.345)

# Large dynamic range
Warning: High dynamic range detected, consider checking output levels
```

## Building and Installation

### Prerequisites
```bash
# Required tools and libraries
gcc (C99 support required)
make
libm (math library)
```

### Compilation Options
```bash
# Standard build
make clean && make

# Debug build
make clean && make CFLAGS="-Wall -Wextra -g -std=c99 -lm"

# Optimized build
make clean && make CFLAGS="-Wall -Wextra -O3 -std=c99 -lm -DNDEBUG"

# Manual compilation
gcc src/main.c src/decoder.c src/ftae_io.c src/wav_io.c src/sinewave_queue.c \
    -o dfta_decode -Wall -Wextra -O2 -std=c99 -lm
```

### Installation
```bash
# System installation
sudo make install

# User installation
cp dfta_decode ~/bin/

# Verify installation
dfta_decode --help
```

## Quality Assessment and Validation

### Reconstruction Accuracy
- **Frequency Domain**: Exact reconstruction of encoded frequency components
- **Time Domain**: Sample-accurate timing with floating-point precision
- **Amplitude Domain**: Preserved dynamic range with optional normalization
- **Phase Domain**: Maintained phase relationships critical for stereo imaging

### Suitable Content Validation
- **Tonal Content**: Excellent reconstruction of harmonic material
- **Complex Music**: Good reconstruction with some high-frequency smoothing
- **Speech**: Very good reconstruction preserving intelligibility
- **Percussive**: Moderate reconstruction depending on encoding parameters

### Testing and Verification
```bash
# Basic functionality test
./dfta_decode test.ftae test_output.wav

# Quality assessment with reference
ffmpeg -i original.wav -i reconstructed.wav -filter_complex \
    "[0:a][1:a]adelay=0|0[a1];[a1]asplit[a1][a2];[a1][a2]acompare" comparison.log

# Spectral analysis
ffmpeg -i reconstructed.wav -lavfi showspectrumpic=s=1920x1080 spectrum.png
```

## Troubleshooting Guide

### Common Issues

#### 1. **"Cannot open FTAE file"**
- **Check**: File existence and permissions
- **Solution**: Verify file path and read permissions

#### 2. **"Invalid FTAE file format"**  
- **Check**: File is actually FTAE format, not corrupted
- **Solution**: Re-encode source file or verify file integrity

#### 3. **"Memory allocation failed"**
- **Check**: Available system memory
- **Solution**: Close other applications or process smaller files

#### 4. **Silent or very quiet output**
- **Check**: Original encoding amplitude threshold
- **Solution**: Check if normalization was applied, adjust playback volume

### Performance Issues

#### Slow Decoding
- **Cause**: Large number of frequency components
- **Monitor**: Progress updates during synthesis
- **Solution**: Normal for high-quality/low-compression files

#### High Memory Usage
- **Cause**: Long audio files or many components
- **Monitor**: System memory usage during processing
- **Solution**: Process shorter segments if necessary

## Development and Extension

### Code Organization
- **Modular Design**: Clear separation of concerns
- **Error Handling**: Comprehensive error checking and user feedback
- **Memory Safety**: Careful allocation tracking and cleanup
- **Standards Compliance**: C99 standard with portable code

### Potential Enhancements
- **Multi-threading**: Parallel synthesis of components
- **Streaming Output**: Real-time playback during decoding
- **Quality Analysis**: Built-in SNR and THD measurement
- **Format Extension**: Support for multi-channel output
- **Optimization**: SIMD acceleration for synthesis loops

### API Integration
```c
// Example library interface
int dfta_decode_to_buffer(const char* ftae_file, float** output_buffer, 
                         uint32_t* sample_count, uint32_t* sample_rate);

int dfta_decode_streaming(const char* ftae_file, 
                         void (*callback)(float* samples, int count));
```

### Testing Framework
```bash
# Automated testing
make test

# Regression testing
./test_suite.sh

# Memory leak checking
valgrind --leak-check=full ./dfta_decode test.ftae output.wav
```

This decoder provides reliable, high-quality reconstruction of audio from the D-FTA compressed format, maintaining the mathematical precision and audio fidelity that makes the D-FTA system effective for various audio compression applications.
