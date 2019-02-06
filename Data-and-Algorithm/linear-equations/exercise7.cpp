#include<cstdio>
#include<cmath>
#include<iostream>
using namespace std;

struct Element{
	int row,col;
	double num;
	Element* pRow;
	Element() { num=0; row=col=0; pRow=NULL; };
};

class crossList{        //十字链表类
private:
	int row,col;
public:
	Element *HRow;
	crossList(int r,int c)  {  row = r;  col = c;  HRow = NULL;  }
	~crossList();
	void crossListSetup(int);
	void innerMult(double*,double*);
	void crossListInsert(int,int,double);
	void Gauss();
};

crossList::~crossList(){
	Element* p;
	for(int i=0;i<row;i++){        //释放申请的节点内存
		while(HRow[i].pRow != NULL){
			p = HRow[i].pRow;
			HRow[i].pRow = p->pRow;
			delete p;
		}
	}
	delete []HRow;
}

void crossList::crossListSetup(int num){      //十字链表的读入和建立函数
	int r=0,c=0,count=0;
	double v=0;
	if(num == 0) return;
	Element *p,*q,*temp=(Element*)new Element();
	HRow = new Element[row];       //为各行各列的表头结点申请内存
	do{            //先读入第一个非零数据
		scanf("%d%d%lf",&r,&c,&v);
		temp->row = r;
		temp->col = c;
		temp->num = v;
		count++;
	}while(v == 0 && count<num);
	HRow[r].pRow = temp;
	temp = NULL;
	for(int i=0;i<num-count;i++){       //读入各个节点数据
		scanf("%d%d%lf",&r,&c,&v);
		if(v == 0)  continue;            //v为0则直接跳过
		temp = (Element*)new Element();
		temp->num = v;
		temp->row = r;
		temp->col = c;

		p = &HRow[r];
		while(p->pRow != NULL){       //将新申请的节点正确的链接到行中
			if(p->pRow->col > c){
				temp->pRow = p->pRow;
				p->pRow = temp;
				break;
			}
			p = p->pRow;
		}
		p->pRow = temp;
	}
}

void crossList::innerMult(double* vect,double*result){       //求十字链表矩阵与给定向量的积
	Element* p;
	for(int i=0;i<row;i++)
		result[i] = 0;
	for(int i=0;i<row;i++){
		p = HRow[i].pRow;
		while(p != NULL){
			result[i] += vect[p->col] * p->num;
			p = p->pRow;
		}
	}
}

int main(){
	int rank,k;
	scanf("%d%d",&rank,&k);
	if(rank < 3000){            //矩阵较小时采用高斯消元法
		int r,c;
		double v;
		double* mA = (double*)new double[rank*rank];
		double* B = (double*)new double[rank];
		double* X = (double*)new double[rank];
		for(int i=0;i<rank*rank;i++)
			mA[i] = 0;
		for(int i=0;i<k;i++){
			scanf("%d%d%lf",&r,&c,&v);
			mA[r * rank + c] = v;
		}
		for(int i=0;i<rank;i++)
			scanf("%lf",&B[i]);

		for(int i=0;i<rank-1;i++){
			double pivot_b=0,pivot_a=0;        //列选主元
			int pivot_r=i;
			for(int t=i+1;t<rank;t++){
				if(abs(mA[t*rank + i]) > abs(mA[pivot_r*rank + i]))
					pivot_r = t;
			}
			if(pivot_r != i){
				pivot_b = B[i];
				B[i] = B[pivot_r];
				B[pivot_r] = pivot_b;
				for(int t=i;t<rank;t++){
					pivot_a = mA[i * rank + t];
					mA[i * rank + t] = mA[pivot_r * rank +t];
					mA[pivot_r * rank + t] = pivot_a;
				}
			}

			for(int j=i+1;j<rank;j++){          //高斯消元
				double factor = mA[j * rank + i] / mA[i * rank + i];
				mA[j*rank + i] = 0;
				B[j] -= B[i] * factor;
				for(int k=i+1;k<rank;k++)
					mA[j * rank + k] -= mA[i * rank + k] * factor;
			}
		}

		for(int i=rank-1;i>=0;i--){         //回代过程
			X[i] = B[i];
			for(int j=rank-1;j>i;j--)
				X[i] -= mA[i*rank + j] * X[j];
			X[i] /= mA[i*rank + i];
		}

		for(int i=0;i<rank;i++)
			printf("%.20lf\n",X[i]);

		delete[]B;
		delete[]X;
		delete[]mA;
		return 0;
	}

	else{            //矩阵较大时采用高斯-塞德尔迭代求解
		crossList matA(rank,rank);
		matA.crossListSetup(k);
		double* b = new double[rank];
		for(int i=0;i<rank;i++)
			scanf("%lf",&b[i]);
	
		double residual=1;
		double* x = new double[rank];
		double* mid=new double[rank];
		for(int i=0;i<rank;i++)        //设置迭代初值
			x[i] = 0;

		while(residual >= 0.01){
			for(int i=0;i<rank;i++){
				double dig;
				x[i] = b[i];
				Element* p= matA.HRow[i].pRow;
				while(p!=NULL && p->col < p->row){      //迭代计算Xk+1
					x[i] -= (p->num) * x[p->col];
					p = p->pRow;
				}
				dig = p->num;      //记下对角元素方便后面计算
				p = p->pRow;
				while(p!=NULL){       //此时若p不是空，一定有p->col > p->row
					x[i] -= (p->num) * x[p->col];
					p = p->pRow;
				}
				x[i] /= dig;
			}

			matA.innerMult(x,mid);     //计算残差
			residual = 0;
			for(int i=0;i<rank;i++)
				residual += (b[i] - mid[i]) * (b[i] - mid[i]);
		}

		for(int i=0;i<rank;i++)
			printf("%.20lf\n",x[i]);

		delete[] b;
		delete[] x;
		delete[] mid;
		return 0;
	}
}