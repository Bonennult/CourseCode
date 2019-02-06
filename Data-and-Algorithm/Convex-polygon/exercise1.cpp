#include<iostream>
using namespace std;

struct PrintOut{
	int num;
	struct PrintOut* Next;
};

/*bool isOut(double xPos[],double yPos[],int sides,int i){        //�жϵ�p[i]�Ƿ�Ϊ͹���ĵ�
	int iLast = (i+sides-1) % sides;
	int iNext = (i+sides+1) % sides;
	if((xPos[iNext] - xPos[iLast]) > 0)
		return ((yPos[i] - yPos[iLast] - (yPos[iNext] - yPos[iLast]) * (xPos[i] - xPos[iLast]) /  (xPos[iNext] - xPos[iLast])) >= 0 ? 1:0);
	else if((xPos[iNext] - xPos[iLast]) < 0)
		return ((yPos[i] - yPos[iLast] - (yPos[iNext] - yPos[iLast]) * (xPos[i] - xPos[iLast]) /  (xPos[iNext] - xPos[iLast])) >= 0 ? 0:1);
	else{
		if((yPos[iNext] - yPos[iLast]) > 0)
			return (xPos[i] < xPos[iLast] ? 1:0);
		else
			return (xPos[i] < xPos[iLast] ? 0:1);
	}
}*/

bool isOut(double xPos[],double yPos[],int sides,int i){        //�жϵ�p[i]�Ƿ�Ϊ͹���ĵ�
	int iLast = (i+sides-1) % sides;
	int iNext = (i+sides+1) % sides;
	double OutMul;      //�����p[i]���������ߵĲ��
	OutMul = (xPos[iLast] - xPos[i]) * (yPos[iNext] - yPos[i]) - (yPos[iLast] - yPos[i]) * (xPos[iNext] - xPos[i]);
	return (OutMul > 0);
}

int main(){
	int K;
	cin>>K;             //�������εĸ���K
	int* SidesNum=new int[K];
	struct PrintOut *Head,*Current,*After;
	Head = Current = NULL;
	for(int i=0;i<K;i++){
		cin>>SidesNum[i];               //�����������εı������䶥������
		double* PolyX=new double[SidesNum[i]];
		double* PolyY=new double[SidesNum[i]];
		for(int j=0;j<SidesNum[i];j++){
			cin>>PolyX[j]>>PolyY[j];
		}
		bool flag=1;                       //�жϵ�ǰ����һ���������γɵĶ�����Ƿ�Ϊ͹�����
		for(int j=0;j<SidesNum[i];j++){
			if(!isOut(PolyX,PolyY,SidesNum[i],j)){
				flag = 0;
				if(Head == NULL){               //���������洢Ҫ���������
					After = (PrintOut*)new PrintOut;
					After->num = j;
					After->Next = NULL;
					Head = Current = After;
				}
				else{
					After = (PrintOut*)new PrintOut;
					After->num = j;
					After->Next = NULL;
					Current = Current->Next = After;
				}
			}
		}
		if(flag){
			if(Head == NULL){
				After = (PrintOut*)new PrintOut;
				After->num = -1;
				After->Next = NULL;
				Head = Current = After;
			}
			else{
				After = (PrintOut*)new PrintOut;
				After->num = -1;
				After->Next = NULL;
				Current = Current->Next = After;
			}
		}
		delete[] PolyX;
		delete[] PolyY;
	}
	Current = Head;
	while(Current->Next != NULL){
		if(Current->num == -1)
			cout<<'y'<<endl;
		else{
			if((Current->Next)->num == -1)
				cout<<Current->num<<endl;
			else
				cout<<Current->num<<' ';
		}
		Current = Current->Next;
	}
	if(Current->num == -1)
		cout<<'y'<<endl;
	else
		cout<<Current->num<<endl;
	delete[] SidesNum;
	return 0;
}