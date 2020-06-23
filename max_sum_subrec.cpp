#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define INF 123456789;

int max_subsequence_sum_1(int **matrix, int row_size, int col_size); //algorithm that time complex is O(n^6)
int max_subsequence_sum_2(int **matrix, int row_size, int col_size); //algorithm that time complex is O(n^4)
int max_subsequence_sum_3(int **matrix, int row_size, int col_size); //algorithm that time complex is O(n^3)
/*
main
-파일이름, fun# 받기 ->file 읽기 -> matrix 저장 -> fun# 호출 -> maxsum 값얻고-> output file 양식에 맞게 file 쓰기
*/
//모두 음수일때 되나 확인
int main(int argc, char* argv[]) {
	
	char *filename = argv[1];
	int  func_num = atoi(argv[2]);
	int row, col;
	int** matrix;
	int maxsum = 0;
	
	FILE *fp = fopen(filename, "r");
	//filename error check
	if (!fp) {
		printf("No such file\n");
		return 0;
	}
	//func_num error check
	if (func_num>3) {
		printf("Wrong function number\n");
		return 0;
	}
	//initialize matrix, row, col by input
	fscanf(fp, "%d %d", &row, &col);
	matrix = (int**)malloc(sizeof(int*)*row);
	for (int i = 0; i < row; i++) {
		matrix[i] = (int*)malloc(sizeof(int)*col);
	}
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			fscanf(fp, "%d", &matrix[i][j]);
		}
	}

	//recode program start time
	clock_t start_time = clock();

	//excute function by function number
	switch (func_num) {
	case 1:
		maxsum = max_subsequence_sum_1(matrix, row, col);
		break;
	case 2:
		maxsum = max_subsequence_sum_2(matrix, row, col);
		break;
	case 3:
		maxsum = max_subsequence_sum_3(matrix, row, col);
		break;
	}
	// record program end time
	clock_t end_time = clock();
	
	fclose(fp);

	//output to file
	char output_filename[100] = "result_";
	strcat(output_filename, filename);
	fp = fopen(output_filename, "w");
	
	fprintf(fp, "%s\n", filename);
	fprintf(fp, "%d\n", func_num);
	fprintf(fp, "%d\n", row);
	fprintf(fp, "%d\n", col);
	fprintf(fp, "%d\n", maxsum);

	fprintf(fp, "%lf\n", (double)(end_time - start_time)/(CLOCKS_PER_SEC)*1000);

	fclose(fp);

	return 0;
}

//algorithm that time complex is O(n^6)
int max_subsequence_sum_1(int **matrix, int row_size, int col_size) {
	int this_sum, max_sum = -INF;
	//set (row,col) (i,j) for start of matrix to end of matrix
	for (int i = 0; i < row_size; i++) {
		for (int j = 0; j < col_size; j++) {
			//set (row,col) (k,l) for start of matrix to end of matrix
			for (int k = i; k < row_size; k++) {
				for (int l = j; l < col_size; l++) {
					this_sum = 0;
					for (int m = i; m <= k; m++) {
						for (int n = j; n <= l; n++) {
							this_sum += matrix[m][n];
						}
					}
					if(this_sum > max_sum)
						max_sum = this_sum;
				}
			}
		}
	}
	return max_sum;
}

//algorithm that time complex is O(n^4)
int max_subsequence_sum_2(int **matrix, int row_size, int col_size) {
	int this_sum, max_sum = -INF;

	//change matrix to prefix matrix
	//sub sum of row
	for (int i = 0; i < row_size; i++) {
		for (int j = 1; j < col_size; j++) {
			matrix[i][j] += matrix[i][j - 1];
		}
	}
	//subsum of col
	for (int j = 0; j < col_size; j++) {
		for (int i = 1; i < row_size; i++) {
			matrix[i][j] += matrix[i - 1][j];
		}
	}

	for (int i = 0; i < row_size; i++) {
		for (int j = 0; j < col_size; j++) {
			this_sum = 0;
			for (int k = i; k < row_size; k++) {
				for (int l = j; l < col_size; l++) {
					if (i > 0 && j > 0){
						//calculate the subsum of matrix starting by (i,j) to (k,l)
						this_sum = matrix[k][l] - matrix[i - 1][l] - matrix[k][j - 1] + matrix[i - 1][j - 1];
					}
					else if(i==0 && j>0){
						//exception when i=0
						this_sum = matrix[k][l]- matrix[k][j - 1];
					}
					else if(i>0 && j==0) {
						//exception when j=0
						this_sum = matrix[k][l] - matrix[i - 1][l];
					}
					else {
						//exception when i=j=0
						this_sum = matrix[0][0];
					}

					if (this_sum > max_sum)
						max_sum = this_sum;
						
				}
			}
		}
	}
	return max_sum;
}

//algorithm that time complex is O(n^3)
int max_subsequence_sum_3(int **matrix,int row_size, int col_size) {
	int this_sum, max_sum = -INF;
	int *row_subsum = (int*)malloc(sizeof(int)*row_size); //array of each row_subsum
	
	for (int i = 0; i < col_size; i++) {
		//initialize array of each row_subsum by 0
		for (int l = 0; l < row_size; l++)
			row_subsum[l] = 0;
		for (int j = i; j < col_size; j++) {
			for(int l = 0; l < row_size; l++)
				row_subsum[l] += matrix[l][j];
			
			//aply 1D array O(N) algorithm to row_subsum
			this_sum = 0;
			for (int k = 0; k < row_size; k++) {
				this_sum += row_subsum[k];

				if (this_sum > max_sum)
					max_sum = this_sum;
				else if (this_sum < 0)
					this_sum = 0;
			}
		}
	}
	free(row_subsum);

	return max_sum;
}
