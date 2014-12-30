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

/*! \mainpage  Description Diablo than regular
 *
 *This is the introduction
 *
 *
 *\section ������ҷ��ͽ��װ�
 *���������� ˫�������ý��׼۸񣬳�ʱʱ��,�����Ϻý��װ�
 *������bool ProcessFirstContract(const FIRST_CONTRACT* pContract);
 *
 *
 *
 *\section ���ȷ�ϰ�
 *����������յ���������ȷ�ϰ�
 *������bool ProcessSecondContract(const NEXT_CONTRACT* pContract);
 *
 */

#define ACCOUNT_ID_SIZE 6
#define MAX_ACCOUNT_LEN 20
#define HASH_SIZE	32


typedef struct  {
	  	unsigned char nType;     //!<����
	ACCOUNT_ID 	buyer;						//!<���ID������6�ֽڵ��˻�ID��
	ACCOUNT_ID seller;       //!<����ID������6�ֽڵ��˻�ID��				
	unsigned long nHeight;							//!<��ʱ���Ը߶�
	Int64 nPayMoney;						//!<���������֧���Ľ��
} FIRST_CONTRACT;

typedef struct {
	unsigned char nType;				//!<��������
	unsigned char hash[HASH_SIZE];		//!<��һ�����װ��Ĺ�ϣ
} NEXT_CONTRACT;

/**
 * @brief   step1:block�߶���Ч��\n
 * 			step2:��Һ����ҵ�ַ�Ƿ���Ч\n
 */
bool CheckfirstContact(const FIRST_CONTRACT* const pContract)
{
	unsigned long nTipHeight = GetTipHeight();
	if(pContract->nHeight <= nTipHeight)
	{
		return false;
	}
	if(!IsRegID(&pContract->buyer))
	{
		return false;
	}
	if(!IsRegID(&pContract->seller))
	{
		return false;
	}
//	if(!IsAuthorited(&pContract->buyer,&pContract->nPayMoney))
//	{
//		return false;
//	}
//	Int64 div;
//	Int64Inital(&div,"\x02", 1);
//	Int64 pout;
//	Int64Div(&pContract->nPayMoney,&div,&pout);
//	if(!IsAuthorited(&pContract->seller,&pout))
//	{
//		return false;
//	}
	return true;
}
/**
 * @brief   step1:������˻�תnamountǮ���ű��˻�\n
 * 			step2:�������˻�תnamount/2Ǯ���ű��˻�\n
 */
void WriteFirstContact(const FIRST_CONTRACT* const pContract)
{
	VM_OPERATE ret;
	memcpy(ret.accountid,&pContract->buyer,sizeof(ret.accountid));
	memcpy(&ret.money,&pContract->nPayMoney,sizeof(Int64));
	ret.opeatortype = MINUS_FREE;
	ret.outheight = GetCurRunEnvHeight() + pContract->nHeight;
	WriteOutput(&ret,1);

	memcpy(ret.accountid,&pContract->seller,sizeof(ret.accountid));
	Int64 div;
	Int64Inital(&div,"\x02", 1);
	Int64 pout;
	Int64Div(&pContract->nPayMoney,&div,&pout);
	memcpy(&ret.money,&pout,sizeof(Int64));
	WriteOutput(&ret,1);
	Int64 result;
	Int64Add(&pContract->nPayMoney,&pout,&result);

	char accountid[6] = {0};
	if(GetCurScritpAccount(&accountid))
	{
		ret.opeatortype = ADD_FREE;
		memcpy(ret.accountid,&accountid,sizeof(ret.accountid));
		memcpy(&ret.money,&result,sizeof(Int64));
		WriteOutput(&ret,1);
	}
}
/*
* @brief 	step1:����Լ����Ч��\n
* 			step1:дָ��\n
* 			step3:����ǰ���׵�hashд�����ݿ���\n
*/
bool ProcessFirstContract(const FIRST_CONTRACT* const pContract)
{
	if(!CheckfirstContact(pContract))
	{
		return false;
	}
	WriteFirstContact(pContract);
	bool flag = false;
	char hash[32]={0};
	if(!GetCurTxHash(&hash))
		return false;
	unsigned long outheight = GetCurRunEnvHeight() + pContract->nHeight;
	WriteDataDB(&hash,32,&flag,1,outheight);
    return true;
}
/*
* @brief 	step1:hash����Ч��\n
* 			step1:�Ƿ�����ҷ��͵�ȷ�ϰ�\n
* 			step3:�����ݿ��ж�ȡ�����ж���һ�������Ƿ��Ѿ������Ƿ��Ѿ�����������ȷ�ϰ�����ֹ�����ظ�ȷ�ϣ�������Ѿ�������˽�����Ч���ء�\n
*/
bool CheckSecondContact(const NEXT_CONTRACT* const pContract)
{
	FIRST_CONTRACT contract;
	if (!GetTxContacts((const unsigned char * const ) pContract->hash, (void* const ) &contract, sizeof(FIRST_CONTRACT)))
		{
			return false;
		}
	LogPrint((char*)&contract,sizeof(contract),HEX);
	char hash[32] = {0};
	if(!GetCurTxHash(hash)){
		return false;
	}
	char account[6] = {0};
	if(!GetCurTxAccount(account,6))
		{
		LogPrint("can not get cur account",sizeof("can not get cur account"),STRING);
		return false;

		}
	if(strcmp((char*)&contract.buyer,account) != 0)
	{
		LogPrint("not buyer account",sizeof("not buyer account"),STRING);
		return false;
	}
	bool flag = false;
	if(!ReadDataValueDB((const unsigned char * const ) pContract->hash,32,&flag,1))
	{
		LogPrint("read db failed",sizeof("read db failed"),STRING);
		return false;
	}
	if(flag)
	{
		return false;
	}
	return true;
}
void WriteSecondContact(const FIRST_CONTRACT* const pContract)
{
	Int64 div;
	Int64Inital(&div,"\x02", 1);
	Int64 pout;
	Int64Div(&pContract->nPayMoney,&div,&pout);
	Int64 result;
	Int64Add(&pContract->nPayMoney,&pout,&result);

	VM_OPERATE ret;
	memcpy(ret.accountid,&pContract->seller,sizeof(ret.accountid));
	memcpy(&ret.money,&result,sizeof(Int64));
	ret.opeatortype = ADD_FREE;
	ret.outheight = GetCurRunEnvHeight() + pContract->nHeight;
	WriteOutput(&ret,1);

	char accountid[6] = {0};
	if(GetCurScritpAccount(&accountid))
	{
		ret.opeatortype = MINUS_FREE;
		memcpy(ret.accountid,&accountid,sizeof(ret.accountid));
		memcpy(&ret.money,&result,sizeof(Int64));
		WriteOutput(&ret,1);
	}
}
/*
* @brief 	step1:����Լ����Ч��\n
* 			step1:���ָ��\n
* 			step3:�޸Ľ��װ������ݿ��е�״̬\n
*/
bool ProcessSecondContract(const NEXT_CONTRACT* pContract)
{
   if(!CheckSecondContact(pContract))
	   return false;
   FIRST_CONTRACT contract;
   	if (!GetTxContacts((const unsigned char * const ) pContract->hash, (void* const ) &contract, sizeof(FIRST_CONTRACT)))
   		return false;
   WriteSecondContact(&contract);
	bool flag = true;
	unsigned long outheight = 0;
	ModifyDataDB(&pContract->hash,32,&flag,1,outheight);
   return true;
}
enum TXTYPE{
	TX_TRADE = 0x01,
	TX_CONFIM = 0x02,
};
int main()
{
  __xdata static  char pcontact[100];

	unsigned long len = 100;
	GetCurTxContact(pcontact,len);
 	switch(pcontact[0])
 	{
		case TX_TRADE:
		{
			if(!ProcessFirstContract((FIRST_CONTRACT*)pcontact))
			{
				LogPrint("ProcessFirstContract error",sizeof("ProcessFirstContract error"),STRING);
				__exit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}
		case TX_CONFIM:
		{
			LogPrint("TX_CONFIM",sizeof("TX_CONFIM"),STRING);
			if(!ProcessSecondContract((NEXT_CONTRACT*) pcontact))
			{
				LogPrint("ProcessSecondContract error",sizeof("ProcessSecondContract error"),STRING);
				__exit(RUN_SCRIPT_DATA_ERR);
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
