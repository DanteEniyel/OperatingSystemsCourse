#include <iostream> 
#include <pthread.h> 
#include <fstream> 
#include <stdlib.h> 
#include <signal.h> 
#include <semaphore.h> 
#include <string> 
#include <sys/wait.h> 


using namespace std;

sem_t K_suma; 
sem_t PR; 
sem_t C; 


int br_K_sume; 
int brojac;
int kraj; 
string red;

ifstream Datoteka; 

pthread_t DR[3]; 

void* citanje(void* X){
    char polje[1000]; 
    while(!Datoteka.eof()){ 
        Datoteka.getline(polje, 1000); 
        red = string(polje); 
        sem_post(&PR); 
        sem_post(&K_suma);
        sem_wait(&C); 
        sem_wait(&C);
    }
    kraj = 1;
    sem_post(&PR);
    sem_post(&K_suma);
}

void* dat_html(void* X){
    int T = 0; 
    while(kraj == 0){
        sem_wait(&PR);
        for (int i = 0; i < red.length(); i++){
            char Z; 
            Z = red[i];
            if (T == 0 && Z == '<') T = 1;
            else if (T == 1 && Z == '(') T = 0;
            else if (T == 1 && Z == '>'){
                brojac++;
                T=0;
            }
        }
    sem_post(&C);
    }
}

void* dat_txt(void* X){
    while(kraj == 0){
        sem_wait(&PR);
        int M = 0; 
        string prekid = ".?! ,:;"; 
        for (int i = 0; i < red.length(); i++){
            char Z; 
            Z = red[i];
            if (M == 0 && isalpha(Z)){
                brojac++;
                M = 1;
            }
            else if (M == 1 && prekid.find(Z, 0) != string::npos) M = 0;
    }
    sem_post(&C);
    }
}

void* kontrola(void* X){
    while(kraj == 0){
        sem_wait(&K_suma);
        for (int i = 0; i < red.length(); i++){
            br_K_sume ^= red[i]; 
        }
    sem_post(&C);
    }
}

void brisanje(int sig){
    Datoteka.close();
    Datoteka.clear();
    sem_destroy(&C); 
    sem_destroy(&PR);
    sem_destroy(&K_suma);
}


int glavna (string datoteka, int opcija){
    Datoteka.open(datoteka.c_str(), ifstream::in); 
    sem_init(&PR, 0, 0); 
    sem_init(&C, 0, 0);
    sem_init(&K_suma, 0, 0);
    kraj = 0;
    pthread_create(&DR[0], NULL, citanje, NULL); 
    pthread_create(&DR[1], NULL, kontrola, NULL);
    if (opcija == 1) pthread_create(&DR[2], NULL, dat_txt, NULL);
    else if (opcija == 2) pthread_create(&DR[2], NULL, dat_html, NULL);

    for (int i = 0; i < 3; i++) pthread_join(DR[i], NULL); 

    if (opcija == 1)
        cout << "Datoteka " << datoteka << " ima " << brojac  << " rijeci; " << "zastitna suma je: " << br_K_sume << endl;
    else if (opcija == 2)
        cout << "Datoteka " << datoteka << " ima " << brojac  << " tagova; " << "zastitna suma je: " << br_K_sume << endl;

    brisanje(0);
}

int main (int argc, char* argv[]){
    int i;
    sigset(SIGINT, brisanje);
    for (i = 1; i< argc; i++){ 
        int opcija = 0; 
        string dat, ekst; 
        dat = argv[i];
        ekst = dat.substr(dat.length() - 4); 
        if (ekst.compare(".txt")==0) opcija = 1;
        else{
            ekst = dat.substr(dat.length() - 5);
            if (ekst.compare(".html") == 0) opcija = 2;
        }
        if (opcija == 0){
            cout << "Greska... Upisite .txt ili .html datoteka..."<<endl;
        }
        else
            switch(fork()){
                case 0:
                    glavna(dat, opcija); 
                    exit(0);
                case -1:
                    exit(1);
            }
    }

    for (i=0; i<argc; i++){
        wait(NULL); 
    }

    return 0;
}
