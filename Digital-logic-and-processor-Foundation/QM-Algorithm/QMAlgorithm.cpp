#include<iostream>
#include<vector>
#include<algorithm>
#include<bitset>
#include<queue>
#include<string.h>
#include<stdio.h>
using namespace std;

//蕴含项类
class Implicant{
public:
	int size,varnum;    //分别存放当前蕴含项的大小，变量数
	int* index;             //存放当前蕴含项所包含的最小项
	char* bistr;           //存放当前蕴含项对应的二进制表示
	char code;           //用于寻找最小覆盖时存放蕴含项的编号
	bool isPrimeImplicant,isEssentialPrimeImp;     //是否为本原蕴含项或本质本原蕴含项
	Implicant* next;

	Implicant()  {  size = 0;  index = NULL;  bistr = NULL;  code = '\0';  isPrimeImplicant=1;  isEssentialPrimeImp = 0;  next = NULL;  }
	Implicant(int s,int vn)  {  size = s;  varnum = vn;  index = new int[size];  bistr = new char[varnum+1];  bistr[varnum] = '\0';  code = '\0';  isPrimeImplicant=1;  isEssentialPrimeImp = 0;  next = NULL;  }
	int compare(Implicant*);
	void append(Implicant*&,Implicant*&);
	void display(char*);
	Implicant& operator=(Implicant&);
	bool operator==(Implicant&);
};

int Implicant::compare(Implicant* imp){    //比较两个蕴含项是否只相差一个比特位，返回不同的比特位的序号
	int count=0,index=0;
	for(int i=0;i<varnum;i++)
		if(this->bistr[i] != imp->bistr[i]){
			index = i;
			count++;
		}
	if(count > 1)
		return -1;
	else if(count == 0)     //防止出现两个合并项完全一样！！
		return -2;
	else
		return  index;
}

void Implicant::append(Implicant* &head,Implicant* &cur){           //将当前蕴含项链接到指定链表中
	if(!head)
		head = cur = this;
	else{
		Implicant* p = head;
		while(p){
			if(this->compare(p) == -2)    //如果当前合并的蕴含项在之前已经出现过，则不再重复链接
				return ;
			p = p->next;
		}
		cur->next = this;
		cur = this;
	}
}

void Implicant::display(char* out){
	bool flag=1;
	int l = strlen(out);
	for(int i=0;i<varnum;i++){
		if(bistr[i] == '1'){
			flag = 0;
			out[l] = 'A'+i;
			out[++l] = '\0';
		}
		else if(bistr[i] == '0'){
			flag = 0;
			out[l] = 'A'+i;
			out[++l] = '\'';
			out[++l] = '\0';
		}
	}
	if(flag){        //卡诺图输出仅包括1和无关项
		out[l] = '1';
		out[++l] = '\0';
	}
}

Implicant& Implicant::operator=(Implicant& a){
	this->size = a.size;
	this->varnum = a.varnum;
	index = new int[size];
	bistr = new char[varnum+1];
	bistr[varnum]  = '\0';
	this->isEssentialPrimeImp = a.isEssentialPrimeImp;
	this->isPrimeImplicant = a.isPrimeImplicant;
	for(int i=0;i<size;i++)
		this->index[i] = a.index[i];
	for(int i=0;i<varnum;i++)
		this->bistr[i] = a.bistr[i];
	this->next = NULL;     //next指针置为NULL
	return *this;
}

bool Implicant::operator==(Implicant& a){        //比较两个蕴含项是否相同
	bool flag = 1;
	for(int i=0;i<this->varnum;i++)
		if(this->bistr[i] != a.bistr[i])
			flag = 0;
	return flag;
}

//寻找数组中某个元素对应的下标
int Index(int* a,int n,int e){
	for(int i=0;i<n;i++)
		if(a[i] == e)
			return i;
	return -1;
}

//专门的比大小函数
bool QM_compare(Implicant a,Implicant b){
	//保证本质本原蕴含项在本原蕴含项之前
	if(a.isEssentialPrimeImp == 1 && b.isEssentialPrimeImp == 0)
		return true;
	else if(a.isEssentialPrimeImp == 0 && b.isEssentialPrimeImp == 1)
		return false;
	for(int i=0;i<a.varnum;i++){
		if(a.bistr[i] > b.bistr[i])
			return true;
		else if(a.bistr[i] < b.bistr[i])
			return false;
	}
}

//用于petrick方法的字符串合并
char* merge(char* a,int n,char b){
	char* c = new char[n];
	for(int i=0;i<n;i++)
		c[i] = a[i];
	int i=0;
	while(c[i] != '\0'){
		if(b == c[i])
			return c;
		i++;
	}
	c[i] = b;
	c[++i] = '\0';
	return c;
}

//QM算法实现
void QM_Algorithm(int Nm,int Nd,char* out){
	int* m = new int[Nm];
	int* d = new int[Nd];
	bitset<10>* mbit = new bitset<10>[Nm];
	bitset<10>* dbit = new bitset<10>[Nd];
	Implicant* PrimeImplicant = NULL, *PrimeTemp = NULL;       //本原蕴含项链表的头指针和临时指针
	Implicant* currentHead = NULL, *currentTemp = NULL;        //当前规模蕴含项链表的当前指针和临时指针
	Implicant* nextHead = NULL,*nextTemp = NULL;      //下一规模蕴含项链表的头指针和临时指针
	int var = 0,varnum = 0;

	//读入最小项、无关项的编号，并转化为10比特二进制数
	for(int k=0;k<Nm;k++){
		cin>>m[k];
		var = m[k] > var ? m[k] : var;
		mbit[k] = m[k];
	}
	for(int k=0;k<Nd;k++){
		cin>>d[k];
		var = d[k] > var ? d[k] : var;
		dbit[k] = d[k];
	}
	while(var){     //计算变量的个数
		varnum++;
		var /= 2;
	}

	//把各个最小项、无关项链接到链表中
	for(int i=0;i<Nm;i++){     //生成最小项链表作为当前链表
		Implicant* a = new Implicant(1,varnum);
		a->index[0] = m[i];
		string s;
		s = mbit[i].to_string();
		for(int j=0;j<varnum;j++)
			a->bistr[varnum-1-j] = s[9-j];
		a->append(currentHead,currentTemp);
	}
	for(int i=0;i<Nd;i++){     //将无关项链接到当前链表中
		Implicant* a = new Implicant(1,varnum);
		a->isPrimeImplicant = 0;      //无关项均置为非本原蕴含项
		a->index[0] = d[i];
		string s;
		s = dbit[i].to_string();
		for(int j=0;j<varnum;j++)
			a->bistr[varnum-1-j] = s[9-j];
		a->append(currentHead,currentTemp);
	}


	//用循环实现合并，找出本原蕴含项（用链表实现，
	while(currentHead){
		while(currentHead){        //当前规模的蕴含项还未结束遍历
			currentTemp = currentHead->next;
			while(currentTemp){
				int t = currentHead->compare(currentTemp);
				if(t != -1 && t!=-2){       //可以合并
					//实现合并两个蕴含项
					Implicant* combine = new Implicant(2*(currentHead->size),currentHead->varnum);
					for(int i=0;i < currentHead->size;i++)
						combine->index[i] = currentHead->index[i];
					for(int i=0;i < currentTemp->size;i++)
						combine->index[i + currentHead->size] = currentTemp->index[i];
					for(int i=0;i < currentHead->varnum;i++)
						if(i != t)
							combine->bistr[i] = currentHead->bistr[i];
						else
							combine->bistr[i] = '-';
					//链接到下一规模蕴含项的链表中
					combine->append(nextHead,nextTemp);
					currentHead->isPrimeImplicant = 0;    //置为非本原蕴含项
					currentTemp->isPrimeImplicant = 0;
				}
				currentTemp = currentTemp->next;
			}
			if(currentHead->isPrimeImplicant){     //当前蕴含项是本原蕴含项
				//将当前蕴含项链接到本原蕴含项链表中
				currentHead->append(PrimeImplicant,PrimeTemp);
				currentHead = currentHead->next;
				PrimeTemp->next = NULL;
			}
			else{    //当前蕴含项不是本原蕴含项
				//将当前蕴含项删除
				Implicant* p = NULL;
				p = currentHead;
				currentHead = currentHead->next;
				delete p;
			}
		}
		currentHead = nextHead;
		nextHead = nextTemp = NULL;
	}

	//构造本原蕴含项图，遍历得到本质本原蕴含项
	vector<Implicant> FinalResult;
	Implicant* ImplicantGraph = new Implicant[Nm];
	PrimeTemp = PrimeImplicant;
	while(PrimeTemp){
		for(int i=0;i<PrimeTemp->size;i++){
			Implicant* t = new Implicant;
			*t = *PrimeTemp;
			int j = Index(m,Nm,PrimeTemp->index[i]);
			if(j == -1)      //不考虑无关项
				continue;
			if(!ImplicantGraph[j].next){
				ImplicantGraph[j].next = t;
				ImplicantGraph[j].size++;
			}
			else{
				Implicant* p = ImplicantGraph[j].next;
				while(p->next)
					p = p->next;
				p->next = t;
				ImplicantGraph[j].size++;
			}
		}
		PrimeTemp = PrimeTemp->next;
	}
	bool* isCovered = new bool[Nm];
	for(int i=0;i<Nm;i++)
		isCovered[i] = 0;
	for(int i=0;i<Nm;i++){
		if(ImplicantGraph[i].size == 1 && !isCovered[i]){     //是本质本原蕴含项，已经被覆盖的元素不可能再出现在其他本质本原蕴含项中
			ImplicantGraph[i].next->isEssentialPrimeImp = 1;
			FinalResult.push_back(*ImplicantGraph[i].next);
			for(int j=0;j<ImplicantGraph[i].next->size;j++){
				int k = Index(m,Nm,ImplicantGraph[i].next->index[j]);
				isCovered[k] = 1;
			}
		}
	}

	//应用petrick方法寻找最下覆盖
	//第一步为各个非本质本原蕴含项编码（用字符A~为他们编号）
	int PrimeCount = 0;    //记录非本质本原蕴含项的个数，以为他们编号
	Implicant* inverse[100] = {NULL};
	//为所有的非本质本原蕴含项编码
	for(int i=0;i<Nm;i++){
		if(!isCovered[i]){
			int tempPrimeCount=0;
			currentTemp = ImplicantGraph[i].next;
			while(currentTemp){
				bool flag = 1;
				for(int j=0;j<PrimeCount && flag;j++){
					if(*inverse[j] == *currentTemp){       //当前蕴含项之前已经被编码
						currentTemp->code = char('A'+j);
						if(ImplicantGraph[i].index == NULL)
							ImplicantGraph[i].index = new int[ ImplicantGraph[i].size ];
						ImplicantGraph[i].index[tempPrimeCount] = j;
						tempPrimeCount++;
						flag = 0;
					}
				}
				if(flag){       //当前蕴含项之前未被编码，在此处编码
					currentTemp->code = char('A'+PrimeCount);
					inverse[PrimeCount] = currentTemp;
					if(ImplicantGraph[i].index == NULL)
							ImplicantGraph[i].index = new int[ ImplicantGraph[i].size ];
					ImplicantGraph[i].index[tempPrimeCount] = PrimeCount;
					tempPrimeCount++;
					PrimeCount++;
				}
				currentTemp = currentTemp->next;
			}
		}
	}

	//广度优先遍历，用队列存储所有可能的覆盖
	queue<char*> minCoverQ;
	int i=0;
	for(;i<Nm;i++){     //先将  包含  第一个未被覆盖的最小项  的  各个非本质本原蕴含项的字符编码入队
		if(!isCovered[i]){
			for(int j=0;j<ImplicantGraph[i].size;j++){
				char* a = new char[Nm];
				a[0] = (char)(ImplicantGraph[i].index[j] + 'A');
				a[1] = '\0';
				minCoverQ.push(a);
			}
			break;
		}
	}
	i++;
	for(;i<Nm;i++){
		if(!isCovered[i]){
			int s = minCoverQ.size();
			for(int j=0;j<s;j++){        //将上一层的所有元素出队
				char* b;
				b = minCoverQ.front();         //下一层的每个字符编码与刚出队的字符数组合并再入队
				minCoverQ.pop();
				for(int k=0;k<ImplicantGraph[i].size;k++)
					minCoverQ.push(merge(b,Nm,(char)(ImplicantGraph[i].index[k] + 'A')));
			}
		}
	}
	//依次出队，寻找最短的编码（字符串）
	int minSize=Nm;
	char* minCover = NULL;
	while(!minCoverQ.empty()){
		char* t = minCoverQ.front();
		minCoverQ.pop();
		int s = strlen(t);
		if(s < minSize){
			minSize = s;
			minCover = t;
		}
	}
	//由编码（字符串）反推出对应的本原蕴含项
	if(minCover){
		int i=0;
		while(minCover[i] != '\0'){
			FinalResult.push_back( *inverse[ (int)(minCover[i] - 'A') ] );
			i++;
		}
	}
	
	//找出还未覆盖的元素，对于每个未覆盖的元素，寻找包含该元素的蕴含项中，包含未覆盖元素个数最多的本原蕴含项（贪心算法）
	/*	for(int i=0;i<Nm;i++){
		if(!isCovered[i]){
			int cover=0,maxcover=0;
			currentHead = currentTemp = ImplicantGraph[i].next;
			while(currentTemp){
				cover = 0;
				for(int j=0;j<currentTemp->size;j++){
					int k = Index(m,Nm,currentTemp->index[j]);
					if(!isCovered[k])
						cover++;
				}
				if(cover >= maxcover){
					maxcover = cover;
					currentHead = currentTemp;
				}
				currentTemp = currentTemp->next;
			}
			FinalResult.push_back(*currentHead);
			for(int t=0;t<currentHead->size;t++){
				int k = Index(m,Nm,currentHead->index[t]);
				isCovered[k] = 1;
			}
		}
	}*/

	//排序，输出
	if(FinalResult.size()){
		sort(FinalResult.begin(),FinalResult.end(),QM_compare);
		vector<Implicant>::iterator it;
		for(it = FinalResult.begin();it < FinalResult.end()-1;it++){
			it->display(out);
			int l = strlen(out);
			out[l] = '+';
			out[++l] = '\0';
		}
		it->display(out);
		int l = strlen(out);
		out[l] = '\n';
		out[++l] = '\0';
	}
	else{
		int l = strlen(out);
		out[l] = '0';
		out[++l] = '\n';
		out[++l] = '\0';
	}

	//释放内存
	currentHead = PrimeImplicant;
	while(currentHead){
		currentTemp = currentHead;
		currentHead = currentHead->next;
		delete currentTemp;
	}
	for(int i=0;i<Nm;i++){
		currentHead = ImplicantGraph[i].next;
		while(currentHead){
			currentTemp = currentHead;
			currentHead = currentHead->next;
			delete currentTemp;
		}
	}
	delete[] ImplicantGraph;
}

int main(){
	int Nq,Nm,Nd;
	cin>>Nq;
	char** output = new char*[Nq];
	for(int i=0;i<Nq;i++){
		output[i] = new char[100];
		output[i][0] = '\0';
	}
	for(int i=0;i<Nq;i++){
		cin>>Nm>>Nd;
		QM_Algorithm(Nm,Nd,output[i]);
	}
	std::printf("李溢 2016011235\n");
	for(int i=0;i<Nq;i++)
		printf("%s",output[i]);
	system("pause");
	return 0;
}
