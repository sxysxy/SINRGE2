/*
** SINRGE2
** Copyright (C) 2013 Shy07
** Gernischt@gmail.com
**
** Ruby Moudle SINRGE2
*/
#include "nge_timer.h"
#include "MRbSinCore.h"
#include "RbBitmap.h"
#include "sin_common.h"
#include "sin_app.h"
#include <math.h>

VALUE rb_mFrame;
VALUE rb_mSin;
VALUE rb_eSinError;
VALUE rb_mGraphics;

VALUE MRbSinCore::init()
{
	if (GetAppPtr()->InitVideo())
		return Qfalse;
	// 为了避免窗口在打开过程中就开始写屏，等待30秒
	int dt = 1800;
	do
	{
		GetAppPtr()->GraphicsUpdate();
		dt--;
	} while (dt);
	return Qtrue;
}

VALUE MRbSinCore::quit()
{
	GetAppPtr()->Quit();
	return Qnil;
}

VALUE MRbSinCore::stop()
{
	do
	{
		GetAppPtr()->GraphicsUpdate();
	} while (true);
	return Qnil;
}

VALUE MRbSinCore::update()
{
	GetAppPtr()->GraphicsUpdate();
	return Qnil;
}

VALUE MRbSinCore::wait(int argc, VALUE duration)
{
	SafeFixnumValue(duration);
	int dt = FIX2INT(duration);
	do
	{
		GetAppPtr()->GraphicsUpdate();
		dt--;
	} while (dt);
	return Qnil;
}

VALUE MRbSinCore::snap_to_bitmap()
{
	VALUE __argv[] = {INT2FIX(2), INT2FIX(2)};
	VALUE bitmap = rb_class_new_instance(2, __argv, rb_cBitmap);
	RbBitmap* pRbBmp = (RbBitmap*)DATA_PTR(bitmap);
	if (!RbBitmap::ScreenToBitmap(pRbBmp->GetBitmapPtr()))
	{
		pRbBmp = NULL;
		bitmap = Qnil;
	}
	return bitmap;
}

VALUE MRbSinCore::freeze()
{
	Freeze();
	return Qnil;
}

VALUE MRbSinCore::transition(int argc, VALUE *argv)
{
	int duration = FIXNUM_P(argv[0]) ? FIX2INT(argv[0]) : 8;
	wchar_t filename[MAX_PATH];
	filename[0] = 0;
	float vague = FIXNUM_P(argv[2]) ? (float)FIX2INT(argv[2]) : 40.0f;
	if (!NIL_P(argv[1]))
	{
		SafeStringValue(argv[1]);
		wcscpy_s(filename, Kconv::UTF8ToUnicode(RSTRING_PTR(argv[1])));
	}
	Transition(duration, filename, vague);
	return Qnil;
}

//VALUE MRbSinCore::frame_reset()
//{
//	return Qnil;
//}

VALUE MRbSinCore::get_brightness()
{
	return INT2FIX(GetAppPtr()->m_brightness);
}

VALUE MRbSinCore::set_brightness(int argc, VALUE value)
{
	SafeFixnumValue(value);
	int brightness = FIX2INT(value);
	brightness = SinBound(brightness, 0, 255);
	GetAppPtr()->m_brightness = brightness;
	return INT2FIX(brightness);
}

VALUE MRbSinCore::fadeout(int argv, VALUE duration)
{
	SafeFixnumValue(duration);
	int dt = FIX2INT(duration);
	int brightness = GetAppPtr()->m_brightness;
	if (!brightness)
	{
		do
		{
			GetAppPtr()->GraphicsUpdate();
			dt--;
		} while (dt);
	}
	else
	{
		float rate = (brightness - 0) / (float)dt;
		float brt = (float)brightness;
		do
		{
			brt -= rate;
			brightness = ceil(brt);
			brightness = SinBound(brightness, 0, 255);
			GetAppPtr()->m_brightness = brightness;
			GetAppPtr()->GraphicsUpdate();
			dt--;
		} while (dt);
	}
	return Qnil;
}

VALUE MRbSinCore::fadein(int argv, VALUE duration)
{
	SafeFixnumValue(duration);
	int dt = FIX2INT(duration);
	int brightness = GetAppPtr()->m_brightness;
	if (brightness == 255)
	{
		do
		{
			GetAppPtr()->GraphicsUpdate();
			dt--;
		} while (dt);
	}
	else
	{
		float rate = (255 - brightness) / (float)dt;
		float brt = (float)brightness;
		do
		{
			brt += rate;
			brightness = ceil(brt);
			brightness = SinBound(brightness, 0, 255);
			GetAppPtr()->m_brightness = brightness;
			GetAppPtr()->GraphicsUpdate();
			dt--;
		} while (dt);
	}
	return Qnil;
}

VALUE MRbSinCore::peek_message()
{
	if (!GetAppPtr()->GetHgePtr()->System_PeekMessage())
		GetAppPtr()->Quit();

	return Qnil;
}

VALUE MRbSinCore::get_real_fps()
{
	return INT2FIX(GetRealFps());
}

VALUE MRbSinCore::get_hwnd()
{
	return LONG2FIX(GetAppPtr()->GetMainHwnd());
}

VALUE MRbSinCore::get_title()
{
	return rb_str_new2(Kconv::UnicodeToUTF8(GetAppPtr()->GetTitle()));
}

VALUE MRbSinCore::set_title(int argc, VALUE title)
{
	SafeStringValue(title);

	wchar_t* tempTitle = Kconv::UTF8ToUnicode(RSTRING_PTR(title));
	wcscpy_s(GetAppPtr()->m_frm_struct.m_title, tempTitle);
	return title;
}

VALUE MRbSinCore::get_width()
{
	return INT2FIX(GetAppPtr()->GetFrameWidth());
}

VALUE MRbSinCore::get_height()
{
	return INT2FIX(GetAppPtr()->m_frm_struct.m_screen_height);
}

VALUE MRbSinCore::get_frame_rate()
{
	return INT2FIX(GetAppPtr()->GetHgePtr()->System_GetState(HGE_FPS));
}

VALUE MRbSinCore::set_frame_rate(int argc, VALUE frame_rate)
{
	SafeFixnumValue(frame_rate);
	GetAppPtr()->GetHgePtr()->System_SetState(HGE_FPS, FIX2INT(frame_rate));
	return frame_rate;
}

VALUE MRbSinCore::get_framecount()
{
	return LONG2NUM(GetAppPtr()->m_frame_count);
}

VALUE MRbSinCore::set_framecount(int argc, VALUE framecount)
{
	SafeNumericValue(framecount);
	GetAppPtr()->m_frame_count = NUM2LONG(framecount);
	return framecount;
}

VALUE MRbSinCore::set_start_width(int argc, VALUE width)
{
	/*if (inited)
		rb_raise(rb_eRuntimeError, "NGE has been inited");*/
	SafeFixnumValue(width);
	GetAppPtr()->m_frm_struct.m_screen_width = FIX2INT(width);
	return width;
}

VALUE MRbSinCore::set_start_height(int argc, VALUE height)
{
	/*if (inited)
		rb_raise(rb_eRuntimeError, "NGE has been inited");*/
	SafeFixnumValue(height);
	GetAppPtr()->m_frm_struct.m_screen_height = FIX2INT(height);
	return height;
}

VALUE MRbSinCore::resize_screen(int argc, VALUE width, VALUE height)
{
	SafeFixnumValue(width);
	SafeFixnumValue(height);

	int w = FIX2INT(width), h = FIX2INT(height);
	GetAppPtr()->m_frm_struct.m_screen_width = w;
	GetAppPtr()->m_frm_struct.m_screen_height = h;
	GetAppPtr()->GetHgePtr()->System_Resize(w, h);
	return Qnil;
}

VALUE MRbSinCore::get_fs_start()
{
	return C2RbBool(GetAppPtr()->m_frm_struct.m_fullscreen_start);
}

VALUE MRbSinCore::set_fs_start(int argc, VALUE fs_start)
{
	GetAppPtr()->m_frm_struct.m_fullscreen_start = RTEST(fs_start);
	return fs_start;
}

VALUE MRbSinCore::get_forbid_fs()
{
	return C2RbBool(GetAppPtr()->m_frm_struct.m_forbid_fullscreen);
}

VALUE MRbSinCore::set_forbid_fs(int argc, VALUE forbid_fs)
{
	GetAppPtr()->m_frm_struct.m_forbid_fullscreen = RTEST(forbid_fs);
	return forbid_fs;
}

VALUE MRbSinCore::get_forbid_switch()
{
	return C2RbBool(GetAppPtr()->m_frm_struct.m_forbid_switch);
}

VALUE MRbSinCore::set_forbid_switch(int argc, VALUE forbid_switch)
{
	GetAppPtr()->m_frm_struct.m_forbid_switch = RTEST(forbid_switch);
	return forbid_switch;
}

void MRbSinCore::InitLibrary()
{
	rb_mSin = rb_define_module("SINRGE2");
	
	rb_define_const(rb_mSin, "SINRGE2_COPYRIGHT",		rb_str_freeze(rb_str_new2(SIN_COPYRIGHT)));
	rb_define_const(rb_mSin, "SINRGE2_DESCRIPTION",		rb_str_freeze(rb_str_new2(SIN_DESCRIPTION)));
	rb_define_const(rb_mSin, "SINRGE2_NAME",			rb_str_freeze(rb_str_new2(SIN_NAME)));
	rb_define_const(rb_mSin, "SINRGE2_RELEASE_DATE",	rb_str_freeze(rb_str_new2(SIN_RELEASE_DATE)));
	rb_define_const(rb_mSin, "SINRGE2_THANKS",			rb_str_freeze(rb_str_new2(SIN_THANKS)));
	rb_define_const(rb_mSin, "SINRGE2_VERSION",			rb_str_freeze(rb_str_new2(SIN_VERSION)));
	
	rb_eSinError = rb_define_class_under(rb_mSin, "StandardError", rb_eStandardError);

	rb_mGraphics = rb_define_module_under(rb_mSin, "Graphics");

	rb_define_singleton_method(rb_mSin, "init", RbFunc(init), 0);
	rb_define_singleton_method(rb_mSin, "quit", RbFunc(quit), 0);
	
	rb_define_module_function(rb_mGraphics, "update", RbFunc(update), 0);
	rb_define_module_function(rb_mGraphics, "wait", RbFunc(wait), 1);
	rb_define_module_function(rb_mGraphics, "fadeout", RbFunc(fadeout), 1);
	rb_define_module_function(rb_mGraphics, "fadein", RbFunc(fadein), 1);
	rb_define_module_function(rb_mGraphics, "freeze", RbFunc(freeze), 0);
	rb_define_module_function(rb_mGraphics, "transition", RbFunc(transition), -1);
	rb_define_module_function(rb_mGraphics, "snap_to_bitmap", RbFunc(snap_to_bitmap), 0);
	//rb_define_module_function(rb_mGraphics, "frame_reset", RbFunc(frame_reset), 0);
	rb_define_module_function(rb_mGraphics, "width", RbFunc(get_width), 0);
	rb_define_module_function(rb_mGraphics, "height", RbFunc(get_height), 0);
	rb_define_module_function(rb_mGraphics, "resize_screen", RbFunc(resize_screen), 2);
	
	rb_define_module_function(rb_mGraphics, "frame_rate", RbFunc(get_frame_rate), 0);
	rb_define_module_function(rb_mGraphics, "frame_rate=", RbFunc(set_frame_rate), 1);
	rb_define_module_function(rb_mGraphics, "frame_count", RbFunc(get_framecount), 0);
	rb_define_module_function(rb_mGraphics, "frame_count=", RbFunc(set_framecount), 1);
	rb_define_module_function(rb_mGraphics, "brightness", RbFunc(get_brightness), 0);
	rb_define_module_function(rb_mGraphics, "brightness=", RbFunc(set_brightness), 1);

	rb_mFrame = rb_define_module_under(rb_mSin, "Frame");
	//rb_iv_set(rb_mFrame,"__title__", rb_str_freeze(rb_str_new2(SIN_DEFAULT_TITLE_NAME)));

	rb_define_module_function(rb_mFrame, "hwnd",	RbFunc(get_hwnd), 0);
	rb_define_module_function(rb_mFrame, "title",	RbFunc(get_title), 0);
	rb_define_module_function(rb_mFrame, "title=",	RbFunc(set_title), 1);

	rb_define_module_function(rb_mFrame, "fullscreen_start",	RbFunc(get_fs_start), 0);
	rb_define_module_function(rb_mFrame, "fullscreen_start=",	RbFunc(set_fs_start), 1);
	rb_define_module_function(rb_mFrame, "forbid_fullscreen",	RbFunc(get_forbid_fs), 0);
	rb_define_module_function(rb_mFrame, "forbid_fullscreen=",	RbFunc(set_forbid_fs), 1);
	rb_define_module_function(rb_mFrame, "forbid_switch",		RbFunc(get_forbid_switch), 0);
	rb_define_module_function(rb_mFrame, "forbid_switch=",		RbFunc(set_forbid_switch), 1);
	rb_define_module_function(rb_mFrame, "start_width=",		RbFunc(set_start_width), 1);
	rb_define_module_function(rb_mFrame, "start_height=",		RbFunc(set_start_height), 1);
	
	rb_define_module_function(rb_mSin, "real_fps", RbFunc(get_real_fps), 0);
	rb_define_module_function(rb_mSin, "peek_message", RbFunc(peek_message), 0);

	rb_define_module_function(rb_mSin, "game_stop", RbFunc(stop), 0);
}