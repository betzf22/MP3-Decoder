#ifndef MD_FRAME_H_
#define MD_FRAME_H_

#include <stdbool.h>
#include <stdint.h>

#include "md_frame_header.h"

typedef struct md_frame_s* md_frame_ref;

md_frame_ref md_frame_alloc();
void md_frame_destroy(md_frame_ref f);

/**
 * Initializes a frame from the buffer data of size data_len.
 * @returns number of bytes read.
 */
uint64_t md_frame_initwithdata(md_frame_ref f, uint8_t* data, uint64_t data_len);

/**
 * @returns frame header for f.
 */
md_frameheader_ref md_frame_header(md_frame_ref f);

/**
 * @returns waveform data for f as pcm int16_t data.
 */
int16_t* md_frame_waveform(md_frame_ref f);

/**
 * @returns length of the waveform data in f
 */
uint64_t md_frame_waveformlen(md_frame_ref f);

/**
 * @returns true if f has been initialized.
 */
bool md_frame_isinitialized(md_frame_ref f);

#endif // MD_FRAME_H_
