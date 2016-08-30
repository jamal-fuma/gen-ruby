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

#include "libUtility.h"

/* Returns index of first occurence of ch in the string 's'
 * Returns -1 on error setting errno
 * Returns index of first occurence of ch on success or -2 if ch is not present
 * EINVAL: invalid args passed*/
int
utility_index_of(int ch,const char *s)
{
char *pos_of_ch;
	errno = EINVAL;
	if(!s || !ch)
		return -1;
	else
		pos_of_ch = strchr(s, ch);

	return (pos_of_ch) ? (pos_of_ch - s) : -2;
}

/* Prints varadic arguments using an output_func and calls utility_gets to get the response
 * 'output_func' is called with a string containing the utility_sprintf'd arguments
 * 'output_func' is expected to return <0 on failure and >=0 on success
 * Returns <0 on error setting errno
 * Returns return value from 'output_func' on success,
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reason as utility_output() or utility_gets*/
int
utility_input(output_func_t output_func,char *dest, int len, const char *fmt,...)
{
va_list ap;

	if(!fmt || !dest || len<=0 || !output_func)
		return -1;

	va_start(ap,fmt);
	utility_output(output_func,fmt,ap);
	va_end(ap);

	return utility_gets(dest,len);
}

/* Printf that uses an output_func to do the write
 * 'output_func' is called with a string containing the utility_sprintf'd arguments
 * 'output_func' is expected to return <0 on failure and >=0 on success
 * Returns <0 on error setting errno
 * Returns return value from 'output_func' on success,
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reason as utility_output()*/
int
utility_printf(output_func_t output_func, const char *fmt,...)
{
int ret;
va_list ap;

	if(!fmt)
		return 0;

	va_start(ap,fmt);
	ret = utility_output(output_func,fmt,ap);
	va_end(ap);

	return ret;
}

/* utility_vsprintf wrapper that allocates enough memory for argument list
 * Returns NULL on error setting errno
 * Returns pointer to allocated copy of 's'  on success, the caller must free this memory
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reason as utility_vsprintf()*/
char *
utility_sprintf(const char *fmt,...)
{
char *ret;
va_list ap;
	errno = EINVAL;
	if(!fmt)
		return NULL;

	va_start(ap,fmt);
	ret = utility_vsprintf(fmt,ap);
	va_end(ap);

	return ret;

}

/* Sprintf that allocates enough memory for argument list ap
 * Relies on vsnprintf actually being posixly correct,
 * on (linux libc4) vsnprintf is defined as a sprintf wrapper which is broken
 * and makes this function unsafe, if you have that libc, complain to your vendor
 *
 * Returns NULL on error setting errno
 * Returns pointer to allocated copy of 's'  on success, the caller must free this memory
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reason as malloc()*/
char *
utility_vsprintf(const char *fmt, va_list ap)
{
size_t size = 128 ; /* this must be a power of two */
char *buf;
int n;

	/* loosely based on code taken from vprintf man page */
	errno = EINVAL;
	if(!fmt || (!(buf = malloc(size))))
		return NULL;

	/* retry until success or we eat
 	 * all available core */
	for(;;)
	{
		/* rely on oversized buffers to distinguish between complete and
		 * partial success, ignoring the case of a result that exactly fits the
		 * buffer
		 */
		if( 0>(n = vsnprintf(buf, size, fmt, ap)))
		{
			/* something went wrong */
			int err = errno;
			va_end(ap);
			free(buf);
			errno = err;
			return NULL;
		}
		/* result fits in buffer of 'size' bytes */
		else if( n < size)
			return buf;

		/* result is larger then 'size' bytes */
		else
		/* this leaks with realloc but safe for utility_realloc */
		       	if(!(buf = utility_realloc(buf,(size <<= 1))))
				return NULL;
	}
}

/* Strndup for systems that don't have it,
 * uses host implementation if natively supported
 * Returns NULL on error setting errno
 * Returns pointer to allocated copy of 's'  on success, the caller must free this memory
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reason as malloc()*/
char *
utility_strndup(const char *s, int len)
{
#ifndef _GNU_SOURCE
	/* uses pgc implementation */
char *p;
	errno = EINVAL;
	p = (!s)? NULL : malloc( len+1 );

	if(p)
	{
		strncpy(p,s,len);
		p[len] = '\0';
	}

	return p;
#else
	return strndup( s, len );
#endif /* _GNU_SOURCE */
}

/* Replace every occurence of 'old' with 'new' in the string 's'
 * Returns -1 on error setting errno
 * Returns  0 on success
 * EINVAL: invalid args passed*/
int
utility_replace_ch(char *s, int old, int new)
{
	errno = EINVAL;
	if(!s || '\0' == old)
		return -1;

	/* overwrite occurences of old with new */
	for(; (*s); ++s)
		if(*s == old)
			*s = new;
	return 0;
}

/* Resize a chunk of memory obtained by a previous call to malloc()
 * The behaviour is different to stdlib realloc in that 'old' is always freed
 * Null pointers and zero sizes are not supported, use malloc/free directly if that behaviour is
 * ... desired.
 * This means that p = utility_realloc(p,size) is safe, while as we all know
 * 		   p = realloc(p,size)      is a leak waiting to happen
 * On success: Returns a pointer to size bytes of uninitialized memory freeing 'old'
 * On Failure: Returns NULL on failure modifiying errno AND freeing old.
 * EINVAL: invalid args passed, 'old' is null or size is '0'
 * Function may fail and set errno for same reason as realloc()*/
void *
utility_realloc(void *old,size_t size)
{
void *p;
int err;

	errno = EINVAL;
	if(!old || !size)
		return NULL;

	/* old is freed on success */
	if((p = realloc(old,size)))
		return p;

	/* old is not freed on stdlib realloc failure */
	err = errno;
	/* it is now */
	free(old);
	errno = err;
	return p;
}

/* Allow handling of variadic arguments without cluttering code
 * 'func' is called with a string containing the utility_sprintf'd arguments
 * 'func' is expected to return <0 on failure and >=0 on success
 * Returns <0 on error setting errno
 * Returns return value from 'func' on success,
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reason as utility_sprintf()*/
int
utility_output(output_func_t func,const char *fmt,va_list ap)
{
char *p;
int ret;
	errno = EINVAL;
	if(!fmt || !func)
		return -1;

	/* makes sure string is big enough for all args */
	if(!(p = utility_vsprintf(fmt,ap)))
		return -1;

	ret = (*func)(p);

	/* plug the leak */
	free(p);
	p = NULL;

	return ret;
}

/* Obtains upto len-1 bytes from stdin and nul terminate the result
 * Returns <0 on error setting errno
 * Returns number of bytes read on success,
 * A return value of 0 means EOF was encountered
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reason as fgets()*/
int
utility_gets(char *dest, int len)
{
char *p = NULL ;
	errno = EINVAL;
	if(!dest || len <= 0 )
		return -1;

	/* obtain newline terminated input lines */
	fflush(stdin);
	dest[0] = '\0';

	/* destinguish errors and eof */
	if(!fgets(dest,len-1,stdin))
		return (feof(stdin)) ? 0 : -1;

	/* replace newline with nul or nul terminate in the case of truncated input */
	*((!(p = strchr(dest,'\n')))? dest+len : p)  = '\0';

	return 0;
}

/* Call a func for each element in the list
 * returns index of entry that caused func to return non zero
 * returns -1 if the whole list was traversed without func returning non-zero
 * returns -2 setting errno on error
 * EINVAL: invalid arguments */
int
utility_for_each_in_list( void *plist,size_t entry_size, size_t nentries,
			  int (*fptr)(void *parg))
{
void *p;
void *pend;
void *pfound = NULL;
int n=0;

	errno = EINVAL;
	if(!plist | !fptr || !entry_size || !nentries)
		return -2;

	/* need to do the maths ourselves as voids provide no hint as to stride width */
	pend = (uint8_t *)plist+(nentries * entry_size);

	/* call a func for each element in the list or until the func returns 1 */
	for(p = plist; !n &&  p < pend; p = (((uint8_t *)p) + entry_size) )
		if( (n = (*fptr)(p)) )
			pfound = p;

	/* return index causing func to return 1 or -1 if the whole list was traversed */
	return (n) ? (((uint8_t *)pfound - (uint8_t *)plist) / entry_size)  : -1 ;
}

/* Downcase a string in place
 * Returns NULL on error setting errno
 * Returns 's' on success
 * EINVAL: invalid args passed*/
char *
utility_downcase(char *s)
{
char *p;
	errno = EINVAL;
	if(!s)
		return NULL;

	for(p =s; (*p); p++)
		*p = tolower(*p);

	return s;
}

/* Upcase a string in place
 * Returns NULL on error setting errno
 * Returns 's' on success
 * EINVAL: invalid args passed*/
char *
utility_upcase(char *s)
{
char *p;
	errno = EINVAL;
	if(!s)
		return NULL;

	for(p=s; (*p); p++)
		*p = toupper(*p);

	return s;
}

/* Duplicates a string converting to uppercase
 * Returns NULL on error setting errno
 * Returns pointer to allocated uppercased copy of 's' on success,
 * the caller must free this memory
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reasons as
 * utility_strndup(), utility_upcase() */
char *
utility_strndup_upcase(const char *s, int len)
{
char *p;

	/* duplicate, letting strndup do the validation */
	p = utility_strndup(s,len);
	 if(!p)
		return NULL;

	 /* uppercase the allocated string */
 	 if(!utility_upcase(p))
	 {
		/* clean up on error */
 		int err = errno;
		free(p);
		errno = err;
		return NULL;
	 }

	 /* one uppercase copy */
	 return p;
}

/* Duplicates a string converting to lowercase
 * Returns NULL on error setting errno
 * Returns pointer to allocated downcased copy of 's' on success,
 * the caller must free this memory
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reasons as
 * utility_strndup(), utility_downcase() */
char *
utility_strndup_downcase(const char *s, int len)
{
char *p;

	/* duplicate, letting strndup do the validation */
	p = utility_strndup(s,len);
	 if(!p)
		return NULL;

	 /* uppercase the allocated string */
 	 if(!utility_downcase(p))
	 {
		/* clean up on error */
 		int err = errno;
		free(p);
		errno = err;
		return NULL;
	 }

	 /* one lowercase copy */
	 return p;
}

/* Converts a string to its integer representation
 * Returns -1 on error setting errno
 * Returns number of unconverted chars on success with numeric value stored in pdest
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reasons as
 * utility_sntol() */
int
utility_stoi(int *pdest,const char *s)
{
	errno = EINVAL;
	return (!pdest || !s) ? -1 : utility_sntoi(pdest,s,strlen(s));
}
/* Converts a string to its unsigned integer representation
 * Returns -1 on error setting errno
 * Returns number of unconverted chars on success with numeric value stored in pdest
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reasons as
 * utility_sntol() */
int
utility_stoui(unsigned int *pdest,const char *s)
{
	errno = EINVAL;
	return (!pdest || !s) ? -1 : utility_sntoui(pdest,s,strlen(s));
}
/* Converts a string to its long integer representation
 * Returns -1 on error setting errno
 * Returns number of unconverted chars on success with numeric value stored in pdest
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reasons as
 * utility_sntol() */
int
utility_stol(long *pdest,const char *s)
{
	errno = EINVAL;
	return (!pdest || !s) ? -1 : utility_sntol(pdest,s,strlen(s));
}
/* Converts a string to its unsigned long integer representation
 * Returns -1 on error setting errno
 * Returns number of unconverted chars on success with numeric value stored in pdest
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reasons as
 * utility_sntol() */
int
utility_stoul(unsigned long *pdest,const char *s)
{
	errno = EINVAL;
	return (!pdest || !s) ? -1 : utility_sntoul(pdest,s,strlen(s));
}

/* Converts a substring to its long integer representation
 * Returns -1 on error setting errno
 * Returns number of unconverted chars on success with numeric value stored in pdest
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reasons as
 * utility_strndup(), strtol() */
int
utility_sntol(long *pdest,const char *s,int len)
{
char *pend;
char *p;
long ret;
int remaining;
int err;

	errno = EINVAL;
	if(!s || !len || !pdest || (!(p = utility_strndup(s,len))))
		return -1;

	/* need this to tell the difference between error and conversion of zero */
	errno = 0;
	ret = strtol(p, &pend,10);

	/* over/underflow/some other error or no digits, don't care which, as conversion failed */
	if(ERANGE == errno || (errno && !ret) ||(pend == p) )
		goto err_ret;

	/* cleanup taking note of unconverted chars in substring */
	remaining = strlen(pend);
	free(p);

	/* store result and return number of unconverted chars remaining */
	*pdest = ret;
	return remaining;

	/* cleanup and split */
err_ret:
	err = errno;
	free(p);
	errno = err;
	return -1;
}

/* Converts a substring to its unsigned long integer representation
 * Returns -1 on error setting errno
 * Returns number of unconverted chars on success with numeric value stored in pdest
 * EINVAL: invalid args passed
 * Function may fail and set errno for same reasons as
 * utility_strndup(), strtoul() */
int
utility_sntoul(unsigned long *pdest,const char *s,int len)
{
char *pend;
char *p;
unsigned long ret;
int remaining;
int err;

	errno = EINVAL;
	if(!s || !len || !pdest || (!(p = utility_strndup(s,len))))
		return -1;

	/* need this to tell the difference between error and conversion of zero */
	errno = 0;
	ret = strtoul(p, &pend,10);

	/* over/underflow/some other error or no digits, don't care which, as conversion failed */
	if(ERANGE == errno || (errno && !ret) ||(pend == p) )
		goto err_ret;

	/* cleanup taking note of unconverted chars in substring */
	remaining = strlen(pend);
	free(p);

	/* store result and return number of unconverted chars remaining */
	*pdest = ret;
	return remaining;

	/* cleanup and split */
err_ret:
	err = errno;
	free(p);
	errno = err;
	return -1;
}

/* Converts a substring to its unsigned integer representation
 * Returns -1 on error setting errno
 * Returns number of unconverted chars on success with numeric value stored in pdest
 * EINVAL: invalid args passed
 * ERANGE: conversion would over/underflow
 * Function may fail and set errno for same reasons as
 * utility_sntoul() */
int
utility_sntoui(unsigned int *pdest,const char *s,int len)
{
int ret;
unsigned long val;

	errno = EINVAL;
	if(!pdest || !s || !len || (0>(ret =  utility_sntoul(&val,s,len))))
		return -1;

	/* don't truncate make caller be explicit about intentions */
	if(val > UINT_MAX)
	{
		errno = ERANGE;
		return -1;
	}

	*pdest = (unsigned int)val;
	return ret;
}

/* Converts a substring to its integer representation
 * Returns -1 on error setting errno
 * Returns number of unconverted chars on success with numeric value stored in pdest
 * EINVAL: invalid args passed
 * ERANGE: conversion would over/underflow
 * Function may fail and set errno for same reasons as
 * utility_sntol() */
int
utility_sntoi(int *pdest,const char *s,int len)
{
int ret;
long val;

	errno = EINVAL;
	if(!pdest || !s || !len || (0>(ret =  utility_sntol(&val,s,len))))
		return -1;

	/* don't truncate make caller be explicit about intentions */
	if(val < INT_MIN || val > INT_MAX)
	{
		errno = ERANGE;
		return -1;
	}

	*pdest = (int)val;
	return ret;
}
