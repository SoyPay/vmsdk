#include <string.h>
#include<stdlib.h>
#include <stdio.h>
#include"VmSdk.h"

typedef struct  {
	ACCOUNT_ID 	Sender;						//!<转账人ID（采用6字节的账户ID）
	Int64 nPayMoney;						//!<转账的人支付的金额
	unsigned short len;                     //!<接受钱账户信息长度
	char buffer[1];         				//!<接受钱账户信息
}CONTRACT;

typedef struct  {
	ACCOUNT_ID 	account;						//!<接受钱的ID（采用6字节的账户ID）
	Int64 nReciMoney;						    //!<	收到钱的金额
} ACCOUNT_INFO;
/*
* @brief 	step1:转账人的账户是否存在\n
* 			step2:检查转账人是否有权限\n
* 			step3:检查接受钱的账户是否存在\n
* 			step4:检查转钱与接受的钱是否平衡\n
*/
bool CheckContact(const CONTRACT* const pContract)
{
	if(!IsRegID(&pContract->Sender))
	{
		LogPrint("not regid",sizeof("not regid"),STRING);
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
		LogPrint(&len,sizeof(len),HEX);
		LogPrint("contact error",sizeof("contact error"),STRING);
		return false;
	}
	Int64 paymoney;
	GetCurPayAmount(&paymoney);
	unsigned char ret =Int64Compare(&paymoney,&pContract->nPayMoney);
	if(COMP_EQU != ret){
		LogPrint("recive and send not equeal1",sizeof("recive and send not equeal1"),STRING);
		return false;
	}

	char *strContace = (char*)pContract->buffer;
	while(len)
	{
		ACCOUNT_INFO accountinfo;
		memcpy(&accountinfo,strContace,sizeof(ACCOUNT_INFO));
		if(!IsRegID(&accountinfo.account)){
			LogPrint("recive not regid",sizeof("recive not regid"),STRING);
			return false;
		}
		Int64Add(&RecAmount,&accountinfo.nReciMoney,&RecAmount);
		len -= sizeof(ACCOUNT_INFO);
		strContace = strContace +sizeof(ACCOUNT_INFO);
	}

	ret =Int64Compare(&RecAmount,&pContract->nPayMoney);
	if(COMP_EQU != ret){
		LogPrint("recive and send not equeal",sizeof("recive and send not equeal"),STRING);
		return false;
	}

	return true;
}
bool ReadContactAndWrite(char *hash)
{
	__xdata static  char txcontac[512];
	if (!GetTxContacts((const unsigned char * const )hash, (void* const ) &txcontac, 512))
	{
		LogPrint("GetTxContacts error",sizeof("GetTxContacts error"),STRING);
		return false;
	}

	if(!DeleteData((const unsigned char * const )hash,32))
	{
		LogPrint("DeleteDataDB error",sizeof("DeleteDataDB error"),STRING);
		return false;
	}

	CONTRACT* pContract = (CONTRACT*)txcontac;
	VM_OPERATE ret;

	char accountid[6] = {0};
	if(GetScriptID(&accountid))
	{
		ret.type =REG_ID;
		ret.opeatortype = MINUS_FREE;
		memcpy(ret.accountid,&accountid,sizeof(ret.accountid));
		memcpy(&ret.money,&pContract->nPayMoney,sizeof(Int64));
		LogPrint(&accountid,32,HEX);
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
//		LogPrint(&accountinfo.account,32,HEX);
		WriteOutput(&ret,1);
		strContace = strContace +sizeof(ACCOUNT_INFO);

	}
	return true;
}
void WriteOperate(const CONTRACT* const pContract)
{
	unsigned long count = GetDBSize();
	if(count <2)
		return ;
	char hash[32] ={0};
	char flag[1] = {0};
	unsigned short valen = 1;
	unsigned short kenlen = 32;
	if(GetDBValue(0,hash,(unsigned char*)&kenlen,kenlen,flag,&valen)== false)
	{
		LogPrint("read db failed",sizeof("read db failed"),STRING);
			return;
	}
	if(!ReadContactAndWrite(hash))
	{
		LogPrint("THE TX NOE IN BLOCK",sizeof("THE TX NOE IN BLOCK"),STRING);
		LogPrint(hash,32,HEX);

	}
	count -=1;
	LogPrint(hash,32,HEX);
	LogPrint(&count,4,HEX);
	while(count--)
	{

		if(GetDBValue(1,hash,(unsigned char*)&kenlen,kenlen,flag,&valen)== false)
		{
			LogPrint("read db failed",sizeof("read db failed"),STRING);
			return;
		}
		LogPrint(hash,32,HEX);

		if(!ReadContactAndWrite(hash))
		{
			LogPrint(hash,32,HEX);
			LogPrint("THE TX NOE IN BLOCK",sizeof("THE TX NOE IN BLOCK"),STRING);
		}
	}

}
/*
* @brief 	step1:检查当前合约内容是否合法\n
* 			step2:遍历数据库中是有否有大于2条的数据，并且数据是有效的，输出指令流\n
* 			step3:写入当前tx的hash到数据库\n
*/
bool ProcessContract(const CONTRACT* const pContract)
{
	LogPrint("ProcessContract",sizeof("ProcessContract"),STRING);
	if(!CheckContact(pContract))
	{
		return false;
	}
	WriteOperate(pContract);
	bool flag = false;
	char hash[32]={0};
	if(!GetCurTxHash(&hash))
		return false;
	if(!WriteData(&hash,32,&flag,1))
		return false;

}
int main()
{
	__xdata static  char pcontact[100]; //={0x00,0x00,0x00,0x00,0x05,0x00,0x00,0xe8,0x76,0x48,0x17,0x00,0x00,0x00,0x38,0x00};
	unsigned long len = 100;
	GetCurTxContact(pcontact,len);
	LogPrint("enter",sizeof("enter"),STRING);
 	if(!ProcessContract((CONTRACT*)pcontact))
 	{
 		__VmExit(RUN_SCRIPT_DATA_ERR);
 	}
 	__VmExit(RUN_SCRIPT_OK);
  return 0;
}
