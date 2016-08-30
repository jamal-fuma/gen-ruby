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

/* class-decl.c */
int 
class_init(const char *name,struct var *vars, int count)
{
char *p;
	TRACE_ME();
	errno = EINVAL;
	if(!name || !vars)
		return -1;

	if(!(p = utility_sprintf("Init_%s",name)))
		return -1;

	/* declare initialisation function */
	func_prolog("void",p,"void");
	free(p);
	p = NULL;

	/* declare the wrapper class */
	class_decl(name);
	/* declare the error that the wrapper class throws */
	class_error_decl(name);	

	/* declare a constructor for the class */
	class_method_decl(name,"initialize",name,"_init","N_RUBY_ARGS_0");

	if(vars && count >0)   
	{
		int x;
		newline();
		tabbed_line_comment("Accessor/Mutators for member vars");
		for (x=0; x < count; x++)
		{
			class_accessor(name,vars[x].name,vars[x].name,"N_RUBY_ARGS_0");
			class_mutator(name,vars[x].name,vars[x].name,"N_RUBY_ARGS_1");
		}
	}

	func_epilog();
	
	return 0;
}

int
class_methods(const char *cls_name,struct var *p, int count)
{
int cnt;
int ret =0;

	TRACE_ME();
	if(count <= 0)
		return 0;

	/* print accessors/ mutators */
	for(cnt =0; cnt < count; cnt++)
	{
		if(TYPE_LONG == (p[cnt].type) )
		{
			long_accessor(cls_name, p[cnt].name );
			long_mutator(cls_name, p[cnt].name );
		}

		else if( TYPE_STRING == p[cnt].type)
		{
			string_accessor(cls_name, p[cnt].name);
			string_mutator(cls_name, p[cnt].name);
		}

	}

	return ret;	
}

int 
string_accessor(const char *cls_name,const char *name)
{
	TRACE_ME();
	return class_method(cls_name,name,TYPE_STRING,ACCESSOR);
}

int
string_mutator(const char *cls_name,const char *name)
{
	TRACE_ME();
	return class_method(cls_name,name,TYPE_STRING,MUTATOR);
}

int
long_accessor(const char *cls_name,const char *name)
{
	TRACE_ME();
	return class_method(cls_name,name,TYPE_LONG,ACCESSOR);
}

int
long_mutator(const char *cls_name,const char *name)
{
	TRACE_ME();
	return class_method(cls_name,name,TYPE_LONG,MUTATOR);
}

int
class_method(const char *cls_name,const char *name, int ret_type, int type)
{        
static const int IMPLEMENT_PROPER_GENERATION_OF_TYPE_MACROS=1;
int ret=0;
char *p;
	TRACE_ME();
	errno = EINVAL;
	if( ret_type < 0 || ret_type > NO_TYPES || !name)
		return -1;

	/* these implement the get/set functions on the C side */
	if(ACCESSOR == type)
	{
		if(!(p = utility_sprintf("rb_%s_get_%s",cls_name,name)))
			return -1;
	
		func_prolog("static VALUE",p,"VALUE self");
		free(p);
		p = NULL;
		
		RUBY_DECL_TYPE_PTR(cls_name);

		/* marshal the C data type for ruby's usage */
		RUBY_RETURN_INT(name);
		
		func_epilog();
	}
	else if(MUTATOR == type)
	{
		if(!(p = utility_sprintf("rb_%s_set_%s",cls_name,name)))
			return -1;
		/* declare initialisation function */
		func_prolog("static VALUE",p,"VALUE self, VALUE v");
		free(p);
		p = NULL;
		RUBY_DECL_TYPE_PTR(cls_name);
		
		RUBY_ASSIGN_INT(name);
		
		/* marshal the C data type for ruby's usage */
		RUBY_RETURN_INT(name);
		
		func_epilog();
	}

	return ret;
}

int
class_source(const char *name, struct var *vars, int count) 
{   
char *p;
	TRACE_ME();
	errno = EINVAL;
	if(!name ||!(p = utility_sprintf("%s_ext.c",name)))
		return -1;

	/* write extension name */	
	line_comment(p);
	free(p);
	p = NULL;

	/* write out the variables that hold ruby side references */
	line_comment("references to Klass objects");
	printf("%s rb_c%s ;\n","static VALUE",name);
	printf("%s rb_e%sError ;\n\n","static VALUE",name);

	/* declare the helper macros used by the class methods */
	if(0>declare_unpack_macro(name))
		return -1;
	
	/* declare the extension initialisation function */
	if(0>class_init(name,vars,count))
		return -1;

	/* declare the accessor/mutator implementations */
	if( (vars && count >0) && (0>class_methods(name,vars,count)) )
		return -1;

	return 0;
}

int
func_prolog(const char *ret_type,const char *func_name, const char *func_args)
{
int ret = 0;
	TRACE_ME();
	errno = EINVAL;
	if(!ret_type  || !func_name || !func_args)
		return -1;
	
	
	/* declare a function */
	line_comment(func_name);
	newline();
	printf("%s",ret_type);	
	newline();
	printf("%s",func_name);
	printf("(%s)",func_args);
	newline();
	left_brace();   
	return ret;
}

int	
class_decl(const char *name)
{	
int ret = 0;
	TRACE_ME();
	errno = EINVAL;
	if(!name)
		return -1;
	
	/* declare a class */
	newline();
	class_header(name);
	tab();
	printf("rb_c%s",name);
	printf(" = rb_define_class(\"%s\",rb_cObject);",name);
	newline();
	return ret;
}

int	
class_error_decl(const char *name)
{	
int ret = 0;
	TRACE_ME();
	errno = EINVAL;
	if(!name)
		return -1;
	
	/* declare a error class */
	newline();
	class_header(name);
	tab();
	printf("rb_e%sError",name);
	printf(" = rb_define_class(\"%sError\",rb_eStandardError);",name);
	newline();
	return ret;
}
int	
class_method_decl(const char *cls,const char *rb_name, 
		  const char *c_name, const char *suffix,const char *nargs)
{	
int ret = 0;
	TRACE_ME();
	errno = EINVAL;
	if(!cls || !rb_name || !c_name || !nargs)
		return -1;

	tab();
	printf("rb_define_method(rb_c%s,\"%s\",rb_%s",cls,rb_name,c_name);
	if(suffix)
		printf("%s",suffix);
	printf(",%s);" ,nargs);
	newline();
	return ret;
}

int	
class_accessor(const char *cls,const char *rb_name, 
	       const char *suffix,const char *nargs)
{	
int ret = 0;
	TRACE_ME();
	errno = EINVAL;
	if(!cls || !rb_name || !nargs)
		return -1;

	tab();
	printf("rb_define_method(rb_c%s,\"%s\",rb_%s_get_",cls,rb_name,cls);
	if(suffix)
		printf("%s",suffix);
	printf(",%s);" ,nargs);
	newline();
	return ret;
}

int	
class_mutator( const char *cls,const char *rb_name,const char *suffix,const char *nargs)
{	
int ret = 0;
	TRACE_ME();
	errno = EINVAL;
	if(!cls || !rb_name || !nargs)
		return -1;

	tab();
	printf("rb_define_method(rb_c%s,\"%s=\",rb_%s_set_",cls,rb_name,cls);
	if(suffix)
		printf("%s",suffix);
	printf(",%s);" ,nargs);
	newline();
	return ret;
}


