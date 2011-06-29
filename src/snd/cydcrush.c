/*
Copyright (c) 2009-2010 Tero Lindeman (kometbomb)

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

#include "cydcrush.h"


#ifdef STEREOOUTPUT
void cydcrush_output(CydCrush *crush, Sint32 in_l, Sint32 in_r, Sint32 *out_l, Sint32 *out_r)
{
	if (crush->counter++ >= crush->downsample)
	{
		crush->counter = 0;
		crush->hold_l = in_l & crush->bit_drop;
		crush->hold_r = in_r & crush->bit_drop;
	}

	*out_l = crush->hold_l;
	*out_r = crush->hold_r;
}
#else
Sint32 cydcrush_output(CydCrush *crush, Sint32 input)
{
	if (crush->counter++ >= crush->downsample)
	{
		crush->counter = 0;
		crush->hold = input & crush->bit_drop;
	}

	return crush->hold;
}
#endif


void cydcrush_set(CydCrush *crush, int downsample, int bit_drop)
{
	crush->downsample = downsample * crush->sample_rate / 44100;
	crush->counter = 0;
	crush->bit_drop = 0xffffffff << bit_drop;
}


void cydcrush_init(CydCrush *crush, int sample_rate)
{
	crush->sample_rate = sample_rate;
	crush->counter = 0;
}


void cydcrush_deinit(CydCrush *crush)
{
}

