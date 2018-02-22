#include <stdlib.h>
#include "stdio.h"

#define N 20
struct pile {
	char a[N];
	int sommet;
} ;

typedef struct pile pile;

pile *init_pile(void)
{
	pile *p;
	p = malloc(sizeof(struct pile));
	p->sommet = -1;
	return p;
}

int pile_est_full(pile *p)
{
	return (p->sommet == N - 1);
}

int pile_est_vide(pile *p)
{
	return (p->sommet == -1);
}

void empile(pile *p, char c)/*既然参数是指针，就不用返回指针*/
{
	if(!pile_est_full(p))
	{
		p->sommet += 1;
		p->a[p->sommet] = c;
	}
}

void depile(pile *p)
{
	if(!pile_est_vide(p))
	{
		p->sommet -= 1;
	}
}

char top(pile *p)
{
	if(!pile_est_vide(p))
		return (p->a[p->sommet]);
	else
		return ' ';
}

void affiche(pile *p, char *l)
{
	int i = 0;
	int n = p->sommet;
	while(n != -1)
	{
		l[i] = p->a[n];
		i += 1;
		n -= 1;
	}
}

void destroy(pile *p)
{
	free(p);
}


