#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <math.h>


//비트맵 헤더를 한묶음으로
typedef struct tagBITMAPHEADER {
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;
	RGBQUAD hRGB[256]; //이 코드에서는 필요없음 (8bit에만 필요)
}BITMAPHEADER;

//비트맵을 읽어와서 화소정보의 포인터를 리턴
BYTE* loadBitmapFile(int bytesPerPixel
	, BITMAPHEADER* bitmapHeader, int* imgWidth, int* imgHeight, char* filename);

//비트맵 파일 쓰기
void writeBitmapFile(int bytesPerPixel, BITMAPHEADER outputHeader, BYTE* output, int imgSize, char* filename);


int** getHarrMatrix(int n); //Harr Matrix만들기
double** makeNormal(int** H, int n); // Normal Matrix화
double** getTrans_mn(double** ObjectMatrix, int m, int n);

double** multipleMatrix_DoubleDouble(double** leftMatrix, double** rightMatrix, int a, int b, int c);
double** multipleMatrix_DoubleInt(double** leftMatrix, int** rightMatrix, int a, int b, int c);
double** getZeroMatrix_mn(int m, int n); // m by n 영행렬 반환
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
	BITMAPHEADER originalHeader;	//비트맵의 헤더부분을 파일에서 읽어 저장할 구조체
	BITMAPHEADER outputHeader;		//변형을 가한 헤더부분을 저장할 구조체
	int imgSize, imgWidth, imgHeight;					//이미지의 크기를 저장할 변수
	int bytesPerPixel = 3;			//number of bytes per pixel (1 byte for R,G,B respectively)

	BYTE* image = loadBitmapFile(bytesPerPixel, &originalHeader, &imgWidth, &imgHeight, "image_lena_24bit.bmp"); //비트맵파일을 읽어 화소정보를 저장 (불러들이는 이미지는 .c와 같은 폴더에 저장)
	if (image == NULL) return 0;

	imgSize = imgWidth * imgHeight; // total number of pixels
	BYTE* output = (BYTE*)malloc(bytesPerPixel * sizeof(BYTE) * imgSize);				//결과값을 저장할 포인터 선언 및 메모리 할당
	outputHeader = originalHeader;										//헤더정보를 출력헤더정보에 할당



	/*******************************************************************/
	/************************ Perform HWT/IHWT *************************/
	/*******************************************************************/
	//이미지 행렬 A 구성 (RGB값이 있으므로 픽셀당 값 하나씩만 읽어서 imgHeight x imgWidth 행렬 구성)

	int** A; //original image matrix
	A = (int**)malloc(sizeof(int*) * imgHeight);
	for (int i = 0; i < imgHeight; i++) {
		A[i] = (int*)malloc(sizeof(int) * imgWidth);
	}

	for (int i = 0; i < imgHeight; i++)
		for (int j = 0; j < imgWidth; j++)
			A[i][j] = image[(i * imgWidth + j) * bytesPerPixel];


	//Haar matrix H 구성 (orthonormal column을 갖도록 구성)
	//이미지가 정사각형(Height==Width)이라고 가정; n = 2^t,t=0,1,2,...

	//H를 절반으로 나눈다.**************************************************************
	int n = imgHeight;
	int** H_int = getHarrMatrix(n); //H구함
	double** H = makeNormal(H_int, n);
	double** Ht = getTrans_mn(H, n, n); //Ht는 H의 전치
	double** Hl = getZeroMatrix_mn(n / 2, n);
	double** Hh = getZeroMatrix_mn(n / 2, n);
	Hl = cutMatrixHalfUp(Ht, Hl, n);
	Hh = cutMatrixHalfDown(Ht, Hh, n);
	double** Hlt = getTrans_mn(Hl, n / 2, n);
	double** Hht = getTrans_mn(Hh, n / 2, n);

	//검증완료**************************************************************************



	//(a)번 수행************************************************************************
	//인수들을 먼저 준비
	//좌변 인수
	double** Ht_A = multipleMatrix_DoubleInt(Ht, A, n, n, n);
	double** Ht_A_H = multipleMatrix_DoubleDouble(Ht_A, H, n, n, n); //A는 n by n이라 가정


	//우변 인수
	double** Hl_A = multipleMatrix_DoubleInt(Hl, A, n / 2, n, n);
	double** Hl_A_Hlt = multipleMatrix_DoubleDouble(Hl_A, Hlt, n / 2, n, n / 2);


	double** Hl_A_Hht = multipleMatrix_DoubleDouble(Hl_A, Hht, n / 2, n, n / 2);

	double** Hh_A = multipleMatrix_DoubleInt(Hh, A, n / 2, n, n);
	double** Hh_A_Hlt = multipleMatrix_DoubleDouble(Hh_A, Hlt, n / 2, n, n / 2);


	double** Hh_A_Hht = multipleMatrix_DoubleDouble(Hh_A, Hht, n / 2, n, n / 2);

	//우변 행렬 생성
	double** rightTermMatrixOf_a = getZeroMatrix_mn(n, n);
	rightTermMatrixOf_a = putUpperLeftcornerMatrix(Hl_A_Hlt, rightTermMatrixOf_a, n);
	rightTermMatrixOf_a = putUpperRightcornerMatrix(Hl_A_Hht, rightTermMatrixOf_a, n);
	rightTermMatrixOf_a = putLowerLeftcornerMatrix(Hh_A_Hlt, rightTermMatrixOf_a, n);
	rightTermMatrixOf_a = putLowerRightcornerMatrix(Hh_A_Hht, rightTermMatrixOf_a, n);

	//같나 테스트
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
		printf("3-(a)번의 수행결과,두행렬이 같음이 입증되었습니다.");
	}
	else {
		printf("두행렬이 다릅니다.");
	}





	//(a)번 수행종료**********************************************************************



	return 0;
}

BYTE* loadBitmapFile(int bytesPerPixel, BITMAPHEADER* bitmapHeader, int* imgWidth, int* imgHeight, char* filename)
{
	FILE* fp = fopen(filename, "rb");	//파일을 이진읽기모드로 열기
	if (fp == NULL)
	{
		printf("파일로딩에 실패했습니다.\n");	//fopen에 실패하면 NULL값을 리턴
		return NULL;
	}
	else
	{
		fread(&bitmapHeader->bf, sizeof(BITMAPFILEHEADER), 1, fp);	//비트맵파일헤더 읽기
		fread(&bitmapHeader->bi, sizeof(BITMAPINFOHEADER), 1, fp);	//비트맵인포헤더 읽기
		//fread(&bitmapHeader->hRGB, sizeof(RGBQUAD), 256, fp);	//색상팔렛트 읽기 (24bitmap 에서는 존재하지 않음)

		*imgWidth = bitmapHeader->bi.biWidth;
		*imgHeight = bitmapHeader->bi.biHeight;
		int imgSizeTemp = (*imgWidth) * (*imgHeight);	// 이미지 사이즈를 상위 변수에 할당

		printf("Size of image: Width %d   Height %d\n", bitmapHeader->bi.biWidth, bitmapHeader->bi.biHeight);
		BYTE* image = (BYTE*)malloc(bytesPerPixel * sizeof(BYTE) * imgSizeTemp);	//이미지크기만큼 메모리할당

		fread(image, bytesPerPixel * sizeof(BYTE), imgSizeTemp, fp);//이미지 크기만큼 파일에서 읽어오기

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

int** getHarrMatrix(int n) { // Harr Matrix 생성 함수 (완성)
	int** H;
	int** Hn;
	int** Old;

	int m = n / 2; //n=2m
	int** Im; //identity 행렬 (m by m)

	if (n == 1) {
		H = (int**)malloc(sizeof(int*) * 1);
		H[0] = (int*)malloc(sizeof(int) * 1);
		H[0][0] = 1; return H;
	}
	else {
		{//Hn 생성만 블록
			Hn = (int**)malloc(sizeof(int*) * n);
			for (int i = 0; i < n; i++) {
				Hn[i] = (int*)malloc(sizeof(int) * n);
			}

		}
		//Im생성블록 (테스트완료)
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
		//Hn 계산 블록
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
//3번 최적화 완료(검증완료)

double** makeNormal(int** H, int n) { //Harmatrix를 normal한다
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
//3번 최적화 완료(검증완료)

double** cutMatrixHalfUp(double** cuttedMatrix, double** UpperMatrix, int n) {

	for (int i = 0; i < n / 2; i++) {
		for (int j = 0; j < n; j++) {
			UpperMatrix[i][j] = cuttedMatrix[i][j];
		}
	}
	return UpperMatrix;
}
//3번에 최적화 완료(검증완료)

double** cutMatrixHalfDown(double** cuttedMatrix, double** downMatrix, int n) {
	for (int i = n / 2; i < n; i++) {
		for (int j = 0; j < n; j++) {
			downMatrix[i - n / 2][j] = cuttedMatrix[i][j];
		}
	}
	return downMatrix;
}
//3번에 최적화 완료(검증완료)

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
//3번에 최적화 완료(검증완료)

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
//3번에 최적화 완료(검증완료)

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
//3번에 최적화 완료(검증완료)

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
//3번에 최적화 완료(검증완료)

double** putUpperLeftcornerMatrix(double** puttedMatrix, double** attachedMatrix, int n) {
	for (int i = 0; i < n / 2; i++) {
		for (int j = 0; j < n / 2; j++) {
			attachedMatrix[i][j] = puttedMatrix[i][j];
		}
	}
	return attachedMatrix;
}
//3번에 최적화 완료(검증완료)

double** putUpperRightcornerMatrix(double** puttedMatrix, double** attachedMatrix, int n) {
	for (int i = 0; i < n / 2; i++) {
		for (int j = 0; j < n / 2; j++) {
			attachedMatrix[i][j + n / 2] = puttedMatrix[i][j];
		}
	}
	return attachedMatrix;
}
//3번에 최적화 완료(검증완료)

double** putLowerLeftcornerMatrix(double** puttedMatrix, double** attachedMatrix, int n) {
	for (int i = 0; i < n / 2; i++) {
		for (int j = 0; j < n / 2; j++) {
			attachedMatrix[i + n / 2][j] = puttedMatrix[i][j];
		}
	}
	return attachedMatrix;
}
//3번에 최적화 완료(검증완료)

double** putLowerRightcornerMatrix(double** puttedMatrix, double** attachedMatrix, int n) {
	for (int i = 0; i < n / 2; i++) {
		for (int j = 0; j < n / 2; j++) {
			attachedMatrix[i + n / 2][j + n / 2] = puttedMatrix[i][j];
		}
	}
	return attachedMatrix;
}
//3번에 최적화 완료(검증완료)
