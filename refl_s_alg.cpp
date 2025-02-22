#include <pthread.h>
#include "refl_s_alg.h"
#define e 1e-10

void synchronize(int total_threads){
	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	static pthread_cond_t condvar_in = PTHREAD_COND_INITIALIZER;
	static pthread_cond_t condvar_out = PTHREAD_COND_INITIALIZER;
	static int threads_in = 0;
	static int threads_out = 0;

	pthread_mutex_lock(&mutex);

	threads_in++;
	if(threads_in >= total_threads){
		threads_out = 0;
		pthread_cond_broadcast(&condvar_in);
	} else{
		while(threads_in < total_threads){
			pthread_cond_wait(&condvar_in,&mutex);
		}
	}

	threads_out++;
	if(threads_out >= total_threads){
		threads_in = 0;
		pthread_cond_broadcast(&condvar_out);
	} else{
		while(threads_out < total_threads){
			pthread_cond_wait(&condvar_out,&mutex);
		}
	}

	pthread_mutex_unlock(&mutex);
}

int findSolRefl(int n, double *a, double *x, double *b, int my_rank, int total_threads){
    int i, j, k;
	int first_row;
	int last_row;
	double s, tmp;

	for(i = 0; i < n; i++){
		
		if(my_rank == 0){
        	s = 0;

			for(j = i + 1; j < n; j++){
				s += a[j * n + i] * a[j * n + i];
        	}

        	tmp = sqrt(s + a[i * n + i] * a[i * n + i]);    	//норма a
			a[i * n + i] -= tmp;
			x[i] = s = sqrt(s + a[i * n + i] * a[i * n + i]);	//норма x
		}
		synchronize(total_threads);

		/*if(x[i] < e){
			return -1;
        }*/

		if(x[i] < e){
			if (my_rank == 0){
				a[i * n + i] += tmp;
			}
			synchronize(total_threads);
			continue;
		}

		if(my_rank == 0){
			for(j = i; j < n; j++){         //нормируем x
				a[j * n + i] /= s;
        	}

			s = 0;
			for(j = i; j < n; j++){         //скалярное произведение x и b
				s += a[j * n + i] * b[j];
        	}

			s *= 2.;
			for(j = i; j < n; j++){         //новый вектор b
				b[j] -= s * a[j * n + i];
       		}
		}
		synchronize(total_threads);

		first_row = (n - i - 1) * my_rank;
		first_row = first_row/total_threads + i + 1;
		last_row = (n - i - 1) * (my_rank + 1);
		last_row = last_row/total_threads + i + 1;

		for(k = first_row; k < last_row; k++){         //новая матрица A
			s = 0.;
			for(j = i; j < n; j++){
				s += a[j * n + i] * a[j * n + k];
            }

			s *= 2.;
			for(j = i; j < n; j++){
				a[j * n + k] -= s * a[j * n + i];
            }
		}
		synchronize(total_threads);

		if(my_rank == 0){
            a[i * n + i] = tmp;
		}
	}

	if(my_rank == 0){
		for(i = n - 1; i >= 0; i--){        //обратный ход
			tmp = b[i];
			for(j = i + 1; j < n; j++){
				tmp -= a[i * n + j] * x[j];
        	}
			x[i] = tmp / a[i * n + i];
		}
	}

    return 0;
}
