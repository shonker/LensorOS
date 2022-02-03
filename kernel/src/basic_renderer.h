#ifndef LENSOR_OS_BASIC_RENDERER_H
#define LENSOR_OS_BASIC_RENDERER_H

#include <stddef.h>
#include <stdint.h>
#include "cstr.h"
#include "math.h"
#include "memory.h"

struct PSF1_HEADER {
    // Magic bytes to indicate PSF1 font type   
    u8 Magic[2];
    u8 Mode;
    u8 CharacterSize;
};

struct PSF1_FONT {
    PSF1_HEADER* PSF1_Header;
    void* GlyphBuffer;
};

struct Framebuffer {
    void* BaseAddress;
    size_t BufferSize;
    u32 PixelWidth;
    u32 PixelHeight;
    u32 PixelsPerScanLine;

    Framebuffer() {
        BaseAddress = nullptr;
        BufferSize = 0;
        PixelWidth = 0;
        PixelHeight = 0;
        PixelsPerScanLine = 0;
    }
};

const u32 BytesPerPixel = 4;

class BasicRenderer {
public:
    // Target = framebuffer to draw to in memory
    Framebuffer* Render          {nullptr};
    Framebuffer* Target          {nullptr};
    PSF1_FONT*   Font            {nullptr};
    uVector2      DrawPos        {0, 0};
    u32 BackgroundColor {0x00000000};

    BasicRenderer() {}
    BasicRenderer(Framebuffer* render, Framebuffer* target, PSF1_FONT* f) {
        Render = render;
        Target = target;
        Font = f;
    }

    /// ENSURE DRAW POSITION IS WITHIN FRAMEBUFFER.
    void clamp_draw_position();

    /// UPDATE MEMORY CONTENTS OF RENDER FROM TARGET
    inline void swap() {
        memcpy(Target->BaseAddress, Render->BaseAddress, Render->BufferSize);
    }

    /// UPDATE SIZE of MEMORY CONTENTS OF RENDER FROM TARGET AT POSITION
    void swap(uVector2 position, uVector2 size);

    void readpix(uVector2 size, u32* buffer);
    
    /// Change every pixel in the target framebuffer to BackgroundColor.
    void clear() {
        // TODO: make more efficient (set first line, memcpy that to each line?)
        // Draw background color to every pixel.
        u32* pixel_ptr = (u32*)Target->BaseAddress;
        for (u64 y = 0; y < Target->PixelHeight; y++) {
            for (u64 x = 0; x < Target->PixelWidth; x++) {
                *(u32*)(pixel_ptr + x + (y * Target->PixelsPerScanLine)) = BackgroundColor;
            }
        }
        // Re-initialize draw position.
        DrawPos = {0, 0};
    }
    /// Update BackgroundColor to given color, then clear screen.
    void clear(u32 color) {
        BackgroundColor = color;
        clear();
    }

    void clear(uVector2 position, uVector2 size);
    void clear(uVector2 position, uVector2 size, u32 color);

    // Remove a single character behind DrawPos.
    void clearchar();
    
    // '\r'
    void cret();
    // '\n'
    void newl();
    // '\r' + '\n'
    void crlf();
    void crlf(u32 offset);

    // Draw `size` of rectangle as `color`.
    void drawrect(uVector2 size, u32 color = 0xffffffff);
    // Draw `size` of `pixels` buffer into target framebuffer.
    void drawpix(uVector2 size, u32* pixels);
    // Draw `size` of `bitmap` as `color`.
    void drawbmp(uVector2 size, u8* bitmap, u32 color = 0xffffffff);
    // Draw `size` of `bitmap` as `color`, but don't clear `0` to background color.
    // This allows the use of bitmaps acting on alpha as well as color.
    void drawbmpover(uVector2 size, u8* bitmap, u32 color = 0xffffffff);
    // Use PSF1 bitmap font to draw a character to the screen (don't advance).
    void drawchar(char c, u32 color = 0xffffffff);
    void drawcharover(char c, u32 color = 0xffffffff);
    
    // Use font to put a character to the screen (advance draw position).
    void putchar(char c, u32 color = 0xffffffff);
    // Put a null-terminated string of characters to the screen, wrapping if necessary.
    void puts(const char* str, u32 color = 0xffffffff);
};

extern BasicRenderer gRend;

#endif
