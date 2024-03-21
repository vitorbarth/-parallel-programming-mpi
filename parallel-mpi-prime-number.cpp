#include <iostream>
#include <cstdlib>
#include <mpi.h>

int prime_number(int num_start, int num_end) {
	int total = 0;
	for (int i = num_start; i <= num_end; i++ ){
		int prime = 1;
		for (int j = 2; j < i; j++ ){
			if ( i % j == 0 ){
				prime = 0;
				break;
			}
		}
		total = total + prime;
	}

    std::cout << "Total: " << total << " recebido: entre " << num_start << " e " << num_end << std::endl;
	return total;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    double start_time, end_time;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 3) {
        std::cerr << "Este programa precisa de pelo menos 3 processos MPI" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    const int MASTER = 0;
    const int RESULT_COLLECTOR = size - 1;

    int number;
    if (rank == MASTER) {
        if (argc != 2) {
            std::cerr << "Uso: " << argv[0] << " <number>" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        number = std::atoi(argv[1]);
    }

    MPI_Bcast(&number, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    if (rank != MASTER && rank != RESULT_COLLECTOR) {

        int start = (rank - 1) * (number / (size - 2)) + 1;
        int end = (rank == size - 2) ? number : rank * (number / (size - 2));

        int count = 0;
        count = prime_number(start, end);

        MPI_Send(&count, 1, MPI_INT, RESULT_COLLECTOR, 0, MPI_COMM_WORLD);

    } else if (rank == RESULT_COLLECTOR) {

        int total = 0;
        for (int i = 1; i < size - 1; ++i) {
            int partialCount;
            MPI_Recv(&partialCount, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total += partialCount;
        }
        std::cout << "Total de números primos: " << total << std::endl;
        
        end_time = MPI_Wtime();
        double duration_ms = (end_time - start_time) * 1000.0; // Convertendo para milissegundos
        std::cout << "Tempo de execução: " << duration_ms << " milissegundos" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
