//
// COMP/XJCO3221 Parallel Computation Coursework 2: MPI / Distributed Memory Parallelism.
//


//
// Includes.
//

// Standard includes.
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// The MPI library.
#include <mpi.h>

// The include file for this coursework. Do not change the routines in "cwk2_extra.h", or avoid calling them,
// as theyt will be changed with different versions for the purposes of assessment.
#include "cwk2_extra.h"

#include "BroadcastBinaryTree.h"

void broadcastXUsingBinaryTree(int numprocs, int rank, float* x, int N);
void broadcastXFromParentToChild(int rank, TreeNode* node, float* x, int N);

void performMatrixVectorMultiplication(int numprocs, int rank, float* b, float* A, float* x, int N, int rowsPerProc, float* A_perProc, float* b_perProc)
{
	// Broadcast x to all processes

	int isNumprocsAPowerOf2 = numprocs && ((numprocs & (numprocs - 1)) == 0);
	if (isNumprocsAPowerOf2)
		broadcastXUsingBinaryTree(numprocs, rank, x, N);
	else
		MPI_Bcast(x, N, MPI_FLOAT, 0, MPI_COMM_WORLD);

	// Scatter distinct sets of rows to the processes
	MPI_Scatter(A, rowsPerProc * N, MPI_FLOAT, A_perProc, rowsPerProc * N, MPI_FLOAT, 0, MPI_COMM_WORLD);

	// Calculate b values for the rows allocated to each process

	int row, col;
	for (row = 0; row < rowsPerProc; row++)
	{
		b_perProc[row] = 0.0f;
		for (col = 0; col < N; col++)
			b_perProc[row] += A_perProc[row * N + col] * x[col];
	}

	// Gather scattered b values into final resulting vector
	MPI_Gather(b_perProc, rowsPerProc, MPI_FLOAT, b, rowsPerProc, MPI_FLOAT, 0, MPI_COMM_WORLD);
}

void broadcastXUsingBinaryTree(int numprocs, int rank, float* x, int N)
{
	BroadcastBinaryTree* tree = createBroadcastBinaryTree(numprocs);

	broadcastXFromParentToChild(rank, tree->root, x, N);

	freeTree(tree);
}

void broadcastXFromParentToChild(int rank, TreeNode* node, float* x, int N)
{
	// Left child we always be the same rank as the parent
	// node so only need to communicate with the right child

	if (!node->rightChild)
		return;
	
	int sendingRank = node->rank;
	int receivingRank = node->rightChild->rank;

	if (rank == sendingRank)
		MPI_Send(x, N, MPI_FLOAT, receivingRank, 0, MPI_COMM_WORLD);
	if (rank == receivingRank)
		MPI_Recv(x, N, MPI_FLOAT, sendingRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	
	broadcastXFromParentToChild(rank, node->leftChild, x, N);
	broadcastXFromParentToChild(rank, node->rightChild, x, N);
}

//
//
//
int main( int argc, char *argv[] )
{
	//
	// Initialisation.
	//

	// Start MPI and get the rank of this process.
	int numprocs, rank;
	MPI_Init( &argc, &argv );
	MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );

	// The matrix is of size NxN, and the vectors are of size N, where N is specified on the command line,
	// so need to first get N. The function 'getNFromCommandLine' does this, and also checks that N is divisible
	// by the number of processes. If it returns -1, something went wrong and MPI_Finalize() has already been called on all ranks.
	int N = getNFromCommandLine( argc, argv, rank, numprocs );		// Do NOT modify this call or the function in cwk2_extra.h 
	if( N==-1 ) return EXIT_FAILURE;

	// The number of matrix rows per process. Note we have checked by this point that N is divisible by numprocs.
	int rowsPerProc = N / numprocs;

	// All processes use the full vector x, i.e. the one that is being multiplied. Allocate memory for it - no error check for simplicity.
	float *x = (float*) malloc( N*sizeof(float) );

	// All processes also have an equal number of matrix rows, and a portion of the solution vector, so allocate memory for these as well.
	float *A_perProc = (float*) malloc( N * rowsPerProc * sizeof(float) );
	float *b_perProc = (float*) malloc(     rowsPerProc * sizeof(float) );

	// Allocate the full matrix and the full solution vector on rank 0 only. Also initialise A and x with values.
	float *A = NULL, *b = NULL;
	if( rank==0 ) initialiseMatrixAndSolution(&A,&b,x,N);		// Do NOT modify this call or the function in cwk2_extra.h 

	// Start the timing.
	double startTime = MPI_Wtime();

	//
	// Perform matrix-vector multiplication in parallel.
	//

	performMatrixVectorMultiplication(numprocs, rank, b, A, x, N, rowsPerProc, A_perProc, b_perProc);

	//
	// Check the answer on rank 0 in serial. Also output the result of the timing.
	//
	if( rank==0 )
	{
		// Time for the parallel computation.
		printf( "Parallel calculation took a total time %g s.\n", MPI_Wtime() - startTime );

		// Call the check routine "checkAgainstSerial()" in cwk2_extra.h, which prints messages explaining if the parallel calculation
		// matches the serial one, to within floating-point precision.
		checkAgainstSerial(A,x,b,N);					// Do NOT modify this call or the function in cwk2_extra.h 
	}

	//
	// Finalise.
	//

	// Free up all memory allocated for x, A_perProc and b_perProc, and (on rank 0 only) A and b.
	freeAllMemory(rank,A,b,x,A_perProc,b_perProc);

	// Tell MPI to free up resources.
	MPI_Finalize();
}
