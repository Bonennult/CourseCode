#include<stdio.h>

//求解环型数组最大子段和，并返回子段最后一个元素下标
int Max_sum(int *a,int n){
	int cur_sum1=0,max_sum1=0;
	int cur_sum2=0,max_sum2=0;
	int t1=0,t2=0,sum=0;
	for(int i=0;i<n;i++){
		sum += a[i];
		//求解非环型数组最大子段和
		if(cur_sum1 < 0)
			cur_sum1 = a[i];
		else
			cur_sum1 += a[i];
		if(cur_sum1 > max_sum1){
			max_sum1 = cur_sum1;
			t1 = i;
		}
		//求解非环型数组最小子段和
		if(cur_sum2 < 0){
			cur_sum2 = -a[i];
			t2 = i;
		}
		else
			cur_sum2 -= a[i];
		if(cur_sum2 > max_sum2)
			max_sum2 = cur_sum2;
	}
	//比较得到环型数组最大子段和
	if(max_sum1 < (sum + max_sum2))
		return (t2 + n -1)%n;
	else
		return t1;
}

int Mario(int *a,int n){
	int start;
	start = 1 + Max_sum(a,n);
	int *current = new int[n+1];    //current[i]存放(当前)含有a[i-1]的k(=1,2,3)段最大子段和
	int *last = new int[n+1];    //last[i]存放(上一轮)截止到a[i-1]的k(=1,2,3)-1段最大子段和
	for(int i=0;i<=n;i++)
		last[i] = 0;
	current[0] = 0;

	for(int i=1;i<=3;i++){
		current[i] = current[i-1] + a[(start+i-1)%n];
//		last[i-1] = current[i];
		int max = current[i];
		for(int j=i+1;j<=n;j++){
			current[j] = current[j-1]>last[j-1] ? current[j-1] : last[j-1];     //不能先更新last[j-1]，因为这里要利用last所存放的上一轮中current[1]~current[j-1]的最大值
			current[j] += a[(start+j-1)%n];
			last[j-1] = max;     //之后的循环中last[j-1]已经没用了，可以更新last，存放目前这一轮中current[1]~current[j-1]的最大值，以便下一轮使用
			if(max < current[j])
				max = current[j];
		}
		last[n] = max;
	}

	int sum=0;
	for(int j=3;j<=n;j++)
		if(sum < current[j])
			sum = current[j];
	printf("%d\n",sum);

	return sum;
}

int main(){
	int n;
	scanf("%d",&n);
	int *money = new int[n];
	for(int i=0;i<n;i++)
		scanf("%d",&money[i]);
	Mario(money,n);
	return 0;
}