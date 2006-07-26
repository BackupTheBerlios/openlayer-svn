/*
 * glyph_main.c  -  Glyph Keeper main routines
 * (initialization/cleanup, rendering text).
 *
 * Copyright (c) 2003-2005 Kirill Kryukov
 *
 * This file is part of Glyph Keeper library, and may only be used,
 * modified, and distributed under the terms of the Glyph Keeper
 * license, located in the file 'license.txt' within this package.
 */

#ifdef included_from_glyph_c


/*
 * Init/cleanup functions
 */

void gk_library_init()
{
    int error;
    FT_Int amajor,aminor,apatch;

    if (ftlib) return;

#ifdef GLYPH_LOG
    if (!glyph_log)
    {
        glyph_log = fopen("glyph.log","w");
        if (!glyph_log)
        {
            _gk_msg("Error: gk_library_init(): Can't create a log-file \"glyph.log\"\n");
            exit(1);
        }

        /* It's a log-file, nothing should be lost. */
        setbuf(glyph_log,0);
        fprintf(glyph_log,"glyph.log started\n");
    }
#endif

    error = FT_Init_FreeType(&ftlib);
    if (error || !ftlib) { _gk_msg("Error: gk_library_init(): can't initialize FreeType\n"); ftlib = 0; return; }
    FT_Library_Version(ftlib,&amajor,&aminor,&apatch);
    _gk_msg("FreeType %d.%d.%d initialized\n",amajor,aminor,apatch);

    FT_Bitmap_New(&_gk_workout_bitmap);

    _gk_install_exit_handler();
}


void gk_library_cleanup(void)
{
    while (first_renderer) gk_done_renderer(first_renderer);
    while (first_keeper) gk_done_keeper(first_keeper);
    while (first_face) gk_unload_face(first_face);

    if (ftlib)
    {
        if (_gk_workout_bitmap.buffer) { FT_Bitmap_Done(ftlib,&_gk_workout_bitmap); }
        FT_Done_FreeType(ftlib); ftlib = 0;
    }

    if (rle_buffer) { free(rle_buffer); rle_buffer = 0; }
    rle_buffer_size = 0;

#ifdef GLYPH_LOG
    if (glyph_log) { fclose(glyph_log); glyph_log = 0; }
#endif
}


/*
 * Checks how many bytes are allocated by Glyph Keeper.
 * (FreeType memory is not counted)
 */
int gk_bytes_allocated()
{
    int n = 0;
    GLYPH_FACE *a;
    GLYPH_KEEP *b;
    GLYPH_REND *c;

    for (a = first_face; a; a=a->next) n += a->allocated;
    for (b = first_keeper; b; b=b->next) n += b->allocated;
    for (c = first_renderer; c; c=c->next) n += sizeof(GLYPH_REND);
    n += rle_buffer_size;
    return n;
}


/*
 * Precaching.
 */ 
void gk_precache_char(GLYPH_REND* const rend,const unsigned unicode)
{
    GLYPH *glyph;

    if (!rend || !rend->index) return;

    glyph = _gk_rend_render(rend,unicode);
    if (glyph && !glyph->index) _gk_unload_glyph(glyph);
}


void gk_precache_range(GLYPH_REND* const rend,
    const unsigned range_start,const unsigned range_end)
{
    GLYPH *glyph;
    unsigned code;

    if (!rend || !rend->index) return;

    for (code=range_start; code<=range_end; code++)
    {
        glyph = _gk_rend_render(rend,code);
        if (glyph && !glyph->index) _gk_unload_glyph(glyph);
    }
}


void gk_precache_set_utf8(GLYPH_REND* const rend,const char* const char_set)
{
    GLYPH *glyph;
    const char* c = char_set;
    unsigned code;

    if (!rend || !rend->index) return;

    while ( (code = _gk_utf8_getx(&c)) )
    {
        glyph = _gk_rend_render(rend,code);
        if (glyph && !glyph->index) _gk_unload_glyph(glyph);
    }
}


void gk_precache_set_utf16(GLYPH_REND* const rend,const unsigned short* const char_set)
{
    GLYPH *glyph;
    const unsigned short* c = char_set;
    unsigned code;

    if (!rend || !rend->index) return;

    _gk_utf16_start_decoding(&c);
    while ( (code = _gk_utf16_decode(&c)) )
    {
        glyph = _gk_rend_render(rend,code);
        if (glyph && !glyph->index) _gk_unload_glyph(glyph);
    }
}


void gk_precache_set_utf32(GLYPH_REND* const rend,const unsigned* const char_set)
{
    GLYPH *glyph;
    const unsigned* c = char_set;
    unsigned code;

    if (!rend || !rend->index) return;

    _gk_utf32_start_decoding(&c);
    while ( (code = _gk_utf32_decode(&c)) )
    {
        glyph = _gk_rend_render(rend,code);
        if (glyph && !glyph->index) _gk_unload_glyph(glyph);
    }
}


/*
 * Draws background rectangle.
 * adv_x and adv_y should not be both zero.
 */
#ifdef GLYPH_TARGET_HAS_RECTFILL
static void _gk_draw_background_rectangle(GLYPH_TARGET_SURFACE* const bmp,GLYPH_REND* const rend,
  const int pen_x,const int pen_y,const int adv_x,const int adv_y)
{
    if (rend->text_angle != 0)
    {
#ifdef GLYPH_TARGET_HAS_RECTFILL_ANGLED
        int ascender = gk_rend_ascender(rend);
        int descender = gk_rend_descender(rend);
        int x1 = pen_x - (int)(sin(rend->text_angle)*(ascender>>6));
        int y1 = pen_y - (int)(cos(rend->text_angle)*(ascender>>6));
        int x2 = x1 + ((adv_x+63)>>6);
        int y2 = y1 + ((adv_y+63)>>6);
        int x4 = pen_x - (int)(sin(rend->text_angle)*(descender>>6));
        int y4 = pen_y - (int)(cos(rend->text_angle)*(descender>>6));
        int x3 = x4 + ((adv_x+63)>>6);
        int y3 = y4 + ((adv_y+63)>>6);
        _gk_driver_rectfill_angled(bmp,x1,y1,x2,y2,x3,y3,x4,y4,rend->back_color);
#endif
    }
    else
    {
        int ascender = gk_rend_ascender(rend);
        int descender = gk_rend_descender(rend);
        _gk_driver_rectfill(bmp,pen_x,pen_y-(ascender>>6),pen_x+((adv_x+63)>>6),pen_y-(descender>>6),rend->back_color);
    }
}
#endif


void gk_put_char(GLYPH_TARGET_SURFACE* const bmp,GLYPH_REND* const rend,
    const unsigned unicode,const int x,const int y)
{
    GLYPH *glyph = 0;
    if (!bmp || !rend) return;
    glyph = _gk_rend_render(rend,unicode);
    if (glyph)
    {
        _gk_prepare_to_draw(bmp,rend);
        _gk_put_glyph(glyph,x,y);
        _gk_done_drawing();
    }
}

void gk_put_char_center(GLYPH_TARGET_SURFACE* const bmp,GLYPH_REND* const rend,const unsigned unicode,
    const int x,const int y)
{
    GLYPH *glyph;
    if (!bmp || !rend) return;
    glyph = _gk_rend_render(rend,unicode);
    if (glyph)
    {
        _gk_prepare_to_draw(bmp,rend);
        _gk_put_glyph(glyph,x-glyph->width/2,y-glyph->height/2);
        _gk_done_drawing();
    }
}





/*
 * 'rend' must be not 0.
 * 'pen_x' and 'pen_y' are expressed in 26.6 pixels here.
 */
static void _gk_internal_render_char(GLYPH_REND* const rend,const unsigned unicode,int *pen_x,int *pen_y)
{
    GLYPH* glyph;
    int adv_x,adv_y;
    CARE(rend);

    glyph = _gk_rend_render(rend,unicode);
    if (!glyph) return;
    adv_x = glyph->advance_x;
    adv_y = glyph->advance_y;
    _gk_put_glyph(glyph,((*pen_x+31)>>6)+glyph->left,((*pen_y+31)>>6)-glyph->top);
    *pen_x += adv_x;
    *pen_y -= adv_y;
}





void gk_render_char(GLYPH_TARGET_SURFACE* const bmp,
    GLYPH_REND* const rend,const unsigned unicode,
    int* const pen_x,int* const pen_y)
{
    if (!bmp || !rend || !pen_x || !pen_y) return;

    _gk_prepare_to_draw(bmp,rend);

#ifdef GLYPH_TARGET_HAS_RECTFILL
    if (rend->back_color >= 0)
    {
        int ax,ay;
        gk_char_advance(rend,unicode,&ax,&ay);
        if (ax || ay) _gk_draw_background_rectangle(bmp,rend,*pen_x,*pen_y,ax,ay);
    }
#endif

    _gk_internal_render_char(rend,unicode,pen_x,pen_y);
    _gk_done_drawing();
}


void gk_render_char_center(GLYPH_TARGET_SURFACE* const bmp,
    GLYPH_REND* const rend,const unsigned unicode,
    const int x,const int y)
{
    GLYPH *glyph;
    if (!bmp || !rend) return;
    glyph = _gk_rend_render(rend,unicode);
    if (glyph)
    {
        int asc = gk_rend_ascender_pixels(rend);
        _gk_prepare_to_draw(bmp,rend);
        _gk_put_glyph(glyph,x-glyph->width/2,y - rend->text_height_pixels/2 + asc - glyph->top);
        _gk_done_drawing();
    }
}





void gk_render_line_utf8(GLYPH_TARGET_SURFACE* const bmp,
    GLYPH_REND* const rend,const char* const text,
    const int pen_x,const int pen_y)
{
    int x,y;
    const char* c = text;
    unsigned code;

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"gk_render_line_utf8() begin\n");
#endif

    if (!bmp || !rend || !text) return;

    _gk_prepare_to_draw(bmp,rend);

#ifdef GLYPH_TARGET_HAS_RECTFILL
    if (rend->back_color >= 0)
    {
        int ax,ay;
        gk_text_advance_subpixel_utf8(rend,text,&ax,&ay);
        if (ax || ay) _gk_draw_background_rectangle(bmp,rend,pen_x,pen_y,ax,ay);
    }
#endif

    x = pen_x<<6;
    y = pen_y<<6;
    while ( (code = _gk_utf8_getx(&c)) )
        _gk_internal_render_char(rend,code,&x,&y);
    _gk_done_drawing();

#ifdef GLYPH_LOG
    if (glyph_log) fprintf(glyph_log,"gk_render_line_utf8() end\n");
#endif
}


void gk_render_line_utf16(GLYPH_TARGET_SURFACE* const bmp,
    GLYPH_REND* const rend,const unsigned short* const text,
    const int pen_x,const int pen_y)
{
    int x,y;
    const unsigned short* c = text;
    unsigned code;

    if (!bmp || !rend || !text) return;

    _gk_prepare_to_draw(bmp,rend);

#ifdef GLYPH_TARGET_HAS_RECTFILL
    if (rend->back_color >= 0)
    {
        int ax,ay;
        gk_text_advance_subpixel_utf16(rend,text,&ax,&ay);
        if (ax || ay) _gk_draw_background_rectangle(bmp,rend,pen_x,pen_y,ax,ay);
    }
#endif

    _gk_utf16_start_decoding(&c);
    x = pen_x<<6;
    y = pen_y<<6;
    while ( (code = _gk_utf16_decode(&c)) )
        _gk_internal_render_char(rend,code,&x,&y);
    _gk_done_drawing();
}


void gk_render_line_utf32(GLYPH_TARGET_SURFACE* const bmp,
  GLYPH_REND* const rend,const unsigned* const text,
  const int pen_x,const int pen_y)
{
    int x,y;
    const unsigned* c = text;
    unsigned code;

    if (!bmp || !rend || !text) return;

    _gk_prepare_to_draw(bmp,rend);

#ifdef GLYPH_TARGET_HAS_RECTFILL
    if (rend->back_color >= 0)
    {
        int ax,ay;
        gk_text_advance_subpixel_utf32(rend,text,&ax,&ay);
        if (ax || ay) _gk_draw_background_rectangle(bmp,rend,pen_x,pen_y,ax,ay);
    }
#endif

    _gk_utf32_start_decoding(&c);
    x = pen_x<<6;
    y = pen_y<<6;
    while ( (code = _gk_utf32_decode(&c)) )
        _gk_internal_render_char(rend,code,&x,&y);
    _gk_done_drawing();
}


#endif  /* included_from_glyph_c */
