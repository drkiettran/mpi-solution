/*
* Circuit Satisfiability, Version 1
* 
* This MPI : program determines whether a circuit is
* satisf~able, that is, whether there is a combination of
* inputs that causes the output of the circuit to be 1.
* The particular circuit being tested is "wired" into the
* logic of[unction 'check_circuit'.All combinations of
* inputs that satisfy the circuit are printed.
* 
* Programmed by Michael J.Quinn
* 
* Last : nodification: 3 September 2002
* Recoded by Kiet Tran.
*/


#include <iostream>
#include <mpi.h>

/* Return 1 if 'i'th bit of 'n' is 1; 0 otherwise. */
#define EXTRACT_BIT(n,i) ((n&(1<<i))?1:0)
#define BITS 16

void check_circuit(int id, int z) {
	int v[BITS]; /* Each element is a bit of 'z' */
	int i;
	for (i = 0; i < BITS; i++) { 
		v[i] = EXTRACT_BIT(z, i); 
	}
	if ((v[0] || v[1]) && (!v[1] || !v[3]) && (v[2] || v[3])
		&& (!v[3] || !v[4]) && (v[4] || !v[5])
		&& (v[5] || !v[6]) && (v[5] || v[6])
		&& (v[6] || !v[15]) && (v[7] || !v[8])
		&& (!v[7] || !v[13]) && (v[8] || v[9])
		&& (v[8] || !v[9]) && (!v[9] || !v[10])
		&& (v[9] || v[11]) && (v[10] || v[11])
		&& (v[12] || v[13]) && (v[13] || !v[14])
		&& (v[14] || v[15])) {
		printf ("%d) %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\n", id,
			v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9],
			v[10], v[11], v[12], v[13], v[14], v[15]);
		fflush(stdout);
	}
}

/*
* Run program on Windows this way:
* mpiexec -n num-processors circuit_satisfiable.exe
* 
*/

int main(int argc, char* argv[])
{
	int i, id, p;
	std::cout << "check circuit ...\n";
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	if (id == 0) {
		std::cout << "there are " << p << " processors...\n";
	}

	for (i = id; i < 65536; i += p) {
		check_circuit(id, i);
	}
	printf("Process %d is done\n", id);
	fflush(stdout);
	MPI_Finalize();
	return 0;

}
