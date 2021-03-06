#ifndef MD_FRAMEHEADER_H_
#define MD_FRAMEHEADER_H_

#include <stdint.h>
#include <stdbool.h>

///////////////////////////////////
// Public Data Type Declarations //
///////////////////////////////////

typedef enum {
  kMPEGAudioVersion2_5,
  kMPEGAudioVersionReserved,
  kMPEGAudioVersion2,
  kMPEGAudioVersion1
} MPEGAudioVersion;

typedef enum {
  kMPEGLayerReserved,
  kMPEGLayer3,
  kMPEGLayer2,
  kMPEGLayer1
} MPEGLayer;

typedef enum {
  kMPEGChannelModeStereo,
  kMPEGChannelModeJointStereo,
  kMPEGChannelModeDualMono,
  kMPEGChannelModeMono
} MPEGChannelMode;

extern const char* MPEGAudioVersionNames[4];
extern const char* MPEGLayerNames[4];
extern const char* MPEGChannelModeNames[4];

typedef struct md_frameheader_s* md_frameheader_ref;

////////////////////
// Public Methods //
////////////////////

// Alloc/Destroy
md_frameheader_ref md_frameheader_alloc();
void md_frameheader_destroy(md_frameheader_ref header);

// Initialize Header
// returns number of bytes read
uint64_t md_frameheader_initwithbytes(md_frameheader_ref header, uint8_t* bytes, uint64_t num_bytes);

// Get Info From Header
MPEGAudioVersion md_frameheader_versionid(md_frameheader_ref header);
MPEGLayer md_frameheader_layer(md_frameheader_ref header);
int32_t md_frameheader_bitrate(md_frameheader_ref header);
int32_t md_frameheader_samplingfrequency(md_frameheader_ref header);
MPEGChannelMode md_frameheader_channelmode(md_frameheader_ref header);
bool md_frameheader_hascrc(md_frameheader_ref header);
char* md_frameheader_description(md_frameheader_ref header);

#endif // MD_FRAMEHEADER_H_
