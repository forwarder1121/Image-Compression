#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <math.h>


//��Ʈ�� ����� �ѹ�������
typedef struct tagBITMAPHEADER {
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;
	RGBQUAD hRGB[256]; //�� �ڵ忡���� �ʿ���� (8bit���� �ʿ�)
}BITMAPHEADER;

//��Ʈ���� �о�ͼ� ȭ�������� �����͸� ����
BYTE* loadBitmapFile(int bytesPerPixel
	, BITMAPHEADER* bitmapHeader, int* imgWidth, int* imgHeight, char* filename);

//��Ʈ�� ���� ����
void writeBitmapFile(int bytesPerPixel, BITMAPHEADER outputHeader, BYTE* output, int imgSize, char* filename);


int** getHarrMatrix(int n); //Harr Matrix�����
double** makeNormal(int** H, int n); // Normal Matrixȭ
double** getTrans_mn(double** ObjectMatrix, int m, int n);

double** multipleMatrix_DoubleDouble(double** leftMatrix, double** rightMatrix, int a, int b, int c);
double** multipleMatrix_DoubleInt(double** leftMatrix, int** rightMatrix, int a, int b, int c);
double** getZeroMatrix_mn(int m, int n); // m by n ����� ��ȯ
double** cutMatrixHalfUp(double** cuttedMatrix, double** UpperMatrix, int n);
double** cutMatrixHalfDown(double** cuttedMatrix, double** downMatrix, int n);
double** putUpperLeftcornerMatrix(double** puttedMatrix, double** attachedMatrix, int n);
double** putUpperRightcornerMatrix(double** puttedMatrix, double** attachedMatrix, int n);
double** putLowerLeftcornerMatrix(double** puttedMatrix, double** attachedMatrix, int n);
double** putLowerRightcornerMatrix(double** puttedMatrix, double** attachedMatrix, int n);



int main() {
	/*******************************************************************/
	/*************************** Read image  ***************************/
	/*******************************************************************/
	BITMAPHEADER originalHeader;	//��Ʈ���� ����κ��� ���Ͽ��� �о� ������ ����ü
	BITMAPHEADER outputHeader;		//������ ���� ����κ��� ������ ����ü
	int imgSize, imgWidth, imgHeight;					//�̹����� ũ�⸦ ������ ����
	int bytesPerPixel = 3;			//number of bytes per pixel (1 byte for R,G,B respectively)

	BYTE* image = loadBitmapFile(bytesPerPixel, &originalHeader, &imgWidth, &imgHeight, "image_lena_24bit.bmp"); //��Ʈ�������� �о� ȭ�������� ���� (�ҷ����̴� �̹����� .c�� ���� ������ ����)
	if (image == NULL) return 0;

	imgSize = imgWidth * imgHeight; // total number of pixels
	BYTE* output = (BYTE*)malloc(bytesPerPixel * sizeof(BYTE) * imgSize);				//������� ������ ������ ���� �� �޸� �Ҵ�
	outputHeader = originalHeader;										//��������� ������������ �Ҵ�



	/*******************************************************************/
	/************************ Perform HWT/IHWT *************************/
	/*******************************************************************/
	//�̹��� ��� A ���� (RGB���� �����Ƿ� �ȼ��� �� �ϳ����� �о imgHeight x imgWidth ��� ����)

	int** A; //original image matrix
	A = (int**)malloc(sizeof(int*) * imgHeight);
	for (int i = 0; i < imgHeight; i++) {
		A[i] = (int*)malloc(sizeof(int) * imgWidth);
	}

	for (int i = 0; i < imgHeight; i++)
		for (int j = 0; j < imgWidth; j++)
			A[i][j] = image[(i * imgWidth + j) * bytesPerPixel];


	//Haar matrix H ���� (orthonormal column�� ������ ����)
	//�̹����� ���簢��(Height==Width)�̶�� ����; n = 2^t,t=0,1,2,...

	//H�� �������� ������.**************************************************************
	int n = imgHeight;
	int** H_int = getHarrMatrix(n); //H����
	double** H = makeNormal(H_int, n);
	double** Ht = getTrans_mn(H, n, n); //Ht�� H�� ��ġ
	double** Hl = getZeroMatrix_mn(n / 2, n);
	double** Hh = getZeroMatrix_mn(n / 2, n);
	Hl = cutMatrixHalfUp(Ht, Hl, n);
	Hh = cutMatrixHalfDown(Ht, Hh, n);
	double** Hlt = getTrans_mn(Hl, n / 2, n);
	double** Hht = getTrans_mn(Hh, n / 2, n);

	//�����Ϸ�**************************************************************************



	//(a)�� ����************************************************************************
	//�μ����� ���� �غ�
	//�º� �μ�
	double** Ht_A = multipleMatrix_DoubleInt(Ht, A, n, n, n);
	double** Ht_A_H = multipleMatrix_DoubleDouble(Ht_A, H, n, n, n); //A�� n by n�̶� ����


	//�캯 �μ�
	double** Hl_A = multipleMatrix_DoubleInt(Hl, A, n / 2, n, n);
	double** Hl_A_Hlt = multipleMatrix_DoubleDouble(Hl_A, Hlt, n / 2, n, n / 2);


	double** Hl_A_Hht = multipleMatrix_DoubleDouble(Hl_A, Hht, n / 2, n, n / 2);

	double** Hh_A = multipleMatrix_DoubleInt(Hh, A, n / 2, n, n);
	double** Hh_A_Hlt = multipleMatrix_DoubleDouble(Hh_A, Hlt, n / 2, n, n / 2);


	double** Hh_A_Hht = multipleMatrix_DoubleDouble(Hh_A, Hht, n / 2, n, n / 2);

	//�캯 ��� ����
	double** rightTermMatrixOf_a = getZeroMatrix_mn(n, n);
	rightTermMatrixOf_a = putUpperLeftcornerMatrix(Hl_A_Hlt, rightTermMatrixOf_a, n);
	rightTermMatrixOf_a = putUpperRightcornerMatrix(Hl_A_Hht, rightTermMatrixOf_a, n);
	rightTermMatrixOf_a = putLowerLeftcornerMatrix(Hh_A_Hlt, rightTermMatrixOf_a, n);
	rightTermMatrixOf_a = putLowerRightcornerMatrix(Hh_A_Hht, rightTermMatrixOf_a, n);

	//���� �׽�Ʈ
	int result = 1;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (rightTermMatrixOf_a[i][j] != Ht_A_H[i][j]) {
				result = 0;
			}
		}
		printf("\n");
	}

	if (result == 1) {
		printf("3-(a)���� ������,������� ������ �����Ǿ����ϴ�.");
	}
	else {
		printf("������� �ٸ��ϴ�.");
	}





	//(a)�� ��������**********************************************************************



	return 0;
}

BYTE* loadBitmapFile(int bytesPerPixel, BITMAPHEADER* bitmapHeader, int* imgWidth, int* imgHeight, char* filename)
{
	FILE* fp = fopen(filename, "rb");	//������ �����б���� ����
	if (fp == NULL)
	{
		printf("���Ϸε��� �����߽��ϴ�.\n");	//fopen�� �����ϸ� NULL���� ����
		return NULL;
	}
	else
	{
		fread(&bitmapHeader->bf, sizeof(BITMAPFILEHEADER), 1, fp);	//��Ʈ��������� �б�
		fread(&bitmapHeader->bi, sizeof(BITMAPINFOHEADER), 1, fp);	//��Ʈ��������� �б�
		//fread(&bitmapHeader->hRGB, sizeof(RGBQUAD), 256, fp);	//�����ȷ�Ʈ �б� (24bitmap ������ �������� ����)

		*imgWidth = bitmapHeader->bi.biWidth;
		*imgHeight = bitmapHeader->bi.biHeight;
		int imgSizeTemp = (*imgWidth) * (*imgHeight);	// �̹��� ����� ���� ������ �Ҵ�

		printf("Size of image: Width %d   Height %d\n", bitmapHeader->bi.biWidth, bitmapHeader->bi.biHeight);
		BYTE* image = (BYTE*)malloc(bytesPerPixel * sizeof(BYTE) * imgSizeTemp);	//�̹���ũ�⸸ŭ �޸��Ҵ�

		fread(image, bytesPerPixel * sizeof(BYTE), imgSizeTemp, fp);//�̹��� ũ�⸸ŭ ���Ͽ��� �о����

		fclose(fp);
		return image;
	}
}

void writeBitmapFile(int bytesPerPixel, BITMAPHEADER outputHeader, BYTE* output, int imgSize, char* filename)
{
	FILE* fp = fopen(filename, "wb");

	fwrite(&outputHeader.bf, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&outputHeader.bi, sizeof(BITMAPINFOHEADER), 1, fp);
	//fwrite(&outputHeader.hRGB, sizeof(RGBQUAD), 256, fp); //not needed for 24bitmap
	fwrite(output, bytesPerPixel * sizeof(BYTE), imgSize, fp);
	fclose(fp);
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
			attachedMatrix[i][j + n / 2] = puttedMatrix[i][j];
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
			attachedMatrix[i + n / 2][j + n / 2] = puttedMatrix[i][j];
		}
	}
	return attachedMatrix;
}
//3���� ����ȭ �Ϸ�(�����Ϸ�)
