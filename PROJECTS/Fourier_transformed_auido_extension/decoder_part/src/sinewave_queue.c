
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
