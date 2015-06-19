#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define BROJ 2
int zastavica[BROJ]={0}, pravo;

void kriticni_odsjecak(int i, int j){
	zastavica[i]= 1;
	while(zastavica[j] != 0){
		if(pravo == j){
			zastavica[i] = 0;
			while(pravo == j){;}
			zastavica[i] = 1;
		}
	}
}

void izlazak_ko(int i, int j){
	pravo = j;
	zastavica[i] = 0;
}

void *dretva(void *vrijednost_d){
	int k, l;
	//prva
	if(*((int *)vrijednost_d)==0){
		for(k=1; k<=5; k++){
			kriticni_odsjecak(0,1);
			for(l=1; l<=5; l++){
				printf("\nDretva:%d, K.O. br: %d(%d/5)\n", *((int *)vrijednost_d)+1, k, l);
				sleep(1);
			}
			izlazak_ko(0,1);
		}
	}
	//druga
	else{
            for(k=1; k<=5; k++){
            	kriticni_odsjecak(1,0);
                	for(l=1; l<=5; l++){
                        	printf("\nDretva:%d, K.O. br: %d(%d/5)\n", *((int *)vrijednost_d)+1, k, l);
                                sleep(1);
                        }
                        izlazak_ko(1,0);
                }
        }

}


int main(void){
	pthread_t id_dretve[BROJ];
	int vrijednost_d[BROJ], i;

	for(i=0; i<BROJ; i++){
		vrijednost_d[i]=i; //odredjivanje redosljeda dretvi
	}

	//stvaranje dretvi
	for(i=0; i<BROJ; i++){
		if(pthread_create(&id_dretve[i], NULL, &dretva, &vrijednost_d[i])!=0){
			printf("\nStvaranje dretve --%d-- neuspjesno!\n",i);
			exit(1);
		}
	}

	//cekanje dretvi da dovrse
	for(i=0; i<BROJ; i++){
		pthread_join(id_dretve[i], NULL);
	}
	return 0;
}
