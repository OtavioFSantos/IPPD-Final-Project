// GRUPO: Eduarda Carvalho, Julia Junqueira, Julia Veiga e Otávio Santos

/*
 O grupo deverá desenvolver um projeto de implementação que envolva programação em ambiente com
 memória compartilhada (multithreading) e memória distribuída (troca de mensagens ou RPC/RMI).
 Qualquer combinação de ferramentas será aceita, como o uso combinado de OpenMP e MPI,
 Rust com os crates rayon e mpi, Java threads e RMI, C++ e RPC, entre outros.
 No entanto, não serão aceitas implementações com Pthreads e/ou Sockets.
*/

// PARA EXECUTAR ESTE PROGRAMA:
// 1) mpic++ velha.cpp -o velha -fopenmp
// 2) mpirun -np 4 ./velha

#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <mpi.h>
#include <omp.h>

#define SIZE 3

using namespace std;

char board[SIZE][SIZE];

void printBoard()
{
    cout << "-------------" << endl;
    for (int i = 0; i < SIZE; ++i)
    {
        cout << "| ";
        for (int j = 0; j < SIZE; ++j)
        {
            cout << board[i][j] << " | ";
        }
        cout << endl;
        cout << "-------------" << endl;
    }
}

char checkWinner()
{
    for (int i = 0; i < SIZE; ++i)
    {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2])
        {
            return board[i][0];
        }
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i])
        {
            return board[0][i];
        }
    }
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2])
    {
        return board[0][0];
    }
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0])
    {
        return board[0][2];
    }

    return ' ';
}

void makeComputerMove()
{
    // verifica se é possível vencer
    for (int i = 0; i < SIZE; ++i)
    {
        for (int j = 0; j < SIZE; ++j)
        {
            if (board[i][j] == ' ')
            {
                board[i][j] = 'O';
                if (checkWinner() == 'O')
                {
                    return;
                }
                board[i][j] = ' ';
            }
        }
    }
    // verifica se é possível que o jogador vença
    for (int i = 0; i < SIZE; ++i)
    {
        for (int j = 0; j < SIZE; ++j)
        {
            if (board[i][j] == ' ')
            {
                board[i][j] = 'X';
                if (checkWinner() == 'X')
                {
                    board[i][j] = 'O';
                    return;
                }
                board[i][j] = ' ';
            }
        }
    }
    // joga aleatoriamente
    int row, col;
    do
    {
        row = rand() % SIZE;
        col = rand() % SIZE;
    } while (board[row][col] != ' ');
    board[row][col] = 'O';
}

vector<pair<int, int>> getWinningPositions()
{
    vector<pair<int, int>> winningPositions;

#pragma omp parallel
    {
// Check rows
#pragma omp for
        for (int i = 0; i < SIZE; ++i)
        {
            if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != ' ')
            {
#pragma omp critical
                {
                    for (int j = 0; j < SIZE; ++j)
                    {
                        winningPositions.push_back({i, j});
                    }
                }
            }
        }

#pragma omp for
        for (int j = 0; j < SIZE; ++j)
        {
            if (board[0][j] == board[1][j] && board[1][j] == board[2][j] && board[0][j] != ' ')
            {
#pragma omp critical
                {
                    for (int i = 0; i < SIZE; ++i)
                    {
                        winningPositions.push_back({i, j});
                    }
                }
            }
        }

        if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != ' ')
        {
#pragma omp critical
            {
                for (int i = 0; i < SIZE; ++i)
                {
                    winningPositions.push_back({i, i});
                }
            }
        }

        if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != ' ')
        {
#pragma omp critical
            {
                for (int i = 0; i < SIZE; ++i)
                {
                    winningPositions.push_back({i, SIZE - 1 - i});
                }
            }
        }
    }

    return winningPositions;
}

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(NULL));

    for (int i = 0; i < SIZE; ++i)
    {
        for (int j = 0; j < SIZE; ++j)
        {
            board[i][j] = ' ';
        }
    }

    int moves = 0;
    char currentPlayer = 'X';
    bool gameOver = false;
    vector<pair<int, int>> winningPositions;

    while (!gameOver)
    {
        if (rank == 0)
        {
            cout << "Tabuleiro atual:" << endl;
            printBoard();
        }

        if (currentPlayer == 'X')
        {
            if (rank == 0)
            {
                cout << "Jogador X - Faça sua jogada (linha coluna): ";
                int row, col;
                cin >> row >> col;

                if (row >= 0 && row < SIZE && col >= 0 && col < SIZE && board[row][col] == ' ')
                {
                    board[row][col] = currentPlayer;
                    moves++;
                }
                else
                {
                    cout << "Jogada inválida, tente novamente!" << endl;
                    continue;
                }
            }
        }
        else
        {
            makeComputerMove();
            moves++;
        }

        MPI_Bcast(&board[0][0], SIZE * SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);

        char winner = checkWinner();
        if (winner != ' ')
        {
            if (rank == 0)
            {
                cout << "Jogador " << winner << " ganhou!" << endl;
                winningPositions = getWinningPositions();
                printBoard();
            }
            gameOver = true;
        }
        else if (moves == SIZE * SIZE)
        {
            if (rank == 0)
            {
                cout << "Empate!" << endl;
            }
            gameOver = true;
        }

        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';

        MPI_Barrier(MPI_COMM_WORLD);
    }

    if (!winningPositions.empty() && rank == 0)
    {
        cout << "Posição(s) vencedora(s):" << endl;
        for (auto pos : winningPositions)
        {
            cout << pos.first << "," << pos.second << endl;
        }
    }

    MPI_Finalize();
    return 0;
}
