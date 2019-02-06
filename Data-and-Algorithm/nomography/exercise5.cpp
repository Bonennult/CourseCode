#include<stdio.h>

int vectMult(bool*a,int m, int n,int length){
	int innerMult=0;
	for(int i=0;i<length;i++)
		innerMult += (int)a[m*length+i] * (int)a[n*length+i];
	return innerMult;
}

class Stack{
private:
	int Size;
public:
	int Length;
	int* path;
	Stack()  {  Size = Length = 0;  path = NULL;  };
	Stack(int);
	~Stack()  {  if(path)  delete path;  }
	int pop();
	void push(int);
};

Stack::Stack(int s){
	Size = s;
	path = new int[s];
	Length = 0;
}

int Stack::pop(){
	if(Length)
		return path[--Length];
	else
		return NULL;
}

void Stack::push(int v){
	if(Length<Size){
		path[Length++] = v;
	}
}

int main(){
	int L,temp=0,a,b;
	scanf("%d",&L);
	bool* adjUser = new bool[L*L];
	for(int i=0;i<L;i++)
		for(int j=0;j<L;j++){
			scanf("%d",&temp);
			adjUser[i*L+j] = (bool)temp;
		}
	scanf("%d%d",&a,&b);

	for(int i=0;i<L;i++){       //输出推荐认识的人的矩阵
		for(int j=0;j<L;j++){
			if(j != i && vectMult(adjUser,i,j,L) >= 3 && !adjUser[i*L+j])
				printf("1 ");
			else
				printf("0 ");
		}
		printf("\n");
	}

	bool* isVisited = new bool[L]();     //数组用于记录该点是否被访问过
	Stack epath(L);     //路径栈

	isVisited[a] = 1;
	epath.push(a);
	while(epath.Length != 0){
		temp = epath.path[epath.Length-1];
		int i = 0;
		while(i<L && (!adjUser[temp*L+i] || isVisited[i]))   //寻找temp点未被访问过的邻接顶点
			i++;
		if(i == L){
			epath.pop();
			continue;
		}
		if(i == b){
			for(int j=0;j<epath.Length;j++)
				printf("%d ",epath.path[j]);
			printf("%d\n",b);
			return 0;
		}
		else{
			isVisited[i] = 1;
			epath.push(i);
		}
	}
	printf("-1\n");
	return 0;
}