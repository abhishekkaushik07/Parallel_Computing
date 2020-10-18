#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int n = 16, s = 1, d = 4;//n = number of vertices, s = source vertex, d = destination vertex
    s--;
    d--;
    MPI_Comm mult_comm;
    MPI_Status status;
    int rank, np;
    int shift;
    int i, j, k, flag = 0, count = 1;
    int dims[2];
    int periods[2];
    int left, right, up, down;
    int *A, *B, *C;
    double start, end;
    int n1;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    // checking whether number of process in a perfect square or not
    dims[0] = 0;
    dims[1] = 0;
    periods[0] = 1;
    periods[1] = 1;
    MPI_Dims_create(np, 2, dims);
    if (dims[0] != dims[1])
    {
        if (rank == 0)
            printf("The number of processors must be a square.\n");
        MPI_Finalize();
        return 0;
    }
    n1 = n / dims[0];//number of rows in each process
    int proc_needed = (s / n1) * dims[0] + (d / n1);// finding the process which contains element matrix[s][d]
    int s_new = s % n1;// row index in that matrix
    int d_new = d % n1;// column index in that matrix
    A = (int *)malloc(n1 * n1 * sizeof(int));
    B = (int *)malloc(n1 * n1 * sizeof(int));
    C = (int *)calloc(n1 * n1, sizeof(int));
    //checking if source and destination are same
    if (rank == 0)
    {
        if (s == d)
        {
            printf("Time: 0.0000s\nPathLength:0\n");
            exit(0);
        }
    }
    //getting the matrix graph input from the file input
    FILE *fptr;
    int flg1 = n1 * n1;
    int x, y, z;
    fptr = fopen("input", "r");
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            fscanf(fptr, "%d", &x);
            if (rank == ((i / n1) * dims[0] + (j / n1)))
            {
                y = i % n1;
                z = j % n1;
                A[y * n1 + z] = x;
                B[y * n1 + z] = x;
                C[y * n1 + z] = 0;
                flg1--;
            }
            if (flg1 <= 0)
                break;
        }
        if (flg1 <= 0)
            break;
    }
    int coord[2];
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &mult_comm);
    start = MPI_Wtime();
    //checking if there is an edge between the source and destination
    if (rank == proc_needed)
    {
        if (A[s_new * n1 + d_new] == 1)
        {
            flag = 1;
        }
    }
    MPI_Bcast(&flag, 1, MPI_INT, proc_needed, mult_comm);
    while (flag == 0)
    {
        // Initial Shift
        MPI_Cart_coords(mult_comm, rank, 2, coord);
        MPI_Cart_shift(mult_comm, 1, coord[0], &left, &right);
        MPI_Sendrecv_replace(A, n1 * n1, MPI_INT, left, 1, right, 1, mult_comm, &status);
        MPI_Cart_shift(mult_comm, 0, coord[1], &up, &down);
        MPI_Sendrecv_replace(B, n1 * n1, MPI_INT, up, 1, down, 1, mult_comm, &status);
        count++;
        for (shift = 0; shift < dims[0]; shift++)
        {
            // Matrix multiplication
            for (i = 0; i < n1; i++)
                for (j = 0; j < n1; j++)
                    for (k = 0; k < n1; k++)
                        C[i * n1 + k] += A[i * n1 + j] * B[j * n1 + k];
            // Shifting after each multiplication
            MPI_Cart_shift(mult_comm, 1, 1, &left, &right);
            MPI_Cart_shift(mult_comm, 0, 1, &up, &down);
            MPI_Sendrecv_replace(A, n1 * n1, MPI_INT, left, 1, right, 1, mult_comm, &status);
            MPI_Sendrecv_replace(B, n1 * n1, MPI_INT, up, 2, down, 2, mult_comm, &status);
        }
        MPI_Barrier(mult_comm);
        //checking if the matrix value became non zero
        if (rank == proc_needed)
        {
            if (C[s_new * n1 + d_new] != 0 || count >= n)
                flag = 1;
        }
        //rewriting value of A^i with A^(i+1)
        MPI_Bcast(&flag, 1, MPI_INT, proc_needed, mult_comm);
        if (flag == 0)
        {
            for (i = 0; i < n1; i++)
                for (j = 0; j < n1; j++)
                {
                    A[i * n1 + j] = C[i * n1 + j];
                    C[i * n1 + j] = 0;
                }
        }
    }
    end = MPI_Wtime();
    //printing the path length
    if (rank == 0)
    {
        if (count >= n)
        {
            printf("Time: %.4fs\nPath Length = infinate\n", end - start);
        }
        else
        {
            printf("Time: %.4fs\nPathLength = %d\n", end - start, count);
        }
    }

    free(A);
    free(B);
    free(C);
    MPI_Finalize();
    return 0;
}
