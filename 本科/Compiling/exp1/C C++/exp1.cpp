// 编译原理实验一 归并排序
// 2018-02-27

#include<cstdlib>
#include<cstdio>
#include<ctime>
using namespace std;

const int M = 50010;

int Array[M];
// 归并排序中的合并算法
void Merge(int array[], int left, int m, int right) {
	int aux[M];
	int i, j, k;
	for (i = left, j = m + 1, k = 0; k <= right - left; k++) {
		if (i == m + 1) {
			aux[k] = array[j++];
			continue;
		}
		if (j == right + 1) {
			aux[k] = array[i++];
			continue;
		}
		if (array[i] < array[j]) {
			aux[k] = array[i++];
			continue;
		}
		else {
			aux[k] = array[j++];
		}
	}
	for (i = left, j = 0; i <= right; i++, j++) {
		array[i] = aux[j];
	}
}

// 归并排序
void MergeSort(int array[], int start, int end) {
	if (start < end) {
		int i = (start + end) / 2;
		MergeSort(array, start, i);
		MergeSort(array, i + 1, end);
		Merge(array, start, i, end);
	}

}

int main() {
	FILE * fp = fopen("F:\\vscodework\\Compiling\\test50000.txt", "r");
	int i = 0;
	while (fscanf(fp, "%d", &Array[i]) != EOF) {i++;}

	clock_t start, end;
	start = clock();
	MergeSort(Array, 0, i - 1);
	end = clock();
	printf("Data Size: %d\nTime: %f ms\n", i, (double)(end - start));

	system("pause");
	return 0;
}