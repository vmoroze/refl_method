all: res1

res1: m_main.o matrix_operations.o refl_s_alg.o
	g++ m_main.o matrix_operations.o refl_s_alg.o -o res1

m_main.o: m_main.cpp
	g++ -c m_main.cpp

matrix_operations.o: matrix_operations.cpp
	g++ -c matrix_operations.cpp

refl_s_alg.o: refl_s_alg.cpp
	g++ -c refl_s_alg.cpp