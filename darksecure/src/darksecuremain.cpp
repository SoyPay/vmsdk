#include <string.h>
#include<stdlib.h>
#include <stdio.h>
#include"VmSdk.h"

 ///  黑币担保交易 脚本
/*
 * A 买家  B 卖家
 * 第一个包:
 * A.发送一个交易包 A B 都要签名
 * 合约内容:  金额 m
 * 从A账户扣m金额到脚本账户
 * 从B账户扣m/2金额到脚本账户
 * 第二个包：
 * A.确认收到货,发送确认包
 * 合约内容：第一个包的hash
*/

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

/*! \mainpage  Description Diablo than regular
 *
 *This is the introduction
 *
 *
 *\section 买家卖家发送交易包
 *描述：买卖 双方商量好交易价格，超时时间,买家组合好交易包
 *函数：bool ProcessFirstContract(const FIRST_CONTRACT* pContract);
 *
 *
 *
 *\section 买家确认包
 *描述：买家收到货，发送确认包
 *函数：bool ProcessSecondContract(const NEXT_CONTRACT* pContract);
 *
 */

#define ACCOUNT_ID_SIZE 6
#define MAX_ACCOUNT_LEN 20
#define HASH_SIZE	32


typedef struct  {
	  	unsigned char nType;     //!<类型
	ACCOUNT_ID 	buyer;						//!<买家ID（采用6字节的账户ID）
	ACCOUNT_ID seller;       //!<卖家ID（采用6字节的账户ID）				
	unsigned long nHeight;							//!<超时绝对高度
	Int64 nPayMoney;						//!<买家向卖家支付的金额
} FIRST_CONTRACT;

typedef struct {
	unsigned char nType;				//!<交易类型
	unsigned char hash[HASH_SIZE];		//!<上一个交易包的哈希
} NEXT_CONTRACT;

/**
 * @brief   step1:block高度有效性\n
 * 			step2:买家和卖家地址是否有效\n
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
 * @brief   step1:从买家账户转namount钱到脚本账户\n
 * 			step2:从卖家账户转namount/2钱到脚本账户\n
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
* @brief 	step1:检查合约的有效性\n
* 			step1:写指令\n
* 			step3:将当前交易的hash写到数据库中\n
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
* @brief 	step1:hash的有效性\n
* 			step1:是否是买家发送的确认包\n
* 			step3:从数据库中读取数据判断上一个交易是否已经处理（是否已经有其他卖家确认包，防止卖家重复确认），如果已经处理则此交易无效返回。\n
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
* @brief 	step1:检查合约的有效性\n
* 			step1:输出指令\n
* 			step3:修改交易包在数据库中的状态\n
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
