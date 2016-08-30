/* This was written by Jamal Natour
 * Copyright 2000, and released under GPL license */

/*
Gen-Java is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Gen-Java is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/* util.c */
#include "gen-ruby.h"
int 
ask_user_for(const char *msg, const char *opts)
{
int ret=0;
char buf[3] = {0};
	if(!msg || !opts)
		return 0;
        
    	do
   	{
       		message(msg);
		fflush(stdin);
		ret = getc(stdin);
		if(ret == EOF)
			return -1;
		getc(stdin);
    	}while( -2 == (ret = utility_index_of(toupper(buf[0]),opts)) );
    
	if(0>ret)
		return -1;

    	return opts[ret];
}

int
silent_output_func(const char *str)
{
int ret=0;
	errno = EINVAL;
	if(!str)
		return -1;
    	
	if( !is_silent() )
		ret = normal_output_func(str);
	
	return ret;
}

int
normal_output_func(const char *str)
{
int ret=0;
int err;
	errno = EINVAL;
	if(!str)
		return -1;
	
	ret = puts(str);

	err = errno;
	fflush(stdout);
	errno = err;

	return ret;
}
