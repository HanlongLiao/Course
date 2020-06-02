#include<cstdlib>
#include<cstdio>
using namespace std;

#define N 50000
int a[N];

int main(){
    FILE *fp = fopen("F:\\vscodework\\Compiling\\test50000.txt", "w");
    for(int i = 0; i < N; i++){
        a[i] = rand();
        fprintf(fp, "%d ", a[i]);
    }

    fclose(fp);
    return 0;
}