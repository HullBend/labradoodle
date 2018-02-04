/*
 * This file is
 *
 *  Copyright (C) 1998-2003 Daniel Veillard.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is fur-
 * nished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FIT-
 * NESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * DANIEL VEILLARD BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CON-
 * NECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Daniel Veillard shall not
 * be used in advertising or otherwise to promote the sale, use or other deal-
 * ings in this Software without prior written authorization from him.
 */

/**
 * encoding.c : implements the encoding conversion functions needed for the
 *              basic UTF-8 and western european ISO Latin-1 encodings.
 *
 * Related specs:
 * rfc2044        (UTF-8 and UTF-16) F. Yergeau Alis Technologies
 * rfc2781        UTF-16, an encoding of ISO 10646, P. Hoffman, F. Yergeau
 * [ISO-10646]    UTF-8 and UTF-16 in Annexes
 * [ISO-8859-1]   ISO Latin-1 characters codes.
 * [UNICODE]      The Unicode Consortium, "The Unicode Standard --
 *                Worldwide Character Encoding -- Version 1.0", Addison-
 *                Wesley, Volume 1, 1991, Volume 2, 1992.  UTF-8 is
 *                described in Unicode Technical Report #4.
 * [US-ASCII]     Coded Character Set--7-bit American Standard Code for
 *                Information Interchange, ANSI X3.4-1986.
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 *
 * Original code for IsoLatin1 by "Martin J. Duerst" <duerst@w3.org>
 */

/**
 * Changes, bugfixes, additional functions and all new bugs by SPZ,
 * Copyright (c) 2012
 *
 * http://www.opensource.org/licenses/mit-license.php
 */


#include "encoding.h"

#include <string.h>
#include <stdlib.h> /* malloc */



#if defined (_WIN64) || defined (_WIN32) && (!defined(__MINGW32__) && !defined(__MINGW64__))
/* disable "'xmltranscodetable_ISO8859_15' : array is too small to include a terminating null character" */
#pragma warning( disable: 4295 )
#endif /* (_WIN64) || (_WIN32) */



/************************************************************************
 *                                                                      *
 *      Conversions To/From UTF8 encoding                               *
 *                                                                      *
 ************************************************************************/


/**
 * UTF8Toisolat1:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of UTF-8 chars
 * @inlen:  the length of @in
 *
 * Take a block of UTF-8 chars in and try to convert it to an ISO Latin 1
 * block of chars out.
 *
 * Returns 0 if success, -2 if the transcoding fails, or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 *     if the return value is positive, else unpredictable.
 * The value of @outlen after return is the number of octets consumed.
 */
int UTF8Toisolat1(unsigned char* out, int *outlen,
              const unsigned char* in, int *inlen) {

    const unsigned char* processed = in;
    const unsigned char* outend;
    const unsigned char* outstart = out;
    const unsigned char* instart = in;
    const unsigned char* inend;
    unsigned int c, d;
    int trailing;

    if (in == NULL) {
        /*
         * initialization, nothing to do
         */
        *outlen = 0;
        *inlen = 0;
        return 0;
    }

    inend = in + (*inlen);
    outend = out + (*outlen);
    while (in < inend) {
        d = *in++;
        if (d < 0x80) {
            c = d;
            trailing = 0;
        } else if (d < 0xC0) {
            /* trailing byte in leading position */
            *outlen = out - outstart;
            *inlen = processed - instart;
            return -2;
        } else if (d < 0xE0) {
            c = d & 0x1F;
            trailing = 1;
        } else if (d < 0xF0) {
            c = d & 0x0F;
            trailing = 2;
        } else if (d < 0xF8) {
            c = d & 0x07;
            trailing = 3;
        } else {
            /* no chance for this in IsoLatin-1 */
            *outlen = out - outstart;
            *inlen = processed - instart;
            return -2;
        }

        if (inend - in < trailing) {
            break;
        }

        for ( ; trailing; trailing--) {
            if (in >= inend) {
                break;
            }
            if (((d = *in++) & 0xC0) != 0x80) {
                *outlen = out - outstart;
                *inlen = processed - instart;
                return -2;
            }
            c <<= 6;
            c |= d & 0x3F;
        }

        /* assertion: c is a single UTF-4 value */
        if (c <= 0xFF) {
            if (out >= outend) {
                break;
            }
            *out++ = (unsigned char) c;
        } else {
            /* no chance for this in IsoLatin-1 */
            if (out >= outend) {
                break;
            }

            /*
             * substitute some well-known Windows-1252 codes (for those
             * characters that are in Latin-9 but not in Latin-1) from
             * the unused range 0x80 - 0x9F, otherwise use an inverted
             * question mark as the replacement character.
             */
            /*
            switch (c) {
                case 0x0152:
                    subs = (unsigned char) 0x8C;
                    break;
                case 0x0153:
                    subs = (unsigned char) 0x9C;
                    break;
                case 0x0160:
                    subs = (unsigned char) 0x8A;
                    break;
                case 0x0161:
                    subs = (unsigned char) 0x9A;
                    break;
                case 0x0178:
                    subs = (unsigned char) 0x9F;
                    break;
                case 0x017D:
                    subs = (unsigned char) 0x8E;
                    break;
                case 0x017E:
                    subs = (unsigned char) 0x9E;
                    break;
                case 0x20AC:
                    subs = (unsigned char) 0x80;
                    break;
                default:
                    subs = (unsigned char) 0xBF;
            }
            *out++ = subs;
            */

            /* substitute inverted question mark */
            *out++ = (unsigned char) 0xBF;

            /*
            *outlen = out - outstart;
            *inlen = processed - instart;
            return -2;
            */
        }
        processed = in;
    } /* while */
    *outlen = out - outstart;
    *inlen = processed - instart;
    return 0;
}


/**
 * isolat1ToUTF8:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of ISO Latin 1 chars
 * @inlen:  the length of @in
 *
 * Take a block of ISO Latin 1 chars in and try to convert it to an UTF-8
 * block of chars out.
 * Returns 0 if success, or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 *     if the return value is positive, else unpredictable.
 * The value of @outlen after return is the number of octets consumed.
 */
int isolat1ToUTF8(unsigned char* out, int *outlen,
              const unsigned char* in, int *inlen) {

    unsigned char* outstart = out;
    const unsigned char* base = in;
    unsigned char* outend = out + *outlen;
    const unsigned char* inend;
    const unsigned char* instop;

    inend = in + (*inlen);
    instop = inend;

    while (in < inend && out < outend - 1) {
        if (*in >= 0x80) {
            *out++ = (((*in) >>  6) & 0x1F) | 0xC0;
            *out++ = ((*in) & 0x3F) | 0x80;
            ++in;
        }
        if (instop - in > outend - out) {
            instop = in + (outend - out);
        }
        while (in < instop && *in < 0x80) {
            *out++ = *in++;
        }
    }
    if (in < inend && out < outend && *in < 0x80) {
        *out++ = *in++;
    }
    *outlen = out - outstart;
    *inlen = in - base;
    return 0;
}


/**
 * UTF8ToISO8859x:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of UTF-8 chars
 * @inlen:  the length of @in
 * @xlattable: the 2-level transcoding table
 *
 * Take a block of UTF-8 chars in and try to convert it to an ISO 8859-*
 * block of chars out.
 *
 * Returns 0 if success, -2 if the transcoding fails, or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 *     as the return value is positive, else unpredictable.
 * The value of @outlen after return is the number of octets consumed.
 */
static int
UTF8ToISO8859x(unsigned char* out, int *outlen,
              const unsigned char* in, int *inlen,
              unsigned char const *xlattable) {

    const unsigned char* outstart = out;
    const unsigned char* inend;
    const unsigned char* instart = in;

    if (in == NULL) {
        /*
         * initialization, nothing to do
         */
        *outlen = 0;
        *inlen = 0;
        return 0;
    }
    inend = in + (*inlen);
    while (in < inend) {
        unsigned char d = *in++;
        if  (d < 0x80)  {
            *out++ = d;
        } else if (d < 0xC0) {
            /* trailing byte in leading position */
            *outlen = out - outstart;
            *inlen = in - instart - 1;
            return -2;
        } else if (d < 0xE0) {
            /* start of 2 byte sequence */
            unsigned char c;
            if (!(in < inend)) {
                /* trailing byte not in input buffer */
                *outlen = out - outstart;
                *inlen = in - instart - 1;
                return -2;
            }
            c = *in++;
            if ((c & 0xC0) != 0x80) {
                /* not a trailing byte */
                *outlen = out - outstart;
                *inlen = in - instart - 2;
                return -2;
            }
            c = c & 0x3F;
            d = d & 0x1F;
            d = xlattable [48 + c + xlattable [d] * 64];
            if (d == 0) {
                /* not in character set */
                *outlen = out - outstart;
                *inlen = in - instart - 2;
                return -2;
            }
            *out++ = d;
        } else if (d < 0xF0) {
            /* start of 3 byte sequence */
            unsigned char c1;
            unsigned char c2;
            if (!(in < inend - 1)) {
                /* trailing bytes not in input buffer */
                *outlen = out - outstart;
                *inlen = in - instart - 1;
                return -2;
            }
            c1 = *in++;
            if ((c1 & 0xC0) != 0x80) {
                /* not a trailing byte (c1) */
                *outlen = out - outstart;
                *inlen = in - instart - 2;
                return -2;
            }
            c2 = *in++;
            if ((c2 & 0xC0) != 0x80) {
                /* not a trailing byte (c2) */
                *outlen = out - outstart;
                *inlen = in - instart - 2;
                return -2;
            }
            c1 = c1 & 0x3F;
            c2 = c2 & 0x3F;
            d = d & 0x0F;
            d = xlattable [48 + c2 + xlattable [48 + c1 + xlattable [32 + d] * 64] * 64];
            if (d == 0) {
                /* not in character set */
                *outlen = out - outstart;
                *inlen = in - instart - 3;
                return -2;
            }
            *out++ = d;
        } else {
            /* cannot transcode >= U+010000 */
            *outlen = out - outstart;
            *inlen = in - instart - 1;
            return -2;
        }
    }
    *outlen = out - outstart;
    *inlen = in - instart;
    return 0;
}


/**
 * ISO8859xToUTF8
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of ISO Latin 1 chars
 * @inlen:  the length of @in
 *
 * Take a block of ISO 8859-* chars in and try to convert it to an UTF-8
 * block of chars out.
 * Returns 0 if success, or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 * The value of @outlen after return is the number of octets produced.
 */
static int
ISO8859xToUTF8(unsigned char* out, int *outlen,
              const unsigned char* in, int *inlen,
              unsigned short const *unicodetable) {

    unsigned char* outstart = out;
    unsigned char* outend = out + *outlen;
    const unsigned char* instart = in;
    const unsigned char* inend = in + *inlen;
    const unsigned char* instop = inend;
    unsigned int c = *in;

    while (in < inend && out < outend - 1) {
        if (c >= 0x80) {
            c = unicodetable [c - 0x80];
            if (c == 0) {
                /* undefined code point */
                *outlen = out - outstart;
                *inlen = in - instart;
                return -1;
            }
            if (c < 0x800) {
                *out++ = ((c >>  6) & 0x1F) | 0xC0;
                *out++ = (c & 0x3F) | 0x80;
            } else {
                *out++ = ((c >>  12) & 0x0F) | 0xE0;
                *out++ = ((c >>  6) & 0x3F) | 0x80;
                *out++ = (c & 0x3F) | 0x80;
            }
            ++in;
            c = *in;
        }
        if (instop - in > outend - out) {
            instop = in + (outend - out);
        }
        while (c < 0x80 && in < instop) {
            *out++ = (unsigned char) c;
            ++in;
            c = *in;
        }
    }
    if (in < inend && out < outend && c < 0x80) {
        *out++ = (unsigned char) c;
        ++in;
    }
    *outlen = out - outstart;
    *inlen = in - instart;
    return 0;
}


static unsigned short const xmlunicodetable_ISO8859_15 [128] = {
    0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
    0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f,
    0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
    0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f,
    0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x20ac, 0x00a5, 0x0160, 0x00a7,
    0x0161, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af,
    0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x017d, 0x00b5, 0x00b6, 0x00b7,
    0x017e, 0x00b9, 0x00ba, 0x00bb, 0x0152, 0x0153, 0x0178, 0x00bf,
    0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7,
    0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf,
    0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7,
    0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df,
    0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7,
    0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
    0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7,
    0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff,
};


static unsigned char const xmltranscodetable_ISO8859_15 [48 + 6 * 64] = {
    "\x00\x00\x01\x05\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f"
    "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f"
    "\xa0\xa1\xa2\xa3\x00\xa5\x00\xa7\x00\xa9\xaa\xab\xac\xad\xae\xaf"
    "\xb0\xb1\xb2\xb3\x00\xb5\xb6\xb7\x00\xb9\xba\xbb\x00\x00\x00\xbf"
    "\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xa4\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\xbc\xbd\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\xa6\xa8\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\xbe\x00\x00\x00\x00\xb4\xb8\x00"
    "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"
    "\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf"
    "\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef"
    "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff"
};


int ISO8859_15ToUTF8(unsigned char* out, int *outlen,
    const unsigned char* in, int *inlen) {

    return ISO8859xToUTF8(out, outlen, in, inlen, xmlunicodetable_ISO8859_15);
}


int UTF8ToISO8859_15(unsigned char* out, int *outlen,
    const unsigned char* in, int *inlen) {

    return UTF8ToISO8859x(out, outlen, in, inlen, xmltranscodetable_ISO8859_15);
}


static int
getISO8859_15ToUTF8RequiredLength(const unsigned char* in,
    int *inlen, int *outlen, int* transcodingNeeded) {

    const unsigned char* instart = in;
    const unsigned char* inend = in + *inlen;
    int needToTranscode = 0;
    unsigned int c = *in;
    int numOut = 0;

    while (in < inend) {
        if (c >= 0x80) {
            c = xmlunicodetable_ISO8859_15[c - 0x80];
            if (c == 0) {
                /* undefined code point */
                *outlen = numOut;
                *inlen = in - instart;
                *transcodingNeeded = needToTranscode;
                return -1;
            }
            if (c < 0x800) {
                numOut += 2;
                needToTranscode = 1;
            } else {
                numOut += 3;
                needToTranscode = 1;
            }
            ++in;
            c = *in;
        }
        while (c < 0x80 && in < inend) {
            ++numOut;
            ++in;
            c = *in;
        }
    } /* while */
    if (in < inend && c < 0x80) {
        ++numOut;
        ++in;
    }
    *outlen = numOut;
    *inlen = in - instart;
    *transcodingNeeded = needToTranscode;
    return 0;
}


static int
getUTF8ToISO8859_15RequiredLength(const unsigned char* in,
    int *inlen, int *outlen, int* transcodingNeeded) {

    const unsigned char* inend;
    const unsigned char* instart = in;
    int needToTranscode = 0;
    int numOut = 0;

    if (in == NULL) {
        /*
         * initialization, nothing to do
         */
        *outlen = 0;
        *inlen = 0;
        *transcodingNeeded = 0;
        return 0;
    }
    inend = in + (*inlen);
    while (in < inend) {
        unsigned char d = *in++;
        if  (d < 0x80)  {
            ++numOut;
        } else if (d < 0xC0) {
            /* trailing byte in leading position */
            *outlen = numOut;
            *inlen = in - instart - 1;
            *transcodingNeeded = needToTranscode;
            return -2;
        } else if (d < 0xE0) {
            unsigned char c;
            if (!(in < inend)) {
                /* trailing byte not in input buffer */
                *outlen = numOut;
                *inlen = in - instart - 1;
                *transcodingNeeded = needToTranscode;
                return -2;
            }
            c = *in++;
            if ((c & 0xC0) != 0x80) {
                /* not a trailing byte */
                *outlen = numOut;
                *inlen = in - instart - 2;
                *transcodingNeeded = needToTranscode;
                return -2;
            }
            c = c & 0x3F;
            d = d & 0x1F;
            d = xmltranscodetable_ISO8859_15[48 + c + xmltranscodetable_ISO8859_15[d] * 64];
            if (d == 0) {
                /* not in character set */
                *outlen = numOut;
                *inlen = in - instart - 2;
                *transcodingNeeded = needToTranscode;
                return -2;
            }
            ++numOut;
            needToTranscode = 1;
        } else if (d < 0xF0) {
            unsigned char c1;
            unsigned char c2;
            if (!(in < inend - 1)) {
                /* trailing bytes not in input buffer */
                *outlen = numOut;
                *inlen = in - instart - 1;
                *transcodingNeeded = needToTranscode;
                return -2;
            }
            c1 = *in++;
            if ((c1 & 0xC0) != 0x80) {
                /* not a trailing byte (c1) */
                *outlen = numOut;
                *inlen = in - instart - 2;
                *transcodingNeeded = needToTranscode;
                return -2;
            }
            c2 = *in++;
            if ((c2 & 0xC0) != 0x80) {
                /* not a trailing byte (c2) */
                *outlen = numOut;
                *inlen = in - instart - 2;
                *transcodingNeeded = needToTranscode;
                return -2;
            }
            c1 = c1 & 0x3F;
            c2 = c2 & 0x3F;
            d = d & 0x0F;
            d = xmltranscodetable_ISO8859_15[48 + c2 + xmltranscodetable_ISO8859_15[48 + c1 + xmltranscodetable_ISO8859_15[32 + d] * 64] * 64];
            if (d == 0) {
                /* not in character set */
                *outlen = numOut;
                *inlen = in - instart - 3;
                *transcodingNeeded = needToTranscode;
                return -2;
            }
            ++numOut;
            needToTranscode = 1;
        } else {
            /* cannot transcode >= U+010000 */
            *outlen = numOut;
            *inlen = in - instart - 1;
            *transcodingNeeded = needToTranscode;
            return -2;
        }
    }
    *outlen = numOut;
    *inlen = in - instart;
    *transcodingNeeded = needToTranscode;
    return 0;
}


/**
 * Attempt to convert UTF-8 encoded 'in' to Latin-9 encoded *out.
 * Correctly sized memory for *out will be malloc'ed automatically
 * and *out will be a zero-terminated string.
 *
 * The out parameter 'wasTranscoded' indicates whether memory has been
 * allocated (yes if 'wasTranscoded' == 1, no if 'wasTranscoded' == 0).
 *
 * In the case of 'wasTranscoded' == 1, the caller is responsible to
 * free() the memory pointed to by *out!
 *
 * NOTE: 'in' needs to be a zero-terminated string!
 *
 * Returns rc == 0 on success, rc == -1 on error.
 *
 * NOTE: In the case of rc == 0 and wasTranscoded == 0, *out contains the pointer
 * to 'in', so 'in' must _NOT_ be free'd afterwards (otherwise, *out would point
 * to garbage because of pointer aliasing!).
 */
int utf8ToLatin9(const unsigned char* in, unsigned char** out, int* wasTranscoded) {

    int byteLenIn = 0;
    int byteLenOut = 0;
    int szLenOut = 0;
    int needToTranscode = 0;

    if (out == NULL) {
        *wasTranscoded = 0;
        return -1;
    }
    if (in == NULL) {
        *out = NULL;
        *wasTranscoded = 0;
        return 0;
    }

    /* Count bytes in 'in' */
    /* UTF-8 strings shouldn't contain the byte 0 (other than used as a terminator) */
    byteLenIn = strlen((const char*) in);

    if (byteLenIn > 0) {
        if (getUTF8ToISO8859_15RequiredLength(in, &byteLenIn, &byteLenOut, &needToTranscode) == 0) {
            if (needToTranscode) {
                szLenOut = byteLenOut + 1;
                *out = malloc(szLenOut * sizeof(unsigned char));
                if (UTF8ToISO8859_15(*out, &byteLenOut, in, &byteLenIn) == 0) {
                    /* transcoding succeeded, zero-terminate *out */
                    (*out)[byteLenOut] = '\0';
                    *wasTranscoded = 1;
                } else {
                    /* transcoding didn't succeed */
                    free(*out);
                    *wasTranscoded = 0;
                    return -1;
                }
            } else {
                /* 'in' is already in Latin-9 encoding and zero-terminated by assumption */
                /* Note: Simply assigning the 'in' pointer assumes that 'in' doesn't get free'd afterwards !!! */
                *out = (unsigned char*) in;
                *wasTranscoded = 0;
            }
        } else {
            /* length determination didn't succeed */
            *wasTranscoded = 0;
            return -1;
        }
    } else {
        /* in is an empty string */
        *out = malloc(1 * sizeof(unsigned char));
        *out = '\0';
        *wasTranscoded = 1;
    }

    return 0;
}


/**
 * Attempt to convert Latin-9 encoded 'in' to UTF-8 encoded *out.
 * Correctly sized memory for *out will be malloc'ed automatically
 * and *out will be a zero-terminated string.
 *
 * The out parameter 'wasTranscoded' indicates whether memory has been
 * allocated (yes if 'wasTranscoded' == 1, no if 'wasTranscoded' == 0).
 *
 * In the case of 'wasTranscoded' == 1, the caller is responsible to
 * free() the memory pointed to by *out!
 *
 * NOTE: 'in' needs to be a zero-terminated string!
 *
 * Returns rc == 0 on success, rc == -1 on error.
 *
 * NOTE: In the case of rc == 0 and wasTranscoded == 0, *out contains the pointer
 * to 'in', so 'in' must _NOT_ be free'd afterwards (otherwise, *out would point
 * to garbage because of pointer aliasing!).
 */
int latin9ToUtf8(const unsigned char* in, unsigned char** out, int* wasTranscoded) {

    int byteLenIn = 0;
    int byteLenOut = 0;
    int szLenOut = 0;
    int needToTranscode = 0;

    if (out == NULL) {
        *wasTranscoded = 0;
        return -1;
    }
    if (in == NULL) {
        *out = NULL;
        *wasTranscoded = 0;
        return 0;
    }

    /* Count bytes in 'in' */
    byteLenIn = strlen((const char*) in);

    if (byteLenIn > 0) {
        if (getISO8859_15ToUTF8RequiredLength(in, &byteLenIn, &byteLenOut, &needToTranscode) == 0) {
            if (needToTranscode) {
                szLenOut = byteLenOut + 1;
                *out = malloc(szLenOut * sizeof(unsigned char));
                if (ISO8859_15ToUTF8(*out, &byteLenOut, in, &byteLenIn) == 0) {
                    /* transcoding succeeded, zero-terminate *out */
                    (*out)[byteLenOut] = '\0';
                    *wasTranscoded = 1;
                } else {
                    /* transcoding didn't succeed */
                    free(*out);
                    *wasTranscoded = 0;
                    return -1;
                }
            } else {
                /* 'in' contains only US-ASCII encoded characters and is zero-terminated by assumption */
                /* Note: Simply assigning the 'in' pointer assumes that 'in' doesn't get free'd afterwards !!! */
                *out = (unsigned char*) in;
                *wasTranscoded = 0;
            }
        } else {
            /* length determination didn't succeed */
            *wasTranscoded = 0;
            return -1;
        }
    } else {
        /* in is an empty string */
        *out = malloc(1 * sizeof(unsigned char));
        *out = '\0';
        *wasTranscoded = 1;
    }

    return 0;
}

/*
 * (SPZ) TODO: utf8ToLatin9() and latin9ToUtf8() could be collapsed
 * into one function using function pointers for the applied conversion
 * and length functions.
 */


/**
 * utf8ToCp1252:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of UTF-8 chars
 * @inlen:  the length of @in
 *
 * Take a block of UTF-8 chars in and try to convert it to an
 * Windows-1252 block of chars out.
 *
 * Returns 0 if success, -2 if the transcoding fails, or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 *     if the return value is positive, else unpredictable.
 * The value of @outlen after return is the number of octets consumed.
 */
int utf8ToCp1252(unsigned char* out, int *outlen,
              const unsigned char* in, int *inlen) {

    const unsigned char* processed = in;
    const unsigned char* outend;
    const unsigned char* outstart = out;
    const unsigned char* instart = in;
    const unsigned char* inend;
    unsigned char subs;
    unsigned int c, d;
    int trailing;

    if (in == NULL) {
        /*
         * initialization, nothing to do
         */
        *outlen = 0;
        *inlen = 0;
        return 0;
    }

    inend = in + (*inlen);
    outend = out + (*outlen);
    while (in < inend) {
        d = *in++;
        if (d < 0x80) {
            c = d;
            trailing = 0;
        } else if (d < 0xC0) {
            /* trailing byte in leading position */
            *outlen = out - outstart;
            *inlen = processed - instart;
            return -2;
        } else if (d < 0xE0) {
            c = d & 0x1F;
            trailing = 1;
        } else if (d < 0xF0) {
            c = d & 0x0F;
            trailing = 2;
        } else if (d < 0xF8) {
            c = d & 0x07;
            trailing = 3;
        } else {
            /* no chance for this in Cp1252 */
            *outlen = out - outstart;
            *inlen = processed - instart;
            return -2;
        }

        if (inend - in < trailing) {
            break;
        }

        for ( ; trailing; trailing--) {
            if (in >= inend) {
                break;
            }
            if (((d= *in++) & 0xC0) != 0x80) {
                *outlen = out - outstart;
                *inlen = processed - instart;
                return -2;
            }
            c <<= 6;
            c |= d & 0x3F;
        }

        /* assertion: c is a single UTF-4 value */
        if (c <= 0xFF) {
            if (out >= outend) {
                break;
            }
            *out++ = (unsigned char) c;
        } else {
            /* attempt to fix this for Cp1252 */
            if (out >= outend) {
                break;
            }
            /*
             * substitute the known Windows-1252 codes in the
             * unused range 0x80 - 0x9F, otherwise use an inverted
             * question mark as the replacement character.
             */
            switch (c) {
                case 0x0152:
                    subs = (unsigned char) 0x8C;
                    break;
                case 0x0153:
                    subs = (unsigned char) 0x9C;
                    break;
                case 0x0160:
                    subs = (unsigned char) 0x8A;
                    break;
                case 0x0161:
                    subs = (unsigned char) 0x9A;
                    break;
                case 0x0178:
                    subs = (unsigned char) 0x9F;
                    break;
                case 0x017D:
                    subs = (unsigned char) 0x8E;
                    break;
                case 0x017E:
                    subs = (unsigned char) 0x9E;
                    break;
                case 0x0192:
                    subs = (unsigned char) 0x83;
                    break;
                case 0x02c6:
                    subs = (unsigned char) 0x88;
                    break;
                case 0x02dc:
                    subs = (unsigned char) 0x98;
                    break;
                case 0x2013:
                    subs = (unsigned char) 0x96;
                    break;
                case 0x2014:
                    subs = (unsigned char) 0x97;
                    break;
                case 0x2018:
                    subs = (unsigned char) 0x91;
                    break;
                case 0x2019:
                    subs = (unsigned char) 0x92;
                    break;
                case 0x201a:
                    subs = (unsigned char) 0x82;
                    break;
                case 0x201c:
                    subs = (unsigned char) 0x93;
                    break;
                case 0x201d:
                    subs = (unsigned char) 0x94;
                    break;
                case 0x201e:
                    subs = (unsigned char) 0x84;
                    break;
                case 0x2020:
                    subs = (unsigned char) 0x86;
                    break;
                case 0x2021:
                    subs = (unsigned char) 0x87;
                    break;
                case 0x2022:
                    subs = (unsigned char) 0x95;
                    break;
                case 0x2026:
                    subs = (unsigned char) 0x85;
                    break;
                case 0x2030:
                    subs = (unsigned char) 0x89;
                    break;
                case 0x2039:
                    subs = (unsigned char) 0x8B;
                    break;
                case 0x203a:
                    subs = (unsigned char) 0x9B;
                    break;
                case 0x20AC:
                    subs = (unsigned char) 0x80;
                    break;
                case 0x2122:
                    subs = (unsigned char) 0x99;
                    break;
                default:
                    /* inverted question mark */
                    subs = (unsigned char) 0xBF;
            }
            *out++ = subs;
        }
        processed = in;
    } /* while */
    *outlen = out - outstart;
    *inlen = processed - instart;
    return 0;
}


static int
getISOLatin1ToUtf8_RequiredLength(const unsigned char* in, int *inlen,
    int *outlen, int* transcodingNeeded) {

    const unsigned char* base = in;
    const unsigned char* inend;
    int needToTranscode = 0;
    int numOut = 0;

    if (in == NULL) {
        /*
         * initialization, nothing to do
         */
        *outlen = 0;
        *inlen = 0;
        *transcodingNeeded = 0;
        return 0;
    }
    inend = in + (*inlen);

    while (in < inend) {
        if (*in >= 0x80) {
            numOut += 2;
            needToTranscode = 1;
            ++in;
        }
        while (in < inend && *in < 0x80) {
            ++in;
            ++numOut;
        }
    }
    if (in < inend && *in < 0x80) {
        ++in;
        ++numOut;
    }
    *outlen = numOut;
    *inlen = in - base;
    *transcodingNeeded = needToTranscode;
    return 0;
}


static int
getUtf8ToISOLatin1_RequiredLength(const unsigned char* in,
    int *inlen, int *outlen, int* transcodingNeeded) {

    const unsigned char* processed = in;
    const unsigned char* instart = in;
    const unsigned char* inend;
    unsigned int d;
    int trailing;
    int needToTranscode = 0;
    int numOut = 0;

    if (in == NULL) {
        /*
         * initialization, nothing to do
         */
        *outlen = 0;
        *inlen = 0;
        *transcodingNeeded = 0;
        return 0;
    }
    inend = in + (*inlen);

    while (in < inend) {
        d = *in++;
        if (d < 0x80) {
            trailing = 0;
        } else if (d < 0xC0) {
            /* trailing byte in leading position */
            *outlen = numOut;
            *inlen = processed - instart;
            *transcodingNeeded = needToTranscode;
            return -2;
        } else if (d < 0xE0) {
            needToTranscode = 1;
            trailing = 1;
        } else if (d < 0xF0) {
            needToTranscode = 1;
            trailing = 2;
        } else if (d < 0xF8) {
            needToTranscode = 1;
            trailing = 3;
        } else {
            /* no chance for this in IsoLatin-1 */
            *outlen = numOut;
            *inlen = processed - instart;
            *transcodingNeeded = needToTranscode;
            return -2;
        }

        if (inend - in < trailing) {
            break;
        }

        for ( ; trailing; trailing--) {
            if (in >= inend) {
                break;
            }
            if (((d = *in++) & 0xC0) != 0x80) {
                *outlen = numOut;
                *inlen = processed - instart;
                *transcodingNeeded = needToTranscode;
                return -2;
            }
        }

        ++numOut;
        processed = in;

    } /* while */

    *outlen = numOut;
    *inlen = processed - instart;
    *transcodingNeeded = needToTranscode;
    return 0;
}


/**
 * Attempt to convert UTF-8 encoded 'in' to Latin-1 encoded *out.
 * Correctly sized memory for *out will be malloc'ed automatically
 * and *out will be a zero-terminated string.
 *
 * The out parameter 'wasTranscoded' indicates whether memory has been
 * allocated (yes if 'wasTranscoded' == 1, no if 'wasTranscoded' == 0).
 *
 * In the case of 'wasTranscoded' == 1, the caller is responsible to
 * free() the memory pointed to by *out!
 *
 * NOTE: 'in' needs to be a zero-terminated string!
 *
 * Returns rc == 0 on success, rc == -1 on error.
 *
 * NOTE: In the case of rc == 0 and wasTranscoded == 0, *out contains the pointer
 * to 'in', so 'in' must _NOT_ be free'd afterwards (otherwise, *out would point
 * to garbage because of pointer aliasing!).
 */
int utf8ToLatin1(const unsigned char* in, unsigned char** out, int* wasTranscoded) {

    int byteLenIn = 0;
    int byteLenOut = 0;
    int szLenOut = 0;
    int needToTranscode = 0;

    if (out == NULL) {
        *wasTranscoded = 0;
        return -1;
    }
    if (in == NULL) {
        *out = NULL;
        *wasTranscoded = 0;
        return 0;
    }

    /* Count bytes in 'in' */
    /* UTF-8 strings shouldn't contain the byte 0 (other than used as a terminator) */
    byteLenIn = strlen((const char*) in);

    if (byteLenIn > 0) {
        if (getUtf8ToISOLatin1_RequiredLength(in, &byteLenIn, &byteLenOut, &needToTranscode) == 0) {
            if (needToTranscode) {
                szLenOut = byteLenOut + 1;
                *out = malloc(szLenOut * sizeof(unsigned char));
                if (UTF8Toisolat1(*out, &byteLenOut, in, &byteLenIn) == 0) {
                    /* transcoding succeeded, zero-terminate *out */
                    (*out)[byteLenOut] = '\0';
                    *wasTranscoded = 1;
                } else {
                    /* transcoding didn't succeed */
                    free(*out);
                    *wasTranscoded = 0;
                    return -1;
                }
            } else {
                /* 'in' is already in Latin-1 encoding and zero-terminated by assumption */
                /* Note: Simply assigning the 'in' pointer assumes that 'in' doesn't get free'd afterwards !!! */
                *out = (unsigned char*) in;
                *wasTranscoded = 0;
            }
        } else {
            /* length determination didn't succeed */
            *wasTranscoded = 0;
            return -1;
        }
    } else {
        /* in is an empty string */
        *out = malloc(1 * sizeof(unsigned char));
        *out = '\0';
        *wasTranscoded = 1;
    }

    return 0;
}


/**
 * Attempt to convert Latin-1 encoded 'in' to UTF-8 encoded *out.
 * Correctly sized memory for *out will be malloc'ed automatically
 * and *out will be a zero-terminated string.
 *
 * The out parameter 'wasTranscoded' indicates whether memory has been
 * allocated (yes if 'wasTranscoded' == 1, no if 'wasTranscoded' == 0).
 *
 * In the case of 'wasTranscoded' == 1, the caller is responsible to
 * free() the memory pointed to by *out!
 *
 * NOTE: 'in' needs to be a zero-terminated string!
 *
 * Returns rc == 0 on success, rc == -1 on error.
 *
 * NOTE: In the case of rc == 0 and wasTranscoded == 0, *out contains the pointer
 * to 'in', so 'in' must _NOT_ be free'd afterwards (otherwise, *out would point
 * to garbage because of pointer aliasing!).
 */
int latin1ToUtf8(const unsigned char* in, unsigned char** out, int* wasTranscoded) {

    int byteLenIn = 0;
    int byteLenOut = 0;
    int szLenOut = 0;
    int needToTranscode = 0;

    if (out == NULL) {
        *wasTranscoded = 0;
        return -1;
    }
    if (in == NULL) {
        *out = NULL;
        *wasTranscoded = 0;
        return 0;
    }

    /* Count bytes in 'in' */
    byteLenIn = strlen((const char*) in);

    if (byteLenIn > 0) {
        if (getISOLatin1ToUtf8_RequiredLength(in, &byteLenIn, &byteLenOut, &needToTranscode) == 0) {
            if (needToTranscode) {
                szLenOut = byteLenOut + 1;
                *out = malloc(szLenOut * sizeof(unsigned char));
                if (isolat1ToUTF8(*out, &byteLenOut, in, &byteLenIn) == 0) {
                    /* transcoding succeeded, zero-terminate *out */
                    (*out)[byteLenOut] = '\0';
                    *wasTranscoded = 1;
                } else {
                    /* transcoding didn't succeed */
                    free(*out);
                    *wasTranscoded = 0;
                    return -1;
                }
            } else {
                /* 'in' contains only US-ASCII encoded characters and is zero-terminated by assumption */
                /* Note: Simply assigning the 'in' pointer assumes that 'in' doesn't get free'd afterwards !!! */
                *out = (unsigned char*) in;
                *wasTranscoded = 0;
            }
        } else {
            /* length determination didn't succeed */
            *wasTranscoded = 0;
            return -1;
        }
    } else {
        /* in is an empty string */
        *out = malloc(1 * sizeof(unsigned char));
        *out = '\0';
        *wasTranscoded = 1;
    }

    return 0;
}

/*
 * (SPZ) TODO: utf8ToLatin1() and latin1ToUtf8() could be collapsed
 * into one function using function pointers for the applied conversion
 * and length functions (same for Latin-9 variants - all 4 could be one
 * function).
 */
