#include <string.h>
#include<stdlib.h>
#include <stdio.h>
#include"VmSdk.h"

///暗黑币 匿名交易
/*
 *第一个包：
 *合约内容：金额m 地址 金额 地址 金额 ....
 *执行此脚本:打款m金额到合约账户，保存此txhash到数据库
 *如果此数据库中有两条记录，就可以执行打款 否则不执行，直到记录>2,执行打款后的数据,从数据中删除
 */

///黑币担保交易
/**
 * 规则
 * @return
 */
/*! \mainpage Developer documentation
 *
 * \section 需要转账的人发送交易包
 *描述：填好需要转的金额，已经地址\n
 *函数：bool ProcessFirstContract(const FIRST_CONTRACT* pContract);\n
 */
typedef struct  {
	ACCOUNT_ID 	Sender;						//!<转账人ID（采用6字节的账户ID）
	unsigned long nHeight;							//!<超时绝对高度
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
	//// 当前合约的钱打到脚本账户中
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
* @brief 	step1:检查当前合约内容是否合法\n
* 			step2:遍历数据库中是有否有大于2条的数据，并且数据是有效的，输出指令流
* 			step3:写入当前tx的hash到数据库\n
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
