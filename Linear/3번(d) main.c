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
double** sumMatrix_DoubleDouble(double** firstMatrix, double** secondMatrix, int n);
double** cutMatrixHalfDown_mn(double** cuttedMatrix, double** downMatrix, int m, int n);
double** cutMatrixHalfUp_mn(double** cuttedMatrix, double** UpperMatrix, int m, int n);



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
	double** B = Ht_A_H;

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

	//a번 수행블록 종료************************************************************************
	


	//d번 수행 시작************************************************************************
	printf("d번 수행시작\n");
	//인수 준비
	double** Hll = getZeroMatrix_mn(n / 4, n);
	double** Hlh = getZeroMatrix_mn(n / 4, n); 
	Hll = cutMatrixHalfUp_mn(Hl, Hll, n / 2, n);  
	Hlh = cutMatrixHalfDown_mn(Hl, Hlh, n / 2, n);
	double** Hllt = getTrans_mn(Hll, n / 4, n);
	double** Hlht = getTrans_mn(Hlh, n / 4, n);
	printf("인수 준비 중..");
	//좌변 인수 
	double** H_B = multipleMatrix_DoubleDouble(H, B, n, n, n);
	double** H_B_Ht = multipleMatrix_DoubleDouble(H_B, Ht, n, n, n);
	double** leftMatrixOfb = H_B_Ht;
	double** Hlt_Hl = multipleMatrix_DoubleDouble(Hlt, Hl, n, n / 2, n);
	double** Hlt_Hl_A = multipleMatrix_DoubleInt(Hlt_Hl, A, n, n, n);
	double** Hlt_Hl_A_Hlt = multipleMatrix_DoubleDouble(Hlt_Hl_A, Hlt, n, n, n / 2);
	double** Hlt_Hl_A_Hlt_Hl = multipleMatrix_DoubleDouble(Hlt_Hl_A_Hlt, Hl, n, n / 2, n);
	double** leftMatrixOfd = Hlt_Hl_A_Hlt_Hl;
	printf("좌변인수 준비 중..");
	//우변인수
	//첫번쨰 항
	double** f1_Hllt_Hll = multipleMatrix_DoubleDouble(Hllt, Hll, n, n / 4, n);
	double** f1_Hllt_Hll_A = multipleMatrix_DoubleInt(f1_Hllt_Hll, A, n, n, n);
	double** f1_Hllt_Hll_A_Hllt = multipleMatrix_DoubleDouble(f1_Hllt_Hll_A, Hllt, n, n, n / 4);
	double** f1_Hllt_Hll_A_Hllt_Hll = multipleMatrix_DoubleDouble(f1_Hllt_Hll_A_Hllt, Hll, n, n / 4, n);
	double** firstTermMatrix = f1_Hllt_Hll_A_Hllt_Hll;
	//두번쨰 항
	double** f2_Hllt_Hll = multipleMatrix_DoubleDouble(Hllt, Hll, n, n / 4, n);
	double** f2_Hllt_Hll_A = multipleMatrix_DoubleInt(f2_Hllt_Hll, A, n, n, n);
	double** f2_Hllt_Hll_A_Hlht = multipleMatrix_DoubleDouble(f2_Hllt_Hll_A, Hlht, n, n, n / 4);
	double** f2_Hllt_Hll_A_Hlht_Hlh = multipleMatrix_DoubleDouble(f2_Hllt_Hll_A_Hlht, Hlh, n, n / 4, n);
	double** secondTermMatrix = f2_Hllt_Hll_A_Hlht_Hlh;
	//세번째 항
	double** f3_Hlht_Hlh = multipleMatrix_DoubleDouble(Hlht, Hlh, n, n / 4, n);
	double** f3_Hlht_Hlh_A = multipleMatrix_DoubleInt(f3_Hlht_Hlh, A, n, n, n);
	double** f3_Hlht_Hlh_A_Hllt = multipleMatrix_DoubleDouble(f3_Hlht_Hlh_A, Hllt, n,n, n / 4);
	double** f3_Hlht_Hlh_A_Hllt_Hll = multipleMatrix_DoubleDouble(f3_Hlht_Hlh_A_Hllt, Hll, n, n / 4, n);
	double** thirdTermMatrix = f3_Hlht_Hlh_A_Hllt_Hll;
	//네번쨰 항
	double** f4_Hlht_Hlh = multipleMatrix_DoubleDouble(Hlht, Hlh, n, n / 4, n);
	double** f4_Hlht_Hlh_A = multipleMatrix_DoubleInt(f4_Hlht_Hlh, A, n, n, n);
	double** f4_Hlht_Hlh_A_Hlht = multipleMatrix_DoubleDouble(f4_Hlht_Hlh_A, Hlht, n, n, n / 4);
	double** f4_Hlht_Hlh_A_Hlht_Hlh = multipleMatrix_DoubleDouble(f4_Hlht_Hlh_A_Hlht, Hlh, n, n / 4, n);
	double** fourthTermMatrix = f4_Hlht_Hlh_A_Hlht_Hlh;
	printf("우변인수 준비 중..");
	//행렬 합
	double** first_second = sumMatrix_DoubleDouble(firstTermMatrix, secondTermMatrix, n);
	double** first_second_third = sumMatrix_DoubleDouble(first_second, thirdTermMatrix, n);
	double** first_second_third_fourth = sumMatrix_DoubleDouble(first_second_third, fourthTermMatrix, n);
	double** rightMatrixOfd = first_second_third_fourth;

	int result_3 = 1;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (abs(leftMatrixOfd[i][j] - rightMatrixOfd[i][j]) > 0.0001) {
				result_3 = 0;
			}

		}
	}
	if (result_3 == 1) {
		printf("\n3-(d)번의 수행결과,두 행렬이 같음이 입증되었습니다.");
	}
	else {
		printf("두행렬이 다릅니다.");
	}
	
	/*******************************************************************/
	/******************* Write reconstructed image  ********************/
	/*******************************************************************/
	//Ahat을 이용해서 위의 image와 같은 형식이 되도록 구성 (즉, Ahat = [a b;c d]면 [a a a b b b c c c d d d]를 만들어야 함)
	BYTE* Are_1 = (BYTE*)malloc(bytesPerPixel * sizeof(BYTE) * imgSize);
	BYTE* Are_2 = (BYTE*)malloc(bytesPerPixel * sizeof(BYTE) * imgSize);
	BYTE* Are_3 = (BYTE*)malloc(bytesPerPixel * sizeof(BYTE) * imgSize);
	BYTE* Are_4 = (BYTE*)malloc(bytesPerPixel * sizeof(BYTE) * imgSize);
	//...
	int count = 0;
	for (int j = 0; j < imgWidth; j++) {
		for (int i = 0; i < imgHeight; i++) {
			for (int k = 0; k < 3; k++)
			{
				Are_1[count] = firstTermMatrix[j][i];
				count++;
			}

		}
	}
	count = 0;
	for (int j = 0; j < imgWidth; j++) {
		for (int i = 0; i < imgHeight; i++) {
			for (int k = 0; k < 3; k++)
			{
				Are_2[count] = secondTermMatrix[j][i];
				count++;
			}

		}
	}
	count = 0;
	for (int j = 0; j < imgWidth; j++) {
		for (int i = 0; i < imgHeight; i++) {
			for (int k = 0; k < 3; k++)
			{
				Are_3[count] = thirdTermMatrix[j][i];
				count++;
			}

		}
	}
	count = 0;
	for (int j = 0; j < imgWidth; j++) {
		for (int i = 0; i < imgHeight; i++) {
			for (int k = 0; k < 3; k++)
			{
				Are_4[count] = fourthTermMatrix[j][i];
				count++;
			}

		}
	}





	//for (int i = 0; i < imgHeight; i++)
	//	for (int j = 0; j < imgWidth; j++)
	//		A[i][j] = image[(i * imgWidth + j) * bytesPerPixel];


	writeBitmapFile(bytesPerPixel, outputHeader, Are_1, imgSize, "changed_cloud_d_1.bmp");
	writeBitmapFile(bytesPerPixel, outputHeader, Are_2, imgSize, "changed_cloud_d_2.bmp");
	writeBitmapFile(bytesPerPixel, outputHeader, Are_3, imgSize, "changed_cloud_d_3.bmp");
	writeBitmapFile(bytesPerPixel, outputHeader, Are_4, imgSize, "changed_cloud_d_4.bmp");


	free(image);
	free(output);
	for (int i = 0; i < imgHeight; i++)
		free(A[i]);
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
//3번에 최적화 완료(검증완료)

double** cutMatrixHalfUp_mn(double** cuttedMatrix, double** UpperMatrix, int m, int n) {

	for (int i = 0; i < m / 2; i++) {
		for (int j = 0; j < n; j++) {
			UpperMatrix[i][j] = cuttedMatrix[i][j];
		}
	}
	return UpperMatrix;
}
//3번에 최적화 완료(검증완료)
double** cutMatrixHalfDown_mn(double** cuttedMatrix, double** downMatrix, int m, int n) {
	for (int i = m / 2; i < m; i++) {
		for (int j = 0; j < n; j++) {
			downMatrix[i - m / 2][j] = cuttedMatrix[i][j];
		}
	}
	return downMatrix;
}
//3번에 최적화 완료(검증완료)