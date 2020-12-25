#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <math.h>


int** getHarrMatrix(int n); //Harr Matrix�����
double** makeNormal(int** H, int n); // Normal Matrixȭ
double** getTrans_mn(double** ObjectMatrix, int m, int n);

double** multipleMatrix_DoubleDouble(double** leftMatrix, double** rightMatrix, int a, int b, int c); //    double*double
double** multipleMatrix_DoubleInt(double** leftMatrix, int** rightMatrix, int a, int b, int c);
double** getZeroMatrix_mn(int m, int n); // m by n ����� ��ȯ
double** cutMatrixHalfUp(double** cuttedMatrix, double** UpperMatrix, int n);
double** cutMatrixHalfDown(double** cuttedMatrix, double** downMatrix, int n);
double** putUpperLeftcornerMatrix(double** puttedMatrix, double** attachedMatrix, int n);
double** putUpperRightcornerMatrix(double** puttedMatrix, double** attachedMatrix, int n);
double** putLowerLeftcornerMatrix(double** puttedMatrix, double** attachedMatrix, int n);
double** putLowerRightcornerMatrix(double** puttedMatrix, double** attachedMatrix, int n);
double** sumMatrix_DoubleDouble(double** firstMatrix, double** secondMatrix, int n);
double** cutMatrixHalfUp_mn(double** cuttedMatrix, double** UpperMatrix, int m, int n);
double** cutMatrixHalfDown_mn(double** cuttedMatrix, double** downMatrix, int m, int n);


int main() {

	int n;
	scanf("%d", &n);
	int** H_int = getHarrMatrix(n);
	{
		printf("�⺻ H_int������ n=%d\n", n);
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				printf("%d \t", H_int[i][j]);
			}
			printf("\n");
		}
		printf("\n\n");
	}
	double** H = makeNormal(H_int,n);
	{
		printf("H normal������ n=%d\n", n);
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				printf("%f \t", H[i][j]);
			}
			printf("\n");
		}
		printf("\n\n");
	}
	
	double** Hl = getZeroMatrix_mn(n / 2, n);
	Hl= cutMatrixHalfUp(H, Hl, n);
	{
		printf("Hl ������ n=%d\n", n);
		for (int i = 0; i < n/2; i++) {
			for (int j = 0; j < n; j++) {
				printf("%f \t", Hl[i][j]);
			}
			printf("\n");
		}
		printf("\n\n");
	}
	double** Hll = getZeroMatrix_mn(n / 4, n);
	Hll= cutMatrixHalfUp_mn(Hl, Hll, n / 2, n);
	{
		printf("Hll ������ n=%d\n", n);
		for (int i = 0; i < n / 4; i++) {
			for (int j = 0; j < n; j++) {
				printf("%f \t", Hll[i][j]);
			}
			printf("\n");
		}
		printf("\n\n");
	}
	return 0;
}


int** getHarrMatrix(int n) { // Harr Matrix ���� �Լ� (�ϼ�)
	int** H;
	int** Hn;
	int** Old;

	int m = n / 2; //n=2m
	int** Im; //identity ��� (m by m)

	if (n == 1) {
		H = (int**)malloc(sizeof(int*) * 1);
		H[0] = (int*)malloc(sizeof(int) * 1);
		H[0][0] = 1; return H;
	}
	else {
		{//Hn ������ ���
			Hn = (int**)malloc(sizeof(int*) * n);
			for (int i = 0; i < n; i++) {
				Hn[i] = (int*)malloc(sizeof(int) * n);
			}

		}
		//Im������� (�׽�Ʈ�Ϸ�)
		{

			Im = (int**)malloc(sizeof(int*) * m);
			for (int i = 0; i < m; i++) {
				Im[i] = (int*)malloc(sizeof(int) * m);
			}

			for (int i = 0; i < m; i++) {
				for (int j = 0; j < m; j++) {
					Im[i][j] = 0;
					if (i == j) {
						Im[i][j] = 1;
					}
				}
			}
		}
		//Hn ��� ���
		{
			Old = getHarrMatrix(n / 2);
			{
				for (int i = 0; i < n; i++) {
					for (int j = 0; j < n / 2; j++) {
						Hn[i][j] = Old[i / 2][j] * 1;
					}


					for (int j = n / 2; j < n; j++) {
						if (i % 2 == 0) {
							Hn[i][j] = Im[i / 2][j - n / 2] * 1;
						}
						else {
							Hn[i][j] = Im[i / 2][j - n / 2] * -1;
						}
					}
				}
			}

		}
		return Hn;
	}
}
//3�� ����ȭ �Ϸ�(�����Ϸ�)

double** makeNormal(int** H, int n) { //Harmatrix�� normal�Ѵ�
	double** Hn = (double**)malloc(sizeof(double*) * n);
	for (int i = 0; i < n; i++) {
		Hn[i] = (double*)malloc(sizeof(double) * n);
	}


	for (int j = 0; j < n; j++) {
		int sumOfColumn = 0;

		for (int i = 0; i < n; i++) {
			sumOfColumn += H[i][j] * H[i][j];
		}

		for (int i = 0; i < n; i++) {
			Hn[i][j] = H[i][j] / sqrt((double)sumOfColumn);
		}
	}

	return Hn;
}
//3�� ����ȭ �Ϸ�(�����Ϸ�)

double** cutMatrixHalfUp(double** cuttedMatrix, double** UpperMatrix, int n) {

	for (int i = 0; i < n / 2; i++) {
		for (int j = 0; j < n; j++) {
			UpperMatrix[i][j] = cuttedMatrix[i][j];
		}
	}
	return UpperMatrix;
}
//3���� ����ȭ �Ϸ�(�����Ϸ�)

double** cutMatrixHalfDown(double** cuttedMatrix, double** downMatrix, int n) {
	for (int i = n / 2; i < n; i++) {
		for (int j = 0; j < n; j++) {
			downMatrix[i - n / 2][j] = cuttedMatrix[i][j];
		}
	}
	return downMatrix;
}
//3���� ����ȭ �Ϸ�(�����Ϸ�)

double** getZeroMatrix_mn(int m, int n) {
	double** result;
	result = (double**)malloc(sizeof(double*) * m);
	for (int i = 0; i < m; i++) {
		result[i] = (double*)malloc(sizeof(double) * n);
	}
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			result[i][j] = 0;
		}
	}
	return result;
}
//3���� ����ȭ �Ϸ�(�����Ϸ�)

double** multipleMatrix_DoubleInt(double** leftMatrix, int** rightMatrix, int a, int b, int c) { //a by b multiple b by c
	double** resultMatrix;
	resultMatrix = (double**)malloc(sizeof(double*) * a);
	for (int i = 0; i < a; i++) {
		resultMatrix[i] = (double*)malloc(sizeof(double) * c);
	}

	for (int i = 0; i < a; i++) {
		for (int j = 0; j < c; j++) {
			resultMatrix[i][j] = 0;
			for (int k = 0; k < b; k++) {
				resultMatrix[i][j] += leftMatrix[i][k] * rightMatrix[k][j];
			}
		}
	}
	return resultMatrix;
}
//3���� ����ȭ �Ϸ�(�����Ϸ�)

double** multipleMatrix_DoubleDouble(double** leftMatrix, double** rightMatrix, int a, int b, int c) { //a by b multiple b by c
	double** resultMatrix;
	resultMatrix = (double**)malloc(sizeof(double*) * a);
	for (int i = 0; i < a; i++) {
		resultMatrix[i] = (double*)malloc(sizeof(double) * c);
	}

	for (int i = 0; i < a; i++) {
		for (int j = 0; j < c; j++) {
			resultMatrix[i][j] = 0;
			for (int k = 0; k < b; k++) {
				resultMatrix[i][j] += leftMatrix[i][k] * rightMatrix[k][j];
			}
		}
	}
	return resultMatrix;
}
//3���� ����ȭ �Ϸ�(�����Ϸ�)

double** getTrans_mn(double** objectMatrix, int m, int n) {
	double** resultMatrix = (double**)malloc(sizeof(double*) * n);
	for (int i = 0; i < n; i++) {
		resultMatrix[i] = (double*)malloc(sizeof(double) * m);
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			resultMatrix[i][j] = objectMatrix[j][i];
		}
	}
	return resultMatrix;
}
//3���� ����ȭ �Ϸ�(�����Ϸ�)

double** putUpperLeftcornerMatrix(double** puttedMatrix, double** attachedMatrix, int n) {
	for (int i = 0; i < n / 2; i++) {
		for (int j = 0; j < n / 2; j++) {
			attachedMatrix[i][j] = puttedMatrix[i][j];
		}
	}
	return attachedMatrix;
}
//3���� ����ȭ �Ϸ�(�����Ϸ�)

double** putUpperRightcornerMatrix(double** puttedMatrix, double** attachedMatrix, int n) {
	for (int i = 0; i < n / 2; i++) {
		for (int j = 0; j < n / 2; j++) {
			attachedMatrix[i][j+n/2] = puttedMatrix[i][j];
		}
	}
	return attachedMatrix;
}
//3���� ����ȭ �Ϸ�(�����Ϸ�)

double** putLowerLeftcornerMatrix(double** puttedMatrix, double** attachedMatrix, int n) {
	for (int i = 0; i < n / 2; i++) {
		for (int j = 0; j < n / 2; j++) {
			attachedMatrix[i + n / 2][j] = puttedMatrix[i][j];
		}
	}
	return attachedMatrix;
}
//3���� ����ȭ �Ϸ�(�����Ϸ�)

double** putLowerRightcornerMatrix(double** puttedMatrix, double** attachedMatrix, int n) {
	for (int i = 0; i < n / 2; i++) {
		for (int j = 0; j < n / 2; j++) {
			attachedMatrix[i + n / 2][j+n/2] = puttedMatrix[i][j];
		}
	}
	return attachedMatrix;
}
//3���� ����ȭ �Ϸ�(�����Ϸ�)

double** sumMatrix_DoubleDouble(double** firstMatrix, double** secondMatrix, int n) {
	double** resultMatrix;
	resultMatrix = (double**)malloc(sizeof(double*) * n);
	for (int i = 0; i < n; i++) {
		resultMatrix[i] = (double*)malloc(sizeof(double) * n);

	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			resultMatrix[i][j] = firstMatrix[i][j] + secondMatrix[i][j];
		}
	}
	return resultMatrix;
}
//3���� ����ȭ �Ϸ�(�����Ϸ�)

double** cutMatrixHalfUp_mn(double** cuttedMatrix, double** UpperMatrix, int m, int n) {

	for (int i = 0; i < m / 2; i++) {
		for (int j = 0; j < n; j++) {
			UpperMatrix[i][j] = cuttedMatrix[i][j];
		}
	}
	return UpperMatrix;
}
//3���� ����ȭ �Ϸ�(�����Ϸ�)
double** cutMatrixHalfDown_mn(double** cuttedMatrix, double** downMatrix, int m, int n) {
	for (int i = m / 2; i < n; i++) {
		for (int j = 0; j < n; j++) {
			downMatrix[i - m / 2][j] = cuttedMatrix[i][j];
		}
	}
	return downMatrix;
}
//3���� ����ȭ �Ϸ�(�����Ϸ�)