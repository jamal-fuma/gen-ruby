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

#ifndef GEN_RUBY_H
#define GEN_RUBY_H

#include <errno.h>
/* lib/libutility-0.0.0/src/libUtility.h */
#include "libUtility.h"
#include "type_macros.h"
#define TRACE_ME() fprintf(stderr,"In %s on line %04d \n",__func__,__LINE__)
#undef TRACE_ME
#define TRACE_ME() 

#define ERR_NO_ARGS 1
#define ERR_NO_NAME 2

#define ACCESSOR    1
#define MUTATOR     2
#define CONSTRUCTOR 0

#define TYPE_NONE   0
#define TYPE_LONG   1
#define TYPE_STRING 2
#define TYPE_VOID   3

#define MAX_VARS    5000
#define MAX_NAM_LEN 150
#define MAX_TYPES (NUM_ELEMENTS(g_types))
#define NO_TYPES 4

#define L_BRACE '{'
#define R_BRACE '}'
#define NUM_ELEMENTS(l) (sizeof(l)/sizeof(l[0]))
#define INDENT   "\t\t\t\t\t\t\t\t\\"
#define S_INDENT "\t\t\t\t\t\\"
#define D_INDENT "\t\t\t\\\n"


#define prompt(m,d,l) 	utility_input(silent_output_func,(d),(l),"\n%s : ",(m))
#define message(m)    	utility_printf(silent_output_func,"\n%s : ",(m))
#define left_brace()   	putc(L_BRACE,stdout)
#define right_brace()  	putc(R_BRACE,stdout)
#define newline()      	putc('\n',stdout)
#define tab()          	putc('\t',stdout)
#define line_comment(c)		utility_printf(normal_output_func,"/* %s */",c)
#define tabbed_line_comment(c)	utility_printf(normal_output_func,"\t/* %s */",c)
#define class_header(c)	       utility_printf(normal_output_func,"\t/* class wrapping %s */",c)

#define func_epilog() 		\
do				\
{				\
	right_brace();		\
	newline();		\
	newline();		\
}while(0);

struct var
{
    int type;
    char name[MAX_NAM_LEN+1];
};

const char * get_type_name(int type);

/*main.c*/
int usage(int why);
int unknown_arg(const char *name);

/* class-decl.c */
int class_init(const char *name,struct var *p, int count) ;
int class_variables(const char *name,struct var *vars, int count);
int class_source(const char *cls_name, struct var *vars, int count);
int class_methods(const char *cls_name,struct var *p, int count);
int string_accessor(const char *cls_name,const char *name);
int string_mutator(const char *cls_name,const char *name);
int long_accessor(const char *cls_name,const char *name);
int long_mutator(const char *cls_name,const char *name);
int class_method(const char *cls_name,const char *name, int ret_type, int type);
int func_prolog(const char *ret_type,const char *func_name, const char *func_args);
int class_method_decl(const char *cls,const char *rb_name,
		      const char *c_name,const char *suffix,const char *nargs);
int class_accessor(const char *cls,const char *rb_name, 
		   const char *suffix,const char *nargs);
int class_mutator(const char *cls,const char *rb_name, 
		  const char *suffix,const char *nargs);
int class_error_decl(const char *name);
int class_decl(const char *name);

/* statics.c */
const char *get_type_name(int type);
const char *set_app_name(const char *p);
const char *get_app_name(void);
int is_silent(void);

/* util.c */
int ask_user_for(const char *msg,const char *opts);
int silent_output_func(const char *str);
int normal_output_func(const char *str);

/* macro-decl.c */
int declare_unpack_macro(const char *name);
char *unpack_macro_ptr(const char *name);
#endif /* GEN_RUBY_H */
