
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dfta.h"

SineWaveQueue* create_sinewave_queue(void) {
    SineWaveQueue* queue = malloc(sizeof(SineWaveQueue));
    if (queue) {
        queue->head = NULL;
        queue->tail = NULL;
        queue->count = 0;
    }
    return queue;
}

void enqueue_sinewave(SineWaveQueue* queue, const SineWave* wave) {
    if (!queue || !wave) return;
    
    SineWaveNode* node = malloc(sizeof(SineWaveNode));
    if (!node) return;
    
    node->wave = *wave;
    node->next = NULL;
    
    if (queue->tail) {
        queue->tail->next = node;
    } else {
        queue->head = node;
    }
    
    queue->tail = node;
    queue->count++;
}

void free_sinewave_queue(SineWaveQueue* queue) {
    if (!queue) return;
    
    SineWaveNode* current = queue->head;
    while (current) {
        SineWaveNode* next = current->next;
        free(current);
        current = next;
    }
    
    free(queue);
}

void apply_frequency_filtering(SineWaveQueue* queue, float min_freq, float max_freq) {
    if (!queue) return;
    
    SineWaveNode* current = queue->head;
    SineWaveNode* prev = NULL;
    int removed_count = 0;
    
    while (current) {
        if (current->wave.frequency < min_freq || current->wave.frequency > max_freq) {
            // Remove this node
            if (prev) {
                prev->next = current->next;
            } else {
                queue->head = current->next;
            }
            
            if (current == queue->tail) {
                queue->tail = prev;
            }
            
            SineWaveNode* to_delete = current;
            current = current->next;
            free(to_delete);
            removed_count++;
        } else {
            prev = current;
            current = current->next;
        }
    }
    
    queue->count -= removed_count;
    printf("Frequency filtering: Removed %d components outside %g-%g Hz range\n", 
           removed_count, min_freq, max_freq);
}

void apply_amplitude_filtering(SineWaveQueue* queue, float threshold) {
    if (!queue) return;
    
    int threshold_scaled = (int)(threshold * 1000);  // Match our amplitude scaling
    SineWaveNode* current = queue->head;
    SineWaveNode* prev = NULL;
    int removed_count = 0;
    
    while (current) {
        if (current->wave.amplitude < threshold_scaled) {
            // Remove this node
            if (prev) {
                prev->next = current->next;
            } else {
                queue->head = current->next;
            }
            
            if (current == queue->tail) {
                queue->tail = prev;
            }
            
            SineWaveNode* to_delete = current;
            current = current->next;
            free(to_delete);
            removed_count++;
        } else {
            prev = current;
            current = current->next;
        }
    }
    
    queue->count -= removed_count;
    printf("Amplitude filtering: Removed %d low-amplitude components\n", removed_count);
}

void apply_phase_optimization(SineWaveQueue* queue, float tolerance) {
    if (!queue) return;
    
    int removed_count = 0;
    SineWaveNode* current = queue->head;
    
    while (current) {
        SineWaveNode* compare = current->next;
        SineWaveNode* prev_compare = current;
        
        while (compare) {
            // Check for opposite phase waves at same frequency
            if (current->wave.frequency == compare->wave.frequency &&
                fabsf(current->wave.start_time - compare->wave.start_time) < 0.001f) {
                
                int phase_diff = abs(current->wave.phase - compare->wave.phase);
                if (phase_diff > 180) phase_diff = 360 - phase_diff;
                
                // If phases are nearly opposite (around 180 degrees)
                if (abs(phase_diff - 180) < (180 * tolerance)) {
                    // Remove the weaker component
                    if (current->wave.amplitude > compare->wave.amplitude) {
                        // Remove compare node
                        prev_compare->next = compare->next;
                        if (compare == queue->tail) {
                            queue->tail = prev_compare;
                        }
                        free(compare);
                        compare = prev_compare->next;
                        removed_count++;
                        continue;
                    }
                }
            }
            
            prev_compare = compare;
            compare = compare->next;
        }
        current = current->next;
    }
    
    queue->count -= removed_count;
    if (removed_count > 0) {
        printf("Phase optimization: Removed %d opposite-phase components\n", removed_count);
    }
}

void apply_similarity_filtering(SineWaveQueue* queue, float threshold) {
    if (!queue) return;
    
    int removed_count = 0;
    SineWaveNode* current = queue->head;
    
    while (current) {
        SineWaveNode* compare = current->next;
        SineWaveNode* prev_compare = current;
        
        while (compare) {
            // Calculate similarity between waves
            float freq_diff = fabsf((float)current->wave.frequency - compare->wave.frequency);
            float amp_diff = fabsf((float)current->wave.amplitude - compare->wave.amplitude);
            float time_diff = fabsf(current->wave.start_time - compare->wave.start_time);
            
            // Normalize differences
            float freq_sim = 1.0f - (freq_diff / fmaxf((float)current->wave.frequency, (float)compare->wave.frequency));
            float amp_sim = 1.0f - (amp_diff / fmaxf((float)current->wave.amplitude, (float)compare->wave.amplitude));
            float time_sim = time_diff < 0.1f ? 1.0f : 0.0f;  // Must be very close in time
            
            float overall_similarity = (freq_sim + amp_sim + time_sim) / 3.0f;
            
            if (overall_similarity > threshold) {
                // Merge or remove the weaker component
                if (current->wave.amplitude >= compare->wave.amplitude) {
                    // Remove compare node
                    prev_compare->next = compare->next;
                    if (compare == queue->tail) {
                        queue->tail = prev_compare;
                    }
                    free(compare);
                    compare = prev_compare->next;
                    removed_count++;
                    continue;
                }
            }
            
            prev_compare = compare;
            compare = compare->next;
        }
        current = current->next;
    }
    
    queue->count -= removed_count;
    if (removed_count > 0) {
        printf("Similarity filtering: Merged %d similar components\n", removed_count);
    }
}
