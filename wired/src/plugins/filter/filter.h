// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

/*-----------------------------.-------------------------------------.
| Filename: filter.h           | MiniSW filters                      |
| Author  : francis            |                                     |
| Started : 14/09/2002 20:19   |                                     |
`------------------------------|                                     |
                               |                                     |
                               `------------------------------------*/

#ifndef MINISW_SYNTH_FILTER_H_
# define MINISW_SYNTH_FILTER_H_

# include "patch.h"

# define FILTER_SIZE	5		// 3 inputs, 2 outpus


// init : memset(0)
typedef struct s_filter
{
	float	history[FILTER_SIZE];
	float	coefs[FILTER_SIZE];
	float	reamp;
}	t_filter;

void filter_set(t_filter* filter, int type, float cutoff, float resonance);
float filter_next(t_filter* filter, float input);


#endif // !MINISW_SYNTH_FILTER_H_
