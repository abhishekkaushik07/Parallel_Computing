#include <random>
#include <iostream>
#include <chrono>
#include "omp.h"

using namespace std::chrono;
int main(int argc, char **argv)
{
    int i, j,temp,count=0,num_threads;
    int n = atoi(argv[1]);
    int n1=n+2;
    bool flag=true;
    float t = atof(argv[2]);
    float* a=(float*) malloc(n1*n1*sizeof(float));
    //float a[n1*n1];
    float d,diff,tmp;
    //float next[n1*n1];
    float* next=(float*) malloc(n1*n1*sizeof(float));
    float lbound = 0;
    float ubound = 100;
    std::uniform_real_distribution<float> urd(lbound, ubound);
    std::default_random_engine re;
    //std::cout << "Initial Matrix:\n";
    for(i=1;i<n1;i++)
    {
        for(j=0;j<n1;j++)
        {
            a[i*n1+j]=0;
            next[i*n1+j]=0;
        }
    }
    for (i = 1; i <= n; i++)
    {
        for (j = 1; j <= n; j++)
        {
            //a[i*n1+j]=0;
            a[i*n1+j] = urd(re);
            //printf("%f ", a[i*n1+j]);
        }
        //printf("\n");
    }

    if(n<20)
        num_threads=1;
    else if(n<=500)
        num_threads=3;
    else if(n<=1000)
        num_threads=5;
    else
        num_threads=10;
    auto start = high_resolution_clock::now();
    omp_set_num_threads(num_threads);
    do
    {
        flag=false;
        # pragma omp parallel
        {
        #pragma omp for collapse(2) private(temp,tmp,d,i,j)
        for(i=1;i<=n;i++)
        {
            for(j=1;j<=n;j++)
            {
                temp=i*n1;
                d=5.0;
                tmp=a[temp-n1+j]+a[temp+n1+j]+a[temp+j-1]+a[temp+j+1]+a[temp+j];
                if(i==1||i==n)
                    d--;
                if(j==1||j==n)
                    d--;
                tmp/=d;
                if(fabs(tmp-a[temp+j])>=t)
                {
                    //printf("%f %d %d \n",tmp-a[temp+j],i,j);
                    flag=true;
                }
                next[temp+j]=tmp;
            }
        }
        #pragma omp for collapse(2) private(i,j)
        for(i=1;i<=n;i++)
        {
            for(j=1;j<=n;j++)
            {
                a[i*n1+j]=next[i*n1+j];
            }    
        }
        }
    count++;
    //printf("%d\n",count);
    }while(flag);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    std::cout <<"Time:"<< duration.count() << std::endl;
    for (i = 0; i < n1; i++)
    {
        for (j = 0; j < n1; j++)
        {
            //printf("%f ", a[i*n1+j]);
        }
        //printf("\n");
    }
    printf("iterations:%d\nThreads:%d\n",count,num_threads);
}
