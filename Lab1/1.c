#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#define n 16//number of vertices
int a[n][n];
int b[n][n];
void fillmatrix() //used to fill the matrix with random values and write the matix in a file
{
    FILE *fptr = fopen("input", "w");
    int i, j;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            if (i == j)
            {
                a[i][j] = 0;
            }
            else if (i > j)
            {
                a[i][j] = a[j][i];
            }
            else
            {
                a[i][j] = rand() % 2;
                //a[i][j]=0;
            }
            b[i][j] = a[i][j];
            fprintf(fptr, "%d ", a[i][j]);
        }
        fprintf(fptr, "\n");
    }
}
int main(int argc, char **argv)
{
    int s = 1, d = 1, count = 1;//s = source vertex, d = destination vertex
    int rank;
    int np;
    int c[n][n];
    int i, j, k, sum;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    fillmatrix();
    if(rank==0)
    {
        if(s==d)
        {
            printf("path length:0\n");
            printf("time taken:0.0000s\n");
            exit(0);
        }
    }
    double t1 = clock();
    int n1 = (n * n) / np;
    int start = (n * rank) / np;
    MPI_Barrier(MPI_COMM_WORLD);
    while (a[s - 1][d - 1] == 0 && count <= n)
    {
        if (rank == 0)
        {
            MPI_Scatter(&a[0][0], n1, MPI_INT, MPI_IN_PLACE, n1, MPI_INT, 0, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Scatter(&a[0][0], n1, MPI_INT, &a[start][0], n1, MPI_INT, 0, MPI_COMM_WORLD);
        }
        MPI_Bcast(b, n * n, MPI_INT, 0, MPI_COMM_WORLD);
        for (i = start; i < (start + n / np); i++)
        {
            for (j = 0; j < n; j++)
            {
                sum = 0;
                for (k = 0; k < n; k++)
                {
                    sum += (a[i][k] * b[k][j]);
                }
                c[i][j] = sum;
            }
        }
        if (rank == 0)
        {
            MPI_Gather(MPI_IN_PLACE, n1, MPI_INT, &c[0][0], n1, MPI_INT, 0, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Gather(&c[start][0], n1, MPI_INT, &c[0][0], n1, MPI_INT, 0, MPI_COMM_WORLD);
        }
        for (i = 0; i < n; i++)
            for (j = 0; j < n; j++)
                a[i][j] = c[i][j];
        MPI_Bcast(a, n * n, MPI_INT, 0, MPI_COMM_WORLD);
        count++;
    }
    t1 = clock() - t1;
    if (rank == 0)
    {
        for (i = 0; i < n; i++)
        {
            for (j = 0; j < n; j++)
                printf("%d ", b[i][j]);
            printf("\n");
        }
        if (count >= n)
        {
            printf("path length:infinity\n");
        }
        else
        {
            printf("path length:%d\n", count);
        }
        printf("time taken:%f\n", ((double)t1) / CLOCKS_PER_SEC);
    }
    MPI_Finalize();
}