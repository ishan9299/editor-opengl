#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <stdio.h>
#include <stdint.h>
#include <windows.h>

#define GAP_BUFFER_MIN_SIZE 256
#define MAX(a, b) (a > b)? a : b

struct GapBuffer {
    char   *buffer;
    size_t bufferSize;
    size_t cursorPos;
    size_t gapStart;
    size_t gapEnd;
    size_t gapSize;
};

void gapBufferInit(struct GapBuffer *gb, size_t init);
void gapBufferFree(struct GapBuffer *gb);
void gapBufferInsertChar(struct GapBuffer *gb, char c);
void gapBufferMoveGapToCursor(struct GapBuffer *gb);
uint8_t gapBufferShiftCursorLeft(struct GapBuffer *gb);
uint8_t gapBufferShiftCursorRight(struct GapBuffer *gb);
void gapBufferShiftCursorUp(struct GapBuffer *gb);
void gapBufferShiftCursorDown(struct GapBuffer *gb);
void gapBufferShiftCursorStartOfLine(struct GapBuffer *gb);
void gapBufferShiftCursorEndOfLine(struct GapBuffer *gb);
void gapBufferDeleteChar(struct GapBuffer *gb);
uint8_t gapBufferBackspaceChar(struct GapBuffer *gb);
void gapBufferDebug(struct GapBuffer *gb);
void gapBufferGetString(struct GapBuffer *gb, char *str, size_t strSize);

inline size_t gapBufferLength(struct GapBuffer *gb);
inline size_t gapBufferGapSize(struct GapBuffer *gb);
inline size_t gapBufferCurrentIndex(struct GapBuffer *gb);
inline char gapBufferCurrentCharacter(struct GapBuffer *gb);
inline char gapBufferCursorPrevChar(struct GapBuffer *gb);
inline char gapBufferCursorNextChar(struct GapBuffer *gb);

#ifdef GAP_BUFFER_IMPLEMENTATION

void gapBufferInit(struct GapBuffer *gb, size_t size)
{
    gb->bufferSize = MAX(size, GAP_BUFFER_MIN_SIZE);
    gb->buffer = (char *)HeapAlloc(GetProcessHeap(), 0, gb->bufferSize);
    gb->cursorPos = 0;
    gb->gapStart = 0;
    gb->gapEnd = gb->bufferSize;
}

void gapBufferFree(struct GapBuffer *gb)
{
    HeapFree(GetProcessHeap(), 0, gb->buffer);
    gb->buffer = 0;
}

void gapBufferInsertChar(struct GapBuffer *gb, char c)
{
    if (gb->gapStart == gb->gapEnd)
    {
        gb->gapStart = gb->bufferSize;
        gb->bufferSize = gb->bufferSize*2;
        gb->gapEnd = gb->bufferSize;
        gb->buffer = (char *)HeapReAlloc(GetProcessHeap(), 0,
                                         gb->buffer, gb->bufferSize);
    }
    gapBufferMoveGapToCursor(gb);
    gb->buffer[gb->gapStart] = c;
    gb->gapStart++;
    gb->cursorPos++;
}

void gapBufferMoveGapToCursor(struct GapBuffer *gb)
{
    size_t gapSize = gapBufferGapSize(gb);
    if (gb->cursorPos < gb->gapStart)
    {
        size_t gapDelta = gb->gapStart - gb->cursorPos;
        gb->gapStart -= gapDelta;
        gb->gapEnd -= gapDelta;
        MoveMemory(gb->buffer + gb->gapEnd,
                   gb->buffer + gb->gapStart, gapDelta);
    }
    else if (gb->cursorPos > gb->gapStart)
    {
        size_t gapDelta = gb->cursorPos - gb->gapStart;
        MoveMemory(gb->cursorPos + gb->buffer,
                   gb->buffer + gb->gapStart, gapSize);
        gb->gapStart += gapDelta;
        gb->gapEnd += gapDelta;
    }
}

uint8_t gapBufferShiftCursorLeft(struct GapBuffer *gb)
{
    if (gb->cursorPos > 0)
    {
        gb->cursorPos--;
        return 1;
    }
    return 0;
}

uint8_t gapBufferShiftCursorRight(struct GapBuffer *gb)
{
    if (gb->cursorPos < gapBufferLength(gb))
    {
        gb->cursorPos++;
        return 1;
    }
    return 0;
}

void gapBufferShiftCursorStartOfLine(struct GapBuffer *gb)
{
    while (gb->cursorPos > 0)
    {
        gb->cursorPos--;
        char currentChar = gapBufferCurrentCharacter(gb);
        if (currentChar == '\n')
        {
            gapBufferShiftCursorRight(gb);
            break;
        }
    }
}

void gapBufferShiftCursorEndOfLine(struct GapBuffer *gb)
{
    size_t bufferLength = gapBufferLength(gb);
    while (gb->cursorPos < bufferLength)
    {
        gb->cursorPos++;
        char currentChar = gapBufferCurrentCharacter(gb);
        if (currentChar == '\n')
        {
            gapBufferShiftCursorLeft(gb);
            break;
        }
    }
}

void gapBufferDeleteChar(struct GapBuffer *gb)
{
    if (gb->cursorPos < gb->bufferSize)
    {
        gapBufferMoveGapToCursor(gb);
        gb->gapEnd++;
    }
}

uint8_t gapBufferBackspaceChar(struct GapBuffer *gb)
{
    if (gb->cursorPos > 0)
    {
        gapBufferMoveGapToCursor(gb);
        gb->gapStart--;
        gapBufferShiftCursorLeft(gb);
        return 1;
    }
    return 0;
}

void gapBufferDebug(struct GapBuffer *gb)
{
    for (size_t i = 0; i < gb->bufferSize; i++)
    {
        if (i < gb->gapStart || i > (gb->gapEnd - 1))
        {
            printf("%c", gb->buffer[i]);
        }
    }
    printf("\n");
}

void gapBufferGetString(struct GapBuffer *gb, char *str)
{
    CopyMemory(str, gb->buffer, gb->gapStart);
    CopyMemory(str + gb->gapStart, gb->buffer + gb->gapEnd,
               gb->bufferSize - gb->gapEnd);
}

inline size_t gapBufferLength(struct GapBuffer *gb)
{
    size_t length = gb->bufferSize - gapBufferGapSize(gb);
    return length;
}

inline size_t gapBufferGapSize(struct GapBuffer *gb)
{
    size_t gapSize = gb->gapEnd - gb->gapStart;
    return gapSize;
}

inline size_t gapBufferCurrentIndex(struct GapBuffer *gb)
{
    size_t index = (gb->cursorPos < gb->gapStart)? gb->cursorPos + gb->gapSize : gb->cursorPos;

    return index;
}

inline char gapBufferCurrentCharacter(struct GapBuffer *gb)
{
    return gb->buffer[gapBufferCurrentIndex(gb)];
}

inline char gapBufferCursorPrevChar(struct GapBuffer *gb)
{
    return gb->buffer[gapBufferCurrentIndex(gb) - 1];
}


#endif // GAP_BUFFER_IMPLEMENTATION

#endif // GAP_BUFFER_H

