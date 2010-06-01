#include "bib/tipos.h"
#include "bib/mar.h"
#include "bib/desloc.h"
#include "bib/colisao.h"
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <allegro.h>

#define VERCOLISOES /*Se nao quizer mais ver os circulos comentar essa linha*/
#ifdef VERCOLISOES
  #define	COLISAO  circle(buffer, proximo->p.pos.x,proximo->p.pos.y ,proximo->p.raio, VERMELHO);
#else
  #define	COLISAO		
#endif

#define AZUL  makecol(65,105,255)
#define PRETO  makecol(0,0,0)
#define BRANCO  makecol(255,255,255)
#define MARROM  makecol(139,69,19)
#define LARANJA  makecol(255,60,0)
#define VERDE makecol(60,179,113)
#define VERMELHO makecol(255,0,0)

#define D 400 /* Parece mto maior do que deveria ser, 10 */ 
#define R_PESS 5
#define R_BOTE 21
#define R_ASIMOV 80


fila atualizaMar(fila naufragos, int l_max, int c_max, double deltaT)
{
	fila proximo = naufragos;
	
	/* Ver se não estão sendo gerados no mesmo lugar */

		while(proximo!=NULL) {proximo->p.atualizada=0; proximo = proximo->prox;} 
	
		naufragos = detectaColisao( naufragos, deltaT);		
	
		proximo = naufragos;
		while( proximo != NULL)
		{
			if(!proximo->p.atualizada){
				markov(&proximo->p,deltaT);
        proximo->p.atualizada = 1;
					/*Se for 0 eh o topo. 1 eh o chao. 2 eh a parede esquerda e 3 eh a parede direita.*/
					if( (proximo->p.pos.y - proximo->p.raio) < 0)
						colideComBorda(&proximo->p, 0, 768, 1024);				
					else if( (proximo->p.pos.y + proximo->p.raio) > l_max)
						colideComBorda(&proximo->p, 1, 768, 1024);
					else if( (proximo->p.pos.x - proximo->p.raio) < 0)
						colideComBorda(&proximo->p, 2, 768, 1024);
					else if( (proximo->p.pos.x + proximo->p.raio) > c_max )
						colideComBorda(&proximo->p, 3, 768, 1024);		
			}
			proximo = proximo -> prox;
		}
	return naufragos;
}

void imprimeMar(fila naufragos)
{
	fila proximo;
	BITMAP *buffer;
	double ang;

	buffer = create_bitmap(screen->w, screen->h);
  
	clear_to_color(buffer, AZUL);

	for(proximo = naufragos; proximo != NULL; proximo = proximo -> prox)
	{
		if( proximo->p.categoria == 'p' )	
			circlefill(buffer, proximo->p.pos.x,proximo->p.pos.y ,proximo->p.raio, PRETO);                          
		else if( proximo->p.categoria == 'r' )
		{	
			rectfill(buffer, (proximo->p.pos.x)-proximo->p.raio/sqrt(2),(proximo->p.pos.y)+proximo->p.raio/sqrt(2),(proximo->p.pos.x)+proximo->p.raio/sqrt(2),(proximo->p.pos.y)-proximo->p.raio/sqrt(2), MARROM);/*risco de seg fault*/
			COLISAO	
		}
		else if( proximo->p.categoria == 'a')
		{
			rectfill(buffer, (proximo->p.pos.x)-100, (proximo->p.pos.y)+50, (proximo->p.pos.x)+100, (proximo->p.pos.y) - 50, VERDE);
			COLISAO	
		}
		else if( proximo->p.categoria == '1' )
		{
			ang = atan2(proximo->p.vel.y,proximo->p.vel.x);

			triangle(buffer, (proximo->p.pos.x-15), (proximo->p.pos.y-17), (proximo->p.pos.x)/*+proximo->p.raio*cos(ang)*/, (proximo->p.pos.y+17)/*+proximo->p.raio*sin(ang)*/, (proximo->p.pos.x+15), (proximo->p.pos.y-17),LARANJA);/* Tentativas de rotacionar o bote */
			COLISAO		
		}

		else if( proximo->p.categoria == '2' )
		{
			triangle(buffer, (proximo->p.pos.x)-15, (proximo->p.pos.y)-17, (proximo->p.pos.x), (proximo->p.pos.y)+17, (proximo->p.pos.x)+15, (proximo->p.pos.y)-17,BRANCO);/*risco de seg fault*/
			COLISAO			
		}
	}
	
	blit(buffer, screen, 0, 0, 0, 0, screen->w, screen->h);  
	destroy_bitmap(buffer);

}



/*Substituicao sera uma variavel mneumonica para sabermos se sera criado uma pessoa em alguma borda para substituir
outra pessoa q tenha saido do mapa do mar.*/

fila geraPessoas(fila naufragos, int numPessoas, int l_max, int c_max)
{
	int cont;
	item p;

	for(cont = 0; cont < numPessoas; cont++)
	{
			p.vel.x = (rand()%(int)((VMAX + 1 - VMIN))) + VMIN;
			p.vel.y = (rand()%(int)((VMAX + 1 - VMIN))) + VMIN;
			p.pos.x = rand()%c_max;
			p.pos.y = rand()%l_max;
			p.atualizada = 0;
			p.raio = R_PESS;
			p.categoria = 'p';

			naufragos = entra(naufragos, p);
	}

	return naufragos;
}

fila geraAsimov(fila naufragos, int l_max, int c_max)
{
	item asimov;

	asimov.vel.x = 0;
	asimov.vel.y = 0;
	asimov.pos.x = rand()%c_max;
	asimov.pos.y = rand()%l_max;
	asimov.atualizada = 0;
	asimov.raio = R_ASIMOV;
	asimov.categoria = 'a';

	naufragos = entra(naufragos, asimov);

	return naufragos;

}


fila geraRecifes(fila naufragos, int numRecifes, int l_max, int c_max)
{
	int cont, decideTam, raioMedio;
	item r;

	raioMedio = sqrt(2*D)/2;

	for(cont = 0; cont < numRecifes; cont++)
	{
		r.vel.x = 0;
		r.vel.y = 0;
		r.pos.x = rand()%c_max;
		r.pos.y = rand()%l_max;
		r.atualizada = 0;
		r.categoria = 'r';

		decideTam = rand()%3; 
		if( decideTam == 0 ) 	 /* RECIFES MEDIOS */
			r.raio = raioMedio;
		else if( decideTam == 1) /* RECIFES PEQUENOS */
			while( (r.raio = rand()%raioMedio) < 5 ); /* Garante que o raio não sera 0 ou muito pequeno */
		else			 /* RECIFES GRANDES */
			r.raio = raioMedio + rand()%raioMedio;/* No maximo será do dobro do tamanho do medio */
								

		naufragos = entra(naufragos, r);
	}
	
	return naufragos;
}

fila geraBotes(fila naufragos, int l_max, int c_max)
{
	item b1,b2;

	boteBorda(&b1,l_max,c_max);
	b1.atualizada = 0;
	b1.raio = R_BOTE;
	b1.categoria = '1';

	naufragos = entra(naufragos, b1);

	boteBorda(&b2,l_max,c_max);
	b2.atualizada = 0;
	b2.raio = R_BOTE;
	b2.categoria = '2';

	naufragos = entra(naufragos, b2);

	return naufragos;
}

void boteBorda(item *bote, int l_max, int c_max)
{

	bote->vel.x = (rand()%(int)((VMAX + 1 - VMIN))) + VMIN;
	bote->vel.y = (rand()%(int)((VMAX + 1 - VMIN))) + VMIN;


	switch( rand()%4 )
	{
		case 0: /* Canto superior esquerdo */
			bote->pos.x = bote->raio;
			bote->pos.y = bote->raio;
			break;
		case 1:  /* Canto superior direito */
			bote->pos.x = c_max - bote->raio;
			bote->pos.y = bote->raio;
			break;
		case 2: /* Canto inferior direito */
			bote->pos.x = c_max - bote->raio;
			bote->pos.y = l_max - bote->raio;
			break;
		case 3: /* Canto inferior esquerdo */
			bote->pos.x = bote->raio;
			bote->pos.y = l_max - bote->raio;
			break;
	}
}


void liberaMar(fila naufragos)
{
	fila proximo;
 	
	for(proximo = naufragos; naufragos != NULL; proximo = proximo -> prox)
	{
 		naufragos = proximo -> prox;
		free(proximo);
	}

}

