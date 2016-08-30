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
#include "gen-ruby.h"
#include <unistd.h>
static char *g_types[] = {"","long","String","void"};
static const char  *g_app_name = NULL;

int 
is_silent(void)
{
    return !isatty(STDIN_FILENO);
}

const char * 
get_app_name(void)
{
    return g_app_name;
}

const char * 
set_app_name(const char *p)
{
    return g_app_name = p;
}

const char * 
get_type_name(int type)
{
    if(type >= 0 && type < (sizeof(g_types)/sizeof(*g_types)))
        return g_types[type] ;
    
    return NULL;
}
