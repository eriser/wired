/*-----------------------------.-------------------------------------.
| Filename: filter.c           | MiniSW Biquad filter                |
| Author  : francis            |                                     |
| Started : 14/09/2002 20:24   |                                     |
`------------------------------|                                     |
                               |                                     |
                               `------------------------------------*/
#include "filter.h"
#include "misc.h"
#include "defs.h"
#include <math.h>
#include <string.h>

#ifndef M_LN2
#define M_LN2 0.69314718055994530942
#endif

static void filter_set_coefs(t_filter* filter, double b0, double b1, double b2, double a0, double a1, double a2)
{
	if (a0)
	{
		filter->coefs[0] = (float)(b0 / a0);
		filter->coefs[1] = (float)(b1 / a0);
		filter->coefs[2] = (float)(b2 / a0);
		filter->coefs[3] = (float)(-a1 / a0);
		filter->coefs[4] = (float)(-a2 / a0);
	}
	else
	{
		memset(filter->coefs, 0, sizeof (float) * FILTER_SIZE);
		filter->coefs[0] = 1;
	}
}

void filter_set(t_filter* filter, int type, float cutoff, float resonance)
{
	static const double dbGain = 24;
	double omega = 2 * M_PI * cutoff * SAMPLE_PERIOD;
	double sn = sin(omega);
	double cs = cos(omega);
	double mcs = 1 - cs;
	double alpha = sn * sinh(M_LN2 /2 * exp(-3*resonance) * omega / sn);
	double A = pow(10, dbGain / 40);
	double beta = sqrt(A + A);

	filter->reamp = 1;
	switch (type)
	{
	case filter_lp:
		filter_set_coefs(filter, mcs / 2, mcs, mcs / 2, 1 + alpha, -2 * cs, 1 - alpha);
		break;
	
	case filter_bp:
		filter->reamp = 6;
		filter_set_coefs(filter, alpha, 0, -alpha, 1 + alpha, - 2 * cs, 1 - alpha);
		break;

	case filter_notch:
		filter_set_coefs(filter, 1, -2 * cs, 1, 1 + alpha, -2 * cs, 1 - alpha);
		break;

	case filter_peq:
		filter->reamp = 0.5f;
		filter_set_coefs(filter, 1 + (alpha * A), -2 * cs, 1 - (alpha * A), 1 + (alpha /A), -2 * cs, 1 - (alpha /A));
		break;

	case filter_hp:
		filter->reamp = 0.15f;
		filter_set_coefs(filter,
			A * ((A + 1) + (A - 1) * cs + beta * sn),
			-2 * A * ((A - 1) + (A + 1) * cs),
			A * ((A + 1) + (A - 1) * cs - beta * sn),
			(A + 1) - (A - 1) * cs + beta * sn,
			2 * ((A - 1) - (A + 1) * cs),
			(A + 1) - (A - 1) * cs - beta * sn);
		break;
	}
}
	
float filter_next(t_filter* filter, float input)
{
	float out;
	int i;

	filter->history[2] = filter->history[1];
	filter->history[1] = filter->history[0];
	filter->history[0] = input;
	out = 0;
	for (i = 0; i < FILTER_SIZE; ++i)
		out += filter->history[i] * filter->coefs[i];
	filter->history[4] = filter->history[3];
	filter->history[3] = out;
	return out * filter->reamp;
}
