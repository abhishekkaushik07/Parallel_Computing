#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
  int n;
  printf("enter number of rows:");
  scanf("%d",&n);
  int a[n][n];
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
            }
            fprintf(fptr, "%d ", a[i][j]);
        }
        fprintf(fptr, "\n");
    }
}