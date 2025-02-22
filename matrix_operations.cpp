#include "matrix_operations.h"
#include <sys/resource.h>
#include <sys/time.h>
#define E 1e-20

double _max(double a, double b, double e){
    return (a - b >= e) ? a : b;
}

double _mod(double a, double b, double e){
    return (a - b >= e) ? a - b : b - a;
}

int createMatrixWithoutFile(double *massA, int n, int k){
    int i, j;

    for(i = 0; i < n; i++){
        for(j = 0; j < n; j++){
            if(k == 1){
                massA[i*n + j] = n - _max(i + 1, j + 1, E) + 1;
                continue;
            }
            if(k == 2){
                massA[i*n + j] = _max(i + 1, j + 1, E);
                continue;
            }
            if(k == 3){
                massA[i*n + j] = _mod(i + 1, j + 1, E);
                continue;
            }
            if(k == 4){
                massA[i*n + j] = 1./(i + j + 1);
            }
            else{
                std::cout << "incorrect k" << std::endl;
                return -1;
            }
        }
    }

    return 1;
}

int createMatrixFromFile(char *filename, double *massA, int n){
    std::ifstream fp(filename);
	int lenOfMas = n * n;
    int i = 0;
    double trash;

    if(!fp.is_open()){
        std::cout << "can't open file" << std::endl;
        return -1;
    }
    
    while(fp >> trash){
        i++;
    }
 
    if(!fp.eof()){
        if(fp.fail()){
            std::cout << "incorrect type in file" << std::endl;
            fp.close();
            return -2;
        }
        std::cout << "file error" << std::endl;
        fp.close();
        return -3;
    }   

    if(i != lenOfMas){
        std::cout << "not enough elements in file" << std::endl;
        fp.close();
        return -4;
    }

    try{
        fp.clear();
        fp.seekg(0);
    } catch(...){
		fp.close();
        std::cout << "incorrect operation in change of cursor" << std::endl;
        return -5;
    }    

    for(i = 0; i < lenOfMas; i++){
        fp >> massA[i];
    }
	
	fp.close();

    return 1;
}

int createColumnB(double *matrix, double *B, int len){
    int i = 0, j = 0;

    for(i = 0; i < len; i++){
        B[i] = 0;
        for(j = 0; j < len; j += 2){
            B[i] += matrix[i*len + j];
        }
    }

    return 1;
}

int printMatrix(double *mass, int n, int m, int mode){
    int i, j = 0, len;
    
    if(m < 0){
        std::cout << "incorrect m" << std::endl;
        return -1;
    }

    if(mode){
        len = m;
    } else{
        len = m*m;
    }
    
    std::cout << "(\t";
    for(i = 0; i < len; i++){
        if(i % m == 0 && i != 0 && i != m * m - 1){
            std::cout << ")" << std::endl << "(\t";
        }
        if(i % m == 0 && i != 0){
            j++;
        }
        printf("%10.3e\t", mass[(n - m) * j + i]);
    }
    std::cout << ")" << std::endl;

    return 1;
}

double checkDiscrepancy(double *temp, double *matrix, double *vector, double *B, int len){
    int i;
	double result1 = 0., result2 = 0.;
    
    multiplyMatrixVector(matrix, vector, temp, len);

    for(i = 0; i < len; i++){
		result1 += (temp[i] - B[i]) * (temp[i] - B[i]); 
	}

    for(i = 0; i < len; i++){
		result2 += B[i] * B[i]; 
	}

	return sqrt(result1) / sqrt(result2);
}

int multiplyMatrixVector(double *matrix, double *vector, double *result, int len){
    int i = 0, j = 0;

    for(i = 0; i < len; i++){
        result[i] = 0;
        for(j = 0; j < len; j++){
            result[i] += matrix[i*len + j] * vector[j];
        }
    }

    return 1;
}

double checkInaccuracy(double *vector, int len){
    double acc = 0.;
    int i;

    for(i = 0; i < len; i++){
        if(i % 2 == 0){
            acc += (vector[i] - 1) * (vector[i] - 1);
        } else{
            acc += vector[i] * vector[i];
        }
    }

    return sqrt(acc);
}

int normMatrix(double *a, double *b, int n){
    int i, j;
    double norm = 0, tmp;

    for(i = 0; i < n; i++){
        for(j = 0; j < n; j++){
            tmp = a[i*n + j] * a[i*n + j];
        }
        tmp = sqrt(tmp);
        if(tmp > norm){
            norm = tmp;
        }
    }

    for(i = 0; i < n; i++){
        for(j = 0; j < n; j++){
            a[i*n + j] /= norm;
        }
    }
    for(i = 0; i < n; i++){
        b[i] /= norm;    
    }

    return 0;
}

long int get_time(void){
	struct rusage buf;
    getrusage(RUSAGE_SELF, &buf);
    return buf.ru_utime.tv_sec * 100 + buf.ru_utime.tv_usec/10000;
}

long int get_full_time(void){
	struct timeval buf;
    gettimeofday(&buf, 0);
    return buf.tv_sec * 100 + buf.tv_usec/10000;
}
