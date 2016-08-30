#ifndef TYPE_MACROS_H
#define TYPE_MACROS_H

#define RUBY_RETURN_UINT(name)					\
do								\
{								\
	newline();						\
	tab();							\
	line_comment("Wrap requested value in a ruby type ");	\
	tab();							\
	printf("return UINT2NUM(ptr->%s);",name);		\
	newline();						\
}while(0)	

#define RUBY_RETURN_UINT64(name)				\
do								\
{								\
	newline();						\
	tab();							\
	line_comment("Wrap requested value in a ruby type ");	\
	tab();							\
	printf("return ULL2NUM(ptr->%s);",name);		\
	newline();						\
}while(0)	

#define RUBY_RETURN_INT(name)					\
do								\
{								\
	newline();						\
	tab();							\
	line_comment("Wrap requested value in a ruby type ");	\
	tab();							\
	printf("return INT2NUM(ptr->%s);",name);		\
	newline();						\
}while(0)	

#define RUBY_RETURN_INT64(name)					\
do								\
{								\
	newline();						\
	tab();							\
	line_comment("Wrap requested value in a ruby type ");	\
	tab();							\
	printf("return LL2NUM(ptr->%s);",name);			\
	newline();						\
}while(0)	

#define RUBY_ASSIGN_INT(name)					\
do								\
{								\
	newline();						\
	tab();							\
	line_comment("unwrap passed value from a ruby type ");	\
	tab();							\
	printf("ptr->%s = NUM2INT(v);",name);			\
	newline();						\
}while(0)	

#define RUBY_DECL_TYPE_PTR(name)		\
do						\
{						\
char *pm_;					\
	newline();				\
	printf("%s ptr;",name);			\
	newline();				\
        if((pm_ = unpack_macro_ptr(name)))	\
	{					\
		tab();				\
		printf("%s;",pm_);		\
	        free(pm_);			\
		pm_ = NULL;			\
	}					\
	newline();				\
}while(0)

#define RUBY_RETURN_INT8(name) RUBY_RETURN_INT(name)
#define RUBY_RETURN_INT16(name) RUBY_RETURN_INT(name)
#define RUBY_RETURN_INT32(name) RUBY_RETURN_INT(name)

#define RUBY_RETURN_UINT8(name) RUBY_RETURN_UINT(name)
#define RUBY_RETURN_UINT16(name) RUBY_RETURN_UINT(name)
#define RUBY_RETURN_UINT32(name) RUBY_RETURN_UINT(name)

#endif  /* TYPE_MACROS_H */
