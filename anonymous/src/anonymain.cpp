#include <string.h>
#include<stdlib.h>
#include <stdio.h>
#include"VmSdk.h"

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
 * \section ��Ҫת�˵��˷��ͽ��װ�
 *�����������Ҫת�Ľ��Ѿ���ַ\n
 *������bool ProcessFirstContract(const FIRST_CONTRACT* pContract);\n
 */
typedef struct  {
	ACCOUNT_ID 	Sender;						//!<ת����ID������6�ֽڵ��˻�ID��
	unsigned long nHeight;							//!<��ʱ���Ը߶�
	Int64 nPayMoney;						//!<ת�˵���֧���Ľ��
	unsigned short len;                     //!<����Ǯ�˻���Ϣ����
	char buffer[1];         				//!<����Ǯ�˻���Ϣ
}CONTRACT;

typedef struct  {
	ACCOUNT_ID 	account;						//!<����Ǯ��ID������6�ֽڵ��˻�ID��
	Int64 nReciMoney;						    //!<	�յ�Ǯ�Ľ��
} ACCOUNT_INFO;
/*
* @brief 	step1:ת���˵��˻��Ƿ����\n
* 			step2:���ת�����Ƿ���Ȩ��\n
* 			step3:������Ǯ���˻��Ƿ����\n
* 			step4:���תǮ����ܵ�Ǯ�Ƿ�ƽ��\n
*/
bool CheckContact(const CONTRACT* const pContract)
{
	if(!IsRegID(&pContract->Sender))
	{
		return false;
	}
//	if(!IsAuthorited(&pContract->Sender,&pContract->nPayMoney))
//	{
//		return false;
//	}
	Int64 RecAmount;
	Int64Inital(&RecAmount,"\x00",1);

	unsigned short len = pContract->len;
	if(len%sizeof(ACCOUNT_INFO) != 0){
		return false;
	}

	char *strContace = (char*)pContract->buffer;
	while(len)
	{
		ACCOUNT_INFO accountinfo;
		memcpy(&accountinfo,pContract->buffer,sizeof(ACCOUNT_INFO));
		if(!IsRegID(&accountinfo.account)){
			return false;
		}
		Int64Add(&RecAmount,&accountinfo.nReciMoney,&RecAmount);
		len -= sizeof(ACCOUNT_INFO);
	}

	unsigned char ret =Int64Compare(&RecAmount,&pContract->nPayMoney);
	if(COMP_EQU != ret){
		return false;
	}
	return true;
}
bool ReadContactAndWrite(char *hash)
{
	__xdata static  char txcontac[512];
	if (!GetTxContacts((const unsigned char * const )hash, (void* const ) &txcontac, 512))
	{
		LogPrint("1",sizeof("1"),STRING);
		return false;
	}

	if(!DeleteDataDB((const unsigned char * const )hash,32))
	{
		LogPrint("2",sizeof("2"),STRING);
		return false;
	}

	CONTRACT* pContract = (CONTRACT*)txcontac;
	VM_OPERATE ret;
	ret.outheight = GetCurRunEnvHeight() + pContract->nHeight;

	char accountid[6] = {0};
	if(GetCurScritpAccount(&accountid))
	{
//		if(!IsAuthorited(&accountid,&pContract.nPayMoney))
//			{
//				return false;
//			}

		ret.opeatortype = MINUS_FREE;
		memcpy(ret.accountid,&accountid,sizeof(ret.accountid));
		memcpy(&ret.money,&pContract->nPayMoney,sizeof(Int64));
		WriteOutput(&ret,1);
	}

	char *strContace = pContract->buffer;
	unsigned short len = pContract->len;
	while(len)
	{
		ACCOUNT_INFO accountinfo;
		memcpy(&accountinfo,strContace,sizeof(ACCOUNT_INFO));
		memcpy(ret.accountid,&accountinfo.account,sizeof(ret.accountid));
		memcpy(&ret.money,&accountinfo.nReciMoney,sizeof(Int64));
		ret.opeatortype = ADD_FREE;
		len -= sizeof(ACCOUNT_INFO);
		WriteOutput(&ret,1);
		strContace = strContace +sizeof(ACCOUNT_INFO);

	}
	return true;
}
void WriteOperate(const CONTRACT* const pContract)
{
	//// ��ǰ��Լ��Ǯ�򵽽ű��˻���
	VM_OPERATE ret;
	ret.outheight = GetCurRunEnvHeight() + pContract->nHeight;
	ret.opeatortype = MINUS_FREE;
	memcpy(ret.accountid,&pContract->Sender,sizeof(ret.accountid));
	memcpy(&ret.money,&pContract->nPayMoney,sizeof(Int64));
	WriteOutput(&ret,1);

	char accountid[6] = {0};
	if(GetCurScritpAccount(&accountid))
	{
		ret.opeatortype = ADD_FREE;
		memcpy(ret.accountid,&accountid,sizeof(ret.accountid));
		memcpy(&ret.money,&pContract->nPayMoney,sizeof(Int64));
		WriteOutput(&ret,1);
	}


	unsigned long count = GetDBSize();
	if(count <2)
		return ;
	char hash[32] ={0};
	char flag[1] = {0};
	unsigned short valen = 1;
	unsigned short kenlen = 32;
	unsigned long ptime = 0;
	if(GetDBValue(0,hash,(unsigned char*)&kenlen,kenlen,flag,&valen,&ptime)== false)
	{
		LogPrint("read db failed",sizeof("read db failed"),STRING);
	}
	if(!ReadContactAndWrite(hash))
	{
		return ;
	}
	count -=1;
	while(count--)
	{

		if(GetDBValue(0,hash,(unsigned char*)&kenlen,kenlen,flag,&valen,&ptime)== false)
		{
			LogPrint("read db failed",sizeof("read db failed"),STRING);
		}
		if(!ReadContactAndWrite(hash))
		{
			return ;
		}
	}

}
/*
* @brief 	step1:��鵱ǰ��Լ�����Ƿ�Ϸ�\n
* 			step2:�������ݿ������з��д���2�������ݣ�������������Ч�ģ����ָ����
* 			step3:д�뵱ǰtx��hash�����ݿ�\n
*/
bool ProcessContract(const CONTRACT* const pContract)
{
	if(!CheckContact(pContract))
	{
		return false;
	}
	WriteOperate(pContract);
	bool flag = false;
	char hash[32]={0};
	if(!GetCurTxHash(&hash))
		return false;
	unsigned long outheight = GetCurRunEnvHeight() + pContract->nHeight;
	WriteDataDB(&hash,32,&flag,1,outheight);

}
int main()
{
	__xdata static  char pcontact[100];
	unsigned long len = 100;
 	GetMemeroyData(pcontact,len);
 	if(!ProcessContract((CONTRACT*)pcontact))
 	{
 		__exit(RUN_SCRIPT_DATA_ERR);
 	}
 	__exit(RUN_SCRIPT_OK);
  return 0;
}
