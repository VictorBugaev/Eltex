#include "stdio.h"
#define N 5
int main()
{
    int i, j, mas[N][N], mas1[N], mas2[N][N];
    int k=1, l=1, right=N-1, left=0, bottom=N-1, top=0, val=0, a=0, b=0;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            mas[i][j] = k;
            k++;
            printf("%d\t", mas[i][j]);
        }
        printf("\n");
    }
    printf("Изначальный массив:\n");
    for (i = 0; i < N; i++)
    {
        mas1[i] = l;
        l++;
        printf("%d ", mas1[i]);
    }
    for (i = 0; i < N/2; i++)
    {
        int t = mas1[i];
        mas1[i]=mas1[N-i-1];
        mas1[N-i-1]=t;
    }
    printf("\nРазвёрнутый массив:\n");
    for (i = 0; i < N; i++)
    {
        printf("%d ", mas1[i]);
    }
    printf("\n\n");
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            if (j > i){
                mas[i][j]=0;
            }
            else mas[i][j]=1;
            printf("%d\t", mas[i][j]);
        }
        printf("\n");        
    }
    printf("\nУлитка:\n\n");
    while(val < N*N){
        for (b=left; b<=right; b++)
            mas2[top][b]=val++;
        top++;
        for(a=top; a<=bottom; a++)
            mas2[a][right]=val++;
        right--;       
        for(b=right; b>=left; b--)
            mas2[bottom][b]=val++;
        bottom--;        
        for(a=bottom; a>=top; a--)
            mas2[a][left]=val++;
        left++;
    }
    for(a=0; a < N; a++){
        for (b=0; b < N; b++){
            printf("%d\t", mas2[a][b]);
        }
        printf("\n");
    }
    
    
}