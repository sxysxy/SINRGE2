/**
 * @file  System.cpp
 * @author Sherry Lynch (or Shy07) <Gernischt@gmail.com>
 * @date 2013/03/12 13:14:54
 *
 *  Copyright  2013  Sherry Lynch (or Shy07) <Gernischt@gmail.com>
 *
 */
#include "RbExport.h"
#include "SINRGE2.h"

using namespace Sin;

namespace
{
	/*
	**	call-seq:
	**		msgbox_p(...)		-> nil
	**
	*/
	static VALUE rdf_msgboxp(int argc, VALUE *argv/*, VALUE caller*/)
	{
		int i;

		VALUE str = rb_str_buf_new(0);

		for (i = 0; i < argc; ++i)
		{
			rb_str_buf_append(str, rb_inspect(argv[i]));
			rb_str_buf_append(str, rb_default_rs);
		}
		
		MessageBoxW(GetHwnd(), Kconv::UTF8ToUnicode(RSTRING_PTR(str)), L"Script", 0);

		return Qnil;
	}
	/*
	**	call-seq:
	**		p(...)		-> nil
	**
	*/
	static VALUE rdf_p(int argc, VALUE *argv/*, VALUE caller*/)
	{
		int i;

		VALUE str = rb_str_buf_new(0);

		for (i = 0; i < argc; ++i)
		{
			rb_str_buf_append(str, rb_inspect(argv[i]));
			rb_str_buf_append(str, rb_default_rs);
		}
		
		printf(Kconv::UTF8ToAnsi(RSTRING_PTR(str)));

		return Qnil;
	}
	/*
	**	call-seq:
	**		msgbox(...)		-> nil
	**
	*/
	static VALUE rdf_msgbox(int argc, VALUE *argv/*, VALUE caller*/)
	{
		int i;

		VALUE str = rb_str_buf_new(0);

		rb_encoding* enc = rb_utf8_encoding();
		for (i = 0; i < argc; ++i)
		{
			rb_str_buf_append(str, rb_enc_associate(NIL_P(argv[i]) ? rb_str_new2("nil") : rb_obj_as_string(argv[i]), enc));
		}
		
		MessageBoxW(GetHwnd(), Kconv::UTF8ToUnicode(RSTRING_PTR(str)), L"Script", 0);

		return Qnil;
	}
	/*
	**	call-seq:
	**		print(...)		-> nil
	**
	*/
	static VALUE rdf_print(int argc, VALUE *argv/*, VALUE caller*/)
	{
		int i;

		VALUE str = rb_str_buf_new(0);

		rb_encoding* enc = rb_utf8_encoding();
		for (i = 0; i < argc; ++i)
		{
			rb_str_buf_append(str, rb_enc_associate(NIL_P(argv[i]) ? rb_str_new2("nil") : rb_obj_as_string(argv[i]), enc));
		}
		
		printf(Kconv::UTF8ToAnsi(RSTRING_PTR(str)));

		return Qnil;
	}

	void InitRubyInnerClassExt()
	{
		rb_define_global_function("msgbox_p",		(RbFunc)rdf_msgboxp,		-1);
		rb_define_global_function("msgbox",			(RbFunc)rdf_msgbox,			-1);
		rb_define_global_function("p",				(RbFunc)rdf_p,				-1);
		rb_define_global_function("print",			(RbFunc)rdf_print,			-1);
		Init_zlib();
	}

	void InitExportSinInterface()
	{
		InitRbGlobal();
		InitNGE();
		InitSeal();
	}
	
	static VALUE __run_sin_in_protect(VALUE argv)
	{
		const ID	id_eval = rb_intern("eval");
		const VALUE	binding = rb_const_get(rb_mKernel, rb_intern("TOPLEVEL_BINDING"));
		const VALUE	lineno  = INT2FIX(1);
		
		const VALUE script	= rb_ary_entry(argv, 0);
		const VALUE name	= rb_ary_entry(argv, 1);

		(void)rb_funcall(rb_mKernel, id_eval, 4, script, binding, name, lineno);

		return Qnil;
	}

	static void __on_failed(VALUE err)
	{
		VALUE errmsg;

		const VALUE message			= rb_funcall(err, rb_intern("message"), 0);
		const VALUE message_str		= rb_funcall(message, rb_intern("gsub"), 2, rb_str_new2("\n"), rb_str_new2("\r\n"));
		const VALUE backtrace		= rb_funcall(err, rb_intern("backtrace"), 0);
		const VALUE backtrace_str	= rb_str_concat(rb_ary_join(backtrace, rb_str_new2("\r\n")), rb_str_new2("\r\n"));

		const char* clsname			= rb_obj_classname(err);
		const char* msg				= RSTRING_PTR(message_str);
		const char* bktr			= RSTRING_PTR(backtrace_str);

		if (rb_obj_is_kind_of(err, rb_eSyntaxError))
			errmsg = rb_sprintf(Kconv::UnicodeToUTF8(L"脚本 '%s' 的 %d 行 发生了 %s。"), rb_sourcefile(), rb_sourceline(), clsname);
		else
			errmsg = rb_sprintf(Kconv::UnicodeToUTF8(L"脚本 '%s' 的 %d 行 发生了 %s。\n\n%s"), rb_sourcefile(), rb_sourceline(), clsname, msg);

		MessageBoxW(GetHwnd(), Kconv::UTF8ToUnicode(RSTRING_PTR(errmsg)), L"SINRGE2 Error",  MB_ICONERROR);
	}

}

void Sin::SINRGE2Initialize()
{
	///<	初始化解释器
	int		argc = 0;
	char**	argv = 0;

	ruby_sysinit(&argc, &argv);
	{
		RUBY_INIT_STACK
		ruby_init();
		ruby_set_argv(argc - 1, argv + 1);
		//	rb_set_kcode("utf8");
		//  ruby_init_loadpath();
		ruby_incpush("./");
		ruby_script("SINRGE2");
	}
	///<	内部类扩展
	InitRubyInnerClassExt();
	///<	导出Sin内部接口
	InitExportSinInterface();
}

int Sin::SINRGE2Eval(const char* script)
{
	int status = -1;

	rb_eval_string_protect(script, &status);

	if ( status )
	{
		rb_eval_string_protect("print $!", &status);
		return 1;
	}

	return 0;
}

int	Sin::SINRGE2Entry(const char* rubyfile)
{
	SINRGE2Initialize();

	int state = 0;

	VALUE rbread = rb_file_open(rubyfile, "rb");
	VALUE script = rb_funcall(rbread, rb_intern("read"), 0);
	(void)rb_io_close(rbread);
	VALUE name	 = rb_str_new2(rubyfile);

	VALUE argv = rb_ary_new2(2);
	rb_ary_push(argv, script);
	rb_ary_push(argv, name);
	rb_ary_freeze(argv);

	VALUE result = rb_protect(__run_sin_in_protect, argv, &state);

	if (state)
	{
		VALUE err = rb_errinfo();
		if (!rb_obj_is_kind_of(err, rb_eSystemExit))
		{
			__on_failed(err);
		}
	}
	return state;
}

void Sin::SetRubyLibHandle(HINSTANCE hModule)
{
	rb_set_lib_handle(hModule);
}
