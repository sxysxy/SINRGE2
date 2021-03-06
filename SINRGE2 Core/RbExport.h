/*
** SINRGE2
** Copyright (C) 2013 Syalon, Shy07
** Gernischt@gmail.com
**
** Ruby Export Header
*/
#ifndef __SIN_RB_EXPORT_H__
#define __SIN_RB_EXPORT_H__
#include "ruby.h"
extern "C"
{
#include "ruby/encoding.h"
};
#include "sin_kconv.h"

#define SafeRbClassValue(obj, klass)\
	if (rb_obj_class(obj) != rb_c##klass){\
		rb_raise(rb_eTypeError, "wrong argument type %s (expected SIN::"#klass")",\
			rb_obj_classname(obj));\
	}

#define SafeAudioValue(aud)		SafeRbClassValue(aud,	Audio)
#define SafeColorValue(col)		SafeRbClassValue(col,	Color)
#define SafeToneValue(ton)		SafeRbClassValue(ton,	Tone)
#define SafeFontValue(fnt)		SafeRbClassValue(fnt,	Font)
#define SafeBitmapValue(btm)	SafeRbClassValue(btm,	Bitmap)
#define SafeTableValue(tbl)		SafeRbClassValue(tbl,	Table)
#define SafeViewportValue(vp)	SafeRbClassValue(vp,	Viewport)
#define SafeRectValue(rt)		SafeRbClassValue(rt,	Rect)
#define SafeImageValue(img)		SafeRbClassValue(img,	Image)

#define	SafeFixnumValue(x)\
	if (!FIXNUM_P(x)){\
		rb_raise(rb_eTypeError, "wrong argument type %s (expected Fixnum)",\
			 rb_obj_classname(x));\
	}

#define SafeIntegerValue(i)\
	if (!rb_obj_is_kind_of(i, rb_cInteger)){\
		rb_raise(rb_eTypeError, "wrong argument type %s (expected Integer)",\
			rb_obj_classname(i));\
	}

#define SafeNumericValue(n)\
	if (!rb_obj_is_kind_of(n, rb_cNumeric)){\
		rb_raise(rb_eTypeError, "wrong argument type %s (expected Numeric)",\
			 rb_obj_classname(n));\
	}

#define	SAFE_DELETE(ptr)		{if (ptr) { delete (ptr); (ptr) = NULL; }}
#define SAFE_DELETE_ARRAY(ptr)	{if (ptr) { delete [] (ptr); (ptr) = NULL; }}

#define	RUBY_0				((VALUE)1)			// 1 equal ruby's value 0

#define	Ruby2RbBool(obj)	(RTEST((obj)) ? Qtrue : Qfalse)
#define	C2RbBool(exp)		((exp) ? Qtrue : Qfalse)

#define NUM2FLOAT(n)		float(NUM2DBL(n))

#define RB_SLEEP(milliseconds) do\
{\
	struct timeval st;\
	st.tv_sec = 0;\
	st.tv_usec = milliseconds * 1000;\
	rb_thread_wait_for(st);\
} while (0)

extern	VALUE	rb_cBitmap;
extern	VALUE	rb_cColor;
extern	VALUE	rb_cFont;
extern	VALUE	rb_cRect;
extern	VALUE	rb_cTone;

extern	VALUE	rb_cViewport;

typedef VALUE	(*RbFunc)(...);

extern VALUE	rb_mSin;

extern VALUE	rb_eSinError;

extern "C"	VALUE	mZlib;

extern "C"
{
	void	Init_zlib();
	void	ruby_Init_Fiber_as_Coroutine(void);
	void	Init_nonblock(void);
	void	Init_wait();
};

#endif	//	__SIN_RB_EXPORT_H__
