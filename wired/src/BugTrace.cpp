// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

void*	operator new(size_t size)
{
	return (malloc(size));
}

void	operator delete(void *ptr)
{
	free(ptr);
}

void*	operator new[](sizte_t size)
{
	return (malloc(size * sizeof(this)));
}

void	operator delete[](void *ptr)
{
	free(ptr);
}
