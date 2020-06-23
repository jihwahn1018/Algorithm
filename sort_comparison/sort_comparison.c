#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SWAP(a,b){int tmp; tmp=a; a=b; b=tmp;}

void algorithm1(int* list, int n); //bubble sort
void algorithm2(int* list, int left, int right); //quick sort
void algorithm3(int* list, int left, int right); //merge sort
void algorithm4(int* list, int n, int* temp); //radix sort
void merge(int* list, int left, int middle, int right);

int* elem;// element's list
int* temp;// temporary list

int main(int argc,char* argv[]) {
	char* filename = argv[1]; //input file name
	int algo_num = atoi(argv[2]); // algorithm number that will be used
	int elem_num;
	
	int i;

	FILE *fp = fopen(filename, "r");

	//file open error
	if (!fp) {
		printf("No such file!\n");
		return -1;
	}
	//get element's number
	fscanf(fp, "%d", &elem_num);
	elem = (int*)malloc(sizeof(int)*elem_num);
	temp = (int*)malloc(sizeof(int)*elem_num);
	
	//save elemnt in array
	for (i = 0; i < elem_num; i++) {
		fscanf(fp, "%d", &elem[i]);
	}

	//recode program start time
	clock_t start_time = clock();

	switch (algo_num) {
	case 1:
		algorithm1(elem, elem_num);
		break;
	case 2:
		algorithm2(elem, 0, elem_num-1);
		break;
	case 3:
		algorithm3(elem, 0, elem_num - 1);
		break;
	case 4:
		algorithm4(elem,elem_num,temp);
		break;
	default:
		printf("%d is wrong algorithm number. Please choose number 1~4\n", algo_num);
		return -1;
	}

	//record program end time
	clock_t end_time = clock();

	fclose(fp);

	//output to file
	char output_filename[100] = "result_";
	char s[5];
	sprintf(s, "%d", algo_num);

	strcat(output_filename, s);
	strcat(output_filename, "_");
	strcat(output_filename, filename);
	fp = fopen(output_filename, "w");

	fprintf(fp, "%s\n", filename);
	fprintf(fp, "%d\n", algo_num);
	fprintf(fp, "%d\n", elem_num); 
	fprintf(fp, "%lf\n", (double)(end_time - start_time) / (CLOCKS_PER_SEC));
	
	for (i = 0; i < elem_num; i++) {
		fprintf(fp, "%d ", elem[i]);
	}

	
	fclose(fp);

	free(elem);
	free(temp);

	return 0;
}
//bubbel sort
void algorithm1(int* list, int n) {
	int i, j;
	for (i = 0; i < n - 1; i++) {
		for (j = n - 1; j > i; j--) {
			if (list[j] < list[j - 1])
				SWAP(list[j], list[j - 1]);
		}
	}
}
//quick sort
void algorithm2(int* list, int left, int right) {
	int pivot, i;

	if (right - left > 0) {
		//partition
		pivot = left;
		for (i = left; i < right; i++) {
			if (list[i] < list[right]) {
				SWAP(list[i], list[pivot]);
				pivot++;
			}
		}
		SWAP(list[right], list[pivot]);

		algorithm2(list, left, pivot - 1);
		algorithm2(list, pivot + 1, right);
	}
}
//merge sort
void algorithm3(int* list,int left,int right) {
	int middle;

	if (left < right) {
		middle = (left + right) / 2;

		algorithm3(list, left, middle);
		algorithm3(list, middle + 1, right);

		merge(list, left, middle, right);

	}
}
//radix sort
void algorithm4(int* list,int n, int* temp) {
	int max_val = 0, min_val = 0, sig_digit = 1;
	int bin_count[10];
	//radix sort for positive numbers and for negative numbers
	int* negative_list = (int*)malloc(sizeof(int)*n);
	int* positive_list = (int*)malloc(sizeof(int)*n);
	int pos_idx = 0, neg_idx = 0;
	int i;

	//separate positive and negative
	for (i = 0; i < n; i++) {
		if (list[i] >= 0) {
			positive_list[pos_idx++] = list[i];
			if (list[i] > max_val) max_val = list[i];
		}
		else {
			negative_list[neg_idx++] = list[i];
			if (list[i] < min_val) min_val = list[i];
		}
	}
	//radix sort for positive
	while (max_val / sig_digit > 0) {
		for (i = 0; i < 10; i++) bin_count[i] = 0;

		for (i = 0; i < pos_idx; i++)
			bin_count[(positive_list[i] / sig_digit) % 10]++;
		for (i = 1; i < 10; i++)
			bin_count[i] += bin_count[i - 1];

		for (i = pos_idx - 1; i >= 0; i--)
			temp[--bin_count[(positive_list[i] / sig_digit) % 10]] = positive_list[i];
		for (i = 0; i < pos_idx; i++) positive_list[i] = temp[i];

		sig_digit *= 10;//go to next significant digit
	}
	//radix sort for negative
	sig_digit = 1;
	while ((-min_val) / sig_digit > 0) {
		for (i = 0; i < 10; i++) bin_count[i] = 0;

		for (i = 0; i < neg_idx; i++)
			bin_count[((-negative_list[i]) / sig_digit) % 10]++;
		for (i = 1; i < 10; i++)
			bin_count[i] += bin_count[i - 1];

		for (i = neg_idx - 1; i >= 0; i--)
			temp[--bin_count[((-negative_list[i]) / sig_digit) % 10]] = negative_list[i];
		for (i = 0; i < pos_idx; i++) negative_list[i] = temp[i];

		sig_digit *= 10;//go to next significant digit
	}
	for (i = 0; i < neg_idx; i++) {
		list[i] = negative_list[neg_idx-1-i];
	}
	for (i = 0; i < pos_idx; i++) {
		list[i + neg_idx] = positive_list[i];
	}
}
void merge(int* list, int left, int middle, int right) {
	int i_left, i_right, i;
	memcpy(temp + left, list + left, sizeof(int)*(right - left + 1));

	i_left = left;
	i_right = middle + 1;
	i = left;

	while ((i_left <= middle) && (i_right <= right)) {
		if (temp[i_left] < temp[i_right])
			list[i++] = temp[i_left++];
		else
			list[i++] = temp[i_right++];
	}

	while (i_left <= middle)
		list[i++] = temp[i_left++];
	while (i_right <= right)
		list[i++] = temp[i_right++];
}
