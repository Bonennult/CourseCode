#include<stdio.h>

//��⻷����������Ӷκͣ��������Ӷ����һ��Ԫ���±�
int Max_sum(int *a,int n){
	int cur_sum1=0,max_sum1=0;
	int cur_sum2=0,max_sum2=0;
	int t1=0,t2=0,sum=0;
	for(int i=0;i<n;i++){
		sum += a[i];
		//���ǻ�����������Ӷκ�
		if(cur_sum1 < 0)
			cur_sum1 = a[i];
		else
			cur_sum1 += a[i];
		if(cur_sum1 > max_sum1){
			max_sum1 = cur_sum1;
			t1 = i;
		}
		//���ǻ���������С�Ӷκ�
		if(cur_sum2 < 0){
			cur_sum2 = -a[i];
			t2 = i;
		}
		else
			cur_sum2 -= a[i];
		if(cur_sum2 > max_sum2)
			max_sum2 = cur_sum2;
	}
	//�Ƚϵõ�������������Ӷκ�
	if(max_sum1 < (sum + max_sum2))
		return (t2 + n -1)%n;
	else
		return t1;
}

int Mario(int *a,int n){
	int start;
	start = 1 + Max_sum(a,n);
	int *current = new int[n+1];    //current[i]���(��ǰ)����a[i-1]��k(=1,2,3)������Ӷκ�
	int *last = new int[n+1];    //last[i]���(��һ��)��ֹ��a[i-1]��k(=1,2,3)-1������Ӷκ�
	for(int i=0;i<=n;i++)
		last[i] = 0;
	current[0] = 0;

	for(int i=1;i<=3;i++){
		current[i] = current[i-1] + a[(start+i-1)%n];
//		last[i-1] = current[i];
		int max = current[i];
		for(int j=i+1;j<=n;j++){
			current[j] = current[j-1]>last[j-1] ? current[j-1] : last[j-1];     //�����ȸ���last[j-1]����Ϊ����Ҫ����last����ŵ���һ����current[1]~current[j-1]�����ֵ
			current[j] += a[(start+j-1)%n];
			last[j-1] = max;     //֮���ѭ����last[j-1]�Ѿ�û���ˣ����Ը���last�����Ŀǰ��һ����current[1]~current[j-1]�����ֵ���Ա���һ��ʹ��
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