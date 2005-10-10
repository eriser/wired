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
