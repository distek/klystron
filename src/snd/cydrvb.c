/*
Copyright (c) 2009 Tero Lindeman (kometbomb)

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

#include "cydrvb.h"

void cydrvb_init(CydReverb *rvb, int rate)
{
	memset(rvb, 0, sizeof(rvb));
	
	int bufsize = CYDRVB_SIZE * rate / 1000;
	
	rvb->size = bufsize;
	rvb->rate = rate;
	rvb->buffer = calloc(sizeof(*rvb->buffer), bufsize);
	
	for (int i = 0 ; i < CYDRVB_TAPS ; ++i)
		cydrvb_set_tap(rvb, i, i * 100 + 50, CYDRVB_0dB / (i*i + 2));
}


void cydrvb_deinit(CydReverb *rvb)
{
	free(rvb->buffer);
	rvb->buffer = NULL;
}


void cydrvb_cycle(CydReverb *rvb, Sint32 input)
{
	for (int i = 0 ; i < CYDRVB_TAPS ; ++i)
	{
		++rvb->tap[i].position;
		if (rvb->tap[i].position >= rvb->size)
			rvb->tap[i].position = 0;
	}
	
	++rvb->position;
	if (rvb->position >= rvb->size)
		rvb->position = 0;
	rvb->buffer[rvb->position] = input;
}


Sint32 cydrvb_output(CydReverb *rvb)
{
	Sint32 o = 0;
	
	for (int i = 0 ; i < CYDRVB_TAPS ; ++i)
	{
		o += rvb->tap[i].gain * rvb->buffer[rvb->tap[i].position] / CYDRVB_0dB;
	}
	
	return o;
}


void cydrvb_set_tap(CydReverb *rvb, int idx, int delay_ms, int gain)
{
	rvb->tap[idx].delay = delay_ms;
	rvb->tap[idx].position = (rvb->position - (delay_ms * rvb->rate / 1000) + rvb->size) % rvb->size;
	rvb->tap[idx].gain = gain;
}