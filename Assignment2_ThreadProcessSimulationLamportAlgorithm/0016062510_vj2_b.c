#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979

int main (int arg, char *arg_p[]){
        int broj_mn, i;
        double opseg, povrsina;

        broj_mn = 0;
        broj_mn = (arg - 1) / 2;

        if (broj_mn < 1 || broj_mn >5){
                printf("\nNeispravan broj parametara!\nMoguce maksimalno 5 mnogokuta.\n\n");
                return(-1);
        }

        int stranice[broj_mn];
        double duljina[broj_mn];

        for(i = 1; i <= (broj_mn * 2); i = i + 2){
                stranice[i/2] = atoi (arg_p[i]);
                duljina[i/2] = atof (arg_p[i+1]);
        }

        for(i = 0; i < broj_mn; i++){
                sleep(1);

                if(fork() == 0){
                        povrsina = (0.25 * stranice[i] * duljina[i]) * (duljina[i] / (tan(PI / stranice[i])));
                        printf("\nBroj stranica = %i\t\tDuljina stranica = %f\t\tPovrsina = %f\n", stranice[i], duljina[i], povrsina);

                        sleep(1);

                        opseg = stranice[i] * duljina[i];
                        printf("\nBroj stranica = %i\t\tDuljina stranica = %f\t\tOpseg = %f\n", stranice[i], duljina[i], opseg);

                        exit(0);
                }
        }

        for(i = 0; i < broj_mn; i++){
                wait(NULL);
        }

        return 0;
}

