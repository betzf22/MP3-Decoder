
#ifndef MD_STRING_H_
#define MD_STRING_H_

#include <stdint.h>

//////////////////////
// Public Interface //
//////////////////////

/**
 * A simple string object implemented internally as a dynamic array.
 */
typedef struct md_string_s* md_string_ref;

/**
 * @returns a new empty malloced md_string.
 */
md_string_ref md_string_alloc();

/**
 * @param string cstring whose contents you want the md_string to be
 * initialized to.
 * @returns a new malloced md_string whose contents are a copy of string.
 */
md_string_ref md_string_allocwithcstring(const char* string);

/**
 * Copy Constructor
 * @param s string to copy.
 * @returns a new malloced string with the same contents as s.
 */
md_string_ref md_string_allocwithstring(md_string_ref s);

/**
 * Destructor
 */
void md_string_destroy(md_string_ref s);

/**
 * Appends the contents of s2 to the contents of s1.
 */
void md_string_appendstring(md_string_ref s1, md_string_ref s2);

/**
 * Appends string to the contents of s1.
 */
void md_string_appendcstring(md_string_ref s, const char* string);

void md_string_appendformat(md_string_ref s, const char* format, ...);

/**
 * @returns a cstring copy of the internal contents of s.
 */
char* md_string_cstring(md_string_ref s);

/**
 * @returns length of s.
 */
uint64_t md_string_length(md_string_ref s);

#endif // MD_STRING_H_
