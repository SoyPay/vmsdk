#include <string.h>
#include<stdlib.h>
#include <stdio.h>
#include"VmSdk.h"

 ///  �ڱҵ������� �ű�
/*
 * A ���  B ����
 * ��һ����:
 * A.����һ�����װ� A B ��Ҫǩ��
 * ��Լ����:  ��� m
 * ��A�˻���m���ű��˻�
 * ��B�˻���m/2���ű��˻�
 * �ڶ�������
 * A.ȷ���յ���,����ȷ�ϰ�
 * ��Լ���ݣ���һ������hash
*/

///���ڱ� ��������
/*
 *��һ������
 *��Լ���ݣ����m ��ַ ��� ��ַ ��� ....
 *ִ�д˽ű�:���m����Լ�˻��������txhash�����ݿ�
 *��������ݿ�����������¼���Ϳ���ִ�д�� ����ִ�У�ֱ����¼>2,ִ�д��������,��������ɾ��
 */

///�ڱҵ�������
/**
 * ����
 * @return
 */
/*! \mainpage Developer documentation
 *
 * \section ��ҹ����װ�
 *���������� ˫�������ý��׼۸񣬳�ʱʱ��,�����Ϻý��װ�\n
 *������bool ProcessFirstContract(const FIRST_CONTRACT* pContract);\n


 *\section ���ȷ�ϰ�
 *����������յ���������ȷ�ϰ�\n
 *������bool ProcessSecondContract(const NEXT_CONTRACT* pContract);\n
 *.
 */

#include <string.h>
#include<stdlib.h>
#include"VmSdk.h"

#define ACCOUNT_ID_SIZE 6
#define MAX_ACCOUNT_LEN 20
#define HASH_SIZE	32


typedef struct  {
	unsigned char nType;					//!<����
	ACCOUNT_ID 	buyer;						//!<���ID������6�ֽڵ��˻�ID��
	ACCOUNT_ID seller;						//!<����ID������6�ֽڵ��˻�ID��
	int nHeight;							//!<��ʱ���Ը߶�
	Int64 nPayMoney;						//!<���������֧���Ľ��
} FIRST_CONTRACT;

typedef struct {
	unsigned char nType;				//!<��������
	unsigned char hash[HASH_SIZE];		//!<��һ�����װ��Ĺ�ϣ
} NEXT_CONTRACT;

/**
 * @brief   step1:block�߶���Ч��\n
 * 			step2:��Һ����ҵ�ַ�Ƿ���Ч\n
 *    		step3:��Һ������Ƿ���Ȩ��\n
 */
bool CheckfirstContact(const FIRST_CONTRACT* const pContract)
{
	return true;
}
/**
 * @brief   step1:������˻�תnamountǮ���ű��˻�\n
 * 			step2:�������˻�תnamount/2Ǯ���ű��˻�\n
 */
void WriteFirstContact(const FIRST_CONTRACT* const pContract)
{

}
bool ProcessFirstContract(const FIRST_CONTRACT* const pContract)
{
	if(!CheckfirstContact(pContract))
	{
		return false;
	}
	WriteFirstContact(pContract);
}
/*
* @brief 	step1:hash����Ч��\n
* 			step2:�����ݿ��ж�ȡ�����ж���һ�������Ƿ��Ѿ������Ƿ��Ѿ�����������ȷ�ϰ�����ֹ�����ظ�ȷ�ϣ�������Ѿ�������˽�����Ч���ء�\n
*/
bool CheckSecondContact(const NEXT_CONTRACT* const pContract)
{
	return true;
}
void WriteSecondContact(const FIRST_CONTRACT* const pContract)
{

}
bool ProcessSecondContract(const NEXT_CONTRACT* pContract)
{
   if(!CheckSecondContact(pContract))
	   return false;
   FIRST_CONTRACT* pfContract;
   WriteSecondContact(pfContract);
}
enum TXTYPE{
	TX_TRADE,
	TX_CONFIM
}
int main()
{
	__xdata static  char pcontact[100];
	unsigned long len = 100;
 	GetMemeroyData(pcontact,len);
 	switch(pcontact[1])
 	{
		case TX_TRADE:
		{
			if(ProcessFirstContract((FIRST_CONTRACT*)pcontact))
			{
				LogPrint("ProcessFirstContract error",sizeof("ProcessFirstContract error"),STRING);
			}
			break;
		}
		case TX_CONFIM:
		{
			if(ProcessSecondContract((NEXT_CONTRACT*) pcontact))
			{
				LogPrint("ProcessSecondContract error",sizeof("ProcessSecondContract error"),STRING);
			}
			break;
		}
		default:
		{
			LogPrint("tx format error",sizeof("tx format error"),STRING);
			__exit(RUN_SCRIPT_DATA_ERR);
			break;
		}
 	}
 	__exit(RUN_SCRIPT_OK);
  return 0;
}
