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
double** getTrans(double** Hn, int n); //전치행렬구하기
double** multipleMatrix_doubleint(double** leftMatrix, int** rightMatrix, int n);//행렬 곱셈 double*int
double** multipleMatrix_doubledouble(double** leftMatrix, double** rightMatrix, int n);//    double*double
double** getSquareZeroMatrix(int n);// n by n 영행렬 반환
double** cutMatrixAndAttach(double** cuttedMatrix, double** attachedMatrix, int k); //k*k만큼복사
int main() {
	/*******************************************************************/
	/*************************** Read image  ***************************/
	/*******************************************************************/
	BITMAPHEADER originalHeader;	//비트맵의 헤더부분을 파일에서 읽어 저장할 구조체
	BITMAPHEADER outputHeader;		//변형을 가한 헤더부분을 저장할 구조체
	int imgSize, imgWidth, imgHeight;					//이미지의 크기를 저장할 변수
	int bytesPerPixel = 3;			//number of bytes per pixel (1 byte for R,G,B respectively)
	int k;

	BYTE* image = loadBitmapFile(bytesPerPixel, &originalHeader, &imgWidth, &imgHeight, "object.bmp"); //비트맵파일을 읽어 화소정보를 저장 (불러들이는 이미지는 .c와 같은 폴더에 저장)
	if (image == NULL) return 0;

	imgSize = imgWidth * imgHeight; // total number of pixels
	BYTE* output = (BYTE*)malloc(bytesPerPixel * sizeof(BYTE) * imgSize);				//결과값을 저장할 포인터 선언 및 메모리 할당
	outputHeader = originalHeader;										//헤더정보를 출력헤더정보에 할당


	printf("Bhat을 원래의 행렬B에서 몇 k by k 로 추출할까요? k : ");
	scanf("%d", &k);

	/*******************************************************************/
	/************************ Perform HWT/IHWT *************************/
	/*******************************************************************/
	//이미지 행렬 A 구성 (RGB값이 있으므로 픽셀당 값 하나씩만 읽어서 imgWidth x imgHeight 행렬 구성)
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

	int n = imgHeight;
	int** H = getHarrMatrix(n);
	double** Hn = makeNormal(H, n);  //Har martrix는 Hn에 orthnomal column을 가지고 있음
	double** Hnt = getTrans(Hn, n); //Hnt는 Hn의 역and전치
	//finish
	
	
	
	//HWT 수행: 행렬곱 B = H'*A*H
	//...
	double** Ht_A = multipleMatrix_doubleint(Hnt, A, n);
	double** B = multipleMatrix_doubledouble(Ht_A, Hn, n);
	//finish


	
	//행렬 B 자르기: B의 upper left corner(subsquare matrix)를 잘라 Bhat에 저장
	//
	double** Bhat = getSquareZeroMatrix(n); //현재 Bhat은 n by n 영행렬
	Bhat = cutMatrixAndAttach(B, Bhat, k);
	//finish
	
	
	

	
	//IHWT 수행: Ahat = H*Bhat*H'
	//...
	
	double** Ahat = getSquareZeroMatrix(n);
	double** H_Bhat = multipleMatrix_doubledouble(Hn,Bhat, n);
	Ahat = multipleMatrix_doubledouble(H_Bhat, Hnt, n);
	 //검증은 못했으나 잘한것같음   
	




	
	/*******************************************************************/
	/******************* Write reconstructed image  ********************/
	/*******************************************************************/
	//Ahat을 이용해서 위의 image와 같은 형식이 되도록 구성 (즉, Ahat = [a b;c d]면 [a a a b b b c c c d d d]를 만들어야 함)
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
		fread(&bitmapHeader->hRGB, sizeof(RGBQUAD), 256, fp);	//색상팔렛트 읽기 (24bitmap 에서는 존재하지 않음)

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
	fwrite(&outputHeader.hRGB, sizeof(RGBQUAD), 256, fp); //not needed for 24bitmap
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
// 검증완료

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
//검증완료


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
//검증완료

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
//검증완료

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
//검증완료

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
//검증완료

double** cutMatrixAndAttach(double** cuttedMatrix, double** attachedMatrix, int k) { //검증완료
	for (int i = 0; i < k; i++) {
		for (int j = 0; j < k; j++) {
			attachedMatrix[i][j] = cuttedMatrix[i][j];
		}
	}
	return attachedMatrix;
}
//검증완료