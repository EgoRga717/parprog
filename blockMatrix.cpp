//Can multiply only 2^n x 2^n matrixs. Type of matrix elements is "short"
#include <iostream>
#include <ctime>

int pow2(int input);
void matrixInit(short *Matrix, int n, bool isZero);
void matrixShow(short *Matrix, int size);
int nav(int i, int j, int i0, int j0, int n, int size);
void matrix2Multiply(short *A, int iA, int jA,  short *B, int iB, int jB, short *C, int iC, int jC, int size);
void blockMatrixMultiply(short *A, int iA, int jA, short *B, int iB, int jB, short *C, int iC, int jC, int n, int size);

typedef struct alignas(32)// cacheLine
{
	short tmp[12];		//sizeof(short) = 2 bytes
	short *Matrix00;	//sizeof = 8 bytes on my computer
	short *Matrix01;	//So, sizeof(cacheLine): (12 * 2 + 4 * 8) -> 64 on my computer
	short *Matrix10;
	short *Matrix11;
}cacheLine;

int main()
{
	srand(time(0));
	short *A, *B, *C;
	int log2size;
	
	std::cout << "Enter log2(size of matrix):\n";
	std::cin >> log2size;
	
	int size = pow2(log2size);
	
	A = new short[size * size];
	B = new short[size * size];
	C = new short[size * size];
	
	matrixInit(A, size, 1);
	matrixInit(B, size, 1);
	matrixInit(C, size, 0);
	
	std::cout << "A:\n";	
	matrixShow(A, size);

	std::cout << "B:\n";	
	matrixShow(B, size);

	double startTime = clock();
	blockMatrixMultiply(A, 0, 0, B, 0, 0, C, 0, 0, size, size);
	double endTime = clock();

	std::cout << "C = A x B:\n";	
	matrixShow(C, size);
	
	std::cout << "Calculation time: " << endTime - startTime << "\n";

	delete [] A;
	delete [] B;
	delete [] C;
	return 0;
}

int pow2(int input)
{
	int size = 1;
	for(int i = 0; i < input; ++i)
		size *= 2;
	return size;
}

void matrixInit(short *Matrix, int size, bool zero)	//creates matrix
{
	int memSize = size * size;
	if(zero == 0)
		for(int i = 0; i < memSize; ++i)
			Matrix[i] = 0;
	else
		for(int i = 0; i < memSize; ++i)
			Matrix[i] = rand() % 21 - 10;
}

void matrixShow(short *Matrix, int size)
{
	for(int i = 0; i < size; ++i)
	{
		for(int j = 0; j < size; ++j)
			std::cout << Matrix[nav(i, j, 0, 0, size, size)] << "  ";
		std::cout << "\n";
	}
}

int nav(int i, int j, int i0, int j0, int n, int size)	//returns real index of matrix
{
	return j + (i + i0) * size + j0;
}

void matrix2Multiply(short *A, int iA, int jA,  short *B, int iB, int jB, short *C, int iC, int jC, int size)	//multiplies 2x2 matrix
{	
	cacheLine s;
	
	s.Matrix00 = &C[nav(0, 0, iC, jC, 2, size)];
	s.Matrix01 = &C[nav(0, 1, iC, jC, 2, size)];
	s.Matrix10 = &C[nav(1, 0, iC, jC, 2, size)];
	s.Matrix11 = &C[nav(1, 1, iC, jC, 2, size)];

	s.tmp[4] = A[nav(0, 0, iA, jA, 2, size)];
	s.tmp[5] = A[nav(0, 1, iA, jA, 2, size)];
	s.tmp[6] = A[nav(1, 0, iA, jA, 2, size)];
	s.tmp[7] = A[nav(1, 1, iA, jA, 2, size)];
	
	s.tmp[8] = B[nav(0, 0, iB, jB, 2, size)];
	s.tmp[9] = B[nav(0, 1, iB, jB, 2, size)];
	s.tmp[10] = B[nav(1, 0, iB, jB, 2, size)];
	s.tmp[11] = B[nav(1, 1, iB, jB, 2, size)];
	
	s.tmp[0] = s.tmp[4] * s.tmp[8];
	s.tmp[1] = s.tmp[5] * s.tmp[10];
	s.tmp[0] += s.tmp[1];

	s.tmp[1] = s.tmp[4] * s.tmp[9];
	s.tmp[2] = s.tmp[5] * s.tmp[11];
	s.tmp[1] += s.tmp[2];

	s.tmp[2] = s.tmp[6] * s.tmp[8];
	s.tmp[3] = s.tmp[7] * s.tmp[10];
	s.tmp[2] += s.tmp[3];

	s.tmp[3] = s.tmp[6] * s.tmp[9];
	s.tmp[4] = s.tmp[7] * s.tmp[11];
	s.tmp[3] += s.tmp[4];
	
	*(s.Matrix00) += s.tmp[0];
	*(s.Matrix01) += s.tmp[1];
	*(s.Matrix10) += s.tmp[2];
	*(s.Matrix11) += s.tmp[3];
}

void blockMatrixMultiply(short *A, int iA, int jA, short *B, int iB, int jB, short *C, int iC, int jC, int n, int size)
{
	if(n <= 2)
	{
		if (n == 2)
			matrix2Multiply(A, iA, jA, B, iB, jB, C, iC, jC, size);
		else //(n == 1)
			C[0] = A[0] * B[0];
	}
	else
	{
		blockMatrixMultiply(A, iA, jA, B, iB, jB, C, iC, jC, n / 2, size);
		blockMatrixMultiply(A, iA, jA + n / 2, B, iB + n / 2, jB, C, iC, jC, n / 2, size);
		
		blockMatrixMultiply(A, iA, jA, B, iB, jB + n / 2, C, iC, jC + n / 2, n / 2, size);
		blockMatrixMultiply(A, iA, jA + n / 2, B, iB + n / 2, jB + n / 2, C, iC, jC + n / 2, n / 2, size);
		
		blockMatrixMultiply(A, iA + n / 2, jA, B, iB, jB, C, iC + n / 2 , jC, n / 2, size);
		blockMatrixMultiply(A, iA + n / 2, jA + n / 2, B, iB + n / 2, jB, C, iC + n / 2, jC, n / 2, size);
		
		blockMatrixMultiply(A, iA + n / 2, jA, B, iB, jB + n / 2, C, iC + n / 2, jC + n / 2, n / 2, size);
		blockMatrixMultiply(A, iA + n / 2, jA + n / 2, B, iB + n / 2, jB + n / 2, C, iC + n / 2, jC + n / 2, n / 2, size);
	}
}

