
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "md_string.h"

///////////////////////
// Private Interface //
///////////////////////

const uint64_t kMDStringDefaultCapacity = 256;

struct md_string_s {
  uint8_t* buffer;
  uint64_t count;
  uint64_t capacity;
};

static uint64_t helper_round_up_to_nearest_power_of_two(uint64_t value);

///////////////////////////
// Public Implementation //
///////////////////////////

md_string_ref md_string_alloc()
{
  md_string_ref s = (md_string_ref)malloc(sizeof(struct md_string_s));
  
  s->count = 0;
  s->capacity = kMDStringDefaultCapacity;
  s->buffer = (uint8_t*)malloc(sizeof(uint8_t)*s->capacity);
  
  return s;
}

md_string_ref md_string_allocwithcstring(const char* string)
{
  md_string_ref s = md_string_alloc();
  md_string_appendcstring(s, string);
  return s;
}

md_string_ref md_string_allocwithstring(md_string_ref s)
{
  assert(NULL != s);
  md_string_ref new_s = md_string_alloc();
  md_string_appendstring(new_s, s);
  return new_s;
}

void md_string_destroy(md_string_ref s)
{
  assert(NULL != s);
  free(s->buffer);
  free(s);
}

void md_string_appendstring(md_string_ref s1, md_string_ref s2)
{
  assert(NULL != s1);
  assert(NULL != s2);
  
  uint64_t new_len = s1->count + s2->count;
  if (s1->capacity < new_len) {
    do {
      s1->capacity <<= 1;
    } while (s1->capacity < new_len);
    s1->buffer = (uint8_t*)realloc(s1->buffer, s1->capacity);
  }
  
  memmove(s1->buffer + s1->count, s2->buffer, s2->count);
  s1->count = new_len;
}

void md_string_appendcstring(md_string_ref s, const char* string)
{
  assert(NULL != s);
  size_t len = strlen(string);
  size_t new_len = len + s->count;
  
  if (s->capacity < new_len) {
    do {
      s->capacity <<= 1;
    } while (s->capacity < new_len);
    s->buffer = (uint8_t*)realloc(s->buffer, s->capacity);
  }
  
  memmove(s->buffer + s->count, string, len);
  s->count = new_len;
}

void md_string_appendformat(md_string_ref s, const char* format, ...) {
  assert(NULL != s);
  
  char* cstr = (char*)malloc(5196);
  
  va_list arg_list;
  va_start(arg_list, format);
  vsprintf(cstr,format,arg_list);
  va_end(arg_list);
  
  md_string_appendcstring(s, cstr);
  free(cstr);
}

char* md_string_cstring(md_string_ref s)
{
  assert(NULL != s);
  char* result = (char*)malloc(sizeof(char)*s->count);
  return (char*)memmove(result, s->buffer, sizeof(char)*s->count);
}

uint64_t md_string_length(md_string_ref s)
{
  assert(NULL != s);
  return s->count;
}

////////////////////////////
// Private Implementation //
////////////////////////////

static uint64_t helper_round_up_to_nearest_power_of_two(uint64_t value)
{
  return 1 << ((uint64_t)ceil(log2((double)value)));
}
