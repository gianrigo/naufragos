int direcao(int x, int y);
double angulo(double x, double y);
double modulo(vetor v);
void markov(item *p, double deltaT);

/*M_PI nao e padrao ansi*/

#ifndef M_PI
#define M_PI 3.141592653589793238462643
#endif

#define VMIN 150
#define VMAX 200

#define L 0
#define NE 1
#define N 2
#define NO 3
#define O 4
#define SO 5
#define S 6
#define SE 7
