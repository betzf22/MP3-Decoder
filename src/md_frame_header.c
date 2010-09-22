
#include <stdlib.h>
#include <assert.h>

#include "md_frame_header.h"
#include "utility/md_string.h"

///////////////////////
// Private Interface //
///////////////////////

struct md_frameheader_s {
  MPEGAudioVersionID audio_version;
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
  
  uint8_t first_byte = bytes[3];
  uint8_t second_byte = bytes[2];
  uint8_t third_byte = bytes[1];
  uint8_t fourth_byte = bytes[0];
  
  // check for sync byte
  if (first_byte != 255 && ((second_byte & 224) >> 5) != 7) {
    return 0;
  }
  
  // get audio version id
  header->audio_version = (second_byte & 24) >> 3;
  // return if we have reserved
  if (header->audio_version == kMPEGAudioVersionReserved) {
    return 0;
  }
  
  // get layer
  header->layer = (second_byte & 6) >> 1;
  // return if we have reserved layer
  if (header->layer == kMPEGLayerReserved) {
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
MPEGAudioVersionID md_frameheader_versionid(md_frameheader_ref header)
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
  
  md_string_appendformat(s, "MPEG Version: %s,\n", MPEGAudioVersionNames[header->audio_version]);
  md_string_appendformat(s, "MPEG Layer: %s,\n", MPEGLayerNames[header->layer]);
  md_string_appendformat(s, "Bitrate: %i,\n", header->bitrate);
  md_string_appendformat(s, "Sampling Frequency: %i,\n", header->sampling_frequency);
  md_string_appendformat(s, "Channel Mode: %s,\n", MPEGChannelModeNames[header->channel_mode]);
  md_string_appendformat(s, "Has CRC: %s,\n", header->has_crc? "YES" : "NO");
  md_string_appendformat(s, "Has Padding: %s,\n", header->has_padding? "YES" : "NO");
  
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
  case kMPEGAudioVersionID1:
    switch(header->layer) {
    case kMPEGLayerLayer1:
      return bitrate_index_table[0][bitrate_index];
    case kMPEGLayerLayer2:
      return bitrate_index_table[1][bitrate_index];
    case kMPEGLayerLayer3:
      return bitrate_index_table[2][bitrate_index];
    default:
      return -1;
    }
  case kMPEGAudioVersionID2:
  case kMPEGAudioVersionID2_5:
    switch(header->layer) {
    case kMPEGLayerLayer1:
      return bitrate_index_table[3][bitrate_index];
      break;
    case kMPEGLayerLayer2:
    case kMPEGLayerLayer3:
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
  case kMPEGAudioVersionID1:
    return sampling_rate_frequency_index_table[0][bitrate_index];
  case kMPEGAudioVersionID2:
    return sampling_rate_frequency_index_table[1][bitrate_index];
  case kMPEGAudioVersionID2_5:
    return sampling_rate_frequency_index_table[2][bitrate_index];
  default:
    return -1;
  }
}

//////////////////////
// Public Constants //
//////////////////////

const char* MPEGAudioVersionNames[4] = {
  "kMPEGAudioVersionID2_5",
  "kMPEGAudioVersionReserved",
  "kMPEGAudioVersionID2",
  "kMPEGAudioVersionID1"
};

const char* MPEGLayerNames[4] = {
  "kMPEGLayerReserved",
  "kMPEGLayerLayer3",
  "kMPEGLayerLayer2",
  "kMPEGLayerLayer1"
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
