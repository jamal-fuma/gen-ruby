#include "gen-ruby.h"
int
declare_unpack_macro(const char *name)
{
char *upcase_name;

	errno = EINVAL;
	if(!name)
		return -1;

	upcase_name = utility_strndup_upcase(name,strlen(name));
	if(!upcase_name)
		return -1;

	line_comment("Structure Access macros");
	printf("#define RB_%s_RAISE(args)",upcase_name);
	tab();
	printf("rb_raise(rb_e%sError,args)",name);
	newline();

	printf("#define GET_RB_%s(obj,ptr)\t%s",upcase_name,S_INDENT);
	newline();
	printf("do");
	tab();
	printf("%s",INDENT);
	newline();
	printf("{");
	tab();
	printf("%s",INDENT);
	newline();
	tab();
	printf("TRACE_ME;");
	tab();
	tab();
	printf("%s",S_INDENT);
	newline();
	tab();
	printf("Data_Get_Struct(obj,%s,ptr);",upcase_name);
	tab();
	printf("%s",D_INDENT);
	tab();
	printf("if(!ptr)");
	tab();
	tab();
	printf("%s",S_INDENT);
	newline();
	tab();
	tab();
	printf("RB_%s_RAISE(\"%s Structure unallocated\");",upcase_name,name);
	tab();
	printf("\\");
	newline();
	printf("}while(0)");
	newline();
	newline();

	free(upcase_name);	
	return 0;
}


char *
unpack_macro_ptr(const char *name)
{
char *ret;
char *upcase_name;
int err;
	errno = EINVAL;
	if(!name)
		return  NULL;

	upcase_name = utility_strndup_upcase(name,strlen(name));
	if(!upcase_name)
		return  NULL;
	
	ret = utility_sprintf("GET_RB_%s(self,ptr)",upcase_name);
	err = errno;
	free(upcase_name);
	errno = err;
	return ret;
}
