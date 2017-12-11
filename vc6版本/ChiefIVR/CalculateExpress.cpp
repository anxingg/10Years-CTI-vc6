// CalculateExpress.cpp: implementation of the CCalculateExpress class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chiefivr.h"
#include "CalculateExpress.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


const int ENDOFARRAY=0;
const int OPERATOR=1;
const int NUMBER=2;
const int SIGN=3;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCalculateExpress::CCalculateExpress()
{

}

CCalculateExpress::~CCalculateExpress()
{

}

/*����x��y�η���ֵ,��������y>=0*/
double CCalculateExpress::power(double x, double y)
{
	if(y<0)		return 0;
	if(y==0)	return 1;
	return x*power(x,y-1);
}

/*������ĺ������һ���ַ�*/
bool CCalculateExpress::insert_data(char *array, char data, int nMaxBuf)
{
	int len;
	len=0;
	while(array[len]!='\0')
		len++;
	if( len >= nMaxBuf-1 ) 
		return false;
	array[len]=data;
	array[len+1]='\0';
	return true;
}
/*������ĺ������һ��������*/
bool CCalculateExpress::insert_datal(double *array, double data, int nMaxBuf)
{
	int len;
	len=0;
	while(array[len]!='\0')
		len++;
	if( len >= nMaxBuf-1 ) 
		return false;
	array[len]=data;
	array[len+1]='\0';
	
	return true;
}
/*ɾ���ַ�Ԫ��*/
void CCalculateExpress::del_element(char *array, int index)
{
	int i;
		i=index;
		while(array[i]!='\0')
		{	array[i]=array[i+1];i++;}
}
/*ɾ��������Ԫ��*/
void CCalculateExpress::del_elementl(double *array, int index)
{
	int i;
		i=index;
		while(array[i]!='\0')
		{	array[i]=array[i+1];i++;}
}
/*����ַ������ڵ�����Ԫ��*/
void CCalculateExpress::clear_data(char *array)
{
	int index=0;
		while(array[index]!='\0')
		{	array[index]='\0';index++;}
}
/*���ַ���ת��Ϊ������*/
double CCalculateExpress::Number(char *num_string)
{
	return atof(num_string);
}
/*���������ȵ�������*/
int CCalculateExpress::tiptop_pri(double *pri)
{
	int max,i;
		max=0;i=0;
		if(pri[0]=='\0') return -1;
		while(pri[i]!='\0')
		{
			if(pri[max]<pri[i])
				max=i;
			i++;
		}
		return max;
}
/*�÷��ż���������������ֵ*/
double CCalculateExpress::calbyoper(double n1, double n2, char oper)
{
	switch(oper)
	{	case '+':return n1+n2;
		case '-':return n1-n2;
		case '*':return n1*n2;
		case '/':return n1/n2;
		case '%':
			if( 0 == n1 ||  0 == n1 )
				return 0;
			return (int)n1%(int)n2;
		default:
			return 0;
	}
}

bool CCalculateExpress::calculate(char string[], double &dResult)
{
	if( NULL == string ) return false;

	double n1 = 0, n2 = 0;
	int last_char = 0,index = 0;					/*ǰһ�ַ�,����ָ��*/
	double number[50],priority[50],Add_pri = 0;	/*���ݣ�����Ȩ,��Ȩ*/
	memset( number, 0 ,sizeof(number) );
	memset( priority, 0 ,sizeof(priority) );

	char Operator[50],data[65],oper;		/*�����*/
	memset( Operator, 0 ,sizeof(Operator) );
	memset( data, 0 ,sizeof(data) );
	last_char = OPERATOR;

		

	while(last_char!=ENDOFARRAY)
	{
		if(( string[index] <= 57&&string[index] >= 48 ) || '.' == string[index])
		{
				if( !insert_data(data,string[index],65) )
					return false;//���������̫��
				last_char=NUMBER;
		}
		else
		{
			switch(string[index])
			{	case '/':
				case '*':
				case '%':
					if(last_char==NUMBER)
					{
						if( !insert_datal(number,Number(data),50) )
							return false;
						clear_data(data);
					}
					else 
					{	//printf("���������������������\n");
						return false;
					}
					if( !insert_data(Operator,string[index],50) )
						return false;//̫���/*�����
					if( !insert_datal(priority,8+Add_pri, 50 ) )
						return false;
					last_char=OPERATOR;
					break;
				case '+':
				case '-':
					if(last_char==SIGN)
					{
						//printf("����ͬʱ�������������\n");
						return false;
					}
					if(last_char==OPERATOR)
					{
						if( !insert_data(data,string[index],65) )
							return false ;//���������̫��
						last_char=SIGN;
					}
					else
					{       
						if( !insert_datal(number,Number(data),50) )
							return false;
						clear_data(data);
						if( !insert_data(Operator,string[index],50) )
							return false;//̫���+-�����
						if( !insert_datal(priority,7+Add_pri, 50 ) )
							return false;
						last_char=OPERATOR;
					}
					break;
				case '(': Add_pri+=2; break;
				case ')': Add_pri-=2; break;
				case '\0':last_char=ENDOFARRAY;
					  if( !insert_datal(number,Number(data), 50) )
						  return false;
					  clear_data(data);
					  break;
				case ' ':
					break;//ȥ���ո�
				default:
					//printf("%c is nonlicet charater\n",string[index]);
					return false;
			}
		}
		index++;
	}
	index=tiptop_pri(priority);
	while(index!=-1)
	{
		n1=number[index];n2=number[index+1];
		oper=Operator[index];
		number[index]=calbyoper(n1,n2,oper);
		del_element(Operator,index);
		del_elementl(number,index+1);
		del_elementl(priority,index);
		index=tiptop_pri(priority);
	}
	dResult = number[0];
	return true;
}
