// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef _SPLINES_H_
#define _SPLINES_H_

#include "Solver.h"

typedef struct s_pt
{
  int x;
  int y;
} t_pt;

class Splines
{
  public:
    Splines();
    ~Splines();
    void SetPoints(unsigned long, t_pt *);
    t_pt *GetPoint(double);
  private:
    unsigned long Fact(unsigned long);
    unsigned long C(unsigned long, unsigned long);
    double B(unsigned long, unsigned long, double);
    void Solve_eq(unsigned long, t_pt *);
    unsigned long NBPT;
    t_pt *pts;
    Solver solver;
};

#endif
