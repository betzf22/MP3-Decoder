
#include <stdlib.h>
#include <assert.h>

#include "md_frameheader.h"
#include "utility/md_string.h"

#define ENABLE_LOG 0

#if ENABLE_LOG
  #include <stdio.h>
#define LOG(x, ...) fprintf(stderr, x "\n", ## __VA_ARGS__)
#else
#define LOG(x, ...)
#endif

///////////////////////
// Private Interface //
///////////////////////

struct md_frameheader_s {
  MPEGAudioVersion audio_version;
  MPEGLayer layer;
  int32_t bitrate;
  int32_t sampling_frequency;
  MPEGChannelMode channel_mode;
  bool has_crc;
  bool has_padding;
};

static int32_t bitrate_index_table[5][16];
static int32_t sampling_rate_frequency_index_table[3][4];

int32_t helper_lookup_bitrate_index_in_table(md_frameheader_ref header,
					     int32_t bitrate_index);
int32_t helper_lookup_sampling_frequency(md_frameheader_ref header,
					 int32_t bitrate_index);
///////////////////////////
// Public Implementation //
///////////////////////////

md_frameheader_ref md_frameheader_alloc()
{
  return (md_frameheader_ref)malloc(sizeof(struct md_frameheader_s));
}

void md_frameheader_destroy(md_frameheader_ref header)
{
  assert(NULL != header);
  free(header);
}

// Initialize Header
// returns number of bytes read. 0 if failed.
uint64_t md_frameheader_initwithbytes(md_frameheader_ref header,
				      uint8_t* bytes,
				      uint64_t num_bytes)
{
  assert(NULL != header);
  assert(NULL != bytes);
  assert(num_bytes >= 4);
  
  // look for sync header
  uint64_t offset;
  for(offset = 0; offset < num_bytes-1;offset++) {
    if ((bytes[offset] == 0xFF) && (bytes[offset+1] & 0xE0) == 0xE0) {
      LOG("Found Offset! Offset: %i; Offset Value: %i; Offset Value + 1: %i;", (int)offset, bytes[offset], bytes[offset+1]);
      break;
    }
  }
  
  if (offset == num_bytes - 3) {
    LOG("Error! Failed to find frame sync!");
    return 0;
  }
  
  uint8_t first_byte = bytes[offset];
  uint8_t second_byte = bytes[offset+1];
  uint8_t third_byte = bytes[offset+2];
  uint8_t fourth_byte = bytes[offset+3];
  
  LOG("First Byte: %x", first_byte);
  LOG("Second Byte: %x", second_byte);
  LOG("Third Byte: %x", third_byte);
  LOG("Fourth Byte: %x", fourth_byte);
  
  // get audio version id
  header->audio_version = (second_byte & 24) >> 3;
  // return if we have reserved
  if (header->audio_version == kMPEGAudioVersionReserved) {
    LOG("Invalid MPEG Header! kMPEGAudioVersionReserved is an invalid audio version.");
    return 0;
  }
  
  // get layer
  header->layer = (second_byte & 6) >> 1;
  // return if we have reserved layer
  if (header->layer == kMPEGLayerReserved) {
    LOG("Invalid MPEG Header! kMPEGAudioLayerReserved is an invalid audio layer.");
    return 0;
  }
  
  // check for crc
  header->has_crc = (second_byte & 1) == 1;
  
  // grab bitrate
  int32_t bitrate_index = (int32_t)((third_byte & 240) >> 4);
  header->bitrate = helper_lookup_bitrate_index_in_table(header,
							 bitrate_index);
  
  int32_t sampling_frequency_index = (int32_t)((third_byte & 12) >> 2);
  header->sampling_frequency =
    helper_lookup_sampling_frequency(header,
				     sampling_frequency_index);
  
  header->has_padding = ((third_byte & 2) >> 1);
  
  header->channel_mode = (int32_t)((fourth_byte & 192) >> 6);
  
  return 4;
}

// Get Info From Header
MPEGAudioVersion md_frameheader_versionid(md_frameheader_ref header)
{
  assert(NULL != header);
  return header->audio_version;
}

MPEGLayer md_frameheader_layer(md_frameheader_ref header)
{
  assert(NULL != header);
  return header->layer;
}

int32_t md_frameheader_bitrate(md_frameheader_ref header)
{
  assert(NULL != header);
  return header->bitrate;
}

int32_t md_frameheader_samplingfrequency(md_frameheader_ref header)
{
  assert(NULL != header);
  return header->sampling_frequency;
}

MPEGChannelMode md_frameheader_channelmode(md_frameheader_ref header)
{
  assert(NULL != header);
  return header->channel_mode;
}

bool md_frameheader_hascrc(md_frameheader_ref header) {
  assert(header != NULL);
  return header->has_crc;
}

char* md_frameheader_description(md_frameheader_ref header) {
  md_string_ref s = md_string_alloc();
  
  md_string_appendcstring(s, "{\n");
  
  md_string_appendformat(s, "\tMPEG Version: %s,\n", MPEGAudioVersionNames[header->audio_version]);
  md_string_appendformat(s, "\tMPEG Layer: %s,\n", MPEGLayerNames[header->layer]);
  md_string_appendformat(s, "\tBitrate: %i,\n", header->bitrate);
  md_string_appendformat(s, "\tSampling Frequency: %i,\n", header->sampling_frequency);
  md_string_appendformat(s, "\tChannel Mode: %s,\n", MPEGChannelModeNames[header->channel_mode]);
  md_string_appendformat(s, "\tHas CRC: %s,\n", header->has_crc? "YES" : "NO");
  md_string_appendformat(s, "\tHas Padding: %s,\n", header->has_padding? "YES" : "NO");
  
  md_string_appendcstring(s, "}\n");
  
  char* result = md_string_cstring(s);
  md_string_destroy(s);
  
  return result;
}

////////////////////////////
// Private Implementation //
////////////////////////////

int32_t helper_lookup_bitrate_index_in_table(md_frameheader_ref header, int32_t bitrate_index) {
  
  if (bitrate_index < 0 || bitrate_index > 16) {
    return -1;
  }
  
  switch(header->audio_version) {
  case kMPEGAudioVersion1:
    switch(header->layer) {
    case kMPEGLayer1:
      return bitrate_index_table[0][bitrate_index];
    case kMPEGLayer2:
      return bitrate_index_table[1][bitrate_index];
    case kMPEGLayer3:
      return bitrate_index_table[2][bitrate_index];
    default:
      return -1;
    }
  case kMPEGAudioVersion2:
  case kMPEGAudioVersion2_5:
    switch(header->layer) {
    case kMPEGLayer1:
      return bitrate_index_table[3][bitrate_index];
      break;
    case kMPEGLayer2:
    case kMPEGLayer3:
      return bitrate_index_table[4][bitrate_index];
    default:
      return -1;
    }
  }
}

int32_t helper_lookup_sampling_frequency(md_frameheader_ref header,
					 int32_t bitrate_index)
{
  if (bitrate_index < 0 || bitrate_index > 4) {
    return -1;
  }
  
  switch(header->audio_version) {
  case kMPEGAudioVersion1:
    return sampling_rate_frequency_index_table[0][bitrate_index];
  case kMPEGAudioVersion2:
    return sampling_rate_frequency_index_table[1][bitrate_index];
  case kMPEGAudioVersion2_5:
    return sampling_rate_frequency_index_table[2][bitrate_index];
  default:
    return -1;
  }
}

//////////////////////
// Public Constants //
//////////////////////

const char* MPEGAudioVersionNames[4] = {
  "kMPEGAudioVersion2_5",
  "kMPEGAudioVersionReserved",
  "kMPEGAudioVersion2",
  "kMPEGAudioVersion1"
};

const char* MPEGLayerNames[4] = {
  "kMPEGLayerReserved",
  "kMPEGLayer3",
  "kMPEGLayer2",
  "kMPEGLayer1"
};

const char* MPEGChannelModeNames[4] = {
  "kMPEGChannelModeStereo",
  "kMPEGChannelModeJointStereo",
  "kMPEGChannelModeDualMono",
  "kMPEGChannelModeMono"
};

///////////////////////
// Private Constants //
///////////////////////

static int32_t bitrate_index_table[5][16] = {
  {-1,32,64,96,28,60,92,24,56,88,20,52,84,16,48,-1},
  {-1,32,48,56,64,80,96,112,128,160,192,224,256,320,384,-1},
  {-1,32,40,48,56,64,80,96,112,128,160,192,224,256,320,-1},
  {-1,32,48,56,64,80,96,112,128,144,160,176,192,224,256,-1},
  {-1,8,16,24,32,40,48,56,64,80,96,112,128,144,160,-1}
};

static int32_t sampling_rate_frequency_index_table[3][4] = {
  {44100,48000,32000,-1},
  {22050,24000,16000,-1},
  {11025,12000,8000,-1}
};
