#ifndef _SOLVER_H_
#define _SOLVER_H_

#include <stdlib.h>
#include <string.h>

typedef struct          s_matrice
{
  double                *mat;
  unsigned int          dimx;
  unsigned int          dimy;
}                       t_matrice;

typedef struct          s_eq
{
  unsigned int          dim;
  t_matrice             *matrice;
  t_matrice             *vecteur;
  t_matrice             *vectres;
}                       t_eq; 

class Solver
{
  public:
    Solver();
    int Solve(t_eq *);
    t_matrice *Allocmat(unsigned int, unsigned int);
    void Freemat(t_matrice *);
  private:
    t_matrice *Identite(unsigned int);
    void Swaplines(t_eq *, unsigned int, unsigned int);
    t_matrice *Multmat(t_matrice *, t_matrice *);
};

#endif
