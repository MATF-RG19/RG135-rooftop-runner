#include<stdio.h>

int niz[9]= {6,9,15,18,24,27,33,36};

int pronadjiJednakiIliManji(int l, int d, int z){
    int i = 0;
    for(i=0; i<d; i++){
        if(niz[i] > z){
            return i-1;
        }
    }
}
int main(){
    
    
    int p = pronadjiJednakiIliManji(0,8,(int)6.5);
    printf("%d \n",niz[p]);
}


