/**
 * @file  RbNGE.cpp
 * @author Sherry Lynch (or Shy07) <Gernischt@gmail.com>
 * @date ‎2013‎/‎3‎/14‎ 15:11:02
 *
 *  Copyright  2013  Sherry Lynch (or Shy07) <Gernischt@gmail.com>
 *
 */
#include "RbExport.h"
#include "SINRGE2.h"
#include "libnge2pure.h"

using namespace Sin;

static VALUE	mNGE;

static bool		inited			= false;
static int		graphicsWidth	= 800;
static int		graphicsHeight	= 600;
static char*	gameTitle		= "SINRGE2";
static int		fullscreen		= 0;
static char*	titleWithFps	= new char[256];

static float	realFps;


HWND Sin::GetHwnd()
{
	return GetNGEhWnd();
}

/*
**	NGE::Graphics::Frame
*/

/*
**	call-seq:
**		NGE::Graphics::Frame.hwnd	-> fixnum
**
*/
static VALUE get_hwnd()
{
	return LONG2FIX(GetHwnd());
}

/*
**	call-seq:
**		NGE::Graphics::Frame.width	-> fixnum
**
*/
static VALUE get_width()
{
	return rb_int_new(graphicsWidth);
}

/*
**	call-seq:
**		NGE::Graphics::Frame.width = fixnum	-> nil
**
*/
static VALUE set_width(int argc, VALUE width)
{
	if (inited)
		rb_raise(rb_eRuntimeError, "NGE has been inited");

	if (NIL_P(width) || NUM2INT(width) == 0)
		rb_raise(rb_eArgError, "width must be greater than zero");
	else
		graphicsWidth = NUM2INT(width);

	return Qnil;
}

/*
**	call-seq:
**		NGE::Graphics::Frame.height	-> fixnum
**
*/
static VALUE get_height()
{
	return rb_int_new(graphicsHeight);
}

/*
**	call-seq:
**		NGE::Graphics::Frame.height = fixnum	-> nil
**
*/
static VALUE set_height(int argc, VALUE height)
{
	if (inited)
		rb_raise(rb_eRuntimeError, "NGE has been inited");

	if (NIL_P(height) || NUM2INT(height) <= 0)
		rb_raise(rb_eArgError, "height must be greater than zero");
	else
		graphicsHeight = NUM2INT(height);

	return Qnil;
}

/*
**	call-seq:
**		NGE::Graphics::Frame.titie = string	-> nil
**
*/
static VALUE set_title(int argc, VALUE title)
{
	if (inited)
		rb_raise(rb_eRuntimeError, "NGE has been inited");

	if (NIL_P(title))
		rb_raise(rb_eArgError, "title could not be nil");
	else
		gameTitle = rb_string_value_ptr(&title);

	return Qnil;
}

/*
**	call-seq:
**		NGE::Graphics::Frame.title	-> nil
**
*/
static VALUE get_title()
{
	return rb_str_new2(gameTitle);
}

/*
**	call-seq:
**		NGE::Graphics::Frame.fullscreen_start = bool	-> nil
**
*/
static VALUE set_fullscreen(int argc, VALUE argv)
{
	if (inited)
		rb_raise(rb_eRuntimeError, "NGE has been inited");

	fullscreen = (argv == Qtrue) ? 1 : 0;

	return Qnil;
}

/*
**	call-seq:
**		NGE::Graphics::Frame.fullscreen_start	-> bool
**
*/
static VALUE get_fullscreen()
{
	return fullscreen == 1 ? Qtrue : Qfalse;
}

/*
**	call-seq:
**		NGE::Graphics::Frame.peek_message	-> bool
**
*/
static VALUE peek_message()
{
	MSG msg;
	if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if ( msg.message == WM_QUIT )
		{
			return Qfalse;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return Qtrue;
}

/*
**	NGE
*/

/*
**	call-seq:
**		NGE.init_video	-> nil
**
*/
static VALUE init_video()
{
	if (inited)
	{
		rb_raise(rb_eRuntimeError, "NGE has been inited");
	}
	else
	{
		NGE_SetScreenContext(gameTitle, graphicsWidth, graphicsHeight, SCREEN_BPP, fullscreen);
		NGE_Init(INIT_VIDEO);
		inited = true;
	}

	return Qnil;
}

/*
**	call-seq:
**		NGE.quit	-> nil
**
*/
static VALUE quit_nge()
{
	NGE_Quit();
	inited = false;

	return Qnil;
}

/*
**	NGE::Image
*/
static VALUE mImage;
/*
**	call-seq:
**		NGE.image_load(filename)	-> NGE::Image
**
*/
static VALUE load_image(int argc, VALUE filename)
{
	image_p img = image_load(Kconv::UTF8ToAnsi(rb_string_value_ptr(&filename)),
		DISPLAY_PIXEL_FORMAT_8888, 1);

	if (img == 0)
		return Qnil;
	else
		return Data_Wrap_Struct(mImage, 0, image_free, img);
}

/*
**	call-seq:
**		NGE.image_load_buf(mbuf, bsize)	-> NGE::Image
**
*/
static VALUE load_buf_image(int argc, VALUE mbuf, VALUE bsize)
{
	char* buf = rb_string_value_ptr(&mbuf);
	image_p img = image_load_buf(buf, NUM2INT(bsize), DISPLAY_PIXEL_FORMAT_8888, 1);
	
	if (img == 0)
		return Qnil;
	else
		return Data_Wrap_Struct(mImage, 0, image_free, img);
}

/*
**	call-seq:
**		NGE.image_create(width, height)	-> NGE::Image
**
*/
static VALUE create_image(int argc, VALUE w, VALUE h)
{
	//if (NIL_P(w) || NIL_P(h))
		//rb_raise(rb_eArgError, "width or height could not be nil");

	int width = NUM2INT(w);
	int height = NUM2INT(h);
	if (width <= 0 || height <= 0 )
		rb_raise(rb_eArgError, "width or height must be greater than zero");

	image_p img = image_create(width, height, DISPLAY_PIXEL_FORMAT_8888);
	if (img == 0)
		return Qnil;
	else
		return Data_Wrap_Struct(mImage, 0, image_free, img);
}

/*
**	call-seq:
**		NGE.image_create_ex(width, height, color)	-> NGE::Image
**
*/
static VALUE create_image_ex(int argc, VALUE w, VALUE h, VALUE color)
{
	int width = NUM2INT(w);
	int height = NUM2INT(h);
	if (width <= 0 || height <= 0 )
		rb_raise(rb_eArgError, "width or height must be greater than zero");

	image_p img = image_create_ex(width, height, NUM2INT(color), DISPLAY_PIXEL_FORMAT_8888);
	if (img == 0)
		return Qnil;
	else
		return Data_Wrap_Struct(mImage, 0, image_free, img);
}

/*
**	call-seq:
**		NGE.image_save(image, filename, aplha, rle)	-> nil
**
*/
static VALUE save_image(int argc, VALUE img, VALUE filename, VALUE alpha, VALUE rle)
{
	image_p sin_img;
	Data_Get_Struct(img, image_t, sin_img);
	image_save(sin_img, Kconv::UTF8ToAnsi(rb_string_value_ptr(&filename)),	alpha == Qtrue, rle == Qtrue);
	return Qnil;
}

/*
**	call-seq:
**		NGE.image_free(image)	-> nil
**
*/
static VALUE free_image(int argc, VALUE img)
{
	image_p sin_img;

	//if (!NIL_P(img))
	//{
		Data_Get_Struct(img, image_t, sin_img);
		image_free(sin_img);
	//}
	return Qnil;
}

/*
**	call-seq:
**		NGE.image_clone(image)	-> NGE::Image
**
*/
static VALUE clone_image(int argc, VALUE img)
{
	image_p sin_img, new_img;
	Data_Get_Struct(img, image_t, sin_img);
	new_img = image_clone(sin_img);
	if (new_img)
		return Data_Wrap_Struct(mImage, 0, image_free, new_img);
	else
		return Qnil;
}

/*
**	call-seq:
**		NGE.image_clear(image)	-> nil
**
*/
static VALUE clear_image(int argc, VALUE img)
{
	image_p sin_img;

	//if (!NIL_P(img))
	//{
		Data_Get_Struct(img, image_t, sin_img);
		image_clear(sin_img);
	//}
	return Qnil;
}

/*
**	call-seq:
**		NGE.image_to_image(src, dex, dx, dy)	-> nil
**
*/
static VALUE image2image(int argc, VALUE src, VALUE des, VALUE dx, VALUE dy)
{
	image_p src_img, des_img;
	//if (!NIL_P(src_img) && !NIL_P(des_img))
	//{
	Data_Get_Struct(src, image_t, src_img);
	Data_Get_Struct(des, image_t, des_img);
	image_to_image(src_img, des_img, NUM2INT(dx), NUM2INT(dx));
	//}
	return Qnil;
}

/*
**	call-seq:
**		NGE.image_to_image_ex(src, des, sx, sy, sw, sh, dx, dy, flag)	-> nil
**
*/
static VALUE image2image_ex(int argc, VALUE src, VALUE des, VALUE sx, VALUE sy, VALUE sw, VALUE sh, VALUE dx, VALUE dy, VALUE flag)
{
	image_p src_img, des_img;

	Data_Get_Struct(src, image_t, src_img);
	Data_Get_Struct(des, image_t, des_img);

	image_to_image_ex(src_img, des_img, NUM2INT(sx), NUM2INT(sy),
		NUM2INT(sw), NUM2INT(sh), NUM2INT(dx), NUM2INT(dy), flag == Qtrue);

	return Qnil;
}

/*
**	call-seq:
**		NGE.image_to_image_alpha(src, dex, dx, dy, alpha)	-> nil
**
*/
static VALUE imsge2image_alpha(int argc, VALUE src, VALUE des, VALUE dx, VALUE dy, VALUE alpha)
{
	image_p src_img, des_img;
	//if (!NIL_P(src_img) && !NIL_P(des_img))
	//{
	Data_Get_Struct(src, image_t, src_img);
	Data_Get_Struct(des, image_t, des_img);
	image_to_image_alpha(src_img, des_img, NUM2INT(dx), NUM2INT(dy), NUM2INT(alpha));
	//}
	return Qnil;
}

/*
**	call-seq:
**		NGE.image_to_image_alpha_ex(src, dex, sx, sy, sw, sh, dx, dy, alpha, flag)	-> nil
**
*/
static VALUE image2image_alpha_ex(int argc, VALUE src, VALUE des, VALUE sx, VALUE sy, VALUE sw, VALUE sh, VALUE dx, VALUE dy, VALUE alpha, VALUE flag)
{
	image_p src_img, des_img;

	Data_Get_Struct(src, image_t, src_img);
	Data_Get_Struct(des, image_t, des_img);

	image_to_image_alpha_ex(src_img, des_img, NUM2INT(sx), NUM2INT(sy),
		NUM2INT(sw), NUM2INT(sh), NUM2INT(dx), NUM2INT(dy), alpha == Qtrue, flag == Qtrue);

	return Qnil;
}

/*static VALUE rawdata2image(int argc, VALUE data, VALUE des, VALUE x, VALUE y, VALUE w, VALUE h)
{
	image_p des_img;

	char* rawdata = rb_string_value_ptr(&data);
	Data_Get_Struct(des, image_t, des_img);

	rawdata_to_image(rawdata, des_img, NUM2INT(x), NUM2INT(y), NUM2INT(w), NUM2INT(h));

	return Qnil;
}*/

/*
**	call-seq:
**		NGE.image_fliph(image)	-> nil
**
*/
static VALUE fliph_image(int argc, VALUE img)
{
	image_p sin_img;
	Data_Get_Struct(img, image_t, sin_img);
	image_fliph(sin_img);

	return Qnil;
}

/*
**	call-seq:
**		NGE.image_flipv(image)	-> nil
**
*/
static VALUE flipv_image(int argc, VALUE img)
{
	image_p sin_img;
	Data_Get_Struct(img, image_t, sin_img);
	image_flipv(sin_img);

	return Qnil;
}

/*
**	call-seq:
**		NGE.image_fillrect(image, x, y, w, h, color)	-> nil
**
*/
static VALUE fillrect_image(int argc, VALUE img, VALUE x, VALUE y, VALUE w, VALUE h, VALUE color)
{
	image_p sin_img;

	Data_Get_Struct(img, image_t, sin_img);
	image_fillrect(sin_img, NUM2INT(x), NUM2INT(y), NUM2INT(w), NUM2INT(h), NUM2UINT(color));

	return Qnil;
}

/*
**	NGE::Graphics
*/

/*
**	call-seq:
**		NGE.begin_scene([clear])	-> nil
**
*/
static VALUE begin_scene(int argc, VALUE* argv)
{
	VALUE rint;

    rb_scan_args(argc, argv, "01", &rint);
	BeginScene(NIL_P(rint) ? 1 : NUM2INT(rint));

	return Qnil;
}

/*
**	call-seq:
**		NGE.end_scene	-> nil
**
*/
static VALUE end_scene()
{
	EndScene();

	return Qnil;
}

/*
**	call-seq:
**		NGE.real_fps	-> fixmun
**
*/
static VALUE get_real_fps()
{
	return rb_float_new(GetRealFps());
}

/*
**	call-seq:
**		NGE.limit_fps([fps])	-> nil
**
*/
static VALUE limit_fps(int argc, VALUE* argv)
{
    VALUE rint;

    rb_scan_args(argc, argv, "01", &rint);
	LimitFps(NIL_P(rint) ? 60 : NUM2INT(rint));

    return Qnil;
}

/*
**	call-seq:
**		NGE.show_fps	-> nil
**
*/
static VALUE show_fps()
{
	sprintf(titleWithFps, "%s - %d FPS", gameTitle, (int)GetRealFps());
	SetWindowText(GetHwnd(), Kconv::AnsiToUnicode(titleWithFps));
	return Qnil;
}

/*
**	call-seq:
**		NGE.begin_target(image [, clear])	-> bool
**
*/
static VALUE begin_target(int argv, VALUE img, VALUE clear)
{
	image_p sin_img;

	Data_Get_Struct(img, image_t, sin_img);

	return BeginTarget(sin_img, NUM2UINT(clear)) ? Qtrue : Qfalse;
}

/*
**	call-seq:
**		NGE.end_target	-> nil
**
*/
static VALUE end_target()
{
	EndTarget();

	return Qnil;
}

/*
**	call-seq:
**		NGE.set_clip(x, y, width, height)	-> nil
**
*/
static VALUE set_clip(int argv, VALUE x, VALUE y, VALUE w, VALUE h)
{
	SetClip(NUM2INT(x), NUM2INT(y), NUM2INT(w), NUM2INT(h));

	return Qnil;
}

/*
**	call-seq:
**		NGE.reset_clip	-> nil
**
*/
static VALUE reset_clip()
{
	ResetClip();

	return Qnil;
}

/*
**	call-seq:
**		NGE.draw_line(x1, y1, x2, y2, color)	-> nil
**
*/
static VALUE draw_line(int argv, VALUE x1, VALUE y1, VALUE x2, VALUE y2, VALUE color)
{
	DrawLine(NUM2DBL(x1), NUM2DBL(y1), NUM2DBL(x2), NUM2DBL(y2), NUM2UINT(color), DISPLAY_PIXEL_FORMAT_8888);

	return Qnil;
}

/*
**	call-seq:
**		NGE.draw_rect(dx, dy, width, height, color)	-> nil
**
*/
static VALUE draw_rect(int argv, VALUE dx, VALUE dy, VALUE width, VALUE height, VALUE color)
{
	DrawRect(NUM2DBL(dx), NUM2DBL(dy), NUM2DBL(width), NUM2DBL(height), NUM2UINT(color), DISPLAY_PIXEL_FORMAT_8888);

	return Qnil;
}

/*
**	call-seq:
**		NGE.fill_rect(dx, dy, width, height, color)	-> nil
**
*/
static VALUE fill_rect(int argv, VALUE dx, VALUE dy, VALUE width, VALUE height, VALUE color)
{
	NGE_FillRect(NUM2DBL(dx), NUM2DBL(dy), NUM2DBL(width), NUM2DBL(height), NUM2UINT(color), DISPLAY_PIXEL_FORMAT_8888);

	return Qnil;
}

/*
**	call-seq:
**		NGE.fill_rect_grad(dx, dy, width, height, colors)	-> nil
**
*/
static VALUE fill_rect_grad(int argv, VALUE dx, VALUE dy, VALUE width, VALUE height, VALUE colors)
{
	int color[] = {
		NUM2UINT(rb_ary_entry(colors, 0)),
		NUM2UINT(rb_ary_entry(colors, 1)),
		NUM2UINT(rb_ary_entry(colors, 2)),
		NUM2UINT(rb_ary_entry(colors, 3)),
	};
	FillRectGrad(NUM2DBL(dx), NUM2DBL(dy), NUM2DBL(width), NUM2DBL(height), color, DISPLAY_PIXEL_FORMAT_8888);

	return Qnil;
}

/*
**	call-seq:
**		NGE.render_quad(image, sx, sy, sw, sh, dx, dy, xscale, yscale, angle, mask)	-> nil
**
*/
static VALUE render_quad(int argv, VALUE img, VALUE sx, VALUE sy, VALUE sw, VALUE sh, VALUE dx, VALUE dy, VALUE xscale, VALUE yscale, VALUE angle, VALUE mask)
{
	image_p sin_img;

	Data_Get_Struct(img, image_t, sin_img);
	RenderQuad(sin_img, NUM2DBL(sx), NUM2DBL(sy), NUM2DBL(sw), NUM2DBL(sh),
		NUM2DBL(dx), NUM2DBL(dy), NUM2DBL(xscale), NUM2DBL(yscale), NUM2DBL(angle), NUM2UINT(mask));

	return Qnil;
}

/*
**	call-seq:
**		NGE.draw_image(image, sx, sy, sw, sh, dx, dy, dw, dh)	-> nil
**
*/
static VALUE draw_image(int argv,VALUE img, VALUE sx, VALUE sy, VALUE sw, VALUE sh, VALUE dx, VALUE dy, VALUE dw, VALUE dh)
{
	image_p sin_img;

	Data_Get_Struct(img, image_t, sin_img);
	DrawImage(sin_img, NUM2DBL(sx), NUM2DBL(sy), NUM2DBL(sw), NUM2DBL(sh),
		NUM2DBL(dx), NUM2DBL(dy), NUM2DBL(dw), NUM2DBL(dh));

	return Qnil;
}

/*
**	call-seq:
**		NGE.draw_image_mask(image, sx, sy, sw, sh, dx, dy, dw, dh, mask)	-> nil
**
*/
static VALUE draw_image_mask(int argv,VALUE img, VALUE sx, VALUE sy, VALUE sw, VALUE sh, VALUE dx, VALUE dy, VALUE dw, VALUE dh, VALUE mask)
{
	image_p sin_img;

	Data_Get_Struct(img, image_t, sin_img);
	DrawImageMask(sin_img, NUM2DBL(sx), NUM2DBL(sy), NUM2DBL(sw), NUM2DBL(sh),
		NUM2DBL(dx), NUM2DBL(dy), NUM2DBL(dw), NUM2DBL(dh), NUM2UINT(mask));

	return Qnil;
}

/*
**	call-seq:
**		NGE.image_to_screen(image, x, y)	-> nil
**
*/
static VALUE image_to_screen(int argc, VALUE img, VALUE x, VALUE y)
{
	image_p sin_img;

	//if (!NIL_P(img))
	//{
		Data_Get_Struct(img, image_t, sin_img);
		ImageToScreen(sin_img, NUM2DBL(x), NUM2DBL(y));
	//}
	return Qnil;
}

/*
**	call-seq:
**		NGE.draw_region(image, sx, sy, sw, sh, transform, dx, dy, anchor)	-> nil
**
*/
static VALUE draw_region(VALUE img, VALUE sx, VALUE sy, VALUE sw, VALUE sh, VALUE transform, VALUE dx, VALUE dy, VALUE anchor)
{
	image_p sin_img;

	Data_Get_Struct(img, image_t, sin_img);
	DrawRegion(sin_img, NUM2INT(sx), NUM2INT(sy), NUM2INT(sw), NUM2INT(sh), NUM2INT(transform),
		NUM2INT(dx), NUM2INT(dy), NUM2INT(anchor));

	return Qnil;
}

/*
**	call-seq:
**		NGE.screen_to_imsge	-> NGE::Image
**
*/
static VALUE screen_to_image()
{
	image_p img = ScreenToImage();
	if (img == 0)
		return Qnil;
	else
		return Data_Wrap_Struct(mImage, 0, image_free, img);
}

/*
**	call-seq:
**		NGE.screen_shot(filename)	-> nil
**
*/
static VALUE screen_shot(int argv, VALUE filename)
{
	ScreenShot(Kconv::UTF8ToAnsi(rb_string_value_ptr(&filename)));

	return Qnil;
}

/*
**	call-seq:
**		NGE.set_screen_color(r, g, b, a)	-> fixnum
**
*/
static VALUE set_screen_color(int argv, VALUE red, VALUE green, VALUE blue, VALUE alpha)
{
	return rb_int_new(SetScreenColor(NUM2INT(red), NUM2INT(green), NUM2INT(blue), NUM2INT(alpha)));
}

/*
**	call-seq:
**		NGE.set_tex_blend(src_blend, dex_blend)	-> nil
**
*/
static VALUE set_tex_blend(int argv, VALUE src_blend, VALUE des_blend)
{
	SetTexBlend(NUM2INT(src_blend), NUM2INT(des_blend));
	return Qnil;
}

/*
**	call-seq:
**		NGE.reset_tex_blend	-> nil
**
*/
static VALUE reset_tex_blend()
{
	ResetTexBlend();
	return Qnil;
}

/*
**	call-seq:
**		NGE.pu_pix(x, y, color)	-> nil
**
*/
static VALUE put_pix(int argv, VALUE x, VALUE y, VALUE color)
{
	PutPix(NUM2DBL(x), NUM2DBL(y), NUM2UINT(color), DISPLAY_PIXEL_FORMAT_8888);
	return Qnil;
}

/*
**	call-seq:
**		NGE.draw_circle(x, y, radius, color)	-> nil
**
*/
static VALUE draw_circle(int argv, VALUE x, VALUE y, VALUE radius, VALUE color)
{
	DrawCircle(NUM2DBL(x), NUM2DBL(y), NUM2DBL(radius), NUM2UINT(color), DISPLAY_PIXEL_FORMAT_8888);
	return Qnil;
}

/*
**	call-seq:
**		NGE.fill_circle(x, y, radius, color)	-> nil
**
*/
static VALUE fill_circle(int argv, VALUE x, VALUE y, VALUE radius, VALUE color)
{
	FillCircle(NUM2DBL(x), NUM2DBL(y), NUM2DBL(radius), NUM2UINT(color), DISPLAY_PIXEL_FORMAT_8888);
	return Qnil;
}

/*
**	call-seq:
**		NGE.draw_ellipse(x, y, xradius, yradius, color)	-> nil
**
*/
static VALUE draw_ellipse(int argv, VALUE x, VALUE y, VALUE xradius, VALUE yradius, VALUE color)
{
	DrawEllipse(NUM2DBL(x), NUM2DBL(y), NUM2DBL(xradius), NUM2DBL(yradius), NUM2UINT(color), DISPLAY_PIXEL_FORMAT_8888);
	return Qnil;
}

/*
**	call-seq:
**		NGE.fill_ellipse(x, y, xradius, yradius, color)	-> nil
**
*/
static VALUE fill_ellipse(int argv, VALUE x, VALUE y, VALUE xradius, VALUE yradius, VALUE color)
{
	FillEllipse(NUM2DBL(x), NUM2DBL(y), NUM2DBL(xradius), NUM2DBL(yradius), NUM2UINT(color), DISPLAY_PIXEL_FORMAT_8888);
	return Qnil;
}

/*
**	call-seq:
**		NGE.draw_polygon(x, y, count, color)	-> nil
**
*/
static VALUE draw_polygon(int argv, VALUE x, VALUE y, VALUE count, VALUE color)
{
	int size = NUM2INT(count);
	float* szx = new float[size];
	float* szy = new float[size];
	for (int i = 0; i < size; i++)
	{
		szx[i] = NUM2DBL(rb_ary_entry(x, i));
		szy[i] = NUM2DBL(rb_ary_entry(y, i));
	}
	DrawPolygon(szx, szy, size, NUM2UINT(color), DISPLAY_PIXEL_FORMAT_8888);
	return Qnil;
}

/*
**	call-seq:
**		NGE.fill_polygon(x, y, count, color)	-> nil
**
*/
static VALUE fill_polygon(int argv, VALUE x, VALUE y, VALUE count, VALUE color)
{
	int size = NUM2INT(count);
	float* szx = new float[size];
	float* szy = new float[size];
	for (int i = 0; i < size; i++)
	{
		szx[i] = NUM2DBL(rb_ary_entry(x, i));
		szy[i] = NUM2DBL(rb_ary_entry(y, i));
	}
	FillPolygon(szx, szy, size, NUM2UINT(color), DISPLAY_PIXEL_FORMAT_8888);
	return Qnil;
}

/*
**	call-seq:
**		NGE.fill_polygon_grad(x, y, count, colors)	-> nil
**
*/
static VALUE fill_polygon_grad(int argv, VALUE x, VALUE y, VALUE count, VALUE colors)
{
	int size = NUM2INT(count);
	float* szx = new float[size];
	float* szy = new float[size];
	int* szcolor = new int[size];
	for (int i = 0; i < size; i++)
	{
		szx[i] = NUM2DBL(rb_ary_entry(x, i));
		szy[i] = NUM2DBL(rb_ary_entry(y, i));
		szcolor[i] = NUM2UINT(rb_ary_entry(colors, i));
	}
	FillPolygonGrad(szx, szy, size, szcolor, DISPLAY_PIXEL_FORMAT_8888);
	return Qnil;
}

/*
**	call-seq:
**		NGE.reset_graphics_cache	-> nil
**
*/
static VALUE reset_graphics_cache()
{
	ResetGraphicsCache();
	return Qnil;
}

/*
** NGE::Font
**
*/

#include "nge_font_internal.h"

static VALUE mFont;

/*
**	call-seq:
**		NGE.create_font_hzk(cname, aname, height)	-> NGE::Font
**
*/
static VALUE load_font_hzk(int argv, VALUE cname, VALUE aname, VALUE height)
{
	PFont font = create_font_hzk(
		Kconv::UTF8ToAnsi(rb_string_value_ptr(&cname)), 
		Kconv::UTF8ToAnsi(rb_string_value_ptr(&aname)),
		NUM2INT(height), DISPLAY_PIXEL_FORMAT_8888);
	
	if (font == 0)
		return Qnil;
	else
		return Data_Wrap_Struct(mFont, 0, font_destory, font);
}

/*
**	call-seq:
**		NGE.create_font_hzk_buf(cbuf, csize, abuf, asize, height)	-> NGE::Font
**
*/
static VALUE load_font_hzk_buf(int argv, VALUE cbuf, VALUE csize, VALUE abuf, VALUE asize, VALUE height)
{
	PFont font = create_font_hzk_buf(
		rb_string_value_ptr(&cbuf), NUM2INT(csize),
		rb_string_value_ptr(&abuf), NUM2INT(asize), 
		NUM2INT(height), DISPLAY_PIXEL_FORMAT_8888);

	if (font == 0)
		return Qnil;
	else
		return Data_Wrap_Struct(mFont, 0, font_destory, font);
}

/*
**	call-seq:
**		NGE.create_font_freetype(filename, height)	-> NGE::Font
**
*/
static VALUE load_font_freetype(int argv, VALUE filename, VALUE height)
{
	PFont font = create_font_freetype(
		Kconv::UTF8ToAnsi(rb_string_value_ptr(&filename)),
		NUM2INT(height), DISPLAY_PIXEL_FORMAT_8888);

	if (font == 0)
		return Qnil;
	else
		return Data_Wrap_Struct(mFont, 0, font_destory, font);
}

/*
**	call-seq:
**		NGE.create_font_freetype_buf(mbuf, bsize, height)	-> NGE::Font
**
*/
static VALUE load_font_freetype_buf(int argv, VALUE mbuf, VALUE bsize, VALUE height)
{
	PFont font = create_font_freetype_buf(rb_string_value_ptr(&mbuf),
		NUM2INT(bsize), NUM2INT(height), DISPLAY_PIXEL_FORMAT_8888);

	if (font == 0)
		return Qnil;
	else
		return Data_Wrap_Struct(mFont, 0, font_destory, font);
}

/*
**	call-seq:
**		NGE.draw_font_text(font, text, len, image, dx, dy, flag)	-> nil
**
*/
static VALUE draw_font_text(int argv, VALUE font, VALUE text, VALUE len, VALUE img, VALUE dx, VALUE dy, VALUE flag)
{
	PFont pfont;
	Data_Get_Struct(font, TFont, pfont);
	image_p textarea;
	Data_Get_Struct(img, image_t, textarea);

	font_drawtext(pfont, rb_string_value_ptr(&text),
		NUM2INT(len), textarea, NUM2INT(dx), NUM2INT(dy), NUM2INT(flag));

	return Qnil;
}

/*
**	call-seq:
**		NGE.set_font_attr(font, attr, setflag)	-> nil
**
*/
static VALUE set_font_attr(int argv, VALUE font, VALUE attr, VALUE setflag)
{
	PFont pfont;
	Data_Get_Struct(font, TFont, pfont);
	font_setattr(pfont, NUM2INT(attr), NUM2INT(setflag));
	return Qnil;
}

/*
**	call-seq:
**		NGE.destory_font(font)	-> nil
**
*/
static VALUE destory_font(int argv, VALUE font)
{
	PFont pfont;
	Data_Get_Struct(font, TFont, pfont);
	font_destory(pfont);
	return Qnil;
}

/*
**	call-seq:
**		NGE.set_font_color(font, color)	-> fixnum
**
*/
static VALUE set_font_color(int argv, VALUE font, VALUE color)
{
	PFont pfont;
	Data_Get_Struct(font, TFont, pfont);
	return rb_int_new(font_setcolor(pfont, NUM2UINT(color)));
}

/*
**	call-seq:
**		NGE.set_font_color_ex(font, fcolor, bcolor, scolor)	-> nil
**
*/
static VALUE set_font_color_ex(int argv, VALUE font, VALUE fcolor, VALUE bcolor, VALUE scolor)
{
	PFont pfont;
	Data_Get_Struct(font, TFont, pfont);
	font_setcolor_ex(pfont, NUM2UINT(fcolor), NUM2UINT(bcolor), NUM2UINT(scolor));
	return Qnil;
}

/*
**	call-seq:
**		NGE.get_textsize(font, text, len)	-> array
**
*/
static VALUE get_textsize(int argv, VALUE font, VALUE text, VALUE len)
{
	PFont pfont;
	int width, height, base;
	VALUE ary;

	Data_Get_Struct(font, TFont, pfont);
	font_textsize(pfont, rb_string_value_ptr(&text), NUM2INT(len), &width, &height, &base);

	ary = rb_ary_new2(3);
	rb_ary_push(ary, rb_int_new(width));
	rb_ary_push(ary, rb_int_new(height));
	rb_ary_push(ary, rb_int_new(base));
	rb_ary_freeze(ary);

	return ary;
}

/*
** NGE::Particle
**
*/

#include "hgeparticle.h"

#define SAFE_FREE(ptr) free(ptr);ptr = NULL

static VALUE mParticle;

static void particle_free(sprite_p pt)
{
	SAFE_FREE(pt);
}

/*
**	call-seq:
**		NGE.create_particle	-> NGE::Particle
**
*/
static VALUE create_particle()
{
	sprite_p mPt = NULL;
	mPt = (sprite_p)malloc(sizeof(sprite_t));
	memset(mPt, 0, sizeof(sprite_t));

	return Data_Wrap_Struct(mParticle, 0, particle_free, mPt);
}

/*
**	call-seq:
**		NGE.free_particle(pt)	-> nil
**
*/
static VALUE free_particle(int argv, VALUE pt)
{
	sprite_p mpt;
	Data_Get_Struct(pt, sprite_t, mpt);
	particle_free(mpt);
	return Qnil;
}

/*
**	call-seq:
**		NGE.particle_set_image(pt, image)	-> nil
**
*/
static VALUE particle_set_image(int argv, VALUE pt, VALUE img)
{
	sprite_p mpt;
	Data_Get_Struct(pt, sprite_t, mpt);
	image_p sin_img;
	Data_Get_Struct(img, image_t, sin_img);
	mpt->sprite_image = sin_img;
	return Qnil;
}

/*
**	call-seq:
**		NGE.particle_set_clip(pt, left, top, right, bottom)	-> nil
**
*/
static VALUE particle_set_clip(int argv, VALUE pt, VALUE left, VALUE top, VALUE right, VALUE bottom)
{
	sprite_p mpt;
	Data_Get_Struct(pt, sprite_t, mpt);
	mpt->sprite_clip.left	= NUM2DBL(left);
	mpt->sprite_clip.top	= NUM2DBL(top);
	mpt->sprite_clip.right	= NUM2DBL(right);
	mpt->sprite_clip.bottom	= NUM2DBL(bottom);
	return Qnil;
}

/*
**	call-seq:
**		NGE.paticle_set_center(pt, x, y)	-> nil
**
*/
static VALUE particle_set_center(int argv, VALUE pt, VALUE x, VALUE y)
{
	sprite_p mpt;
	Data_Get_Struct(pt, sprite_t, mpt);
	mpt->sprite_center.x = NUM2DBL(x);
	mpt->sprite_center.y = NUM2DBL(y);
	return Qnil;
}

typedef struct {
	hgeParticleSystem* ptSys;
}ptSys_t, *ptSys_p;

static VALUE mParticleSystem;

static void DistoryParticleSystem(ptSys_p psys)
{
	delete psys->ptSys;
	SAFE_FREE(psys);
}

/*
**	call-seq:
**		NGE.create_particle_system(filename, pt)	-> NGE::ParticleSystem
**
*/
static VALUE create_particle_system(int argv, VALUE filename, VALUE pt)
{
	sprite_p mpt;
	Data_Get_Struct(pt, sprite_t, mpt);

	ptSys_p psys = NULL;
	psys = (ptSys_p)malloc(sizeof(ptSys_p));
	memset(psys, 0, sizeof(ptSys_p));
	psys->ptSys = new hgeParticleSystem(Kconv::UTF8ToAnsi(rb_string_value_ptr(&filename)), mpt);

	return Data_Wrap_Struct(mParticleSystem, 0, DistoryParticleSystem, psys);
}

/*
**	call-seq:
**		NGE.particle_system_render(psys)	-> nil
**
*/
static VALUE particle_system_render(int argv, VALUE psys)
{
	ptSys_p mpsys;
	Data_Get_Struct(psys, ptSys_t, mpsys);
	mpsys->ptSys->Render();
	return Qnil;
}

/*
**	call-seq:
**		NGE.particle_system_fire(psys)	-> nil
**
*/
static VALUE particle_system_fire(int argv, VALUE psys)
{
	ptSys_p mpsys;
	Data_Get_Struct(psys, ptSys_t, mpsys);
	mpsys->ptSys->Fire();
	return Qnil;
}

/*
**	call-seq:
**		NGE.particle_system_fire_at(psys, x, y)	-> nil
**
*/
static VALUE particle_system_fire_at(int argv, VALUE psys, VALUE x, VALUE y)
{
	ptSys_p mpsys;
	Data_Get_Struct(psys, ptSys_t, mpsys);
	mpsys->ptSys->FireAt(NUM2DBL(x), NUM2DBL(y));
	return Qnil;
}

/*
**	call-seq:
**		NGE.particle_system_stop(psys)	-> nil
**
*/
static VALUE particle_system_stop(int argv, VALUE psys)
{
	ptSys_p mpsys;
	Data_Get_Struct(psys, ptSys_t, mpsys);
	mpsys->ptSys->Stop();
	return Qnil;
}

/*
**	call-seq:
**		NGE.particle_system_update(psys, time)	-> nil
**
*/
static VALUE particle_system_update(int argv, VALUE psys, VALUE time)
{
	ptSys_p mpsys;
	Data_Get_Struct(psys, ptSys_t, mpsys);
	mpsys->ptSys->Update(NUM2DBL(time));
	return Qnil;
}

/*
**	call-seq:
**		NGE.particle_system_move_to(psys, x, y, move)	-> nil
**
*/
static VALUE particle_system_move_to(int argv, VALUE psys, VALUE x, VALUE y, VALUE move)
{
	ptSys_p mpsys;
	Data_Get_Struct(psys, ptSys_t, mpsys);
	mpsys->ptSys->MoveTo(NUM2DBL(x), NUM2DBL(y), move == Qtrue);
	return Qnil;
}

/*
**	NGE::Timer
**
*/

static VALUE mTimer;

/*
**	call-seq:
**		NGE.create_timer	-> NGE::Timer
**
*/
static VALUE create_timer()
{
	nge_timer* timer = nge_timer_create();
	return Data_Wrap_Struct(mTimer, 0, nge_timer_free, timer);
}

/*
**	call-seq:
**		NGE.free_timer	-> nil
**
*/
static VALUE free_timer(int argv, VALUE timer)
{
	nge_timer* ptimer;
	Data_Get_Struct(timer, nge_timer, ptimer);
	nge_timer_free(ptimer);
	return Qnil;
}

/*
**	call-seq:
**		NGE.get_ticks(timer)	-> fixnum
**
*/
static VALUE get_ticks(int argv, VALUE timer)
{
	nge_timer* ptimer;
	Data_Get_Struct(timer, nge_timer, ptimer);
	return rb_int_new(ptimer->get_ticks(ptimer));
}

/*
**	call-seq:
**		NGE.timer_start(timer)	-> nil
**
*/
static VALUE timer_start(int argv, VALUE timer)
{
	nge_timer* ptimer;
	Data_Get_Struct(timer, nge_timer, ptimer);
	ptimer->start(ptimer);
	return Qnil;
}

/*
**	call-seq:
**		NGE.timer_stop(timer)	-> nil
**
*/
static VALUE timer_stop(int argv, VALUE timer)
{
	nge_timer* ptimer;
	Data_Get_Struct(timer, nge_timer, ptimer);
	ptimer->stop(ptimer);
	return Qnil;
}

/*
**	call-seq:
**		NGE.timer_pause(timer)	-> nil
**
*/
static VALUE timer_pause(int argv, VALUE timer)
{
	nge_timer* ptimer;
	Data_Get_Struct(timer, nge_timer, ptimer);
	ptimer->pause(ptimer);
	return Qnil;
}

/*
**	call-seq:
**		NGE.timer_unpause(timer)	-> nil
**
*/
static VALUE timer_unpause(int argv, VALUE timer)
{
	nge_timer* ptimer;
	Data_Get_Struct(timer, nge_timer, ptimer);
	ptimer->unpause(ptimer);
	return Qnil;
}

/*
** bind to ruby
**
*/

static void bind_core()
{
	mNGE = rb_define_module_under(rb_mSin, "NGE");
	
	rb_define_singleton_method(mNGE, "init", RbFunc(init_video), 0);
	rb_define_singleton_method(mNGE, "quit", RbFunc(quit_nge), 0);

	rb_define_module_function(mNGE, "limit_fps", RbFunc(limit_fps), -1);
	rb_define_module_function(mNGE, "real_fps", RbFunc(get_real_fps), 0);
	rb_define_module_function(mNGE, "begin_scene", RbFunc(begin_scene), -1);
	rb_define_module_function(mNGE, "end_scene", RbFunc(end_scene), 0);
	rb_define_module_function(mNGE, "show_fps", RbFunc(show_fps), 0);
	
	rb_define_module_function(mNGE, "begin_target", RbFunc(begin_target), 2);
	rb_define_module_function(mNGE, "end_target", RbFunc(end_target), 0);
	rb_define_module_function(mNGE, "set_clip", RbFunc(set_clip), 4);
	rb_define_module_function(mNGE, "reset_clip", RbFunc(reset_clip), 0);
	rb_define_module_function(mNGE, "draw_line", RbFunc(draw_line), 5);
	rb_define_module_function(mNGE, "draw_rect", RbFunc(draw_rect), 5);
	rb_define_module_function(mNGE, "fill_rect", RbFunc(fill_rect), 5);
	rb_define_module_function(mNGE, "fill_rect_grad", RbFunc(fill_rect_grad), 5);
	rb_define_module_function(mNGE, "render_quad", RbFunc(render_quad), 11);
	rb_define_module_function(mNGE, "draw_image", RbFunc(draw_image), 9);
	rb_define_module_function(mNGE, "draw_image_mask", RbFunc(draw_image_mask), 10);
	rb_define_module_function(mNGE, "image_to_screen", RbFunc(image_to_screen), 3);
	rb_define_module_function(mNGE, "draw_region", RbFunc(draw_region), 8);
	rb_define_module_function(mNGE, "screen_to_image", RbFunc(screen_to_image), 0);
	rb_define_module_function(mNGE, "screen_shot", RbFunc(screen_shot), 1);
	rb_define_module_function(mNGE, "set_screen_color", RbFunc(set_screen_color), 4);
	rb_define_module_function(mNGE, "set_tex_blend", RbFunc(set_tex_blend), 2);
	rb_define_module_function(mNGE, "reset_tex_blend", RbFunc(reset_tex_blend), 0);
	rb_define_module_function(mNGE, "put_pix", RbFunc(put_pix), 3);
	rb_define_module_function(mNGE, "draw_circle", RbFunc(draw_circle), 4);
	rb_define_module_function(mNGE, "fill_circle", RbFunc(fill_circle), 4);
	rb_define_module_function(mNGE, "draw_ellipse", RbFunc(draw_ellipse), 5);
	rb_define_module_function(mNGE, "fill_ellipse", RbFunc(fill_ellipse), 5);
	rb_define_module_function(mNGE, "draw_polygon", RbFunc(draw_polygon), 4);
	rb_define_module_function(mNGE, "fill_polygon", RbFunc(fill_polygon), 4);
	rb_define_module_function(mNGE, "fill_polygon_grad", RbFunc(fill_polygon_grad), 4);
	rb_define_module_function(mNGE, "reset_graphics_cache", RbFunc(reset_graphics_cache), 0);
}

static void bind_frame()
{
	VALUE mFrame = rb_define_module_under(mNGE, "Frame");

	rb_define_module_function(mFrame, "hwnd", RbFunc(get_hwnd), 0);
	rb_define_module_function(mFrame, "title", RbFunc(get_title), 0);
	rb_define_module_function(mFrame, "title=", RbFunc(set_title), 1);
	rb_define_module_function(mFrame, "width", RbFunc(get_width), 0);
	rb_define_module_function(mFrame, "width=", RbFunc(set_width), 1);
	rb_define_module_function(mFrame, "height", RbFunc(get_height), 0);
	rb_define_module_function(mFrame, "height=", RbFunc(set_height), 1);
	rb_define_module_function(mFrame, "fullscreen_start", RbFunc(get_fullscreen), 0);
	rb_define_module_function(mFrame, "fullscreen_start=", RbFunc(set_fullscreen), 1);
	rb_define_module_function(mFrame, "peek_message", RbFunc(peek_message), 0);
}

static void bind_image()
{
	mImage = rb_define_class_under(mNGE, "Image", rb_cObject);
	rb_undef_method(mImage, "new");
	rb_undef_method(mImage, "dup");
	rb_undef_method(mImage, "clone");
	//rb_define_alloc_func(mImage, alloc_img);
	
	rb_define_module_function(mNGE, "image_load", RbFunc(load_image), 1);
	rb_define_module_function(mNGE, "image_load_buf", RbFunc(load_buf_image), 2);
	rb_define_module_function(mNGE, "image_create", RbFunc(create_image), 2);
	rb_define_module_function(mNGE, "image_create_ex", RbFunc(create_image_ex), 3);
	rb_define_module_function(mNGE, "image_save", RbFunc(save_image), 4);
	rb_define_module_function(mNGE, "image_free", RbFunc(free_image), 1);
	rb_define_module_function(mNGE, "image_clone", RbFunc(clone_image), 1);
	rb_define_module_function(mNGE, "image_clear", RbFunc(clear_image), 1);
	rb_define_module_function(mNGE, "image_to_image", RbFunc(image2image), 4);
	rb_define_module_function(mNGE, "image_to_image_ex", RbFunc(image2image_ex), 9);
	rb_define_module_function(mNGE, "image_to_image_alpha", RbFunc(imsge2image_alpha), 5);
	rb_define_module_function(mNGE, "image_to_image_alpha_ex", RbFunc(image2image_alpha_ex), 10);
	//rb_define_module_function(mNGE, "rawdata_to_image", RbFunc(rawdata2image), 6);
	rb_define_module_function(mNGE, "image_fliph", RbFunc(fliph_image), 1);
	rb_define_module_function(mNGE, "image_flipv", RbFunc(flipv_image), 1);
	rb_define_module_function(mNGE, "image_fillrect", RbFunc(fillrect_image), 6);
}

static void bind_font()
{
	mFont = rb_define_class_under(mNGE, "Font", rb_cObject);
	rb_undef_method(mFont, "new");
	rb_undef_method(mFont, "dup");
	rb_undef_method(mFont, "clone");
	
	rb_define_module_function(mNGE, "create_font_hzk", RbFunc(load_font_hzk), 3);
	rb_define_module_function(mNGE, "create_font_hzk_buf", RbFunc(load_font_hzk_buf), 5);
	rb_define_module_function(mNGE, "create_font_freetype", RbFunc(load_font_freetype), 2);
	rb_define_module_function(mNGE, "create_font_freetype_buf", RbFunc(load_font_freetype_buf), 3);
	rb_define_module_function(mNGE, "font_drawtext", RbFunc(draw_font_text), 7);
	rb_define_module_function(mNGE, "font_setattr", RbFunc(set_font_attr), 3);
	rb_define_module_function(mNGE, "font_destory", RbFunc(destory_font), 1);
	rb_define_module_function(mNGE, "font_setcolor", RbFunc(set_font_color), 2);
	rb_define_module_function(mNGE, "font_setcolor_ex", RbFunc(set_font_color_ex), 4);
	rb_define_module_function(mNGE, "font_textsize", RbFunc(get_textsize), 3);
}

static void bind_particle()
{
	mParticle = rb_define_class_under(mNGE, "Particle", rb_cObject);
	rb_undef_method(mParticle, "new");
	rb_undef_method(mParticle, "dup");
	rb_undef_method(mParticle, "clone");
	
	rb_define_module_function(mNGE, "create_particle", RbFunc(create_particle), 0);
	rb_define_module_function(mNGE, "particle_free", RbFunc(free_particle), 1);
	rb_define_module_function(mNGE, "particle_set_image", RbFunc(particle_set_image), 2);
	rb_define_module_function(mNGE, "particle_set_clip", RbFunc(particle_set_clip), 5);
	rb_define_module_function(mNGE, "particle_set_center", RbFunc(particle_set_center), 3);
}

static void bind_particle_system()
{
	mParticleSystem = rb_define_class_under(mNGE, "ParticleSystem", rb_cObject);
	rb_undef_method(mParticleSystem, "new");
	rb_undef_method(mParticleSystem, "dup");
	rb_undef_method(mParticleSystem, "clone");
	
	rb_define_module_function(mNGE, "create_particle_system", RbFunc(create_particle_system), 2);
	rb_define_module_function(mNGE, "particle_system_render", RbFunc(particle_system_render), 1);
	rb_define_module_function(mNGE, "particle_system_fire", RbFunc(particle_system_fire), 1);
	rb_define_module_function(mNGE, "particle_system_fire_at", RbFunc(particle_system_fire_at), 3);
	rb_define_module_function(mNGE, "particle_system_stop", RbFunc(particle_system_stop), 1);
	rb_define_module_function(mNGE, "particle_system_update", RbFunc(particle_system_update), 2);
	rb_define_module_function(mNGE, "particle_system_move_to", RbFunc(particle_system_move_to), 4);
}

static void bind_timer()
{
	mTimer = rb_define_class_under(mNGE, "Timer", rb_cObject);
	rb_undef_method(mParticleSystem, "new");
	rb_undef_method(mParticleSystem, "dup");
	rb_undef_method(mParticleSystem, "clone");
	
	rb_define_module_function(mNGE, "create_timer", RbFunc(create_timer), 0);
	rb_define_module_function(mNGE, "timer_free", RbFunc(free_timer), 1);
	rb_define_module_function(mNGE, "get_ticks", RbFunc(get_ticks), 1);
	rb_define_module_function(mNGE, "timer_start", RbFunc(timer_start), 1);
	rb_define_module_function(mNGE, "timer_stop", RbFunc(timer_stop), 1);
	rb_define_module_function(mNGE, "timer_pause", RbFunc(timer_pause), 1);
	rb_define_module_function(mNGE, "timer_unpause", RbFunc(timer_unpause), 1);
}

static void BindNGE()
{
	bind_core();
	bind_frame();
	bind_image();
	bind_font();
	bind_particle();
	bind_particle_system();
	bind_timer();
}

namespace Sin
{
	void InitNGE()
	{
		BindNGE();
	}
}