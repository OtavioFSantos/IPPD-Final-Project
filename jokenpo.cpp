// GRUPO: Eduarda Carvalho, Julia Junqueira, Julia Veiga e Otávio Santos

/*
 O grupo deverá desenvolver um projeto de implementação que envolva programação em ambiente com
 memória compartilhada (multithreading) e memória distribuída (troca de mensagens ou RPC/RMI).
 Qualquer combinação de ferramentas será aceita, como o uso combinado de OpenMP e MPI,
 Rust com os crates rayon e mpi, Java threads e RMI, C++ e RPC, entre outros.
 No entanto, não serão aceitas implementações com Pthreads e/ou Sockets.
*/

// PARA EXECUTAR ESTE PROGRAMA:
// 1) mpic++ jokenpo.cpp -o jokenpo -fopenmp
// 2) mpirun -np 4 ./jokenpo

#include <iostream>
#include <mpi.h>
#include <omp.h>
#include <ctime>

using namespace std;

int main(int argc, char *argv[])
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(NULL) + rank);

    int choice;
    int opponent_choice;

    if (rank == 0)
    {
        cout << "Escolha sua jogada: (0 - Pedra, 1 - Papel, 2 - Tesoura) " << endl;
        cin >> choice;
    }

    MPI_Bcast(&choice, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int opponent_rank = (rank + 1) % size;
    MPI_Recv(&opponent_choice, 1, MPI_INT, opponent_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

#pragma omp parallel default(none) shared(choice)
    {
#pragma omp single
        {
            if (rank != 0)
            {
#pragma omp task
                {
                    choice = rand() % 3;
                    cout << "Processo " << rank << " escolheu: " << choice << endl;
                }
            }
        }
    }

    MPI_Send(&choice, 1, MPI_INT, opponent_rank, 0, MPI_COMM_WORLD);

    int result;
    if (choice == opponent_choice)
        result = 0; // empate
    else if ((choice == 0 && opponent_choice == 2) ||
             (choice == 1 && opponent_choice == 0) ||
             (choice == 2 && opponent_choice == 1))
        result = 1; // vitoria
    else
        result = -1; // derrota

    int *results = new int[size];
    MPI_Gather(&result, 1, MPI_INT, results, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        int wins = 0, draws = 0, losses = 0;
        for (int i = 0; i < size; ++i)
        {
            if (results[i] == 1)
                ++wins;
            else if (results[i] == 0)
                ++draws;
            else
                ++losses;
        }

        cout << "Resultados:" << endl;
        cout << "Vitórias: " << wins << endl;
        cout << "Empates: " << draws << endl;
        cout << "Derrotas: " << losses << endl;
    }

    delete[] results;

    MPI_Finalize();
    return 0;
}
