#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

const int n = 152512534;

int isComposite(int n)
{
    int composite = 0;
    if(n == 1)
        composite = 1;
    else if(n % 2 == 0 && n != 2)
        composite = 1;

    if(composite != 1) 
    {
        int i;
        for(i = 3; i < n; i = i + 2) 
        {
            if(n % i == 0)
            {
                composite = 1;
                break;
            }
            if(i * i > n) 
                break;
        }
    }

    return composite;
}

int main() 
{
    long local_count = 0, global_count = 0,     //Prime counts
    start, finish,                              //Processor share of the total problem
    i; 

    int world_rank,                             //Processor ID 
    world_size,                                 //World size
    size, proc0_size;                           
    
    double time;                                //Time elapsed

    //Initialize the MPI environment
    MPI_Init(NULL, NULL);

    //Get the rank of processors
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    //Get the number of processors
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    //Synchronises the processors and ensures they enter the code at the same time
    MPI_Barrier(MPI_COMM_WORLD);

    //Start the timer
    time = -(MPI_Wtime());

    //Processor share of the problem
    start = 2 + (world_rank * (n - 1) / world_size);
    finish = 1 + ((world_rank + 1) * (n - 1) / world_size);
    size = finish - start + 1;

    //Size of problem for Processor 0
    proc0_size = (n - 1) / world_size;

    //Check if there are too many processors for the problem
    if((2 + proc0_size) < (long)sqrt((double)n)) 
    {
        if(world_rank == 0)
            printf ("Too many processors for problem size %d\n", n);
        //Finalize the MPI environment
        MPI_Finalize();
        exit(1);
    }

    //Count Primes
    for(i = start; i <= finish; i++)
    {
        //If prime increase count
        if(!isComposite(i))
            local_count++;
    }

    //MPI_Reduce to combine the local counts of each processor
    MPI_Reduce(&local_count, &global_count, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    //Total runtime
    time = time + MPI_Wtime();
    printf("Processor - %d finished execution\n", world_rank); 

    //Print primes and elapsed time from root processor
    if(world_rank == 0) 
    {
        printf ("There are %ld primes less than or equal to %d\n", global_count, n);
        printf ("\nTotal elapsed time :  %f s\n", time);
    }

    //Finalize the MPI environment
    MPI_Finalize ();
    return 0;
}
