#include "bib/tipos.h"
#include "bib/colisao.h"
#include "bib/desloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

double distancia(vetor pos1, vetor pos2)
{

	return sqrt((pos1.x - pos2.x)*(pos1.x - pos2.x) + (pos1.y - pos2.y)*(pos1.y - pos2.y));

}

fila detectaColisao(fila pessoas, double deltaT)
{
	fila principal, aux;
	double max_dist;

	if(quantidade(pessoas) > 1)
	{  
		principal = aux = pessoas;

		while(principal != NULL)
		{
			while(aux != NULL)
			{
			  if(aux != principal)
				{
					max_dist = principal->p.raio + aux->p.raio;

					if(max_dist >= distancia(principal->p.pos, aux->p.pos))
          {
                if(principal->p.categoria == 'p' && aux->p.categoria == 'p')
					        colide(&principal->p, &aux->p, deltaT);
                else if((principal->p.categoria == '1' || principal->p.categoria == '2') && aux->p.categoria == 'p'){
                  pessoas = aux = principal = recolhePessoa(pessoas, aux);
                }else if(principal->p.categoria == '1' && aux->p.categoria == '2'){
                  colide(&principal->p, &aux->p, deltaT);
                  principal->p.atualizada = aux ->p.atualizada = 0;
                } 
          }
        }

		    if(aux != NULL) aux = aux->prox;
      }
      		
			aux = pessoas;
      if(principal != NULL) principal = principal->prox;
    }
	}
  return pessoas;
}

fila recolhePessoa(fila pessoas, fila p)
{
  fila aux, ant;
  
  ant = NULL;
  aux = pessoas;

  while(aux != NULL)
  {
    if(aux == p)
    {
      if(ant == NULL)
      {
        ant = aux->prox;
        free(aux);
        return(ant);
      }
      else
      {
        ant->prox = aux->prox;
        free(aux);
        return(pessoas);
      }      
    }
    ant = aux;
    aux = aux->prox;
  }
  return(NULL);
}

void movePessoa(item *p, double deltaT)
{
	int dir;
	double vel;

	vel = modulo(p->vel);
  	dir = direcao(p->vel.x, p->vel.y);

  	switch(dir)
	{
    	case L:
      		p->pos.x += (int) vel*deltaT;
      		break;
    	case NE:
      		p->pos.x += (int) vel*cos(M_PI/4)*deltaT;
      		p->pos.y += (int) vel*sin(M_PI/4)*deltaT;
      		break;
    	case N:
      		p->pos.y += (int) vel*deltaT;
      		break;
    	case NO:
      		p->pos.x += (int) vel*cos(M_PI/4)*(-1)*deltaT;
      		p->pos.y += (int) vel*sin(M_PI/4)*deltaT;
      		break;
    	case O:
      		p->pos.x += (int) vel*(-1)*deltaT;
      		break;
    	case SO:
      		p->pos.x += (int) vel*cos(M_PI/4)*(-1)*deltaT;
      		p->pos.y += (int) vel*sin(M_PI/4)*(-1)*deltaT;
     		break;
    	case S:
      		p->pos.y += (int) vel*(-1)*deltaT;
      		break;
    	case SE:
      		p->pos.x += (int) vel*cos(M_PI/4)*deltaT;
      		p->pos.y += (int) vel*sin(M_PI/4)*(-1)*deltaT;
      		break;
  	}
}

void colideCoral(item coral, item *p, double deltaT){
  int dir;
  double dif;
  item aux;

  aux = *p;
  dif = (angulo(p->vel.x, p->vel.y) - angulo(coral.pos.x - p->pos.x, coral.pos.y - p->pos.y));
  
  dir = direcao(p->vel.x, p->vel.y);

  switch(dir){
    /*casos bons!*/
    case N:
      p->vel.y *= -1;
      break;
    case S:
      p->vel.y *= -1;
      break;
    case L:
      p->vel.x *= -1;
      break;
    case O:
      p->vel.x *= -1;
      break;
    /*casos ruins!*/
    case NE:
      if(dif == 0){
        p->vel.y *= -1;
        p->vel.x *= -1;
      }else if(dif < 0){
        p->vel.y *= -1;
      }else if(dif > 0){
        p->vel.x *= -1;
      }
      break;
    case NO:
       if(dif == 0){
        p->vel.y *= -1;
        p->vel.x *= -1;
      }else if(dif < 0){
        p->vel.y *= -1;
      }else if(dif > 0){
        p->vel.x *= -1;
      }
      break;
    case SE:
      if(dif == 0){
        p->vel.y *= -1;
        p->vel.x *= -1;
      }else if(dif < 0){
        p->vel.x *= -1;
      }else if(dif > 0){
        p->vel.y *= -1;
      }
      break;
    case SO:
      if(dif == 0){
        p->vel.y *= -1;
        p->vel.x *= -1;
      }else if(dif < 0){
        p->vel.x *= -1;
      }else if(dif > 0){
        p->vel.y *= -1;
      }
      break;
  }
  movePessoa(p, deltaT);
  p->atualizada = 1;
}

void colide(item *p1, item *p2, double deltaT)
{
/*Colisao perfeitamente elastica entre objetos de mesma massa, suas direcoes e velocidades se invertem.*/
    
	vetor aux_vel;

	aux_vel = p1->vel; 
	p1->vel = p2->vel;
	p2->vel = aux_vel;

	movePessoa(p1,deltaT);
	movePessoa(p2,deltaT);
	
	  p1->atualizada = 1;
	  p2->atualizada = 1;
  
}

void colideComBorda( item *p, int borda, int l_max, int c_max)
{
 int novaBorda;

 /* 0  -- borda superior
    1  -- borda inferior
    2  -- borda esquerda
    3  -- borda direita. */

	if( p->categoria == 'p')
	{
		/* Sorteia uma borda ate que ela seja diferente da atual */
		while( (novaBorda = rand()%4) == borda);

		switch(novaBorda)
		{
			case 0:
				p->pos.x = p->raio + 20;
				p->pos.y = rand()%l_max;
				break;
			case 1:
				p->pos.x = c_max - p->raio - 20;
				p->pos.y = rand()%l_max;
				break;
			case 2:
				p->pos.x = rand()%c_max;
				p->pos.y = p->raio + 20;
				break;
			case 3:
				p->pos.x = rand()%c_max;
				p->pos.y = l_max - p->raio - 20;
				break;
		}
	}

	else if( p->categoria == '1' || p->categoria == '2' )
	{
		/*if( borda == 0 || borda == 1 )
			p->pos.x = c_max - p->pos.x;

		else 
			p->pos.y = l_max - p->pos.y;
		*/
	}
}
