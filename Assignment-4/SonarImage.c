#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int *getAddress(int *arr , int n , int i , int j){
    return (arr + i * n + j);
}

void printArray(int *arr, int n){
    for(int i = 0 ; i < n ; i++){
        for(int j = 0 ; j < n ; j++){
            printf("%d " , *getAddress(arr , n , i , j));
        }
        printf("\n");
    }
}

void getrandomMatrix(int *arr , int n){
    srand(time(NULL));
    for(int i = 0 ; i < n ; i++){
        for(int j = 0 ; j < n ; j++){
            *getAddress(arr , n , i , j) = rand() % 256;
        }
    }
}

void transposeMatrix(int *arr , int n){
    for(int i = 0 ; i < n ; i++){
        for(int j = i ; j < n ; j++){
            int *a = getAddress(arr , n , i , j);
            int *b = getAddress(arr , n , j , i);
            int temp = *a;
            *a = *b;
            *b = temp;
        }
    }
}

void reverseRows(int *arr , int n){
    for(int i = 0 ; i < n ; i++){
        int j = 0;
        int k = n-1;
        while(j < k){
            int *a = getAddress(arr , n , i , j);
            int *b = getAddress(arr , n , i , k);
            int temp = *a;
            *a = *b;
            *b = temp;
            j++;
            k--;
        }
    }
}

void rotate90Degrees(int *arr , int n){
    transposeMatrix((int*)arr , n);
    reverseRows((int*)arr , n);
}

void applyingSmoothingFilter(int *arr, int n){
    int prevRow[10];   
    int currRow[10];   
    int nextRow[10];   
    int outRow[10];    
    
    for(int j = 0; j < n; j++){
        currRow[j] = *(arr + 0*n + j);
    }
    
    if(n > 1){
        for(int j = 0; j < n; j++){
            nextRow[j] = *(arr + 1*n + j);
        }
    }
    
    for(int i = 0; i < n; i++){     
        for(int j = 0; j < n; j++){
            int sum = 0;
            int count = 0;
            for(int ki = -1; ki <= 1; ki++){
                int li = i + ki;
                if(li < 0 || li >= n) continue;

                for(int kj = -1; kj <= 1; kj++){
                    int lj = j + kj;
                    if(lj < 0 || lj >= n) continue;
                    int val;
                    if(li == i - 1){
                        val = prevRow[lj];
                    } else if(li == i){
                        val = currRow[lj];
                    } else { 
                        val = nextRow[lj];
                    }
                    sum += val;
                    count++;
                }
            } 
            outRow[j] = sum / count; 
        } 
        for(int j = 0; j < n; j++){
            *(arr + i*n + j) = outRow[j];
        }
        
        for(int j = 0; j < n; j++){
            prevRow[j] = currRow[j];
        }

        if(i + 2 < n){            
            for(int j = 0; j < n; j++){
                currRow[j] = nextRow[j];                    
                nextRow[j] = *(arr + (i+2)*n + j);          
            }
        } else {
            for(int j = 0; j < n; j++){
                currRow[j] = nextRow[j];
            }
        }
    } 
}

int main(){
    int n;
    printf("Enter size N for N x N matrix: ");
    scanf("%d" , &n);
    int arr[n][n];
    getrandomMatrix((int*)arr , n);
    printf("\nGenerated array= \n");
    printArray((int*)arr , n);
    rotate90Degrees((int*)arr , n);
    printf("\nArray after rotating 90 degrees= \n");
    printArray((int*)arr , n);
    applyingSmoothingFilter((int*)arr , n);
    printf("\nArray after smoothing effect= \n");
    printArray((int*)arr , n);
}




