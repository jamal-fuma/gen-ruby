/* This was written by Jamal Natour
 * Copyright 2004, and released under GPL license */

/*
libUtility is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

libUtility is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef LIB_UTILITY_H
#define LIB_UTILITY_H

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>

/* These functions should return <0 on failure and >=0 on success */
typedef int (*output_func_t)(const char *str);

/* Returns index of first occurence of ch in the string 's'
 * Returns -1 on error setting errno
 * Returns index of first occurence of ch on success or -2 if ch is not present
 * EINVAL: invalid args passed*/
int utility_index_of(int ch,const char *s);

/* Replace every occurence of 'old' with 'new' in the string 's'
 *
 * Returns -1 on error setting errno
 * Returns  0 on success 
 * EINVAL: invalid args passed*/
int utility_replace_ch(char *p, int old, int new);

/* Strndup for systems that don't have it,
 * uses host implementation if natively supported
 * otherwise uses PGC's lean mean implementation
 * Returns NULL on error setting errno
 * Returns pointer to allocated copy of 's'  on success, the caller must free this memory 
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reason as malloc()*/
char *utility_strndup(const char *s, int len);

/* Resize a chunk of memory obtained by a previous call to malloc()	
 * The behaviour is different to stdlib realloc in that 'old' is always freed
 * Null pointers and zero sizes are not supported, use malloc/free directly if that behaviour is 
 * ... desired.
 * This means that p = utility_realloc(p,size) is safe while as we all know
 * 		   p = realloc(p,size)      is a leak waiting to happen 
 * On success: Returns a pointer to size bytes of uninitialized memory freeing 'old'
 * On Failure: Returns NULL on failure modifiying errno AND freeing old.
 * EINVAL: invalid args passed, 'old' is null or size is '0'
 * Function may fail and set errno for same reason as realloc()*/
void *utility_realloc(void *old,size_t size);

/* Sprintf that allocates enough memory for all passed args
 * Relies on vsnprintf actually being posixly correct, 
 * on some systems (linux libc4) vsnprintf is defined as a sprintf wrapper which makes them broken
 * and this function unsafe, if you have one of thoses system, complain to your vendor
 * Returns NULL on error setting errno
 * Returns pointer to allocated copy of 's'  on success, the caller must free this memory 
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reason as malloc()*/
char *utility_vsprintf(const char *fmt, va_list ap);

/* utility_vsprintf wrapper that allocates enough memory for argument list
 * Returns NULL on error setting errno
 * Returns pointer to allocated copy of 's'  on success, the caller must free this memory 
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reason as utility_vsprintf()*/
char *utility_sprintf(const char *fmt,...);

/* Obtains upto len-1 bytes from stdin and nul terminate the result
 * Returns <0 on error setting errno
 * Returns number of bytes read on success,
 * A return value of 0 means EOF was encountered
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reason as fgets()*/
int utility_gets(char *dest, int len);

/* Allow handling of variadic arguments without cluttering code 
 * 'func' is called with a string containing the sprintf'd arguments
 * 'func' is expected to return <0 on failure and >=0 on success
 * Returns <0 on error setting errno
 * Returns return value from 'func' on success,
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reason as utility_sprintf()*/
int utility_output(output_func_t func,const char *fmt,va_list ap);

/* Prints varadic arguments using an output_func and calls utility_gets to get the response
 * 'output_func' is called with a string containing the utility_sprintf'd arguments
 * 'output_func' is expected to return <0 on failure and >=0 on success
 * Returns <0 on error setting errno
 * Returns return value from 'output_func' on success,
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reason as utility_output() or utility_gets*/
int utility_input(output_func_t output_func,char *dest, int len, const char *fmt,...);

/* Printf that uses an output_func to do the write
 * 'output_func' is called with a string containing the utility_sprintf'd arguments
 * 'output_func' is expected to return <0 on failure and >=0 on success
 * Returns <0 on error setting errno
 * Returns return value from 'output_func' on success,
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reason as utility_output()*/
int utility_printf(output_func_t output_func, const char *fmt,...);

/* Call a func for each element in the list
 * returns index of entry that caused func to return non zero
 * returns -1 if the whole list was traversed without func returning non-zero 
 * returns -2 setting errno on error
 * EINVAL: invalid arguments */
int utility_for_each_in_list( void *plist,size_t entry_size, size_t nentries,
	       		      int (*fptr)(void *parg));

/* Downcase a string in place
 * Returns NULL on error setting errno
 * Returns 's' on success
 * EINVAL: invalid args passed*/
char *utility_downcase(char *s);

/* Upcase a string in place
 * Returns NULL on error setting errno
 * Returns 's' on success
 * EINVAL: invalid args passed*/
char *utility_upcase(char *s);

/* Duplicates a string converting to lowercase
 * Returns NULL on error setting errno
 * Returns pointer to allocated downcased copy of 's' on success,
 * the caller must free this memory 
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reasons as 
 * utility_strndup(), utility_downcase() */
char * utility_strndup_downcase(const char *s, int len);

/* Duplicates a string converting to uppercase
 * Returns NULL on error setting errno
 * Returns pointer to allocated uppercased copy of 's' on success,
 * the caller must free this memory 
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reasons as 
 * utility_strndup(), utility_upcase() */
char * utility_strndup_upcase(const char *s, int len);

/* Converts a substring to its integer representation
 * Returns -1 on error setting errno
 * Returns number of unconverted chars on success with numeric value stored in pdest 
 * EINVAL: invalid args passed
 * ERANGE: conversion would over/underflow 
 * Function may fail and set errno for same reasons as 
 * utility_sntol() */
int utility_sntoi(int *pdest,const char *s,int len);

/* Converts a substring to its unsigned integer representation
 * Returns -1 on error setting errno
 * Returns number of unconverted chars on success with numeric value stored in pdest 
 * EINVAL: invalid args passed
 * ERANGE: conversion would over/underflow 
 * Function may fail and set errno for same reasons as 
 * utility_sntoul() */
int utility_sntoui(unsigned int *pdest,const char *s,int len);

/* Converts a substring to its unsigned long integer representation
 * Returns -1 on error setting errno
 * Returns number of unconverted chars on success with numeric value stored in pdest 
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reasons as 
 * utility_strndup(), strtoul() */
int utility_sntoul(unsigned long *pdest,const char *s,int len);

/* Converts a substring to its long integer representation
 * Returns -1 on error setting errno
 * Returns number of unconverted chars on success with numeric value stored in pdest 
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reasons as 
 * utility_strndup(), strtol() */
int utility_sntol(long *pdest,const char *s,int len);

/* Converts a string to its unsigned long integer representation
 * Returns -1 on error setting errno
 * Returns number of unconverted chars on success with numeric value stored in pdest 
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reasons as 
 * utility_sntol() */
int utility_stoul(unsigned long *pdest,const char *s);

/* Converts a string to its long integer representation
 * Returns -1 on error setting errno
 * Returns number of unconverted chars on success with numeric value stored in pdest 
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reasons as 
 * utility_sntol() */
int utility_stol(long *pdest,const char *s);

/* Converts a string to its unsigned integer representation
 * Returns -1 on error setting errno
 * Returns number of unconverted chars on success with numeric value stored in pdest 
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reasons as 
 * utility_sntol() */
int utility_stoui(unsigned int *pdest,const char *s);

/* Converts a string to its integer representation
 * Returns -1 on error setting errno
 * Returns number of unconverted chars on success with numeric value stored in pdest 
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reasons as 
 * utility_sntol() */
int utility_stoi(int *pdest,const char *s);


#endif /* ndef LIB_UTILITY_H */

/* libUtility.c */

