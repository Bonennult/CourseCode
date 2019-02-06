#include<iostream>
#include<cmath>
using namespace std;

int main(){
	 int L;
	 scanf("%d",&L);
	 int len= (int)pow(2,L),delt=1;
	 float *F=new float[len];
	 for(int i=0;i<len;i++)
		 scanf("%f",&F[i]);
	 len /= 2;
	 for(int i=0;i<L;i++){
		 for(int j=0;j<len;){
			 F[2*j] = (F[2*j] + F[2*j+delt]) / 2;
			 F[2*j+delt] -= F[2*j];
			 j += delt;
		 }
		 delt *= 2;     //步长变为2倍
	 }
	 len *= 2;
	 printf("%.4f ",F[0]);
	 for(int i=0;i<L;i++){
		 for(int j=delt/2;j<len;){
			 printf("%.4f ",F[j]);
			 j += delt;
		 }
		 delt /= 2;
	 }
	 delete[] F;
	 return 0;
}