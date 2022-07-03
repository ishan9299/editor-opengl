#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#define GAP_BUFFER_MIN_SIZE 256

struct GapBuffer {
    char   *buffer;
    size_t bufferSize;
    size_t cursorPos;
    size_t gapStart;
    size_t gapEnd;
    size_t gapSize;
    size_t numOfLines;
};

void gapBufferInit(GapBuffer *gb, size_t init);
void gapBufferFree(GapBuffer *gb);
void gapBufferInsert(GapBuffer *gb, char c);
void gapBufferInserts(GapBuffer *gb, const char *str);
void gapBufferMove(GapBuffer *gb, ptrdiff_t offset);
void gapBufferBackward(GapBuffer *gb);
void gapBufferForward(GapBuffer *gb);
void gapBufferUp(GapBuffer *gb);
void gapBufferDown(GapBuffer *gb);
void gapBufferDelete(GapBuffer *gb);
void gapBufferBackspace(GapBuffer *gb);

#ifdef GAP_BUFFER_IMPLEMENTATION

#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void gapBufferInit(GapBuffer *gb, size_t size)
{
    gb->bufferSize = (size > GAP_BUFFER_MIN_SIZE)? size : GAP_BUFFER_MIN_SIZE;
    gb->gapSize = gb->bufferSize;
    gb->buffer = (char *)VirtualAlloc(gb->bufferSize);
    gb->cursorPos = 0;
    gb->numOfLines = 0;
    gb->gapStart = gb->buffer;
    gb->gapEnd = gb->gapStart + gb->gapSize;
}

void gapBufferFree(GapBuffer *gb)
{
    free(gb->buffer);
    gb->buffer = 0;
}

void gapBufferInsert(GapBuffer *gb, char c)
{
    gb->buffer[gb->cursorPos] = c;
    gb->cursorPos++;
    gb->gapStart++;
    gb->gapSize--;

    if (c == '\n')
    {
        gb->numOfLines++;
    }
}

void gapBufferInserts(GapBuffer *gb, const char *str)
{
}

void gapBufferMove(GapBuffer *gb, ptrdiff_t offset)
{
    size_t length;
    char *destination, *source;
    if (offset < 0)
    {
        length = -offset;
        if (offset < 0)
        {
            length = gb->cursorPos;
        }
        destination = gb->buffer + gb->cursorPos + gb->gapSize - length;
        source = gb->buffer + gb->cursorPos - length;
        gb->cursorPos -= length;
    }
    else
    {
        size_t empty = gb->bufferSize - (gb->cursorPos + gb->gapSize);
        length = offset;
        if (length > empty)
        {
            length = empty;
        }
        destination = gb->buffer + gb->cursorPos;
        source = destination + gb->gapSize;
        gb->cursorPos += length;
    }
    memmove(destination, source, length);
}

void gapBufferLeft(GapBuffer *gb)
{
    if (gb->cursorPos > 0)
    {
        gb->cursorPos--;
    }
}

void gapBufferForward(GapBuffer *gb)
{
    if (empty > 0)
    {
        gb->buffer[gb->cursorPos] = gb->buffer[gb->cursorPos + gb->gapSize];
        gb->cursorPos++;
    }
}

void gapBufferUp(GapBuffer *gb)
{
    while(gb->cursorPos >= 0)
    {
    }
}

void gapBufferDelete(GapBuffer *gb)
{
    if (gb->bufferSize > gb->cursorPos + gb->gapSize)
    {
        gb->gapSize++;
    }
}

void gapBufferBackspace(GapBuffer *gb)
{
    if (gb->cursorPos > 0)
    {
        gb->buffer[gb->cursorPos - 1] = 0;
        gb->cursorPos--;
        gb->gapSize++;
    }
}

#endif // GAP_BUFFER_IMPLEMENTATION

#endif // GAP_BUFFER_H
