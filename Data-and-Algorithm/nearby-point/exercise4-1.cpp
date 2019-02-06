#include<stdio.h>

struct bgNode{
	int x,y;
};

struct seNode{
	int x,y,d;
};

void quickSort(bgNode a[],int downLimit,int upLimit){      //快速排序
	int i,j;
	bgNode t;
	i = downLimit;
	j = upLimit;
	if(i >= j)
		return;
	while(i != j){
		while(a[j].x >= a[downLimit].x && i < j)
			j--;
		while(a[i].x <= a[downLimit].x && i < j)
			i++;
		if(i < j){
			t = a[i];
			a[i] = a[j];
			a[j] = t;
		}
	}
	t = a[downLimit];
	a[downLimit] = a[i];
	a[i] = t;
	quickSort(a,downLimit,i-1);
	quickSort(a,i+1,upLimit);
	return;
}

int main(){
	int n,m;
	int a,b;
	scanf("%d%d",&n,&m);

	bgNode* backNode = (bgNode*)new bgNode[n];
	for(int i=0;i<n;i++){          //读入背景点
		scanf("%d%d",&a,&b);
		backNode[i].x = a;
		backNode[i].y = b;
	}

	quickSort(backNode,0,n-1);

	int x,y,d;
	seNode* searNode = (seNode*)new seNode[m];
	for(int i=0;i<m;i++){         //读入搜索点
		scanf("%d%d%d",&x,&y,&d);
		searNode[i].x = x;
		searNode[i].y = y;
		searNode[i].d = d;
	}

	for(int i=0;i<m;i++){
		int top=n-1,bottom=0,mid;
		while(bottom <= top){         //二分查找当前点搜索范围的下界所在位置
			mid = (top + bottom) / 2;
			if(backNode[mid].x >= (searNode[i].x - searNode[i].d)){
				top = mid-1;
			}
			else{
				bottom = mid+1;
			}
		}

		int count = 0;
		while(bottom<n && backNode[bottom].x <= (searNode[i].x + searNode[i].d)){
			if((backNode[bottom].y <= (searNode[i].y + searNode[i].d)) && (backNode[bottom].y >= (searNode[i].y - searNode[i].d)) && backNode[bottom].x >= (searNode[i].x - searNode[i].d))
				count++;
			bottom++;
		}
		printf("%d\n",count);
	}

	return 0;
}