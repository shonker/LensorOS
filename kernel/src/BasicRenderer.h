#ifndef LENSOR_OS_BASIC_RENDERER_H
#define LENSOR_OS_BASIC_RENDERER_H

#include <stddef.h>
#include "math.h"

typedef struct {
	// Magic bytes to indicate PSF1 font type	
	unsigned char Magic[2];
	unsigned char Mode;
	unsigned char CharacterSize;
} PSF1_HEADER;

typedef struct {
	PSF1_HEADER* PSF1_Header;
	void* GlyphBuffer;
} PSF1_FONT;

struct Framebuffer {
	void* BaseAddress;
	size_t BufferSize;
	unsigned int PixelWidth;
	unsigned int PixelHeight;
	unsigned int PixelsPerScanLine;
};

const unsigned int BytesPerPixel = 4;

class BasicRenderer {
public:
	Framebuffer* framebuffer     {nullptr};
	PSF1_FONT* Font              {nullptr};
	Vector2 PixelPosition        {0, 0};
	unsigned int BackgroundColor {0x00000000};

	BasicRenderer(Framebuffer* fbuffer, PSF1_FONT* f) {
		framebuffer = fbuffer;
		Font = f;
	}

	// Change every pixel in the framebuffer to BackgroundColor.
	void clear();
	
	// '\r'
	void cret();
	// '\n'
	void newl();
	// '\r' + '\n'
	void crlf();
	// Use font to put a character to the screen.
	void putchar(char c, unsigned int color = 0xffffffff);
	// Put a string of characters to the screen, wrapping if necessary.
	void putstr(const char* str, unsigned int color = 0xffffffff);
	void putrect(Vector2 size, unsigned int color = 0xffffffff);
};

#endif
