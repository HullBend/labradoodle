/**
 * Summary: interface for the encoding conversion functions needed for
 *          basic UTF-8 and western european ISO Latin-1 encoding.
 *
 * Related specs are
 * rfc2044        (UTF-8 and UTF-16) F. Yergeau Alis Technologies
 * [ISO-10646]    UTF-8 and UTF-16 in Annexes
 * [ISO-8859-1]   ISO Latin-1 characters codes.
 * [UNICODE]      The Unicode Consortium, "The Unicode Standard --
 *                Worldwide Character Encoding -- Version 1.0", Addison-
 *                Wesley, Volume 1, 1991, Volume 2, 1992.  UTF-8 is
 *                described in Unicode Technical Report #4.
 * [US-ASCII]     Coded Character Set--7-bit American Standard Code for
 *                Information Interchange, ANSI X3.4-1986.
 */


#ifndef __CHAR_ENCODING_UTF8_H__
#define __CHAR_ENCODING_UTF8_H__



#ifdef __cplusplus
extern "C" {
#endif



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
int utf8ToLatin1(const unsigned char* in, unsigned char** out, int* wasTranscoded);


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
int latin1ToUtf8(const unsigned char* in, unsigned char** out, int* wasTranscoded);


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
int utf8ToLatin9(const unsigned char* in, unsigned char** out, int* wasTranscoded);


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
int latin9ToUtf8(const unsigned char* in, unsigned char** out, int* wasTranscoded);


/**
 * ISO8859_15ToUTF8:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of ISO Latin 1 chars
 * @inlen:  the length of @in
 *
 * Take a block of ISO 8859-15 chars in and try to convert it to an UTF-8
 * block of chars out.
 * Returns 0 if success, or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 * The value of @outlen after return is the number of octets produced.
 */
int ISO8859_15ToUTF8(unsigned char* out, int *outlen,
    const unsigned char* in, int *inlen);


/**
 * UTF8ToISO8859_15:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of UTF-8 chars
 * @inlen:  the length of @in
 *
 * Take a block of UTF-8 chars in and try to convert it to an ISO 8859-15
 * block of chars out.
 *
 * Returns 0 if success, -2 if the transcoding fails, or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 *     as the return value is positive, else unpredictable.
 * The value of @outlen after return is the number of octets consumed.
 */
int UTF8ToISO8859_15(unsigned char* out, int *outlen,
    const unsigned char* in, int *inlen);


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
              const unsigned char* in, int *inlen);


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
              const unsigned char* in, int *inlen);


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
              const unsigned char* in, int *inlen);



#ifdef __cplusplus
}
#endif

#endif /* __CHAR_ENCODING_UTF8_H__ */
