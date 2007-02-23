// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991


#ifndef _LIST_H_
#define _LIST_H_

typedef struct	s_list
{
  void		*elem;
  struct s_list	*next;
}		t_list;

#define listIsEmpty(l)		(!l)
#define elt(l, t)		((t)l->elem)

void		listPrepend(t_list **, void *);
void		listAppend(t_list **, void *);
int		listLength(t_list *);
void		listDelete(t_list **, void (*)(void *));

#endif
