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
double** getTrans(double** Hn, int n); //��ġ��ı��ϱ�
double** multipleMatrix_doubleint(double** leftMatrix, int** rightMatrix, int n);//��� ���� double*int
double** multipleMatrix_doubledouble(double** leftMatrix, double** rightMatrix, int n);//    double*double
double** getSquareZeroMatrix(int n);// n by n ����� ��ȯ
double** cutMatrixAndAttach(double** cuttedMatrix, double** attachedMatrix, int k); //k*k��ŭ����
int main() {
	/*******************************************************************/
	/*************************** Read image  ***************************/
	/*******************************************************************/
	BITMAPHEADER originalHeader;	//��Ʈ���� ����κ��� ���Ͽ��� �о� ������ ����ü
	BITMAPHEADER outputHeader;		//������ ���� ����κ��� ������ ����ü
	int imgSize, imgWidth, imgHeight;					//�̹����� ũ�⸦ ������ ����
	int bytesPerPixel = 3;			//number of bytes per pixel (1 byte for R,G,B respectively)
	int k;

	BYTE* image = loadBitmapFile(bytesPerPixel, &originalHeader, &imgWidth, &imgHeight, "object.bmp"); //��Ʈ�������� �о� ȭ�������� ���� (�ҷ����̴� �̹����� .c�� ���� ������ ����)
	if (image == NULL) return 0;

	imgSize = imgWidth * imgHeight; // total number of pixels
	BYTE* output = (BYTE*)malloc(bytesPerPixel * sizeof(BYTE) * imgSize);				//������� ������ ������ ���� �� �޸� �Ҵ�
	outputHeader = originalHeader;										//��������� ������������ �Ҵ�


	printf("Bhat�� ������ ���B���� �� k by k �� �����ұ��? k : ");
	scanf("%d", &k);

	/*******************************************************************/
	/************************ Perform HWT/IHWT *************************/
	/*******************************************************************/
	//�̹��� ��� A ���� (RGB���� �����Ƿ� �ȼ��� �� �ϳ����� �о imgWidth x imgHeight ��� ����)
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

	int n = imgHeight;
	int** H = getHarrMatrix(n);
	double** Hn = makeNormal(H, n);  //Har martrix�� Hn�� orthnomal column�� ������ ����
	double** Hnt = getTrans(Hn, n); //Hnt�� Hn�� ��and��ġ
	//finish
	
	
	
	//HWT ����: ��İ� B = H'*A*H
	//...
	double** Ht_A = multipleMatrix_doubleint(Hnt, A, n);
	double** B = multipleMatrix_doubledouble(Ht_A, Hn, n);
	//finish


	
	//��� B �ڸ���: B�� upper left corner(subsquare matrix)�� �߶� Bhat�� ����
	//
	double** Bhat = getSquareZeroMatrix(n); //���� Bhat�� n by n �����
	Bhat = cutMatrixAndAttach(B, Bhat, k);
	//finish
	
	
	

	
	//IHWT ����: Ahat = H*Bhat*H'
	//...
	
	double** Ahat = getSquareZeroMatrix(n);
	double** H_Bhat = multipleMatrix_doubledouble(Hn,Bhat, n);
	Ahat = multipleMatrix_doubledouble(H_Bhat, Hnt, n);
	 //������ �������� ���ѰͰ���   
	




	
	/*******************************************************************/
	/******************* Write reconstructed image  ********************/
	/*******************************************************************/
	//Ahat�� �̿��ؼ� ���� image�� ���� ������ �ǵ��� ���� (��, Ahat = [a b;c d]�� [a a a b b b c c c d d d]�� ������ ��)
	BYTE* Are = (BYTE*)malloc(bytesPerPixel * sizeof(BYTE) * imgSize);
	//...
	int count = 0;
	for (int j = 0; j < imgWidth; j++) {
		for (int i = 0; i < imgHeight; i++) {
			for (int k = 0; k < 3; k++)
			{
				Are[count] = Ahat[j][i];
				count++;
			}
			
		}
	}
		
			





	//for (int i = 0; i < imgHeight; i++)
	//	for (int j = 0; j < imgWidth; j++)
	//		A[i][j] = image[(i * imgWidth + j) * bytesPerPixel];


	writeBitmapFile(bytesPerPixel, outputHeader, Are, imgSize, "changed_img.bmp");


	free(image);
	free(output);
	for (int i = 0; i < imgHeight; i++)
		free(A[i]);
	for (int i = 0; i < n; i++) {
		free(Hnt[i]); free(Ht_A[i]); free(B[i]); free(Bhat[i]); free(Ahat[i]); free(H_Bhat[i]);
	}
	free(A);
	free(Are);
	free(H);
	free(Hn);
	free(Hnt); free(Ht_A); free(B); free(Bhat); free(Ahat); free(H_Bhat);
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
		fread(&bitmapHeader->hRGB, sizeof(RGBQUAD), 256, fp);	//�����ȷ�Ʈ �б� (24bitmap ������ �������� ����)

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
	fwrite(&outputHeader.hRGB, sizeof(RGBQUAD), 256, fp); //not needed for 24bitmap
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
// �����Ϸ�

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
//�����Ϸ�


double** getTrans(double** Hn, int n) {
	double** Hnt = (double**)malloc(sizeof(double*) * n);
	for (int i = 0; i < n; i++) {
		Hnt[i] = (double*)malloc(sizeof(double) * n);
	}
	for (int i = 0; i < n; i++) {

		for (int j = 0; j < n; j++) {
			Hnt[j][i] = Hn[i][j];
		}
	}
	return Hnt;
} 
//�����Ϸ�

double** multipleMatrix_doubleint(double** leftMatrix, int** rightMatrix, int n) {
	double** resultMatrix = (double**)malloc(sizeof(double*) * n);
	for (int i = 0; i < n; i++) {
		resultMatrix[i] = (double*)malloc(sizeof(double) * n);
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			resultMatrix[i][j] = 0;
			for (int k = 0; k < n; k++) {
				resultMatrix[i][j] += leftMatrix[i][k] * rightMatrix[k][j];
			}

		}
	}
	return resultMatrix;
}
//�����Ϸ�

double** multipleMatrix_doubledouble(double** leftMatrix, double** rightMatrix, int n) {
	double** resultMatrix = (double**)malloc(sizeof(double*) * n);
	for (int i = 0; i < n; i++) {
		resultMatrix[i] = (double*)malloc(sizeof(double) * n);
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			resultMatrix[i][j] = 0;
			for (int k = 0; k < n; k++) {
				resultMatrix[i][j] += leftMatrix[i][k] * rightMatrix[k][j];
			}

		}
	}
	return resultMatrix;
}
//�����Ϸ�

double** getSquareZeroMatrix(int n) {
	double** result;
	result = (double**)malloc(sizeof(double*) * n);
	for (int i = 0; i < n; i++) {
		result[i] = (double*)malloc(sizeof(double) * n);
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			result[i][j] = 0.0;
		}
	}
	return result;
}
//�����Ϸ�

double** cutMatrixAndAttach(double** cuttedMatrix, double** attachedMatrix, int k) { //�����Ϸ�
	for (int i = 0; i < k; i++) {
		for (int j = 0; j < k; j++) {
			attachedMatrix[i][j] = cuttedMatrix[i][j];
		}
	}
	return attachedMatrix;
}
//�����Ϸ�