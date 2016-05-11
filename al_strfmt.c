/*
Written by John MacCallum, The Center for New Music and Audio Technologies,
University of California, Berkeley.  Copyright (c) 2009-ll, The Regents of
the University of California (Regents). 
Permission to use, copy, modify, distribute, and distribute modified versions
of this software and its documentation without fee and without a signed
licensing agreement, is hereby granted, provided that the above copyright
notice, this paragraph and the following two paragraphs appear in all copies,
modifications, and distributions.

IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/

#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <float.h>

#include "al_strfmt.h"

#if __DBL_DIG__ == 15 && __DBL_MANT_DIG__ == 53  && FLT_RADIX == 2 
//#define AL_STRFMT_PRINTALLBITS
#else
#error nope
#endif

int al_strfmt_precision(double d);

int al_strfmt_int8(char *buf, size_t n, int8_t i)
{
	return snprintf(buf, n, "'%c'", i);
}

int al_strfmt_int16(char *buf, size_t n, int16_t i)
{
	return snprintf(buf, n, "%"PRId16, i);
}

int al_strfmt_int32(char *buf, size_t n, int32_t i)
{
	return snprintf(buf, n, "%"PRId32, i);
}

int al_strfmt_int64(char *buf, size_t n, int64_t i)
{
	return snprintf(buf, n, "%"PRId64, i);
}


int al_strfmt_uint8(char *buf, size_t n, uint8_t i)
{
	return snprintf(buf, n, "'%c'", i);
}

int al_strfmt_uint16(char *buf, size_t n, uint16_t i)
{
	return snprintf(buf, n, "%"PRIu16, i);
}

int al_strfmt_uint32(char *buf, size_t n, uint32_t i)
{
	return snprintf(buf, n, "%"PRIu32, i);
}

int al_strfmt_uint64(char *buf, size_t n, uint64_t i)
{
	return snprintf(buf, n, "%"PRIu64, i);
}

int al_strfmt_float32(char *buf, size_t n, float f)
{
	return al_strfmt_float64(buf, n, f);
}

int al_strfmt_float(char *buf, size_t n, float f)
{
	return al_strfmt_float32(buf, n, f);
}

int al_strfmt_double(char *buf, size_t n, float f)
{
	return al_strfmt_float64(buf, n, f);
}

#ifdef AL_STRFMT_PRINTALLBITS
int al_strfmt_float64(char *buf, size_t n, double f)
{
	snprintf(buf, n, "%.*f", al_strfmt_precision(f), f);
}
#else
int al_strfmt_float64(char *buf, size_t n, double f)
{
	int need_point = f - floor(f) == 0 ? 1 : 0;
	if(need_point){
		return snprintf(buf, n, "%g.", f);
	}else{
		return snprintf(buf, n, "%g", f);
	}

}
#endif

int al_strfmt_bool(char *buf, size_t n, char b)
{
	switch(b){
	case 'T':
		return snprintf(buf, n, "true");
	case 'F':
		return snprintf(buf, n, "false");
	default:
		return 0;
	}
}

int al_strfmt_null(char *buf, size_t n)
{
	return snprintf(buf, n, "nil");
}

int al_strfmt_quotedString(char *buf, size_t n, char *str)
{
	if(!str){
		return 0;
	}
	return snprintf(buf, n, "\"%s\"", str);
}

int al_strfmt_stringWithQuotedMeta(char *buf, size_t n, char *str)
{
	if(!str){
		return 0;
	}
	long len = strlen(str);
	if(!buf){
		return al_strfmt_countMeta(len, str) + len; 
	}

	{
		int i = 0;
		for(int j = 0; j < len; j++){
			if(al_strfmt_isMeta(str[j])){
				if(i < n){
					buf[i] = '\\';
				}
				i++;
			}
			if(i < n){
				buf[i] = str[j];
			}
			i++;
		}
		if(i < n){
			buf[i] = '\0';
		}else{
			buf[i - 1] = '\0';
		}
		return i;
	}
}
/*
int al_strfmt_quotedStringWithQuotedMeta(char *buf, size_t n, char *str)
{
	if(!str){
		return 0;
	}
	if(!buf){
		return al_strfmt_stringWithQuotedMeta(buf, n, str) + 2;
	}else{
		char tmp[n];
		int nn = al_strfmt_stringWithQuotedMeta(tmp, n, str);
		al_strfmt_quotedString(buf, n, tmp);
		return nn + 2;
	}
}

int al_strfmt_addQuotes(int len, char *buf, char **out)
{
	if(!buf){
		return 0;
	}
	if(!(*out)){
		*out = osc_mem_alloc(len + 3);
	}
	int i = 0;
	if(buf[0] != '\"'){
		(*out)[i++] = '\"';
	}
	strncpy((*out) + i, buf, len);
	i += len;
	if(buf[len - 1] != '\"'){
		(*out)[i++] = '\"';
	}
	(*out)[i] = '\0';
	return i;
}
*/
int al_strfmt_countMeta(int len, char *buf)
{
	int i;
	int n = 0;
	for(i = 0; i < len; i++){
		if(al_strfmt_isMeta(buf[i])){
			n++;
		}
	}
	return n;
}

int al_strfmt_isMeta(char c)
{
	int i;
	for(i = 0; i < sizeof(AL_STRFMT_META_CHARS); i++){
		if(c == AL_STRFMT_META_CHARS[i]){
			return 1;
		}
	}
	return 0;
}
/*
int al_strfmt_addQuotesAndQuoteMeta(int len, char *buf, char **out)
{
	if(!buf){
		return 0;
	}
	int n = len + 2 + al_strfmt_countMeta(len, buf);
	if(!(*out)){
		*out = osc_mem_alloc(n);
	}
	int i = 0;
	if(1){//buf[0] != '\"'){
		(*out)[i++] = '\"';
	}

	//strncpy((*out) + i, buf, len);
	//i += len;
	int j;
	for(j = 0; j < len; j++){
		if(al_strfmt_isMeta(buf[j])){
			(*out)[i++] = '\\';
		}
		(*out)[i++] = buf[j];
	}

	if(1){//buf[len - 1] != '\"'){
		(*out)[i++] = '\"';
	}
	(*out)[i] = '\0';
	return n;
}
*/
int al_strfmt_strlenPadded(char *str)
{
	int len = strlen(str);
	int rem = len % 4;
	if(rem){
		return len + (4 - rem);
	}else{
		return len + 4;
	}
}
/*
int al_strfmt_precision(double d)
{
	char buf[43];
		 //1 + // sign, '-' or '+'
		 //(sizeof(d) * CHAR_BIT + 3) / 4 + // mantissa hex digits max
		 //1 + // decimal point, '.'
		 //1 + // mantissa-exponent separator, 'p'
		 //1 + // mantissa sign, '-' or '+'
		 //(sizeof(d) * CHAR_BIT + 2) / 3 + // exponent decimal digits max
		 //1 // string terminator, '\0'
		 //];
	int n;
	char *pp, *p;
	int e, lsbFound, lsbPos;

	// convert d into "+/- 0x h.hhhh p +/- ddd" representation and check for errors
	if ((n = snprintf(buf, sizeof(buf), "%+a", d)) < 0 ||
	    (unsigned)n >= sizeof(buf))
	return -1;

	//printf("{%s}", buf);

	// make sure the conversion didn't produce something like "nan" or "inf"
	// instead of "+/- 0x h.hhhh p +/- ddd"
	if (strstr(buf, "0x") != buf + 1 ||
	    (pp = strchr(buf, 'p')) == NULL)
	return 0;

	// extract the base-2 exponent manually, checking for overflows
	e = 0;
	p = pp + 1 + (pp[1] == '-' || pp[1] == '+'); // skip the exponent sign at first
	for (; *p != '\0'; p++)
	{
		if (e > INT_MAX / 10)
		return -2;
		e *= 10;
		if (e > INT_MAX - (*p - '0'))
		return -2;
		e += *p - '0';
	}
	if (pp[1] == '-') // apply the sign to the exponent
	e = -e;

	//printf("[%s|%d]", buf, e);

	// find the position of the least significant non-zero bit
	lsbFound = lsbPos = 0;
	for (p = pp - 1; *p != 'x'; p--)
	{
		if (*p == '.')
		continue;
		if (!lsbFound)
		{
			int hdigit = (*p >= 'a') ? (*p - 'a' + 10) : (*p - '0'); // assuming ASCII chars
			if (hdigit)
			{
				static const int lsbPosInNibble[16] = { 0,4,3,4,  2,4,3,4, 1,4,3,4, 2,4,3,4 };
				lsbFound = 1;
				lsbPos = -lsbPosInNibble[hdigit];
			}
		}
    else
	    {
		    lsbPos -= 4;
	    }
	}
	lsbPos += 4;

	if (!lsbFound)
		return 0; // d is 0 (integer)

	// adjust the least significant non-zero bit position
	// by the base-2 exponent (just add them), checking
	// for overflows

	if (lsbPos >= 0 && e >= 0)
		return 0; // lsbPos + e >= 0, d is integer

	if (lsbPos < 0 && e < 0)
		if (lsbPos < INT_MIN - e)
			return -2; // d isn't integer and needs too many fractional digits

	if ((lsbPos += e) >= 0)
		return 0; // d is integer

	if (lsbPos == INT_MIN && -INT_MAX != INT_MIN)
		return -2; // d isn't integer and needs too many fractional digits

	return -lsbPos;
}
*/
