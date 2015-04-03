#include "sftd.h"
#include <sf2d.h>
#include <ft2build.h>
#include FT_FREETYPE_H

static int sftd_initialized = 0;
static FT_Library ftlibrary;

struct sftd_font {
	FT_Face face;
};

int sftd_init()
{
	if (sftd_initialized) return 2;

	FT_Error error = FT_Init_FreeType(&ftlibrary);
	if (error != FT_Err_Ok) {
		return 0;
	}

	sftd_initialized = 1;
	return 1;
}

int sftd_fini()
{
	if (!sftd_initialized) return 2;

	FT_Error error = FT_Done_FreeType(ftlibrary);
	if (error != FT_Err_Ok) {
		return 0;
	}

	sftd_initialized = 0;
	return 1;
}

sftd_font *sftd_load_font_file(const char *pathname)
{
	FT_Face face;
	FT_Error error = FT_New_Face(
		ftlibrary,
		pathname,
		0,
		&face);

	if (error != FT_Err_Ok) {
		return NULL;
	}

	sftd_font *font = malloc(sizeof(*font));
	font->face = face;

	return font;
}

sftd_font *sftd_load_font_mem(const void *buffer, unsigned int size)
{
	FT_Face face;
	FT_Error error = FT_New_Memory_Face(
		ftlibrary,
		buffer,
		size,
		0,
		&face);

	if (error != FT_Err_Ok) {
		return NULL;
	}

	sftd_font *font = malloc(sizeof(*font));
	font->face = face;

	return font;
}

void sftd_free_font(sftd_font *font)
{
	if (font) {
		FT_Done_Face(font->face);
		free(font);
	}
}

void sftd_draw_text(sftd_font *font, int x, int y, unsigned int color, unsigned int size, const char *text)
{
	FT_Face face = font->face;
	FT_GlyphSlot slot = face->glyph;
	int pen_x, pen_y, i, n;
	FT_Error error;
	FT_UInt glyph_index;

	FT_Set_Pixel_Sizes(face, 0, size);

	pen_x = x;
	pen_y = y;
	n = strlen(text);

	for (i = 0; i < n; i++) {
		glyph_index = FT_Get_Char_Index(face, text[i]);
		
		error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
		if (error) continue;

		error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		if (error) continue;

		FT_Bitmap *bitmap = &slot->bitmap;

		//This is too ugly
		sf2d_texture *tex = sf2d_create_texture(bitmap->width, bitmap->rows, GPU_RGBA8, SF2D_PLACE_TEMP);
		
		int j, k;
		for (j = 0; j < bitmap->rows; j++) {
			for (k = 0; k < bitmap->width; k++) {
				((u32 *)tex->data)[j*tex->pow2_w + k] = __builtin_bswap32((color & ~0xFF) | bitmap->buffer[j*bitmap->width + k]);
			}
		}

		sf2d_texture_tile32(tex);
		sf2d_draw_texture(tex, pen_x + slot->bitmap_left, pen_y + slot->bitmap_top);
		sf2d_free_texture(tex);
		
		pen_x += (slot->advance.x >> 6);
		pen_y += 0;
	}
}
