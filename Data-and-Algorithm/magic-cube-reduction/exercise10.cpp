#include<stdio.h>
#include<queue>
#include<unordered_map>
using namespace std;

struct note{
	unsigned long long f_node;
	bool q;
	char operation,dep;
	note()  {  f_node = -1;  q = 0;  operation = -1;  dep=0;  };
	note(unsigned long long a,bool b,char c,char d)  {  f_node = a;  q = b;  operation = c;  dep = d;  };
};

//F+
unsigned long long rotate_F(unsigned long long c){
	int a[24],t;
	for(int i=0;i<24;i++){
		a[i] = c % 6;
		c /= 6;
	}
	t = a[6];  a[6] = a[14];  a[14] = a[15];  a[15] = a[7];  a[7] = t;
	t = a[2];  a[2] = a[13];  a[13] = a[21];  a[21] = a[8];  a[8] = t;
	t = a[3];  a[3] = a[5];  a[5] = a[20];  a[20] = a[16];  a[16] = t;
	c = 0;
	unsigned long long temp=1;
	for(int i=0;i<24;i++){
		c += temp * a[i];
		temp *= 6;
	}
	return c;
}
//R+
unsigned long long rotate_R(unsigned long long c){
	int a[24],t;
	for(int i=0;i<24;i++){
		a[i] = c % 6;
		c /= 6;
	}
	t = a[8];  a[8] = a[16];  a[16] = a[17];  a[17] = a[9];  a[9] = t;
	t = a[1];  a[1] = a[7];  a[7] = a[21];  a[21] = a[18];  a[18] = t;
	t = a[3];  a[3] = a[15];  a[15] = a[23];  a[23] = a[10];  a[10] = t;
	c = 0;
	unsigned long long temp=1;
	for(int i=0;i<24;i++){
		c += temp * a[i];
		temp *= 6;
	}
	return c;
}
//U+
unsigned long long rotate_U(unsigned long long c){
	int a[24],t;
	for(int i=0;i<24;i++){
		a[i] = c % 6;
		c /= 6;
	}
	t = a[0];  a[0] = a[2];  a[2] = a[3];  a[3] = a[1];  a[1] = t;
	t = a[4];  a[4] = a[6];  a[6] = a[8];  a[8] = a[10];  a[10] = t;
	t = a[5];  a[5] = a[7];  a[7] = a[9];  a[9] = a[11];  a[11] = t;
	c = 0;
	unsigned long long temp=1;
	for(int i=0;i<24;i++){
		c += temp * a[i];
		temp *= 6;
	}
	return c;
}

//˫������������
void search(unsigned long long start, unsigned long long end){
	if(start == end){
		printf("0\n");
		return ;
	}
	unsigned long long head[2],temp;
	char op;
	char dep[2]={0};       //��¼�����������������
	unordered_map<unsigned long long,struct note> map;
	queue<unsigned long long> que[2];
	que[0].push(start);
	que[1].push(end);
	map.insert(make_pair(start,note(-1,0,-1,0)));
	map.insert(make_pair(end,note(-1,1,-1,0)));
	int t = 0;
	while(!que[0].empty() && !que[1].empty()){
		if(que[0].size() > que[1].size())        //ѡ��϶̵Ķ�������չ
			t = 1;
		else
			t = 0;
		//��t���������չ
		head[t] = que[t].front();
		while(map[head[t]].dep == dep[t]){       //�����ò�����Ԫ��
			que[t].pop();
			temp = rotate_F(head[t]);
			if(!map.count(temp)){      //��״̬֮ǰδ�����ʹ�
				que[t].push(temp);
				map[temp]  = note(head[t],t,0,dep[t]+1);
			}
			else if(map[temp].q == 1-t){      //��������
				//˳��head[t]��������һ�������л��ݣ�˳�����temp��������һ�������л���
				op = 0;
				goto Backtracking;
			}
			temp = rotate_F(temp);
			if(!map.count(temp)){
				que[t].push(temp);
				map[temp]  = note(head[t],t,1,dep[t]+1);
			}
			else if(map[temp].q == 1-t){      //��������
				op = 1;
				goto Backtracking;
			}
			temp = rotate_F(temp);
			if(!map.count(temp)){
				que[t].push(temp);
				map[temp]  = note(head[t],t,2,dep[t]+1);
			}
			else if(map[temp].q == 1-t){      //��������
				op = 2;
				goto Backtracking;
			}
		
			temp = rotate_R(head[t]);
			if(!map.count(temp)){
				que[t].push(temp);
				map[temp]  = note(head[t],t,3,dep[t]+1);
			}
			else if(map[temp].q == 1-t){      //��������
				op = 3;
				goto Backtracking;
			}
			temp = rotate_R(temp);
			if(!map.count(temp)){
				que[t].push(temp);
				map[temp]  = note(head[t],t,4,dep[t]+1);
			}
			else if(map[temp].q == 1-t){      //��������
				op = 4;
				goto Backtracking;
			}
			temp = rotate_R(temp);
			if(!map.count(temp)){
				que[t].push(temp);
				map[temp]  = note(head[t],t,5,dep[t]+1);
			}
			else if(map[temp].q == 1-t){      //��������
				op = 5;
				goto Backtracking;
			}

			temp = rotate_U(head[t]);
			if(!map.count(temp)){
				que[t].push(temp);
				map[temp]  = note(head[t],t,6,dep[t]+1);
			}
			else if(map[temp].q == 1-t){      //��������
				op = 6;
				goto Backtracking;
			}
			temp = rotate_U(temp);
			if(!map.count(temp)){
				que[t].push(temp);
				map[temp]  = note(head[t],t,7,dep[t]+1);
			}
			else if(map[temp].q == 1-t){      //��������
				op = 7;
				goto Backtracking;
			}
			temp = rotate_U(temp);
			if(!map.count(temp)){
				que[t].push(temp);
				map[temp]  = note(head[t],t,8,dep[t]+1);
			}
			else if(map[temp].q == 1-t){      //��������
				op = 8;
				goto Backtracking;
			}
			head[t] = que[t].front();
		}
		dep[t]++;

		//��1-t��������չ
		t = 1-t;
		head[t] = que[t].front();
		while(map[head[t]].dep == dep[t]){
			que[t].pop();
			temp = rotate_F(head[t]);
			if(!map.count(temp)){      //��״̬֮ǰδ�����ʹ�
				que[t].push(temp);
				map[temp]  = note(head[t],t,0,dep[t]+1);
			}
			else if(map[temp].q == 1-t){      //��������
				op = 0;
				goto Backtracking;
			}
			temp = rotate_F(temp);
			if(!map.count(temp)){
				que[t].push(temp);
				map[temp]  = note(head[t],t,1,dep[t]+1);
			}
			else if(map[temp].q == 1-t){      //��������
				op = 1;
				goto Backtracking;
			}
			temp = rotate_F(temp);
			if(!map.count(temp)){
				que[t].push(temp);
				map[temp]  = note(head[t],t,2,dep[t]+1);
			}
			else if(map[temp].q == 1-t){      //��������
				op = 2;
				goto Backtracking;
			}
		
			temp = rotate_R(head[t]);
			if(!map.count(temp)){
				que[t].push(temp);
				map[temp]  = note(head[t],t,3,dep[t]+1);
			}
			else if(map[temp].q == 1-t){      //��������
				op = 3;
				goto Backtracking;
			}
			temp = rotate_R(temp);
			if(!map.count(temp)){
				que[t].push(temp);
				map[temp]  = note(head[t],t,4,dep[t]+1);
			}
			else if(map[temp].q == 1-t){      //��������
				op = 4;
				goto Backtracking;
			}
			temp = rotate_R(temp);
			if(!map.count(temp)){
				que[t].push(temp);
				map[temp]  = note(head[t],t,5,dep[t]+1);
			}
			else if(map[temp].q == 1-t){      //��������
				op = 5;
				goto Backtracking;
			}

			temp = rotate_U(head[t]);
			if(!map.count(temp)){
				que[t].push(temp);
				map[temp]  = note(head[t],t,6,dep[t]+1);
			}
			else if(map[temp].q == 1-t){      //��������
				op = 6;
				goto Backtracking;
			}
			temp = rotate_U(temp);
			if(!map.count(temp)){
				que[t].push(temp);
				map[temp]  = note(head[t],t,7,dep[t]+1);
			}
			else if(map[temp].q == 1-t){      //��������
				op = 7;
				goto Backtracking;
			}
			temp = rotate_U(temp);
			if(!map.count(temp)){
				que[t].push(temp);
				map[temp]  = note(head[t],t,8,dep[t]+1);
			}
			else if(map[temp].q == 1-t){      //��������
				op = 8;
				goto Backtracking;
			}
			head[t] = que[t].front();
		}
		dep[t]++;
	}

	//������״̬������Ѱ·��
	Backtracking:
	int path[11];
	int depth=0;
	if(t){
		switch(op){
		case 0:
		case 1:
		case 2:  path[0] = 2 - op;  break;
		case 3:
		case 4:
		case 5:  path[0] = 8 - op;  break;
		case 6:
		case 7:
		case 8:  path[0] = 14 -op;  break;
		};
		depth++;
		while(temp != start) {      //�����ʼ״̬����
			path[depth] = (int)map[temp].operation;
			temp = map[temp].f_node;
			depth++;
		}
		for(int i=0;i<depth/2;i++){      //�����ݵõ������������Եõ���ȷ��ħ����ԭ˳��
			int k;
			k = path[i];
			path[i] = path[depth-1-i];
			path[depth-1-i] = k;
		}
		temp = head[t];
		while(temp != end) {      //����Ŀ��״̬����
			switch(map[temp].operation){
			case 0:
			case 1:
			case 2:  path[depth] = 2 - map[temp].operation;  break;
			case 3:
			case 4:
			case 5:  path[depth] = 8 - map[temp].operation;  break;
			case 6:
			case 7:
			case 8:  path[depth] = 14 - map[temp].operation;  break;
			};
			temp = map[temp].f_node;
			depth++;
		}
	}

	else{
		path[depth] = (int)op;
		depth++;
		unsigned long long th = head[t];
		while(th != start) {      //�����ʼ״̬����
			path[depth] = (int)map[th].operation;
			th = map[th].f_node;
			depth++;
		}
		for(int i=0;i<depth/2;i++){      //�����ݵõ������������Եõ���ȷ��ħ����ԭ˳��
			int k;
			k = path[i];
			path[i] = path[depth-1-i];
			path[depth-1-i] = k;
		}
		while(temp != end) {      //����Ŀ��״̬����
			switch(map[temp].operation){
			case 0:
			case 1:
			case 2:  path[depth] = 2 - map[temp].operation;  break;
			case 3:
			case 4:
			case 5:  path[depth] = 8 - map[temp].operation;  break;
			case 6:
			case 7:
			case 8:  path[depth] = 14 - map[temp].operation;  break;
			};
			temp = map[temp].f_node;
			depth++;
		}
	}

	//�����ԭ����
	printf("%d\n",depth);
	for(int i=0;i<depth;i++){
		switch(path[i]){
		case 0:  printf("F+\n");  break;
		case 1:  printf("F2\n");  break;
		case 2:  printf("F-\n");  break;
		case 3:  printf("R+\n");  break;
		case 4:  printf("R2\n");  break;
		case 5:  printf("R-\n");  break;
		case 6:  printf("U+\n");  break;
		case 7:  printf("U2\n");  break;
		case 8:  printf("U-\n");  break;
		};
	}
}

int main(){
	unsigned long long temp[24],t=1;
	unsigned long long cubic0=0,cubic1=0;
	for(int i=0;i<24;i++)
		scanf("%llu",&temp[i]);
	//�����ʼ״̬
	for(int i=0;i<24;i++){
		cubic0 += temp[i] * t;
		t *= 6;
	}
	//����Ŀ��״̬
	cubic1 += temp[12] * 15237485424;    //�����
	cubic1 += temp[19] * 710920119942144;    //����
	cubic1 += temp[22] * 946945035976310784;    //����
	if(temp[12] == 1 || temp[12] == 3)          //�Ҳ���
		cubic1 += (4-temp[12]) * 19747781109504;
	else if(temp[12] == 2 || temp[12] == 4)
		cubic1 += (6-temp[12]) * 19747781109504;
	else
		cubic1 += (5-temp[12]) * 19747781109504;
	
	if(temp[19] == 1 || temp[19] == 3)          //ǰ��
		cubic1 += (4-temp[19]) * 548549475264;
	else if(temp[19] == 2 || temp[19] == 4)
		cubic1 += (6-temp[19]) * 548549475264;
	else
		cubic1 += (5-temp[19]) * 548549475264;

	if(temp[22] == 1 || temp[22] == 3)          //����
		cubic1 += (4-temp[22]) * 259;
	else if(temp[22] == 2 || temp[22] == 4)
		cubic1 += (6-temp[22]) * 259;
	else
		cubic1 += (5-temp[22]) * 259;

	search(cubic0,cubic1);

	return 0;
}