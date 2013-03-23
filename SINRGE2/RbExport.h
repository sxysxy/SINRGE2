#ifndef __SIN_RB_EXPORT_H__
#define __SIN_RB_EXPORT_H__
extern "C"
{
#include "ruby.h"
#include "ruby/encoding.h"
}
#include "Kconv.h"
#include <string>

namespace Sin
{
	typedef VALUE(*RbFunc)(...);

	extern VALUE rb_mSin;
	
	void InitRbGlobal();
	void InitNGE();
	void InitSeal();

	#ifdef __cplusplus
	extern "C"
	{
	#endif	//	__cplusplus

	void	Init_zlib();

	#ifdef __cplusplus
	}
	#endif	//	__cplusplus

}

#endif	//	__SIN_RB_EXPORT_H__
