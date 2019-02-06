#include<stdio.h>

int Mario(int *a,int len,int *jump){
	int cur_sum=0,max_sum=0;
	int max_sum1=0,max_sum2=0;
	int r_limit=len,l_limit=-1;
	//确定从a[len-1]开始的子数组中，和最大的子数组的下界r_limit
	for(int i=len-1;i>=0;i--){
		cur_sum += a[i];        //cur_sum记录a[i]之后的数组元素之和
		if(cur_sum < 0)
			break;
		else if(cur_sum > max_sum1){
			max_sum1 = cur_sum;
			r_limit = i;
		}
	}
	jump[0] = r_limit;    //第一个要跳的点
	max_sum += max_sum1;
//	printf("%d\n",max_sum1);

	//确定从a[0]开始的子数组中，和最大的子数组的上界jump[1]
	cur_sum = max_sum1 = 0;
	for(int i=0;i<r_limit;i++){
		cur_sum += a[i];
		if(cur_sum >= max_sum1){
			max_sum1 = cur_sum;
			l_limit = i;
		}
		else if(cur_sum < 0)
			break;
	}
	jump[1] = l_limit;
	max_sum += max_sum1;
//	printf("%d\n",max_sum1);

	//jump[1]与r_right之间只取一个子数组使和最大
	cur_sum = max_sum1 = 0;
	int border1[2]={jump[1]};
	int temp1;
	for(int i=jump[1];i<r_limit;i++){
		cur_sum += a[i];
		if(cur_sum > max_sum1){
			max_sum1 = cur_sum;
			border1[0] = temp1;
			border1[1] = i;
		}
		else if(cur_sum < 0){
			cur_sum = 0;
			temp1 = i+1;
		}
	}
	if(max_sum1 == 0){        //全为负数的情况
		max_sum1 = jump[1];
		for(int i=jump[1]+1;i<r_limit;i++)
			if(a[i] > max_sum1){
				max_sum1 = a[i];
				border1[0] = border1[1] = i;
			}
	}
//	printf("%d\n",max_sum1);

	//jump[1]与r_right之间取两个子数组使和最大
	int *sum1 = new int[r_limit - jump[1]];
	int *sum2 = new int[r_limit - jump[1]];
	int *border3 = new int[r_limit - jump[1]];
	int *border4 = new int[r_limit - jump[1]];
	int *border5 = new int[r_limit - jump[1]];
	int *border6 = new int[r_limit - jump[1]];
	int temp_border=jump[1];
	cur_sum = sum1[0] = a[jump[1]];
	for(int i=jump[1]+1 ; i<r_limit ; i++){
		if(cur_sum < 0){            //cur_sum一定是含有当前a[i]元素的最大子数组和
			cur_sum = a[i];
			temp_border = i;
		}
		else
			cur_sum += a[i];
		if(cur_sum > sum1[i - 1 - jump[1]]){
			sum1[i - jump[1]] = cur_sum;
			border3[i - jump[1]] = temp_border;
			border4[i - jump[1]] = i;
		}
		else{
			sum1[i - jump[1]] = sum1[i - 1 - jump[1]];
			border3[i - jump[1]] = border3[i - jump[1] - 1];
			border4[i - jump[1]] = border4[i - jump[1] - 1];
		}
	}
	cur_sum = sum2[r_limit - jump[1] - 1] = a[r_limit -1];
	for(int i=r_limit -2 ; i>=jump[1] ; i--){
		if(cur_sum < 0){            //cur_sum一定是含有当前a[i]元素的最大子数组和
			cur_sum = a[i];
			temp_border = i;
		}
		else
			cur_sum += a[i];
		if(cur_sum > sum2[i + 1 - jump[1]]){
			sum2[i - jump[1]] = cur_sum;
			border6[i - jump[1]] = temp_border;
			border5[i - jump[1]] = i;
		}
		else{
			sum2[i - jump[1]] = sum2[i + 1 - jump[1]];
			border6[i - jump[1]] = border6[i - jump[1] + 1];
			border5[i - jump[1]] = border5[i - jump[1] + 1];
		}
	}
	max_sum2 = sum1[0] + sum2[1];
	int flag=0;
	for(int i=1 ; i<r_limit - jump[1] -1 ; i++){
		if((sum1[i] + sum2[i+1]) > max_sum2){
			max_sum2 = sum1[i] + sum2[i+1];
			flag = i;
		}
	}
//	printf("%d\n",max_sum2);

	if(max_sum1 > max_sum2){
		max_sum += max_sum1;
		if(jump[1]+1 + len-jump[0] + border1[1]-border1[0]+1 == 2){    //目前Mario只经过了2个驿站
			int t=a[jump[1]+1];       //这里一定有jump[1]+1<border1[0]
			for(int i=jump[1]+2;i<border1[0];i++){
				if(a[i] > t)
					t = a[i];
			}
			for(int i=border1[1]+1;i<jump[0];i++){
				if(a[i] > t)
					t = a[i];
			}
			max_sum += t;
		}
		else if(jump[1]+1 + len-jump[0] + border1[1]-border1[0]+1 == 1){      //目前Mario只经过了1个驿站
			int t=a[jump[1]+1],k=jump[1]+1;       //这里一定有k<border1[0]
			for(int i=jump[1]+2;i<border1[0];i++){
				if(a[i] > t){
					k = i;
					t = a[i];
				}
			}
			for(int i=border1[1]+1;i<jump[0];i++){
				if(a[i] > t){
					k = i;
					t = a[i];
				}
			}
			max_sum += t;
			t = -1000;
			for(int i=jump[1]+1;i<border1[0];i++){
				if(i == k)
					continue;
				if(a[i] > t)
					t = a[i];
			}
			for(int i=border1[1]+1;i<jump[0];i++){
				if(i == k)
					continue;
				if(a[i] > t)
					t = a[i];
			}
			max_sum += t;
		}
	}
	else{
		max_sum += max_sum2;
		if(jump[1]+1 + len-jump[0] + border4[flag]-border3[flag]+1 + border6[flag]-border5[flag]+1 == 2){    //目前Mario只经过了2个驿站
			int t=a[jump[1]+1];       //这里一定有jump[1]+1<border3[flag]
			for(int i=jump[1]+2;i<border3[flag];i++){
				if(a[i] > t)
					t = a[i];
			}
			for(int i=border4[flag]+1;i<border5[flag];i++){
				if(a[i] > t)
					t = a[i];
			}
			for(int i=border6[flag]+1;i<jump[0];i++){
				if(a[i] > t)
					t = a[i];
			}
			max_sum += t;
		}
	}
	return max_sum;
}

int main(){
	int n,sum;
	scanf("%d",&n);
	int *money=new int[n];
	for(int i=0;i<n;i++)
		scanf("%d",&money[i]);

	int jump[3] = {0};
	sum = Mario(money,n,jump);
	printf("%d\n",sum);
	return 0;
}