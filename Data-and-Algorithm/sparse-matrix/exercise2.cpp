#include<iostream>
using namespace std;

struct Element{
	int num,row,col;
	Element* pRow;
	Element* pCol;
	Element() { num=row=col=0; pRow=NULL; pCol=NULL; };
};

int main(){
	int m=0,n=0,l=0;
	int r=0,c=0,v=0,count=0;
	scanf("%d%d%d",&m,&n,&l);
	if(l == 0) return 0;
	Element *p,*q,*temp=(Element*)new Element();
	Element *HRow=new Element[m],*HCol=new Element[n];    //定义各行各列的表头结点
	do{          //先读入第一个非零数据
		scanf("%d%d%d",&r,&c,&v);
		temp->row = r;
		temp->col = c;
		temp->num = v;
		count++;
	}while(v == 0 && count<l);
	HRow[r].pRow = HCol[c].pCol = temp;
	temp = NULL;
	for(int i=0;i<l-count;i++){       //读入各个节点数据
		scanf("%d%d%d",&r,&c,&v);
		if(v == 0)  continue;            //v为0则直接跳过
		temp = (Element*)new Element();
		temp->num = v;
		temp->row = r;
		temp->col = c;

		p = &HRow[r];
		q = &HCol[c];
		while(p->pRow!=NULL){       //将新申请的节点正确的链接到行中
			if(p->pRow->col > c){
				temp->pRow = p->pRow;
				p->pRow = temp;
				break;
			}
			p = p->pRow;
		}
		p->pRow = temp;
		while(q->pCol!=NULL){        //将新申请的节点正确的链接到列中
			if(q->pCol->row > r){
				temp->pCol = q->pCol;
				q->pCol = temp;
				break;
			}
			q = q->pCol;
		}
		q->pCol = temp;
		temp = NULL;
	}

	for(int i=0;i<n;i++){         //逐列打印矩阵转置
		q = HCol[i].pCol;
		while(q != NULL){
			printf("%d %d %d\n",q->col,q->row,q->num);
			q = q->pCol;
		}
	}
	for(int i=0;i<m;i++){            //打印输入矩阵和其转置矩阵的和矩阵
		p = HRow[i].pRow;
		q = HCol[i].pCol;
		while(q!=NULL && p!=NULL){
			if(p->col < q->row){
				printf("%d %d %d\n",p->row,p->col,p->num);
				p = p->pRow;
			}
			else if(p->col > q->row){
				printf("%d %d %d\n",q->col,q->row,q->num);
				q = q->pCol;
			}
			else{
				if(p->num + q->num)
					printf("%d %d %d\n",p->row,p->col,p->num + q->num);
				p = p->pRow;
				q = q->pCol;
			}
		}
		while(p != NULL){
			printf("%d %d %d\n",p->row,p->col,p->num);
			p = p->pRow;
		}
		while(q != NULL){
			printf("%d %d %d\n",q->col,q->row,q->num);
			q = q->pCol;
		}
	}
	for(int i=0;i<m;i++){            //打印输入矩阵和其转置矩阵的乘积矩阵
		for(int j=0;j<n;j++){
			p = HRow[i].pRow;
			q = HRow[j].pRow;
			int sum=0;
			while(q!=NULL && p!=NULL){
				if(p->col < q->col)
					p = p->pRow;
				else if(p->col > q->col)
					q = q->pRow;
				else{
					sum += (p->num) * (q->num);
					p = p->pRow;
					q = q->pRow;
				}
			}
			if(sum)
				printf("%d %d %d\n",i,j,sum);
		}
	}

	for(int i=0;i<m;i++){        //释放申请的节点内存
		while(HRow[i].pRow!=NULL){
			p = HRow[i].pRow;
			HRow[i].pRow = p->pRow;
			delete p;
		}
	}
	delete []HRow;
	delete []HCol;
	return 0;
}