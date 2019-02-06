#include<stdio.h>

void quickSort(int a[],int downLimit,int upLimit){      //快速排序
	int i,j;
	int t;
	i = downLimit;
	j = upLimit;
	if(i >= j)
		return;
	while(i != j){
		while(a[j] >= a[downLimit] && i < j)
			j--;
		while(a[i] <= a[downLimit] && i < j)
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

int by_Search_lower(int* a,int left,int right,int e){         //二分查找第一个小于e的元素
	int l=left,r=right,mid;
	while(l<=r){
		mid=(l+r)/2;
		if(a[mid]>=e)
			r = mid-1;
		else
			l = mid+1;
	}
	return r;
}

int by_Search_upper(int* a,int left,int right,int e){         //二分查找第一个大于e的元素
	int l=left,r=right,mid;
	while(l<=r){
		mid=(l+r)/2;
		if(a[mid]>e)
			r = mid-1;
		else
			l = mid+1;
	}
	return l;
}

int main(){
	int len,down,up;
	scanf("%d%d%d",&len,&down,&up);
	int* data = new int[len];
	long long count=0;
	for(int i=0;i<len;i++)       //读入数据并排序
		scanf("%d",&data[i]);
	quickSort(data,0,len-1);

	for(int i=0;i<len-2 && data[i] <= up;i++){
		int j = i+1,d1,d2;
		d1 = by_Search_lower(data,j+1,len-1,down-data[i]-data[j]) + 1;
		d2 = by_Search_upper(data,j+1,len-1,up-data[i]-data[j]) - 1;
		count += d2-d1+1;
		j++;
		while(j < len-1){
			d1 = d1>j ? d1:(j+1);
			while(d1>j+1 && data[i]+ data[j] + data[d1-1] >= down){
				d1--;
			}
			if(d2<j)
				break;
			while(d2>j && data[i]+ data[j] + data[d2] > up){
				d2--;
			}
			count += d2-d1+1;
			j++;
		}
	}

	printf("%lld\n",count);
	return 0;
}