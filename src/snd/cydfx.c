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

#include "cydfx.h"
#include "cyd.h"

#ifdef STEREOOUTPUT
void cydfx_output(CydFx *fx, Sint32 fx_l, Sint32 fx_r, Sint32 *left, Sint32 *right)
{
	*left = fx_l;
	*right = fx_r;
#else
Sint32 cydfx_output(CydFx *fx, Sint32 fx_input)
{
	Sint32 v = fx_input;
#endif

	if (fx->flags & CYDFX_ENABLE_CHORUS)
	{
#ifdef STEREOOUTPUT
		cydchr_output(&fx->chr, fx_l, fx_r, left, right);
#else
		// it's a stereo effect
#endif
	}

	if (fx->flags & CYDFX_ENABLE_REVERB)
	{
#ifdef STEREOOUTPUT
		cydrvb_cycle(&fx->rvb, fx_l, fx_l);
		cydrvb_output(&fx->rvb, &fx_l, &fx_r);
		*left += fx_l;
		*right += fx_r;
#else
		cydrvb_cycle(&fx->rvb, fx_input);
		v = cydrvb_output(&fx->rvb);
#endif
	}
	
	if (fx->flags & CYDFX_ENABLE_CRUSH)
	{
#ifdef STEREOOUTPUT
		*left &= fx->bit_drop;
		*right &= fx->bit_drop;
#else
		v &= fx->bit_drop;
#endif
	}
	
#ifndef STEREOOUTPUT
	return v;
#endif
}


void cydfx_init(CydFx *fx, int rate)
{
	cydrvb_init(&fx->rvb, rate);
#ifdef STEREOOUTPUT
	cydchr_init(&fx->chr, rate);
#endif
}


void cydfx_deinit(CydFx *fx)
{
	cydrvb_deinit(&fx->rvb);
#ifdef STEREOOUTPUT
	cydchr_deinit(&fx->chr);
#endif
}


void cydfx_set(CydFx *fx, const CydFxSerialized *ser)
{
	fx->flags = ser->flags;
	
	cydrvb_set_stereo_spread(&fx->rvb, ser->rvb.spread);
	
	for (int i = 0 ; i < CYDRVB_TAPS ; ++i)
	{
		cydrvb_set_tap(&fx->rvb, i, ser->rvb.tap[i].delay, ser->rvb.tap[i].gain);
	}
	
	cydchr_set(&fx->chr, ser->chr.rate, ser->chr.min_delay, ser->chr.max_delay, ser->chr.stereo_sep);
	
	fx->bit_drop = 0xffffffff << ser->crush.bit_drop;
}
