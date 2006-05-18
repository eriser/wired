// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "Splines.h"
#include <math.h>

Splines::Splines()
{
  NBPT = 0;
  pts = NULL;
}

unsigned long Splines::Fact(unsigned long n)
{
  unsigned long f = 1;
  unsigned long i;

  for (i = 2; i <= n; i++)
    f *= i;
  return f;
}

unsigned long Splines::C(unsigned long n, unsigned long i)
{
  return Fact(n) / (Fact(i) * Fact(n - i));
}

double Splines::B(unsigned long i, unsigned long n, double t)
{
  return C(n, i) * pow(1 - t, n - i) * pow(t, i);
}

t_pt  *Splines::GetPoint(double t)
{
  double x = 0;
  double y = 0;
  unsigned long i;
  for (i = 0; i < NBPT; i++)
  {
    double b = B(i, NBPT - 1, t);
    x += b * pts[i].x;
    y += b * pts[i].y;
  }
  t_pt *res = (t_pt *)malloc(sizeof(t_pt));
  res->x = (int)x;
  res->y = (int)y;
  return (res);
}

void Splines::Solve_eq(unsigned long n, t_pt *pts)
{
  t_eq eq;
  unsigned long i, j;

  eq.dim = n - 1;
  eq.matrice = solver.Allocmat(n - 1, n - 1);
  eq.vecteur = solver.Allocmat(1, n - 1);
  eq.vectres = solver.Allocmat(1, n - 1);
  for (i = 1; i < n; i++)
  {
    for (j = 1; j < n; j++)
      eq.matrice->mat[(i - 1) * (n - 1) + (j - 1)] = C(n, j) * pow(1 - ((double)i)/n, n - j) * pow(((double)i) / n, j);
    eq.vecteur->mat[i - 1] = pts[i].x - pts[0].x * pow(1 - ((double)i)/n, n) - pts[n].x * pow(((double)i)/n, n);
  }
  solver.Solve(&eq);
  for (i = 1; i < n; i++)
    pts[i].x = (int)eq.vectres->mat[i - 1];
  for (i = 1; i < n; i++)
  {
    for (j = 1; j < n; j++)
      eq.matrice->mat[(i - 1) * (n - 1) + (j - 1)] = C(n, j) * pow(1 - ((double)i)/n, n - j) * pow(((double)i) / n, j);
    eq.vecteur->mat[i - 1] = pts[i].y - pts[0].y * pow(1 - ((double)i)/n, n) - pts[n].y * pow(((double)i)/n, n);
  }
  solver.Solve(&eq);
  for (i = 1; i < n; i++)
    pts[i].y = (int)eq.vectres->mat[i - 1];
  solver.Freemat(eq.matrice);
  solver.Freemat(eq.vecteur);
  solver.Freemat(eq.vectres);
}

void Splines::SetPoints(unsigned long NBPT, t_pt *points)
{
  if (pts)
    free(pts);
  pts = (t_pt *)malloc(sizeof(t_pt) * NBPT);
  this->NBPT = NBPT;
  memcpy(pts, points, sizeof(t_pt) * NBPT);
  Solve_eq(NBPT - 1, pts);
}

Splines::~Splines()
{
  if (pts)
    free(pts);
}
