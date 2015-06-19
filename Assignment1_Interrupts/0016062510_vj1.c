#include <stdio.h>
#include <signal.h>
#include <time.h>

#define BROJ_RAZINA 10

int cekanje[BROJ_RAZINA];
int prioritet[BROJ_RAZINA];
int trenutni_pr = 0;

void obrada(int broj){
	int i;

	printf("\n-------\nPocela obrada prekida: %i\n\n", broj);
	for(i = 1; i <= 5; i++){
		printf("Obrada prekida %i: %i/5\n", broj, i);
		sleep(1);
	}
	printf("\nZavrsena obrada prekida %i\n-------\n", broj);
}

void pr_rutina(int signal){

	time_t vrijeme;
	int razina, x, j;
	time(&vrijeme);

	printf("\nPrekidna rutina pozvana u %s\n",ctime (&vrijeme));

	printf("Unesite razinu prekida: ");
	scanf("%i", &razina);

	if(razina < 1 | razina > BROJ_RAZINA){
		printf("\nNeispravan prekid %i ignoriran\n", razina);
		return;
	}

	cekanje[razina-1]++;

	do{
		x = 0;
		for(j = trenutni_pr + 1; j <= BROJ_RAZINA; j++){
			if(cekanje[j] != 0) x = j;
		}
		if(x != 0){
			cekanje[x]--;
			prioritet[x] = trenutni_pr;
			trenutni_pr = x;
			sigrelse(SIGINT);
			obrada(trenutni_pr);
			sighold(SIGINT);
			trenutni_pr = prioritet[x];
		}
	}while(x > 0);
}

int main(){

	int i;
	sigset(SIGINT, pr_rutina);

	printf("--------\nPocelo izvodjenje glavnog programa\n\n");
	for(i = 1; i <= 10; i++){
		printf("Izvodjenje glavnog programa %i/10\n", i);
		sleep(1);
	}
	printf("\nZavrseno izvodjenje glavnog programa!\n-------\n");
	return 0;
}
