/*compiler avec gcc -Wall -Wextra -ansi -lm*/
/*mon programme satisfait tous les demandes du professeur*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pile.h"

static int p_size;/*le nombre de pile*/
static int t_size;/*la longueur de la table*/
static int step = 0;/*pour assurer jouer tour a tour*/
static int x = 1;/*la coordonnee initiale de x*/
static int y = 1;/*la coordonnee initiale de y*/
static char qua = 'n';/*'n' signifie que la choix de seisme est 'non'*/
static char var = 'n';/*'n' signifie que la choix de la variante est 'non'*/
static int q[100][2];/*garder les cases qui s'effondrer(au maximum 100 cases), '2' garde les coordonnees de x et y*/
static int xyz[2000][3];/*garder les cases qui sont en meme couleur(au maximum 2000), 3 garde les coordonnees tridimensionnels*/
static int q_size = 0;/*le numbre des cases qui s'effrondrent*/
static int con_size = 0;/*tous les compositions de p_size jetons en meme couleurs*/

void init_table(pile *p1[][p_size]);
void affiche_table(pile *p[][p_size]);
void empty_table(pile *p[][p_size]);
int collect_jeton(pile *p[][p_size], char c);
void combine(int con[][p_size], int n, int m, int b[], int M);
int combine2(int con[], int con2[][2], int n, int con2_size);
int test(int con2[][2], int con2_size, char c);
int is_success(pile *p[][p_size], char c);
void mettre_jeton(pile *p[][p_size], int *x, int *y);
void bouger(pile *p[][p_size], char c, int *x, int *y);
void quake(pile *p[][p_size]);
void clear(void);
int collect_jeton2(pile *p[][p_size], char c);
int is_success2(pile *p[][p_size], char c);
int in_q(int i, int j);

/*enxure:l'affichage est rafraichi apres chaque choix, pour differents systems, les commandes sont differents*/
void clear(void) {
	#ifdef __linux
		system("clear");
	#elif defined _WIN32 || _WIN64
		system("cls");
	#endif
}
/*ensure:initiate la table, les elements de p[][p_size] sont les pointeurs qui orientent les piles */
void init_table(pile *p1[][p_size]) {
	int i, j;
	for(i = 0; i < p_size; i++)
		for(j = 0; j < p_size; j++)
			p1[i][j] = init_pile();
}

int in_q(int i, int j) {
	int k;
	for(k = 0; k < q_size; k++)
		if((i == q[k][0] && j == q[k][1] + 1)||(i == q[k][0] && j == q[k][1] - 1)||(i == q[k][0] + 1 && j == q[k][1])||(i == q[k][0] - 1 && j == q[k][1])||(i == q[k][0] - 1 && j == q[k][1] + 1)||(i == q[k][0] + 1 && j == q[k][1] + 1)||(i == q[k][0] - 1 && j == q[k][1] - 1)||(i == q[k][0] + 1 && j == q[k][1] - 1))
			return 1;
	return 0;
}

/*ensure:faire l'affichage de la table, les jetons selectionnes sont entournes par quatre '+', les jetons qui s'effondrent sont entournes par huit '*', il y a une ligne de la longeur du table et en dussus une ligne de quel joueur c'est le tour et un invite de choix d'action*/
void affiche_table(pile *p[][p_size]) {
	char c;
	int i, j;
	char ch[t_size];
	for(i = 0; i < t_size; i++)
		ch[i] = ' ';
	affiche(p[(x-1)/3][(y-1)/3], ch);
	clear();
	for(i = 0; i < t_size; i++) {
		for(j = 0; j < t_size; j++) {

				if(i % 3 == 1 && j % 3 == 1) {
					c = top(p[(i-1)/3][(j-1)/3]);
					printf("%c", c);
				}
				else if(in_q(i, j))
					printf("*");
				else if((i == x && j == y + 1)||(i == x && j == y - 1)||(i == x + 1 && j == y)||(i == x - 1 && j == y))
					printf("+");
				else
					printf(" ");
		}
		printf("\t[%c]\n", ch[i]);
	}
	printf(" ");
	for(i = 0; i < p_size; i++)
		printf("-  ");
	char next_user;
	if(step % 2 == 0)
		next_user = 'X';
	else
		next_user = 'O';
	printf("\n%c opt?", next_user);
	printf(" \n");
}

void empty_table(pile *p[][p_size]) {
	int i, j;
	for(i = 0; i < p_size; i++)
		for(j = 0; j < p_size; j++)
			free(p[i][j]);
}

/*ensure:collecter tous les jetons de couleur 'c', et les gardent dans la table xyz[][3]*/
int collect_jeton(pile *p[][p_size], char c) {
	int xyz_size = 0;
	int i, j, k;
	for(i = 0; i < p_size; i++)
		for(j = 0; j < p_size; j++)
			for (k = p[i][j]->sommet; k > -1; k--)
				if(p[i][j]->a[k] == c) {
					xyz[xyz_size][0] = i;
					xyz[xyz_size][1] = j;
					xyz[xyz_size][2] = k;
					xyz_size += 1;
				}
	return xyz_size;
}

/*ensure:faire tous les compositions de p_size jetons qui sont dans xyz[][3], et les gardent dans la table con[][p_size](plus des details sont dans mon rapport)*/
void combine(int con[][p_size], int n, int m, int b[], int M) {
	int i, j;
	for (i = n; i >= m; i--) {
		b[m-1] = i - 1;
		if (m > 1)
			combine(con, i-1, m-1, b, M);
		else {
			for (j = M - 1; j >= 0; j--)
				con[con_size][j] = b[j];
			con_size += 1;
		}
	}
}

/*ensure:faire les tous les combisitions de 2 jetons qui sont dans con[][p_size], et les gardent dans con2[][2](plus des details sont dans mon rapport)*/
int combine2(int con[], int con2[][2], int n, int con2_size) {
	int i, j;
	for (i = 0; i < n; i++)
		for (j = i + 1; j < n; j ++) {
			con2[con2_size][0] = con[i];
			con2[con2_size][1] = con[j];
			con2_size += 1;
		}
	return con2_size;
}

/*ensure:tester si les p_size jetons dans con[][p_size] s'accordent avec les conditions de la victoire(p_size jetons sont colineaire est sans cesse), max_distance est la distance plus longe des ces p_size jetons.moyenne[i] est le premiere vecteur entre 2 jetons.Si tous les vecteurs unitaires sont equivalents, ils sont colineaire.Et pour assurer ils sont sans cesse, je verifier la max_distance(plus des details sont dans mon rapport) */
int test(int con2[][2], int con2_size, char c) {
	double vecteur[con2_size][3];
	int i, j;

	double max_distance = 0.0;

	for (i = 0; i < con2_size; i++) {
		for (j = 0; j < 3; j++)
			vecteur[i][j] = (double)(xyz[con2[i][0]][j] - xyz[con2[i][1]][j]);

		double distance = sqrt(pow(vecteur[i][0], 2.0) + pow(vecteur[i][1], 2.0) + pow(vecteur[i][2], 2.0));

		if (distance > max_distance)
			max_distance = distance;

		for (j = 0; j < 3; j++)
			vecteur[i][j] = fabs(vecteur[i][j] / distance);
	}
	double moyenne[3];
	for (i = 0; i < 3; i++)
		moyenne[i] = vecteur[0][i];
	int result = 1;
	for (i = 0; i < con2_size; i++) {
		for (j = 0; j < 3; j++) {
			if (fabs(vecteur[i][j] - moyenne[j]) > 0.0001)
				result = 0;
		}
	}

	if (!(fabs(max_distance - (p_size - 1)) < 0.0001 || fabs(max_distance - sqrt(2) * (p_size-1)) < 0.0001 || fabs(max_distance - sqrt(3) * (p_size-1)) < 0.0001))
		result = 0;

	if (result == 1) {
		printf("\n%c is winner\n", c);
	}
	return result;
}

/*ensure:le judgement de la victoire*/
int is_success(pile *p[][p_size], char c) {
	int result = 0;
	int con[2000][p_size];
	int b[p_size];
	int xyz_size = collect_jeton(p, c);
	if (xyz_size > p_size - 1) {
		con_size = 0;
		combine(con, xyz_size, p_size, b, p_size);

		int i;
		for (i = 0; i < con_size; i++) {
			int con2[p_size*(p_size-1) / 2][2];
			int con2_size = combine2(con[i], con2, p_size, 0);
			if(test(con2, con2_size, c))
				result = 1;
		}
	}
	return result;
}

/*ensure:apres la seisme, il faut verifier tous les deux couleurs, mais s'il n'y a pas de la seisme, il est suffisant de verifier la couleur de ce tour, et apres la seisme, si le joueur gagne, le jeu termine*/
void mettre_jeton(pile *p[][p_size], int *x, int *y) {
	char c;
	if(step % 2 == 0)
		c = 'X';
	else
		c = 'O';
	empile(p[(*x-1)/3][(*y-1)/3], c);
	step += 1;
	affiche_table(p);
	if(var == 'y')
	{
		if (is_success(p, c) && (qua != 'y' && qua != 'Y'))
			exit(1);
		else if(qua == 'y'||qua == 'Y')
		{
			quake(p);
			if(is_success(p, c))
				exit(1);
		}
	}
	else
	{
		if (is_success2(p, c) && (qua != 'y' && qua != 'Y'))
			exit(1);
		if(qua == 'y'||qua == 'Y')
		{
			quake(p);
			if(is_success2(p, 'X'))
				exit(1);
			else if(is_success2(p, 'O'))
				exit(1);
		}
	}
}

/*ensure:si le joueur effectue 'p' ou 'r', le 'step' va additionner 1, si effectue 'q', le joueur quitte et le jeu termine*/
void bouger(pile *p[][p_size], char c, int *x, int *y) {
	if(c == 'l' && *y < t_size - 4)
	{
		*y += 3;
		affiche_table(p);
	}
	else if(c == 'i' && *x > 3)
	{
		*x -= 3;
		affiche_table(p);
	}
	else if(c == 'j' && *y > 3)
	{
		*y -= 3;
		affiche_table(p);
	}
	else if(c == 'k' && *x < t_size - 4) {
		*x += 3;
		affiche_table(p);
	}
	else if(c == 'p')
		mettre_jeton(p, x, y);
	else if(c == 'r') {
		depile(p[(*x-1)/3][(*y-1)/3]);
		step += 1;
		affiche_table(p);
	}
	else
		printf("you have entered a wrong commande!\n");
}

/*ensure:j'utilise un nombre aleatoir pour simuler la probabilite de la seisme,et le nombre des jetons qui vont disparaitre sont toujours randoms et q[][2] garde les jeton qui s'effondre(c'est pour faire l'affichage de la table)*/
void quake(pile *p[][p_size]) {
	int i, j, h, q1;
    double probabilite;
    for (i = 0; i < p_size; i++) {
        for (j = 0; j < p_size; j++) {
            if (! pile_est_vide(p[i][j])) {
                h = p[i][j]->sommet + 1;
                probabilite = 1.0 - pow(2.0, -1.0*h / (2.0*p_size
                	));
                double rand_p = (rand() % 10 + 1) / 10.0;
                if (probabilite > rand_p) {
					int count = rand() % h + 1;
                    for (q1 = 0; q1 < count; q1++)
                        depile(p[i][j]);
                    q[q_size][0] = 3 * i + 1;
                    q[q_size][1] = 3 * j + 1;
                    q_size += 1;
                }
            }
        }
    }
    affiche_table(p);
    q_size = 0;
}

/*ensure:collecter les jeton d'une meme couleur, c'est pour la variante vue du dessus*/
int collect_jeton2(pile *p[][p_size], char c) {
	int i, j;
	int xyz_size = 0;
	for(i = 0; i < p_size; i++)
		for(j = 0; j < p_size; j++)
		{
			if(top(p[i][j]) == c)
			{
				xyz[xyz_size][0] = i;
				xyz[xyz_size][1] = j;
				xyz[xyz_size][2] = 0;
				xyz_size += 1;
			}
		}
		return xyz_size;
}

/*ensure:le jugement de la victoire, c'est pour la variante vue du dessus*/
int is_success2(pile *p[][p_size], char c) {
	int result = 0;
	int con[2000][p_size];
	int b[p_size];
	int xyz_size = collect_jeton2(p, c);
	if (xyz_size > p_size - 1) {
		con_size = 0;
		combine(con, xyz_size, p_size, b, p_size);

		int i;
		for (i = 0; i < con_size; i++) {
			int con2[p_size*(p_size-1) / 2][2];
			int con2_size = combine2(con[i], con2, p_size, 0);
			if(test(con2, con2_size, c))
				result = 1;
		}
	}
	return result;
}

int main()
{
	printf("the size of your table? ");
	scanf("%d", &p_size);
	printf("\nvariante 3D?(n/y) ");
	scanf(" %c", &var);
	printf("\nDo you want quake in the game?(n/y) ");
	scanf(" %c", &qua);
	t_size = 3 * p_size;
	pile *p[p_size][p_size];
	init_table(p);
	affiche_table(p);
	char c;
	scanf(" %c", &c);
	/*le jeu termine quand input 'q'*/
	while(c != 'q') {
		bouger(p, c, &x, &y);
		scanf(" %c", &c);
	}
	empty_table(p);
	return 0;
}
