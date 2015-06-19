#include <iostream> 
#include <pthread.h> 
#include <stdlib.h> 
#include <semaphore.h> 

using namespace std;

int ST; 

class BSEM{
    private:
        sem_t sem;
        int i;
    public:
        int BR_sem;

        void cekaj_BSEM(){
            sem_wait(&sem);
            i=0;
        }
        void postavi_BSEM(){
            if(i==0){ 
                i=1;
                sem_post(&sem);
            }
        }
        BSEM(int BR_sem){
            sem_init(&sem, 0, BR_sem);
        }
        ~BSEM(){
            sem_destroy(&sem);
        }
};

BSEM P1(0), P2(0), P3(0), PR(0); 

void *trgovac (void *X){
    while(1){
        ST = rand()%3+1;
        if (ST == 1) cout << "Trgovac stavlja na stol papir i duhan"<<endl;
        if (ST == 2) cout << "Trgovac stavlja na stol sibice i duhan"<<endl;
        if (ST == 3) cout << "Trgovac stavlja na stol papir i sibice"<<endl;

        P1.postavi_BSEM(); 
        P2.postavi_BSEM();
        P3.postavi_BSEM();
        PR.cekaj_BSEM(); 
    }
}

void *pusaci(void *pu){
    int P = *((int*) pu);
    while(1){
        switch(P){
        case 1:
            P1.cekaj_BSEM();
            if(P==ST){
                cout << "Pusac " << P << ": uzima sastojke i ..."<<endl<<endl;
                sleep(1);
                PR.postavi_BSEM();
            }
            break;
        case 2:
            P2.cekaj_BSEM();
            if(P==ST){
                cout << "Pusac " << P << ": uzima sastojke i ..."<<endl<<endl;
                sleep(1);
                PR.postavi_BSEM();
            }
            break;
        case 3:
            P3.cekaj_BSEM();
            if(P==ST){
                cout << "Pusac " << P << ": uzima sastojke i ..."<<endl<<endl;
                sleep(1);
                PR.postavi_BSEM();
            }
            break;
        }
    }
}

int main (){
    int i;
    pthread_t ID_dretve[3];
    int pu[3] = {1, 2, 3};
    cout << "Pusac 1 posjeduje sibice"<<endl<<"Pusac 2 posjeduje papir"<<endl<<"Pusac 3 posjeduje duhan"<<endl<<endl;
    pthread_create(&ID_dretve[0], NULL, &trgovac, NULL); 
    pthread_create(&ID_dretve[1], NULL, &pusaci, (void *) &pu[0]);
    pthread_create(&ID_dretve[2], NULL, &pusaci, (void *) &pu[1]);
    pthread_create(&ID_dretve[3], NULL, &pusaci, (void *) &pu[2]);
 
    for (i=0; i<=3; i++) 
        pthread_join(ID_dretve[i], NULL);

    return 0;
}
