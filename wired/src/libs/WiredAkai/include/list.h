/*
** Copyright (C) 2004 by Wired Team and Robert Melby
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
