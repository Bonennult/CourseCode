#include<cstdio>
#include<cmath>
using namespace std;

class Complex{         //复数类，重载+、-、*、/运算符
private:
	double real,imag;
public:
	Complex(double r=0,double i=0):real(r),imag(i)  {  }
	void Set(double r,double i)  {  real = r;  imag = i;  }
	Complex operator+(Complex c)  {  return Complex(this->real + c.real , this->imag + c.imag);  }
	Complex operator-(Complex c)  {  return Complex(this->real - c.real , this->imag - c.imag);  }
	Complex operator*(Complex c)  {  return Complex((this->real * c.real) - (this->imag * c.imag),(this->real * c.imag)+(this->imag * c.real));  }
	Complex operator/(Complex c)  {  return Complex((this->real*c.real + this->imag*c.imag) / (c.real*c.real + c.imag*c.imag) , (this->imag*c.real - this->real*c.imag) / (c.real*c.real + c.imag*c.imag));  }
	Complex  operator*(double d)  {  return Complex(d * this->real,d * this->imag);  }
	double Module()  {  return sqrt(real*real + imag*imag);  }
	void Display()  {  printf("%.16lf %.16lf\n",real,imag);  }
};

Complex f(double a[],Complex X,Complex Y){     //计算f(x,y)
	Complex f;
	Complex temp[4];
	for(int i=0;i<4;i++){
		temp[i] = a[i*4+3];
		for(int j=2;j>=0;j--)
			temp[i] = Y * temp[i] + a[i*4+j];
	}
	f =temp[3];
	for(int i=2;i>=0;i--)
		f = f * X + temp[i];
	return f;
}

Complex dX(double a[],Complex X,Complex Y){        //计算df(x,y)/dx
	Complex fx;
	Complex temp[4];
	for(int i=0;i<4;i++){
		temp[i] = a[i*4+3];
		for(int j=2;j>=0;j--)
			temp[i] = Y * temp[i] + a[i*4+j];
	}
	fx = temp[3];
	for(int i=3;i>1;i--)
		fx = fx * X * i + temp[i-1];
	return fx;
}

Complex dY(double a[],Complex X,Complex Y){        //计算df(x,y)/dy
	Complex fy;
	Complex temp[4];
	for(int j=0;j<4;j++){
		temp[j] = a[12+j];
		for(int i=2;i>=0;i--)
			temp[j] = X * temp[j] + a[i*4+j];
	}
	fy = temp[3];
	for(int i=3;i>1;i--)
		fy = fy * Y * i + temp[i-1];
	return fy;
}

int main(){
	double A1[16],A2[16];
	for(int i=0;i<16;i++)
		scanf("%lf",&A1[i]);
	for(int i=0;i<16;i++)
		scanf("%lf",&A2[i]);

	double residule=1;
	Complex x(1,1), y(1,1);   //设置迭代初值
	Complex tempx,tempy;
	Complex fx1,fx2,fy1,fy2;
	while(residule >= 1e-6){
		fx1 = dX(A1,x,y);           //雅可比矩阵
		fy1 = dY(A1,x,y);
		fx2 = dX(A2,x,y);
		fy2 = dY(A2,x,y);
		tempx = fy2 * f(A1,x,y) - fy1 * f(A2,x,y);       //求δX
		tempy = fx1 * f(A2,x,y) - fx2 * f(A1,x,y);
		x = x - tempx / (fx1*fy2 - fx2*fy1);         //迭代得到的X
		y = y - tempy / (fx1*fy2 - fx2*fy1);

		residule = f(A1,x,y).Module() > f(A2,x,y).Module() ?  f(A1,x,y).Module() : f(A2,x,y).Module();
	}
	x.Display();
	y.Display();
	return 0;
}