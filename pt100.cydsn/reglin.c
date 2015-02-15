/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/


/* Start of linear regression code */

// #include <math.h>
#include "reglin.h"
#define MAX 20


/* Déclaratin globale des variables */

	float SommeXi,SommeYi,MoyenneXi,MoyenneYi;
	float SommeProduitXiYi;
	float SommeCarreXiMoinMoyenneXi;
	float SommeCarreYiMoinMoyenneYi;
	float ProduitXiYi[MAX];
	float tab_ecart_a_moyenne[MAX];
	float carre_ecart_a_moyenne_Xi[MAX];
	float carre_ecart_a_moyenne_Yi[MAX];

/* Fonction de calcul de somme d'element d'un tableau */

float sommetab(float tab[],int N)

{
	float somme=0;
	int i=0;
	
	while (i<N)
	{
	 somme=((tab[i])+(somme));i++;
   	};
	
	return(somme);
}

/* Fonction de calcul de moyenne d'element d'un tableau */

float moyennetab(float tab[],int N)

{
	float moyenne,somme;
	
	somme = sommetab(tab,N);

	moyenne = ((somme)/(float)N);

	return (moyenne);
}

/* Fonction de calcul de moyenne d'element d'un tableau (2) */

float moyennetab2(float somme,int N)

{
	float moyenne;

	moyenne=((somme)/(float)N);

	return (moyenne);
}

/* Fonction de calcul du produit d'element de deux tableau ligne par ligne */

void produittab(float tab1[],float tab2[],float tab_produit[],int N)
{
	int i=0;

	while (i<N)
	{
		tab_produit[i]=(tab1[i]*tab2[i]);i++;
	};

}

/* Fonction de calcul des écarts à la moyenne */

void ecart_a_moyenne(float tab[],float tab_ecart_a_moyenne[],float Moyenne,int N)

{
	int i=0;

	while (i<N)
	{
		tab_ecart_a_moyenne[i]=(tab[i]-Moyenne);i++;
	};

}

/* Fonction de calcul de la pente "a" de la droite */

float pente(float Xi[],float Yi[],int N)
{
	float a;
	int i=0;
	

	/*Calcul de la somme des Xi et Yi*/

		SommeXi= sommetab(Xi,N);
   		SommeYi= sommetab(Yi,N);

	/*Calcul des moyennes des Xi et Yi*/
	
		MoyenneXi = moyennetab2(SommeXi,N);
		MoyenneYi = moyennetab2(SommeYi,N);

	/*Somme des Xi*Yi*/

		produittab(Xi,Yi,ProduitXiYi,N);
		SommeProduitXiYi = sommetab(ProduitXiYi,N);
	
	/*calcul des (Xi- MoyenneXi)² */

		ecart_a_moyenne(Xi,tab_ecart_a_moyenne,MoyenneXi,N);
	
	
		while (i<N)
		{
			carre_ecart_a_moyenne_Xi[i] = ((tab_ecart_a_moyenne[i])*(tab_ecart_a_moyenne[i]));i++;
		};
	
	/*Somme des (Xi- MoyenneXi)²*/

		SommeCarreXiMoinMoyenneXi = sommetab(carre_ecart_a_moyenne_Xi,N);

	/*Calcul de la pente*/

		a=((SommeProduitXiYi)-(N * MoyenneXi * MoyenneYi))/(SommeCarreXiMoinMoyenneXi);

	return (a);
}


/* Fonction de calcul de l'ordonnee à l'orrigine "b" */

float ordonnee(float a)

{
	float b;	
	
   	b=(MoyenneYi-(a*MoyenneXi));

	return (b);
}


/* Calcul du coefficient de corrélation "r" */

float corr(float Yi[],float a,int N)

{
	int i=1;	
	float r,q,rCarre;


	/*calcul des (Yi- MoyenneYi)² */

		ecart_a_moyenne(Yi,tab_ecart_a_moyenne,MoyenneYi,N);
	
		i=0;

		while (i<N)
		{
			carre_ecart_a_moyenne_Yi[i] = ((tab_ecart_a_moyenne[i])*(tab_ecart_a_moyenne[i]));i++;
		};
	
	/*Somme des (Yi- MoyenneYi)² */

		SommeCarreYiMoinMoyenneYi = sommetab(carre_ecart_a_moyenne_Yi,N);

	/*Calcul de r*/

		rCarre=(((a*a)*(SommeCarreXiMoinMoyenneXi))/SommeCarreYiMoinMoyenneYi);

	/*Le signe de r est le meme que a*/
	
		if (a>=0.0)
		{q = 1;
		}
		else
		{q = -1;
		};

		r=(sqrt_custom(rCarre))*q;

	return (r);
}

int sqrt_custom(int a_nInput)
{
    int op  = a_nInput;
    int res = 0;
    int one = 1uL << 30; // The second-to-top bit is set: use 1u << 14 for uint16_t type; use 1uL<<30 for ufloat_t type


    // "one" starts at the highest power of four <= than the argument.
    while (one > op)
    {
        one >>= 2;
    }

    while (one != 0)
    {
        if (op >= res + one)
        {
            op = op - (res + one);
            res = res +  2 * one;
        }
        res >>= 1;
        one >>= 2;
    }
    return res;
}


/* Fonction de calcul de la régression linéaire */

void reglin(float Xi[],float Yi[],int N,float result[])

{
	float a,b,r;

   
	/*Appel de la fonction de calcul de la pente "a" de la droite*/
	
		a=pente(Xi,Yi,N);
	
	/*Appel de la fonction de calcul de l'ordonee a l'orrigine "b"*/

		b=ordonnee(a);


	/*Appel de la fonction de calcul du coefficient de corrélation "r"*/

		r=corr(Yi,a,N);
	
	/*Stockage des résultats dans une tableau de varialbes*/


		result[0]=a;
		result[1]=b;
		result[2]=r;
}

/* End of linear regression */


/* [] END OF FILE */
