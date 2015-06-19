#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

int n, *trazim, *br, id[2] = {0}; //trazim je polje zastavica za trazenje red. br

int zadnji(){ //odreðuje koji je bio zadnji broj
	int i, j;
	j = 0;
	for(i=0; i<n; i++){
		if(*(br+1)>j)
			j=*(br+1);
	}
	return j;
}

void kriticni_odsjecak(int i){
	int j;
	*(trazim+i) = 1; //zastavica trazim se dize na 1
	*(br+i) = zadnji()+1; //trazi zadnji najveci koji je bio i doda mu 1 i taj broj se doda procesu
	*(trazim+i) = 0; //azstavica trazim se spusta na 0

	for(j=0; j<n; j++){ //prolazi se kroz polje procesa i provjera da li koji proces trazi broj
		while(*(trazim+j)!= 0); //ako nitko ne trazi broj ide se u drugu while petlju
		while(*(br+j)!=0 && (*(br+j)<*(br+i) || (*(br+j)==*(br+i) && j<i)));
	} //drugi while: gleda se da li postoji proc razl od 0, a manji od mog trenutnog br ili
}     //ako su dva procesa s istim brojem, prvi ide onaj koji je manjeg rednog broja

void izlazak_ko(int i){
	*(br+i) = 0;
}

void proces(int i){
	int k, l;
	for(k=1; k<=5; k++){
		kriticni_odsjecak(i);
		for(l=1; l<=5; l++)
			printf("\nProces: %d K.O. br: %d(%d/5)\n", i+1,k, l);
		izlazak_ko(i);
		sleep(1);
	}
}

void brisi(int i){ //brisanje /oslobadjanje zajednicke memorije
	shmdt((char *) br);
	shmctl(id[0], IPC_RMID, NULL);
  	shmdt((char *) trazim);
  	shmctl(id[1], IPC_RMID, NULL);
  	exit(0);
}

int main(int argc, char *argv[]){
	int i;
	n = atoi(argv[1]);

	//zauzima memoriju
	for(i=0; i<2; i++){
		id[i] = shmget(IPC_PRIVATE, sizeof(int)*50*n, 0600); //zauzimanje mem segmenta
		if(id[i] == -1)
			exit(1);
	}

	br = (int*) shmat(id[0], NULL, 0); //vezanje segmenta za adr prostor
	trazim = (int*) shmat(id[1], NULL, 0);

	for(i=0; i<n; i++){
		*(trazim+i) = *(br+i) = 0; //dodjeljuje se redni broj procesu
	}

	sigset(SIGINT, brisi);

	//pokretanje proc
	for(i=0; i<n; i++){
		switch(fork()){
			case 0:
				proces(i);
				exit(0);
			case -1:
				printf("\n\nProces se ne moze stvoriti!");
				exit(1);
		}
	}

	for(i=0; i<n; i++)
		wait(NULL);

	brisi(0);

	return 0;
}
