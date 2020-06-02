package Compiling_1stexp;

import java.lang.System;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;



public class MergeSort {
	private static BufferedReader br;

	public static void main(String[] args) throws IOException {
		br = new BufferedReader(new FileReader("F:\\vscodework\\Compiling\\test100000.txt"));
		String s = "";
		String s1;
		while((s1 = br.readLine()) != null) {
			s = s + s1;
		}
		String [] data =s.split(" ");
		int [] datas = new int [data.length];
		for(int j = 0; j < data.length; j++) {
			datas[j] = Integer.parseInt(data[j]);
		}
		
		//获取时间
		long startTime = System.nanoTime();
		mergeSort(datas);
		long endTime = System.nanoTime();
		
		System.out.println("Data Size: 100000\nTime: " + (double)(endTime - startTime)/1000000 + "ms");
		
	}
	
	public static void mergeSort(int[] data) {
		sort(data, 0, data.length-1);
	}
	
	public static void sort(int[] data, int left, int right) {
		if(left < right) {
			int i = (left + right) / 2;
			
			sort(data, left, i);
			sort(data, i+1, right);
			
			merge(data, left, i, right);
		}
	}
	
	public static void merge(int[] data, int left, int m, int right) {
		int[] aux = new int[data.length];
		int i, j, k;
		
		for(i = left, j = m+1, k = 0; k <= right - left; k++) {
			if (i == m+1) {
				aux[k] = data[j++];
				continue;
			}
			if (j == right+1) {
				aux[k] = data[i++];
				continue;
			}
			if(data[i] < data[j]) {
				aux[k] = data[i++];
			}
			else {
				aux[k] = data[j++];
			}
		}
		
		for(i = left, j = 0; i <= right; i++, j++) {
			data[i] = aux[j];
		}
	}

}
