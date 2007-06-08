/*
** Copyright (C) 2004-2006 by Wired Team and Robert Melby
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
** 
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <list.h>
#include <stdlib.h>

#define CREATE_NODE(node, e, n) node = (t_list *)malloc(sizeof(t_list));\
				node->elem = e;\
				node->next = n;

void		 listPrepend(t_list **l, void *e)
{
  t_list	*newnode;

  CREATE_NODE(newnode, e, *l);
  *l = newnode;
}

void		listAppend(t_list **l, void *e)
{
  t_list	*newnode;
  t_list	*t;

  CREATE_NODE(newnode, e, 0);
  if (!*l)
    *l = newnode;
  else
  {
    for (t = *l; t->next; t = t->next) ;
    t->next = newnode;
  }
}

void		listDelete(t_list **l, void (*f)(void *))
{
  t_list	*t;

  for (t = *l; *l; (*l) = (*l)->next)
  {
    f(t->elem);
    free(t);
  }
}

int		listLength(t_list *l)
{
  int		i;

  for (i = 0; l; i++, l = l->next);
  return (i);
}
