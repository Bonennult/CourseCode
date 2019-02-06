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

	for(int i=0;i<L;i++){       //����Ƽ���ʶ���˵ľ���
		for(int j=0;j<L;j++){
			if(j != i && vectMult(adjUser,i,j,L) >= 3 && !adjUser[i*L+j])
				printf("1 ");
			else
				printf("0 ");
		}
		printf("\n");
	}

	bool* isVisited = new bool[L]();     //�������ڼ�¼�õ��Ƿ񱻷��ʹ�
	Stack epath(L);     //·��ջ

	isVisited[a] = 1;
	epath.push(a);
	while(epath.Length != 0){
		temp = epath.path[epath.Length-1];
		int i = 0;
		while(i<L && (!adjUser[temp*L+i] || isVisited[i]))   //Ѱ��temp��δ�����ʹ����ڽӶ���
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