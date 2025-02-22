#include <iostream>
#include <fstream>
#include <time.h>
#include <pthread.h>
#include "matrix_operations.h"
#include "refl_s_alg.h"

typedef struct{
	int n;
	double *a;
	double *b;
	double *x;
	int my_rank;
	int total_threads;
} ARGS;

long int thread_time = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *Solution(void *p_arg)
{
	ARGS *arg = (ARGS*)p_arg;
	long int t1;

	t1 = get_time();
	findSolRefl(arg->n, arg->a, arg->x, arg->b, arg->my_rank, arg->total_threads);
	t1 = get_time() - t1;

	pthread_mutex_lock(&mutex);
	thread_time += t1;
	pthread_mutex_unlock(&mutex);

	return NULL;
}

int main(int argc, char *argv[]){
    /*
        n - dim of matrix
	    m - num of output row and column
	    k - option of input matrix (0 - from file)
	    filename - file name which consist matrix
    */

    int n, m, k;
	//int err;
    char *filename;
	double *massA, *massB, *massX, *forAcc;
	long int t_full;
	int total_threads;
	pthread_t *threads;
	ARGS *args;

        if((argc < 5)||(argc > 6)){
            std::cout << "incorrect input" << std::endl;
            return -1;
        }

    try{
		n = atoi(argv[1]);
		total_threads = atoi(argv[2]);
		m = atoi(argv[3]);
		k = atoi(argv[4]);
	} catch(...){
		std::cout << "incorrect input" << std::endl;
		return -1;
	}

        if(n == 0){
            std::cout << "0x0 matrix" << std::endl;
            return -10;
        }

    try{
		massA = new double[n * n];
		massB = new double[n];
		massX = new double[n];
		threads = new pthread_t[total_threads];
		args = new ARGS[total_threads];
	} catch(...){
		std::cout << "some trouble with memory" << std::endl;
		return -2;
	}

    if(k == 0){
		try{
			filename = argv[5];
		} catch(...){
			std::cout << "incorrect name of file" << std::endl;
			return -3;
		}
		if(createMatrixFromFile(filename, massA, n) != 1){
			delete [] massA;
			delete [] massB;
			delete [] massX;
			delete [] threads;
			delete [] args;
			return -4;
		}
	} else{
		if(createMatrixWithoutFile(massA, n, k) != 1){
			delete [] massA;
			delete [] massB;
            delete [] massX;
			delete [] threads;
			delete [] args;
			return -5;
		}
	}

    std::cout << std::endl;
	std::cout << "Matrix A:" << std::endl;
	if(printMatrix(massA, n, m, 0) != 1){
		delete [] massA;
		delete [] massB;
		delete [] massX;
		delete [] threads;
		delete [] args;
		return -6;
	}
	std::cout << std::endl;

    createColumnB(massA, massB, n);

	normMatrix(massA, massB, n);

	for(i = 0; i < total_threads; i++){
		args[i].n = n;
		args[i].a = massA;
		args[i].b = massB;
		args[i].x = massX;
		args[i].my_rank = i;
		args[i].total_threads = total_threads;
	}

    t_full = get_full_time();

	for(i = 0; i < total_threads; i++){
		if(pthread_create(threads + i, 0, Solution, args + i)){
			printf("cannot create thread %d\n", i);
			delete [] massA;
			delete [] massB;
            delete [] massX;
			delete [] threads;
			delete [] args;
			return -10;
		}
	}

	for(i = 0; i < total_threads; i++){
		if(pthread_join(threads[i], 0)){
			printf("cannot wait thread %d!\n", i);
			delete [] massA;
			delete [] massB;
            delete [] massX;
			delete [] threads;
			delete [] args;
			return -11;
		}
	}

	t_full = get_full_time() - t_full;

    /*if(err != 0){
		std::cout << "incorrect matrix" << std::endl;
		delete [] massA;
		delete [] massB;
		delete [] massX;
		delete [] threads;
		delete [] args;
		return -7;
	}*/

    std::cout << "Vector X:" << std::endl;
	printMatrix(massX, n, m, 1);
	std::cout << std::endl;

	if(t_full == 0){
		t_full = 1;
	}

	printf("\n\nSolution time\t\t%ld\nTotal threads time\t%ld"\
		"(%ld%%)\nPer thread\t\t\t%ld\n",
		t_full, thread_time, thread_time * 100 / t_full,
		thread_time / total_threads);

	try{
        forAcc = new double[n];
    } catch(...){
        std::cout << "some trouble with memory" << std::endl;
        return -1;
    }

    if(k == 0){
        try{
            filename = argv[4];
        } catch(...){
            std::cout << "incorrect name of file" << std::endl;
            return -3;
        }
        if(createMatrixFromFile(filename, massA, n) != 1){
            delete [] massA;
            delete [] massB;
            delete [] massX;
            delete [] forAcc;
			delete [] threads;
			delete [] args;
			return -4;
        }
    } else{
        if(createMatrixWithoutFile(massA, n, k) != 1){
            delete [] massA;
            delete [] massB;
            delete [] forAcc;
            delete [] massX;
			delete [] threads;
			delete [] args;
            return -5;
        }
    }

    createColumnB(massA, massB, n);

    std::cout << "Discrepancy:\t\t\t" << checkDiscrepancy(forAcc, massA, massX, massB, n) << std::endl;

	delete [] forAcc;

    std::cout << "Inaccuracy:\t\t\t" << checkInaccuracy(massX, n) << std::endl;
	std::cout << std::endl;

    delete [] massA;
	delete [] massB;
	delete [] massX;
	delete [] threads;
	delete [] args;
	return 1;
}
