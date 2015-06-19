#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
using namespace std;

const int pisi=0;
const int pun=1;
const int prazan=2;
int SemId, SegId, i, k;

struct zajednicki_p {
        int ulaz,izlaz;
        char medjuspremnik[5];
};

zajednicki_p *p;

void SemGet(int n){
        SemId=semget(IPC_PRIVATE,n,0600);
        if(SemId==-1){
                cout << "Semafor ne postoji..."<<endl;
                exit(1);
        }
}
int SemSetVal(int SemNum, int SemVal){
        return semctl(SemId,SemNum,SETVAL,SemVal);
}

int SemOp (int SemNum, int SemOp){
        struct sembuf SemBuf;
        SemBuf.sem_num = SemNum;
        SemBuf.sem_op = SemOp;
        SemBuf.sem_flg = 0;
        return semop (SemId, &SemBuf,1);
}


int proizvodjac (int red_br){
        char polje[200];
        SemOp(pisi,-1);
        cout << "Upisati znakove za proizvodjaca " << red_br+1 << ": " ;
        cin.getline(polje,200);
        SemOp(pisi,1);
        i=0;
        int gotovo=0;
        while(gotovo==0){
                SemOp(pun,-1);
                SemOp(pisi,-1);
                p->medjuspremnik[p->ulaz] = polje[i];
                p->ulaz = (p->ulaz+1)%5;
                cout << "Proizvodjac" << red_br+1 << " --> " << polje[i]<<endl;
                SemOp(pisi,1);
                SemOp(prazan,1);
                sleep(1);
                if(polje[i]=='\0') gotovo++;
                i++;
        }
}

int potrosac(){
        k=0;
        char polje[200];
        int gotovo=0;
        while (gotovo != 2){
                SemOp(prazan,-1);
                polje[i]=p->medjuspremnik[p->izlaz];
                p->izlaz = (p->izlaz+1)%5;
                cout << "Potrosac <- " << polje[i] << endl;
                SemOp(pun,1);
                if (polje[i]=='\0') gotovo++;
                else i++;
        }
        cout << endl << "Primljeno je... " << polje << endl;
}

void SemRemove(void){
        (void)semctl(SemId, 0, IPC_RMID, 0);
}

void brisi_sve(int sig){
        SemRemove();
        (void) shmdt((char*)p);
        (void) shmctl (SegId,IPC_RMID,NULL);
        exit(0);
}

int main(){
        sigset(SIGINT,brisi_sve);
        int j;
        SemGet(3);
        SegId=shmget(IPC_PRIVATE,sizeof(zajednicki_p),0600);
        if(SegId == -1){
                cout << "Segment ne postoji..." << endl;
                exit(1);
        }

        p=(zajednicki_p*)shmat(SegId,NULL,0);
        p->ulaz = 0;
        p->izlaz = 0;

        SemSetVal(pun,1);
        SemSetVal(pisi,1);
        SemSetVal(prazan,0);

        for(j=0;j<2;j++){
                switch(fork()){
                        case 0: proizvodjac(j);
                                exit(0);
                        default: break;
                }
        }

        switch(fork()){
                case 0: potrosac();
                        exit(0);
                default: break;
        }

        for (i=0; i< 3; i++)
                wait (NULL);

        brisi_sve(0);
        return 0;
}
