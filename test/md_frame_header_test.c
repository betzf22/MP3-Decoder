
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <sys/stat.h>

#include "md_frame_header.h"

int64_t get_file_size(const char* file);
bool read_file(const char* file, uint8_t** data, uint64_t* data_count);

const char* TARGET_FILE = "test.mp3";

int main(int argc, char* argv[])
{ 
  uint8_t* data;
  uint64_t data_count;
  
  bool result = read_file(TARGET_FILE, &data, &data_count);
  if (!result) {
    return -1;
  }
  
  md_frameheader_ref header = md_frameheader_alloc();
  
  md_frameheader_initwithbytes(header, data, data_count);
  
  char* description = md_frameheader_description(header);
  printf("%s\n", description);
  free(description);
  
  md_frameheader_destroy(header);
  free(data);
  
  return 0;
}

bool read_file(const char* file, uint8_t** data, uint64_t* data_count)
{
  int64_t file_size = get_file_size(file);
  if (file_size == -1) {
    return false;
  }
  
  uint8_t* buf = (uint8_t*)malloc(sizeof(uint8_t)*file_size);
  
  FILE* f = fopen(file, "r");
  fread(buf, 1, file_size, f);
  fclose(f);
  
  *data = buf;
  *data_count = file_size;
  return true;
}

int64_t get_file_size(const char* file)
{
  struct stat buffer;
  
  if (stat(file, &buffer) == -1) {
    return -1;
  } else {
    return (int64_t)buffer.st_size;
  }
}
