#include <openmpi/mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define CORE_PROCESS 0

// broadcast a number from 0 to others
void broadcast(int *data_ptr, int begin, int end) {
    for (int i = begin; i < end; ++i)
        MPI_Send(data_ptr, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
}

int main(int argc, char** argv) {
    // Counting time
    clock_t start_clock = clock();

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Information about the MPI 
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Setting N
    int N = 100;
    if (argc > 1) {
        N = 0;
        for (char *p = argv[1]; *p; ++p)
            N = N*10+(*p)-'0';
    }

    // Block size
    int PROCESSES = world_size;
    int BLOCK_SIZE = N/PROCESSES+ (N%PROCESSES?1:0);
    // If sqrt(N) is not included in the first block
    if (BLOCK_SIZE+1 < ceil(sqrt(N))) {
        printf("Error occurs: BLock size is smaller than Sqrt(N)!\n");
        return 0;
    }


    // Variables
    bool prime[BLOCK_SIZE];
    for (int i = 0; i < BLOCK_SIZE; ++i) prime[i] = true;
    int start = world_rank * BLOCK_SIZE, 
        end = (world_rank+1)*BLOCK_SIZE;
    // boundary
    if (end > N) end = N;
    int num, loc, count = 0;

    // Processes
    if (world_rank == CORE_PROCESS) {
        printf("N = %d\n", N);
        printf("#Processes = %d\n", world_size);
        for (num = 2; num < BLOCK_SIZE; ++num) 
            if (prime[num]) { 
                count++;
                // broadcast the message to others
                broadcast(&num, 1, PROCESSES);
                for (loc = num*2; loc < end; loc+=num) 
                    prime[loc] = false;
            }
        // Send 0 as the ending signal
        num = 0;
        broadcast(&num, 1, PROCESSES); 
    }
    else { // workers
        // Keep receive message from 0 until message equals to 0
        while (MPI_Recv(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE), num != 0) {
            for (loc = start/num * num; loc < end; loc += num)
                if (loc >= start) prime[loc-start] = false;
        }

        // Sieve
        for (int i = start; i < end; ++i) 
            if (prime[i - start]) 
                count++;
    }

    // Finalize the MPI environment.
    MPI_Finalize();

    // Counting time 
    clock_t end_clock = clock();
    double duration = (end_clock-start_clock)/(double)CLOCKS_PER_SEC;

    // Output the result
    printf("Process[%d] found %d primes within [%d, %d) in %.4fs\n", world_rank, count, start, end, duration);
}
