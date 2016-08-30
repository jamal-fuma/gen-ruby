#include "../include/util.h"

static VALUE rb_cBlah;
static VALUE rb_eBlahError;

struct blah
{
	int32_t aint;
};

typedef struct blah BLAH;

#define RB_BLAH_RAISE(args) rb_raise(rb_eBlahError,args)

#define GET_RB_BLAH(obj,ptr)					    \
	do							    \
	{							    \
		TRACE_ME;					    \
		Data_Get_Struct(obj,BLAH,ptr); 	   	    	    \
		if(!ptr)					    \
			RB_BLAH_RAISE("Structure Uninitialised");   \
	}while(0)


void Init_struct(void);
static VALUE rb_blah_init(VALUE self);
static VALUE rb_blah_alloc(VALUE self);
static void rb_blah_mark(BLAH *ptr);
static void rb_blah_free(BLAH *ptr);
static VALUE rb_blah_get_aint(VALUE self);
static VALUE rb_blah_set_aint(VALUE self,VALUE v);

void
Init_struct(void)
{
	TRACE_ME;

	rb_cBlah 	= rb_define_class("Blah",rb_cObject);
	rb_eBlahError 	= rb_define_class("BlahError",rb_eStandardError);

	rb_define_method(rb_cBlah,"initialize",rb_blah_init,N_RUBY_ARGS_0);
	rb_define_method(rb_cBlah,"aint=",rb_blah_set_aint,N_RUBY_ARGS_1);
	rb_define_method(rb_cBlah,"aint",rb_blah_get_aint,N_RUBY_ARGS_0);
	rb_define_alloc_func(rb_cBlah,rb_blah_alloc);
}

static VALUE
rb_blah_alloc(VALUE self)
{
	TRACE_ME;
	BLAH *ptr = ALLOC(BLAH);
	return Data_Wrap_Struct(self,rb_blah_mark,rb_blah_free,ptr);
}

static void
rb_blah_mark(BLAH *ptr)
{
	TRACE_ME;
	/* tell ruby about any VALUEs in structure */
}

static void
rb_blah_free(BLAH *ptr)
{
	TRACE_ME;
	/* release storage for structure */
	free(ptr);
	ptr = NULL;
}


static VALUE
rb_blah_init(VALUE self)
{
BLAH *ptr;
	/* get the structure */
	GET_RB_BLAH(self,ptr);

	ptr->aint = 0;

	return self;
}

static VALUE
rb_blah_get_aint(VALUE self)
{
BLAH *ptr;
	/* get the structure */
	GET_RB_BLAH(self,ptr);
	
	/* convert to ruby fixnum */
	return INT2NUM(ptr->aint);
}

static VALUE
rb_blah_set_aint(VALUE self,VALUE v)
{
BLAH *ptr;

	/* get the structure */
	GET_RB_BLAH(self,ptr);
	
	/* can we convert from ruby fixnum, this also prevents overflowing an int32_t */
	if(!FIXNUM_P(v))
		RB_BLAH_RAISE("v must be a fixnum");
	
	/* convert from ruby fixnum */
	ptr->aint = NUM2INT(v);

	/* convert to ruby fixnum */
	return INT2NUM(ptr->aint);
}
