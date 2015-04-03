/**
 * @file sftd.h
 * @author Sergi Granell (xerpi)
 * @date 2 April 2015
 * @brief sftdlib header
 */

#ifndef SFTD_H
#define SFTD_H

#include <3ds.h>

#ifdef __cplusplus
extern "C" {
#endif

// Defines

// Enums

// Structs

typedef struct sftd_font sftd_font;

// Basic functions
int sftd_init();
int sftd_fini();

sftd_font *sftd_load_font_file(const char *pathname);
sftd_font *sftd_load_font_mem(const void *buffer, unsigned int size);
void sftd_free_font(sftd_font *font);

// Draw functions
void sftd_draw_text(sftd_font *font, int x, int y, unsigned int color, unsigned int size, const char *text);

#ifdef __cplusplus
}
#endif

#endif
