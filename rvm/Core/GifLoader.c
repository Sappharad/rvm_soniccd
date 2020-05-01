//
//  GifLoader.c
//  rvm
//

#include "GifLoader.h"

const int LOADING_IMAGE = 0;
const int LOAD_COMPLETE = 1;
const int LZ_MAX_CODE = 4095;
const int LZ_BITS = 12;
const int FLUSH_OUTPUT = 4096;
const int FIRST_CODE = 4097;
const int NO_SUCH_CODE = 4098;
const int HT_SIZE = 8192;
const int HT_KEY_MASK = 8191;

struct GifDecoder gifDecoder;
int codeMasks[] = {
    0,
    1,
    3,
    7,
    15,
    31,
    63,
    127,
    255,
    511,
    1023,
    2047,
    4095
};

int GifLoader_ReadGifCode(void);
uint8_t GifLoader_ReadGifByte(void);
uint8_t GifLoader_TracePrefix(uint32_t* prefix, int code, int clearCode);

void Init_GifDecoder()
{
    uint8_t b = FileIO_ReadByte();
    int num = (int)b;
    int num2 = num;
    gifDecoder.fileState = LOADING_IMAGE;
    gifDecoder.position = 0;
    gifDecoder.bufferSize = 0;
    gifDecoder.buffer[0] = 0;
    gifDecoder.depth = num2;
    gifDecoder.clearCode = 1 << num2;
    gifDecoder.eofCode = gifDecoder.clearCode + 1;
    gifDecoder.runningCode = gifDecoder.eofCode + 1;
    gifDecoder.runningBits = num2 + 1;
    gifDecoder.maxCodePlusOne = 1 << gifDecoder.runningBits;
    gifDecoder.stackPtr = 0;
    gifDecoder.prevCode = NO_SUCH_CODE;
    gifDecoder.shiftState = 0;
    gifDecoder.shiftData = 0u;
    for (int i = 0; i <= LZ_MAX_CODE; i++)
    {
        gifDecoder.prefix[i] = (uint8_t)NO_SUCH_CODE;
    }
}
void GifLoader_ReadGifPictureData(int width, int height, bool interlaced, uint8_t* gfxData, int offset)
{
    int array[] = {
        0,
        4,
        2,
        1
    };
    int array2[] = {
        8,
        8,
        4,
        2
    };
    Init_GifDecoder();
    if (interlaced)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = array[i]; j < height; j += array2[i])
            {
                GifLoader_ReadGifLine(gfxData, width, j * width + offset);
            }
        }
        return;
    }
    for (int j = 0; j < height; j++)
    {
        GifLoader_ReadGifLine(gfxData, width, j * width + offset);
    }
}
void GifLoader_ReadGifLine(uint8_t* line, int length, int offset)
{
    int i = 0;
    int num = gifDecoder.stackPtr;
    int eofCode = gifDecoder.eofCode;
    int clearCode = gifDecoder.clearCode;
    int num2 = gifDecoder.prevCode;
    if (num != 0)
    {
        while (num != 0)
        {
            if (i >= length)
            {
                break;
            }
            line[offset++] = gifDecoder.stack[--num];
            i++;
        }
    }
    while (i < length)
    {
        int num3 = GifLoader_ReadGifCode();
        if (num3 == eofCode)
        {
            if (i != length - 1 | gifDecoder.pixelCount != 0u)
            {
                return;
            }
            i++;
        }
        else
        {
            if (num3 == clearCode)
            {
                for (int j = 0; j <= LZ_MAX_CODE; j++)
                {
                    gifDecoder.prefix[j] = NO_SUCH_CODE;
                }
                gifDecoder.runningCode = gifDecoder.eofCode + 1;
                gifDecoder.runningBits = gifDecoder.depth + 1;
                gifDecoder.maxCodePlusOne = 1 << gifDecoder.runningBits;
                num2 = (gifDecoder.prevCode = NO_SUCH_CODE);
            }
            else
            {
                if (num3 < clearCode)
                {
                    line[offset] = (uint8_t)num3;
                    offset++;
                    i++;
                }
                else
                {
                    if (num3 < 0 | num3 > LZ_MAX_CODE)
                    {
                        return;
                    }
                    int num4;
                    if (gifDecoder.prefix[num3] == NO_SUCH_CODE)
                    {
                        if (num3 != gifDecoder.runningCode - 2)
                        {
                            return;
                        }
                        num4 = num2;
                        gifDecoder.suffix[gifDecoder.runningCode - 2] = (gifDecoder.stack[num++] = GifLoader_TracePrefix(gifDecoder.prefix, num2, clearCode));
                    }
                    else
                    {
                        num4 = num3;
                    }
                    int j = 0;
                    while (j++ <= LZ_MAX_CODE && num4 > clearCode && num4 <= LZ_MAX_CODE)
                    {
                        gifDecoder.stack[num++] = gifDecoder.suffix[num4];
                        num4 = (int)gifDecoder.prefix[num4];
                    }
                    if (j >= LZ_MAX_CODE | num4 > LZ_MAX_CODE)
                    {
                        return;
                    }
                    gifDecoder.stack[num++] = (uint8_t)num4;
                    while (num != 0 && i < length)
                    {
                        line[offset++] = gifDecoder.stack[--num];
                        i++;
                    }
                }
                if (num2 != NO_SUCH_CODE)
                {
                    if (gifDecoder.runningCode < 2 | gifDecoder.runningCode > FIRST_CODE)
                    {
                        return;
                    }
                    gifDecoder.prefix[gifDecoder.runningCode - 2] = (uint32_t)num2;
                    if (num3 == gifDecoder.runningCode - 2)
                    {
                        gifDecoder.suffix[gifDecoder.runningCode - 2] = GifLoader_TracePrefix(gifDecoder.prefix, num2, clearCode);
                    }
                    else
                    {
                        gifDecoder.suffix[gifDecoder.runningCode - 2] = GifLoader_TracePrefix(gifDecoder.prefix, num3, clearCode);
                    }
                }
                num2 = num3;
            }
        }
    }
    gifDecoder.prevCode = num2;
    gifDecoder.stackPtr = num;
}

int GifLoader_ReadGifCode()
{
    while (gifDecoder.shiftState < gifDecoder.runningBits)
    {
        uint8_t b = GifLoader_ReadGifByte();
        gifDecoder.shiftData |= (uint32_t)((uint32_t)b << gifDecoder.shiftState);
        gifDecoder.shiftState += 8;
    }
    int result = (int)((unsigned long)gifDecoder.shiftData & (unsigned long)(codeMasks[gifDecoder.runningBits]));
    gifDecoder.shiftData >>= gifDecoder.runningBits;
    gifDecoder.shiftState -= gifDecoder.runningBits;
    if (++gifDecoder.runningCode > gifDecoder.maxCodePlusOne && gifDecoder.runningBits < LZ_BITS)
    {
        gifDecoder.maxCodePlusOne <<= 1;
        gifDecoder.runningBits++;
    }
    return result;
}

uint8_t GifLoader_ReadGifByte()
{
    char c = '\0';
    if (gifDecoder.fileState == LOAD_COMPLETE)
    {
        return (uint8_t)c;
    }
    uint8_t b;
    if (gifDecoder.position == gifDecoder.bufferSize)
    {
        b = FileIO_ReadByte();
        gifDecoder.bufferSize = (int)b;
        if (gifDecoder.bufferSize == 0)
        {
            gifDecoder.fileState = LOAD_COMPLETE;
            return (uint8_t)c;
        }
        FileIO_ReadByteArray(gifDecoder.buffer, gifDecoder.bufferSize);
        b = gifDecoder.buffer[0];
        gifDecoder.position = 1;
    }
    else
    {
        b = gifDecoder.buffer[gifDecoder.position++];
    }
    return b;
}
uint8_t GifLoader_TracePrefix(uint32_t* prefix, int code, int clearCode)
{
    int num = 0;
    while (code > clearCode && num++ <= LZ_MAX_CODE)
    {
        code = (int)prefix[code];
    }
    return (uint8_t)code;
}
