#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define __USE_BSD
#include <sys/time.h>
#include <time.h>
#include <inttypes.h>

#include "al_endian.h"
#include "al_region.h"
#include "al_osctime.h"

static void al_osctime_float_to_ntp(double d, al_osctime_ntptime *n);
static double al_osctime_ntp_to_float(al_osctime_ntptime n);
//static void al_osctime_ut_to_ntp(time_t ut, al_osctime_ntptime *n);
//static time_t al_osctime_ntp_to_ut(al_osctime_ntptime n);
static uint32_t al_osctime_ntp_getSeconds(al_osctime t);
//static uint32_t al_osctime_ntp_getFraction(al_osctime t);
static unsigned short int al_osctime_isleap(unsigned short int year);
static unsigned short int al_osctime_getmonth(unsigned short int *day, unsigned short int leap);

#if defined(_WIN32) && !defined(__CYGWIN__)
static char *strptime(const char *buf, const char *fmt, struct tm *tm);
#endif

al_osctime al_osctime_add(al_region region, al_osctime t1, al_osctime t2)
{
	al_osctime_ntptime t1_ntp = *((al_osctime_ntptime *)(&t1));
	al_osctime_ntptime t2_ntp = *((al_osctime_ntptime *)(&t2));
	al_osctime_ntptime r;
	r.sec = t1_ntp.sec + t2_ntp.sec;
	r.frac_sec = t1_ntp.frac_sec + t2_ntp.frac_sec;
    
	if(r.frac_sec < t1_ntp.frac_sec) { // rollover occurred
		r.sec += 1;
	}
	return *((al_osctime *)(&r));
}

al_osctime al_osctime_subtract(al_region region, al_osctime lhs, al_osctime rhs)
{
	al_osctime t1 = lhs, t2 = rhs;
	al_osctime_ntptime t1_ntp = *((al_osctime_ntptime *)(&t1));
	al_osctime_ntptime t2_ntp = *((al_osctime_ntptime *)(&t2));
	al_osctime_ntptime r;

	if(t1_ntp.sec > t2_ntp.sec || (t1_ntp.sec == t2_ntp.sec && t1_ntp.frac_sec >= t2_ntp.frac_sec)){
		r.sec = t1_ntp.sec - t2_ntp.sec;
		if(t1_ntp.frac_sec >= t2_ntp.frac_sec){
			r.frac_sec = t1_ntp.frac_sec - t2_ntp.frac_sec;
		}else{
			if(r.sec == 0){
				r.frac_sec = t2_ntp.frac_sec - t1_ntp.frac_sec;
			}else{
				r.sec--;
				r.frac_sec = t1_ntp.frac_sec - t2_ntp.frac_sec;
			}
		}
	}else{
		r.sec = t2_ntp.sec - t1_ntp.sec;
		if(t1_ntp.frac_sec >= t2_ntp.frac_sec){
			r.frac_sec = t1_ntp.frac_sec - t2_ntp.frac_sec;
		}else{
			r.frac_sec = t2_ntp.frac_sec - t1_ntp.frac_sec;
		}
	}

	return *((al_osctime *)(&r));
}

int al_osctime_compare(al_region region, al_osctime t1, al_osctime t2)
{
	al_osctime_ntptime t1_ntp = *((al_osctime_ntptime *)(&t1));
	al_osctime_ntptime t2_ntp = *((al_osctime_ntptime *)(&t2));
	if(t1_ntp.sec < t2_ntp.sec || (t1_ntp.sec == t2_ntp.sec && t1_ntp.frac_sec < t2_ntp.frac_sec)) {
		return -1;
	}
  
	if(t1_ntp.sec > t2_ntp.sec || (t1_ntp.sec == t2_ntp.sec && t1_ntp.frac_sec > t2_ntp.frac_sec)) {
		return 1;
	}
  
	return 0;
}

double al_osctime_toFloat(al_region region, al_osctime t)
{
	return al_osctime_ntp_to_float(*((al_osctime_ntptime *)&t));
}

al_osctime al_osctime_fromFloat(al_region region, double f)
{
	al_osctime_ntptime n;
	al_osctime_float_to_ntp(f, &n);
	return *((al_osctime *)&n);
}

al_osctime al_osctime_ntoht(al_region region, char *buf)
{
	if(!buf){
		return AL_OSCTIME_NULL;
	}
	char *p1 = buf;
	char *p2 = buf + 4;
	al_osctime tt = AL_OSCTIME_NULL;
	char *ttp1 = (char *)&tt;
	char *ttp2 = ttp1 + 4;
	*((uint32_t *)ttp1) = ntoh32(*((uint32_t *)p1));
	*((uint32_t *)ttp2) = ntoh32(*((uint32_t *)p2));
	return tt;
}

void al_osctime_htont(al_region region, al_osctime t, char *buf)
{
	if(!buf){
		return;
	}
	char *p1 = buf;
	char *p2 = buf + 4;
	char *ttp1 = (char *)&t;
	char *ttp2 = ttp1 + 4;

	*((uint32_t *)p1) = hton32(*((uint32_t *)ttp1));
	*((uint32_t *)p2) = hton32(*((uint32_t *)ttp2));
}

char *al_osctime_toISO8601(al_region r, al_osctime t)
{
	long l = _al_osctime_toISO8601(NULL, 0, t) + 1;
	char *buf = al_region_getBytes(r, l);
	_al_osctime_toISO8601(buf, l, t);
	return buf;
}

size_t _al_osctime_toISO8601(char *buf, size_t n, al_osctime t)
{
	al_osctime timetag = t;
	char s1[20];
	if(!buf){
		return 19 + snprintf(NULL, 0, "%05fZ", fmod(al_osctime_ntp_to_float(timetag), 1.0));//osc_strfmt_float64(NULL, 0, fmod(al_osctime_ntp_to_float(timetag), 1.0));
	}
	uint32_t secs = al_osctime_ntp_getSeconds(timetag);
	unsigned short int year, month, day, hour, minute, leap = 0;
	
	const uint64_t SEC_PER_YEAR = 31536000;
	const uint64_t SEC_PER_DAY = 86400;
	const unsigned short int SEC_PER_HR = 3600;
	const unsigned short int SEC_PER_MIN = 60;

    
	//secs=-2208988800;//SNTPGetUTCSeconds();
	//secs = abs(secs);
	for(year = 0; secs >= SEC_PER_YEAR; year++){ //determine # years elapse since epoch
		secs -= SEC_PER_YEAR;
		if(al_osctime_isleap(year)){
			secs -= SEC_PER_DAY;
		}
	}
	//year+=1970;  //1/1/1970, 00:00 is epoch
	year += 1900;  //1/1/1900, 00:00 is ntp epoch
	//for (yrcount=1970;yrcount<year;yrcount++) //scroll from 1970 to last year to find leap yrs.
	/*
	for(yrcount = 1900; yrcount < year; yrcount++){
		leap = al_osctime_isleap(yrcount);  
		if(leap == 1){
			secs -= SEC_PER_DAY;  //if it's a leap year, subtract a day's worth of seconds
		}
	} 
	*/
	leap = al_osctime_isleap(year); //Is this a leap year?


	for(day = 1; secs >= SEC_PER_DAY; day++){ //determine # days elapsed in current year
		secs -= SEC_PER_DAY;
	}
	for(hour = 0; secs >= SEC_PER_HR; hour++){  //determine hours elapsed in current day
		secs -= SEC_PER_HR;
	}
	for(minute = 0; secs >= SEC_PER_MIN; minute++){  //determine minutes elapsed in current hour
		secs -= SEC_PER_MIN;
	}
	
	//The value of secs at the end of the minutes loop is the seconds elapsed in the 
	//current minute.
	//Given the year & day of year, determine month & day of month
	month = al_osctime_getmonth(&day, leap);
	
	//convert it all to the iso-8601 string
	{
		char yearstr[5], monthstr[3], daystr[3], hourstr[3], minutestr[3], secstr[3];
		/*
		  itoa(year,yearstr);
		  itoa(month,monthstr);
		  itoa(day,daystr);
		  itoa(hour,hourstr);
		  itoa(minute,minutestr);
		  itoa(sec,secstr);
		*/
		snprintf(yearstr, sizeof(yearstr), "%04u", year);
		snprintf(monthstr, sizeof(monthstr), "%02u", month);
		snprintf(daystr, sizeof(daystr), "%02u", day);
		snprintf(hourstr, sizeof(hourstr), "%02u", hour);
		snprintf(minutestr, sizeof(minutestr), "%02u", minute);
		snprintf(secstr, sizeof(secstr), "%02u", secs);

		//All items except for year can be 1 or two digits, need to adjust accordingly
		/*
		if(month < 10){
			monthstr[1] = monthstr[0];
			monthstr[0] = '0';
		}
		if(day < 10){
			daystr[1] = daystr[0];
			daystr[0] = '0';
		}
		if(hour < 10){
			hourstr[1] = hourstr[0];
			hourstr[0] = '0';
		}
		if(minute < 10){
			minutestr[1] = minutestr[0];
			minutestr[0] = '0';
		}
		if(secs < 10){
			secstr[1] = secstr[0];
			secstr[0] = '0';
		}
		*/

		//put it all together
		long i = 0;
		s1[i++] = yearstr[0];
		s1[i++] = yearstr[1];
		s1[i++] = yearstr[2];
		s1[i++] = yearstr[3];
		s1[i++] = '-';
		s1[i++] = monthstr[0];
		s1[i++] = monthstr[1];
		s1[i++] = '-';
		s1[i++] = daystr[0];
		s1[i++] = daystr[1];
		s1[i++] = 'T';
		s1[i++] = hourstr[0];
		s1[i++] = hourstr[1];
		s1[i++] = ':';
		s1[i++] = minutestr[0];
		s1[i++] = minutestr[1];
		s1[i++] = ':';
		s1[i++] = secstr[0];
		s1[i++] = secstr[1];
		s1[i++] = '\0';
	}

	double d = al_osctime_ntp_to_float(timetag);
	double dm1 = fmod(d, 1.0);
	//int l = osc_strfmt_float64(NULL, 0, dm1);
	int l = snprintf(NULL, 0, "%05fZ", dm1);
	char s2[l + 1];
	snprintf(s2, l, "%05fZ", dm1);
	//osc_strfmt_float64(s2, l + 1, dm1);
	return snprintf(buf, n, "%s.%sZ", s1, s2+2);

	/*
	time_t i;
	struct tm *t;
	char s1[24];
	//char s2[10];
	double d;

	t_al_osctime_ntptime ntptime = *((t_al_osctime_ntptime *)&timetag);
    
	i = (time_t)al_osctime_ntp_to_ut(ntptime);
	d = al_osctime_ntp_to_float(ntptime);
	t = gmtime(&i);
	//printf("fucked: 0x%llx %f\n", i, d);
    	//t = localtime(&i);

	strftime(s1, 24, "%Y-%m-%dT%H:%M:%S", t);
	double dm1 = fmod(d, 1.0);
	int l = osc_strfmt_float64(NULL, 0, dm1);
	char s2[l + 1];
	//sprintf(s2, "%05fZ", fmod(d, 1.0));
	osc_strfmt_float64(s2, l + 1, dm1);
	return snprintf(buf, n, "%s.%sZ", s1, s2+2);
	*/
}

al_osctime al_osctime_fromISO8601(al_region region, char *s)
{
	al_osctime timetag;
	struct tm t;
	memset(&t, '\0', sizeof(struct tm));
	double fsec;
	//char s1[];
	//memset(s1, '\0', sizeof(s1));
    
	// read out the fractions part
	sscanf(s, "%*d-%*d-%*dT%*d:%*d:%lfZ", &fsec);
	/*
	time_t now = time(NULL);
	struct tm *unsafe = localtime(&now);
	if(unsafe){
		t = *unsafe;
	}
	*/
	// null-terminate the string
	//strncat(s1, s, AL_OSCTIME_MAX_STRING_LENGTH - 1);

	// parse the time
	strptime(s, "%Y-%m-%dT%H:%M:%S", &t);
	//printf("year: %d, month: %d, mday: %d, yday: %d, hour: %d, min: %d, sec: %d\n", t.tm_year, t.tm_mon, t.tm_mday, t.tm_yday, t.tm_hour, t.tm_min, t.tm_sec);

	const uint64_t SEC_PER_YEAR = 31536000;
	const uint64_t SEC_PER_DAY = 86400;
	const unsigned short int SEC_PER_HR = 3600;
	const unsigned short int SEC_PER_MIN = 60;

	uint32_t sec = 0;
	for(int i = 0; i < t.tm_year; i++){
		sec += SEC_PER_YEAR;
		if(al_osctime_isleap(i + 1900)){
			sec += SEC_PER_DAY;
		}
	}
	int leap = al_osctime_isleap(t.tm_year);
	int days_in_months[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
	if(leap){
		for(int i = 2; i < sizeof(days_in_months) / sizeof(int); i++){
			days_in_months[i] += 1;
		}
	}
	sec += ((days_in_months[t.tm_mon] + (t.tm_mday - 1)) * SEC_PER_DAY);
	sec += (t.tm_hour * SEC_PER_HR);
	sec += (t.tm_min * SEC_PER_MIN);
	sec += t.tm_sec;
	timetag.sec = sec;

	//timetag.sec = (t.tm_year * 3.15569e7) + (t.tm_yday * 86400) + (t.tm_hour * 3600) + (t.tm_min * 60) + t.tm_sec;
	//al_osctime_ut_to_ntp(mktime(&t), timetag);
	//t_al_osctime_ntptime n;
	//al_osctime_ut_to_ntp(mktime(&t), &n);

	/**************************************************
	 * The connversion from double precision float to 32-bit int obviously 
	 * results in a loss of precision that I don't think can be overcome...
	 **************************************************/
	timetag.frac_sec = (uint32_t)(fmod(fsec, 1.0) * 4294967295.0);
	return timetag;
	//*timetag = *((t_al_osctime *)(&n));
}

////////////////////////////////////////////////////////////////////////////////
// helper functions
////////////////////////////////////////////////////////////////////////////////
static void al_osctime_float_to_ntp(double d, al_osctime_ntptime *n)
{
	double sec;
	double frac_sec;

	/*
	if(d > 0) {
		n->sign = 1;
	} else {
		d *= -1;
		n->sign = -1;
	}
	*/
	frac_sec = fmod(d, 1.0);
	sec = d - frac_sec;
    
	n->sec = (uint32_t)(sec);
	n->frac_sec= (uint32_t)(frac_sec * 4294967295.0);
	//n->type = TIME_STAMP;
}

static double al_osctime_ntp_to_float(al_osctime_ntptime n)
{
	return ((double)(n.sec)) + ((double)((uint64_t)(n.frac_sec))) / 4294967295.0;
}

/*
static void al_osctime_ut_to_ntp(time_t ut, al_osctime_ntptime *n)
{
	struct timeval tv;
	struct timezone tz;
    
	gettimeofday(&tv, &tz); // this is just to get the timezone...
    
	n->sec = (uint64_t)2208988800UL + 
		(uint32_t)ut - 
		(uint64_t)(60 * tz.tz_minuteswest) + 
		(uint32_t)(tz.tz_dsttime == 1 ? 3600 : 0);
	//printf("%s ut: %lu n->sec: %u minuteswest: %d dst: %d\n", __func__, ut, n->sec, 60 * tz.tz_minuteswest, tz.tz_dsttime == 1 ? 3600 : 0);

	n->frac_sec = 0;
}

static time_t al_osctime_ntp_to_ut(al_osctime_ntptime n)
{
	struct timeval tv;
	struct timezone tz;
    
	gettimeofday(&tv, &tz); // this is just to get the timezone...

	// 2147483647
	// 2208988800
	time_t ut = n.sec - (time_t)2208988800UL + (time_t)(60 * tz.tz_minuteswest) - (time_t)(tz.tz_dsttime == 1 ? 3600 : 0);

	//printf("%s: ut: %lu n.sec: %u minuteswest: %d dst: %d\n", __func__, ut, n.sec, 60 * tz.tz_minuteswest, tz.tz_dsttime == 1 ? 3600 : 0);
	return ut;
}
*/
static uint32_t al_osctime_ntp_getSeconds(al_osctime t)
{
	return t.sec;
}
/*
static uint32_t al_osctime_ntp_getFraction(al_osctime t)
{
	return t.frac_sec;
}
*/
//Function isleap:  Determines if year is a leap year or not, without using modulo.
//Year is a leap year if year mod 400 = 0, OR if (year mod 4 = 0 AND year mod 100 <> 0).
static unsigned short int al_osctime_isleap(unsigned short int year)
{
	short int yrtest;
	yrtest=year;

	//year modulo 400 = 0? if so, it's a leap year
	while(yrtest>0){
		yrtest-=400;
	} 
	if(yrtest==0){ //then year modulo 400 = 0
		return 1; //it's a leap year
	}
	yrtest=year;  

	//year modulo 4 = 0 and year modulo 100 <>0? if so, it's a leap year
	while(yrtest>0){
		yrtest-=4;
	} 
	if(yrtest==0){ //then year modulo 4 = 0
		yrtest=year;
		while(yrtest>0){  //so test for modulo 100
			yrtest-=100;
		}
		if(yrtest<0){  //then year modulo 100 <>0
			return 1; //it's a leap year
		}else{
			return 0; //not a leap year
		}
	}else{
		return 0;  //year modulo 4 <> 0, not a leap year
	}
}

//Function getmonth computes the month and day of month given the day of the year,
//accounting for leap years
static unsigned short int al_osctime_getmonth(unsigned short int *day, unsigned short int leap)
{
	const unsigned short int JAN = 1;
	const unsigned short int FEB = 2;
	const unsigned short int MAR = 3;
	const unsigned short int APR = 4;
	const unsigned short int MAY = 5;
	const unsigned short int JUN = 6;
	const unsigned short int JUL = 7;
	const unsigned short int AUG = 8;
	const unsigned short int SEP = 9;
	const unsigned short int OCT = 10;
	const unsigned short int NOV = 11;
	const unsigned short int DEC = 12; 
	
	unsigned short int JAN_LAST = 31;
	unsigned short int FEB_LAST = 59;
	unsigned short int MAR_LAST = 90;
	unsigned short int APR_LAST = 120;
	unsigned short int MAY_LAST = 151;
	unsigned short int JUN_LAST = 181;
	unsigned short int JUL_LAST = 212;
	unsigned short int AUG_LAST = 243;
	unsigned short int SEP_LAST = 273;
	unsigned short int OCT_LAST = 304;
	unsigned short int NOV_LAST = 334;
	unsigned short int DEC_LAST = 365;

	//correct monthly end dates for leap years (leap=1=leap year, 0 otherwise)
	if(leap > 0){
		if(leap <= 1){
			FEB_LAST += leap;
			MAR_LAST += leap;
			APR_LAST += leap;
			MAY_LAST += leap;
			JUN_LAST += leap;
			JUL_LAST += leap;
			AUG_LAST += leap;
			SEP_LAST += leap;
			OCT_LAST += leap;
			NOV_LAST += leap;
			DEC_LAST += leap;

		}else if(leap > 1){ //error condition
			return 0;
		}
	}else if(leap < 0){  //error condition
		return 0;
	}
	//Determine month & day of month from day of year
	if(*day <= JAN_LAST){
		return JAN;  //day is already correct
	}else if(*day <= FEB_LAST){
		*day -= JAN_LAST;
		return FEB;
	}else if(*day <= MAR_LAST){
		*day -= (FEB_LAST);
		return MAR;
	}else if(*day <= APR_LAST){
		*day -= (MAR_LAST);
		return APR;
	}else if(*day <= MAY_LAST){
		*day -= (APR_LAST);
		return MAY;
	}else if(*day <= JUN_LAST){
		*day -= (MAY_LAST);
		return JUN;
	}else if(*day <= JUL_LAST){
		*day -= (JUN_LAST);
		return JUL;
	}else if(*day <= AUG_LAST){
		*day -= (JUL_LAST);
		return AUG;
	}else if(*day <= SEP_LAST){
		*day -= (AUG_LAST);
		return SEP;
	}else if(*day <= OCT_LAST){
		*day -= (SEP_LAST);
		return OCT;
	}else if(*day <= NOV_LAST){
		*day -= (OCT_LAST);
		return NOV;
	}else if(*day <= DEC_LAST){
		*day -= (NOV_LAST);
		return DEC;
	}else{
		return 0;
	}
}

////////////////////////////////////////////////////////////////////////////////
// for windows
////////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32) && !defined(__CYGWIN__)
/*  $NetBSD: strptime.c,v 1.28 2008/04/28 20:23:01 martin Exp $ */

/*-
 * Copyright (c) 1997, 1998, 2005, 2008 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code was contributed to The NetBSD Foundation by Klaus Klein.
 * Heavily optimised by David Laight
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD: strptime.c,v 1.28 2008/04/28 20:23:01 martin Exp $");
#endif
#include "namespace.h"
#include <sys/localedef.h>
#include <ctype.h>
#include <locale.h>
#include <string.h>
#include <time.h>
#include <tzfile.h>
#ifdef __weak_alias
__weak_alias(strptime,_strptime)
#endif
#define _ctloc(x)       (_CurrentTimeLocale->x)

/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#define ALT_E           0x01
#define ALT_O           0x02
#define LEGAL_ALT(x)        { if (alt_format & ~(x)) return NULL; }

static const char gmt[4] = { "GMT" };

static const u_char *conv_num(const unsigned char *, int *, uint, uint);
static const u_char *find_string(const u_char *, int *, const char * const *,
				 const char * const *, int);


static char *strptime(const char *buf, const char *fmt, struct tm *tm)
{
	unsigned char c;
	const unsigned char *bp;
	int alt_format, i, split_year = 0;
	const char *new_fmt;

	bp = (const u_char *)buf;

	while (bp != NULL && (c = *fmt++) != '\0') {
		/* Clear `alternate' modifier prior to new conversion. */
		alt_format = 0;
		i = 0;

		/* Eat up white-space. */
		if (isspace(c)) {
			while (isspace(*bp))
				bp++;
			continue;
		}

		if (c != '%')
			goto literal;


	again:      switch (c = *fmt++) {
		case '%':   /* "%%" is converted to "%". */
		literal:
			if (c != *bp++)
				return NULL;
			LEGAL_ALT(0);
			continue;

			/*
			 * "Alternative" modifiers. Just set the appropriate flag
			 * and start over again.
			 */
		case 'E':   /* "%E?" alternative conversion modifier. */
			LEGAL_ALT(0);
			alt_format |= ALT_E;
			goto again;

		case 'O':   /* "%O?" alternative conversion modifier. */
			LEGAL_ALT(0);
			alt_format |= ALT_O;
			goto again;

			/*
			 * "Complex" conversion rules, implemented through recursion.
			 */
		case 'c':   /* Date and time, using the locale's format. */
			new_fmt = _ctloc(d_t_fmt);
			goto recurse;

		case 'D':   /* The date as "%m/%d/%y". */
			new_fmt = "%m/%d/%y";
			LEGAL_ALT(0);
			goto recurse;

		case 'F':   /* The date as "%Y-%m-%d". */
			new_fmt = "%Y-%m-%d";
			LEGAL_ALT(0);
			goto recurse;

		case 'R':   /* The time as "%H:%M". */
			new_fmt = "%H:%M";
			LEGAL_ALT(0);
			goto recurse;

		case 'r':   /* The time in 12-hour clock representation. */
			new_fmt =_ctloc(t_fmt_ampm);
			LEGAL_ALT(0);
			goto recurse;

		case 'T':   /* The time as "%H:%M:%S". */
			new_fmt = "%H:%M:%S";
			LEGAL_ALT(0);
			goto recurse;

		case 'X':   /* The time, using the locale's format. */
			new_fmt =_ctloc(t_fmt);
			goto recurse;

		case 'x':   /* The date, using the locale's format. */
			new_fmt =_ctloc(d_fmt);
		recurse:
			bp = (const u_char *)strptime((const char *)bp,
						      new_fmt, tm);
			LEGAL_ALT(ALT_E);
			continue;

			/*
			 * "Elementary" conversion rules.
			 */
		case 'A':   /* The day of week, using the locale's form. */
		case 'a':
			bp = find_string(bp, &tm->tm_wday, _ctloc(day),
					 _ctloc(abday), 7);
			LEGAL_ALT(0);
			continue;

		case 'B':   /* The month, using the locale's form. */
		case 'b':
		case 'h':
			bp = find_string(bp, &tm->tm_mon, _ctloc(mon),
					 _ctloc(abmon), 12);
			LEGAL_ALT(0);
			continue;

		case 'C':   /* The century number. */
			i = 20;
			bp = conv_num(bp, &i, 0, 99);

			i = i * 100 - TM_YEAR_BASE;
			if (split_year)
				i += tm->tm_year % 100;
			split_year = 1;
			tm->tm_year = i;
			LEGAL_ALT(ALT_E);
			continue;

		case 'd':   /* The day of month. */
		case 'e':
			bp = conv_num(bp, &tm->tm_mday, 1, 31);
			LEGAL_ALT(ALT_O);
			continue;

		case 'k':   /* The hour (24-hour clock representation). */
			LEGAL_ALT(0);
			/* FALLTHROUGH */
		case 'H':
			bp = conv_num(bp, &tm->tm_hour, 0, 23);
			LEGAL_ALT(ALT_O);
			continue;

		case 'l':   /* The hour (12-hour clock representation). */
			LEGAL_ALT(0);
			/* FALLTHROUGH */
		case 'I':
			bp = conv_num(bp, &tm->tm_hour, 1, 12);
			if (tm->tm_hour == 12)
				tm->tm_hour = 0;
			LEGAL_ALT(ALT_O);
			continue;

		case 'j':   /* The day of year. */
			i = 1;
			bp = conv_num(bp, &i, 1, 366);
			tm->tm_yday = i - 1;
			LEGAL_ALT(0);
			continue;

		case 'M':   /* The minute. */
			bp = conv_num(bp, &tm->tm_min, 0, 59);
			LEGAL_ALT(ALT_O);
			continue;

		case 'm':   /* The month. */
			i = 1;
			bp = conv_num(bp, &i, 1, 12);
			tm->tm_mon = i - 1;
			LEGAL_ALT(ALT_O);
			continue;

		case 'p':   /* The locale's equivalent of AM/PM. */
			bp = find_string(bp, &i, _ctloc(am_pm), NULL, 2);
			if (tm->tm_hour > 11)
				return NULL;
			tm->tm_hour += i * 12;
			LEGAL_ALT(0);
			continue;

		case 'S':   /* The seconds. */
			bp = conv_num(bp, &tm->tm_sec, 0, 61);
			LEGAL_ALT(ALT_O);
			continue;

		case 'U':   /* The week of year, beginning on sunday. */
		case 'W':   /* The week of year, beginning on monday. */
			/*
			 * XXX This is bogus, as we can not assume any valid
			 * information present in the tm structure at this
			 * point to calculate a real value, so just check the
			 * range for now.
			 */
			bp = conv_num(bp, &i, 0, 53);
			LEGAL_ALT(ALT_O);
			continue;

		case 'w':   /* The day of week, beginning on sunday. */
			bp = conv_num(bp, &tm->tm_wday, 0, 6);
			LEGAL_ALT(ALT_O);
			continue;

		case 'Y':   /* The year. */
			i = TM_YEAR_BASE;   /* just for data sanity... */
			bp = conv_num(bp, &i, 0, 9999);
			tm->tm_year = i - TM_YEAR_BASE;
			LEGAL_ALT(ALT_E);
			continue;

		case 'y':   /* The year within 100 years of the epoch. */
			/* LEGAL_ALT(ALT_E | ALT_O); */
			bp = conv_num(bp, &i, 0, 99);

			if (split_year)
				/* preserve century */
				i += (tm->tm_year / 100) * 100;
			else {
				split_year = 1;
				if (i <= 68)
					i = i + 2000 - TM_YEAR_BASE;
				else
					i = i + 1900 - TM_YEAR_BASE;
			}
			tm->tm_year = i;
			continue;

		case 'Z':
			tzset();
			if (strncmp((const char *)bp, gmt, 3) == 0) {
				tm->tm_isdst = 0;
				#ifdef TM_GMTOFF
				tm->TM_GMTOFF = 0;
				#endif
				#ifdef TM_ZONE
				tm->TM_ZONE = gmt;
				#endif
				bp += 3;
			} else {
				const unsigned char *ep;

				ep = find_string(bp, &i,
						 (const char * const *)tzname,
						 NULL, 2);
				if (ep != NULL) {
					tm->tm_isdst = i;
					#ifdef TM_GMTOFF
					tm->TM_GMTOFF = -(timezone);
					#endif
					#ifdef TM_ZONE
					tm->TM_ZONE = tzname[i];
					#endif
				}
				bp = ep;
			}
			continue;

			/*
			 * Miscellaneous conversions.
			 */
		case 'n':   /* Any kind of white-space. */
		case 't':
			while (isspace(*bp))
				bp++;
			LEGAL_ALT(0);
			continue;


		default:    /* Unknown/unsupported conversion. */
			return NULL;
		}
	}

	return __UNCONST(bp);
}


static const u_char *
conv_num(const unsigned char *buf, int *dest, uint llim, uint ulim)
{
	uint result = 0;
	unsigned char ch;

	/* The limit also determines the number of valid digits. */
	uint rulim = ulim;

	ch = *buf;
	if (ch < '0' || ch > '9')
		return NULL;

	do {
		result *= 10;
		result += ch - '0';
		rulim /= 10;
		ch = *++buf;
	} while ((result * 10 <= ulim) && rulim && ch >= '0' && ch <= '9');

	if (result < llim || result > ulim)
		return NULL;

	*dest = result;
	return buf;
}

static const u_char *
find_string(const u_char *bp, int *tgt, const char * const *n1,
	    const char * const *n2, int c)
{
	int i;
	unsigned int len;

	/* check full name - then abbreviated ones */
	for (; n1 != NULL; n1 = n2, n2 = NULL) {
		for (i = 0; i < c; i++, n1++) {
			len = strlen(*n1);
			if (strncasecmp(*n1, (const char *)bp, len) == 0) {
				*tgt = i;
				return bp + len;
			}
		}
	}

	/* Nothing matched */
	return NULL;
}
#endif
