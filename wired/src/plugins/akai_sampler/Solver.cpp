#include "Solver.h"

#define A(x, y)	eq->matrice->mat[y * eq->dim + x]
#define P(x, y)	prod->mat[y * eq->dim + x]
#define B(x)	eq->vecteur->mat[x]
#define X(x)	eq->vectres->mat[x]

Solver::Solver()
{
}

t_matrice       *Solver::Allocmat(unsigned int dimx, unsigned int dimy)
{
  t_matrice     *mat;

  mat = (t_matrice *)malloc(sizeof(t_matrice));
  mat->dimx = dimx;
  mat->dimy = dimy;
  mat->mat = (double *)malloc(sizeof(double) * dimx * dimy);
  memset(mat->mat, 0, sizeof(double) * dimx * dimy);
  return (mat);
}

void            Solver::Freemat(t_matrice *mat)
{
  free(mat->mat);
  free(mat);
}

t_matrice	*Solver::Identite(unsigned int dim)
{
  t_matrice	*mat;
  unsigned int i;

  mat = Allocmat(dim, dim); 
  for (i = 0; i < dim; i++)
    mat->mat[i * dim + i] = 1;
  return (mat);
}

void		Solver::Swaplines(t_eq *eq, unsigned int l1, unsigned int l2)
{
  unsigned int i;
  double 	tmp;

  for (i = 0; i < eq->dim; i++)
  {
    tmp = A(i, l1);
    A(i, l1) = A(i, l2);
    A(i, l2) = tmp;
  } 
  tmp = B(l1);
  B(l1) = B(l2);
  B(l2) = tmp;
}

t_matrice	*Solver::Multmat(t_matrice *mat1, t_matrice *mat2)
{
  t_matrice	*mat;
  unsigned int		dimx, dimy;
  unsigned int		i, j, k;

  if (mat1->dimx != mat2->dimy)
    return (0);
  dimx = mat2->dimx;
  dimy = mat1->dimy;
  mat = Allocmat(dimx, dimy);
  for (i = 0; i < dimy; i++)
    for (j = 0; j < dimx; j++)
      for (k = 0; k < mat2->dimy; k++)
        mat->mat[i * dimx + j] += mat1->mat[i * mat1->dimx + k] * 
                                  mat2->mat[k * mat2->dimx + j];
  return (mat);
}

int		Solver::Solve(t_eq *eq)
{
  unsigned int		i, j, step;
  t_matrice	*prod, *a1, *b1;
 
  for (step = 0; step < eq->dim - 1; step++)
  {
    if (!A(step, step))
    {
      for (i = step + 1; (i < eq->dim) && (!A(step, i)); i++) ;
      if (i >= eq->dim)
        return (-1);
      Swaplines(eq, step, i);
    }
    prod = Identite(eq->dim);
    for (i = step + 1; i < eq->dim; i++)
      P(step, i) = -A(step, i) / A(step, step);
    a1 = Multmat(prod, eq->matrice);
    b1 = Multmat(prod, eq->vecteur);
    Freemat(prod);
    Freemat(eq->matrice);
    Freemat(eq->vecteur);
    eq->matrice = a1;
    eq->vecteur = b1;
  }
  for (i = eq->dim - 1; ((signed)i) >= 0; i--)
  {
    X(i) = B(i);
    for (j = eq->dim - 1; j > i; j--)
      X(i) -= X(j) * A(j, i);
    X(i) /= A(i, i);
  }
  return (0);
}
