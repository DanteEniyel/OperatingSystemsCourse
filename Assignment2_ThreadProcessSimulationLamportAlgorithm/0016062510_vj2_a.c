#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define BROJEVI 30

int broj_dretvi;
int polje[20][BROJEVI];

void *dretva(void *parametar){
	int i;
	int p = *((int*)parametar);

	for(i = 0; i < BROJEVI; i++){
		polje[p][i] = rand() % 500;
	}
}


void *dretva_2(void *parametar){
	int p = *((int*)parametar);
	int i, zbroj = 0;
	float a_sredina = 0;

	for(i = 0; i < BROJEVI; i++){
		zbroj += polje[p][i];
	}
	printf("Polje %d suma = %d\n", p+1, zbroj);

	a_sredina = zbroj;
	a_sredina = a_sredina / BROJEVI;
	printf("Polje %d aritmeticka sredina = %f\n\n", p+1, a_sredina);
}


int main (int broj_p, char **vrijednost_p){
	int uspjesno, i, j;
	srand (time (0));
        rand();


	if(broj_p != 2){
		printf("\nNeispravan broj parametara\n\n");
		return -1;
	}

	broj_dretvi = atoi (vrijednost_p[1]);
	pthread_t ID_dretve_1[broj_dretvi], ID_dretve_2[broj_dretvi];

	int vrijednost_d [broj_dretvi];

	for(i = 0; i < broj_dretvi; i++){
		vrijednost_d[i] = i;
		uspjesno = pthread_create (&ID_dretve_1[i], NULL, &dretva, &vrijednost_d[i]);

		if(uspjesno < 0){
			printf("Greska u stvaranju dretve\n");
			exit (-2);
		}
	}

//	for(i = 0; i < broj_dretvi; i++){
//		pthread_join(ID_dretve_1[i], NULL); //da priceka dretvu koja možda nije zavrsila
//	}


	for(i = 0; i < broj_dretvi; i++){
        	pthread_join(ID_dretve_1[i], NULL);
	        vrijednost_d[i] = i;
                uspjesno = pthread_create (&ID_dretve_2[i], NULL, &dretva_2, &vrijednost_d[i]);

                if(uspjesno < 0){
                        printf("Greska u stvaranju dretve\n");
                        exit (-2);
                }
		sleep(1);
        }

	for(i = 0; i < broj_dretvi; i++){
                pthread_join(ID_dretve_2[i], NULL); //da priceka dretvu koja možda nije zavrsila
        }

	return 0;
}
