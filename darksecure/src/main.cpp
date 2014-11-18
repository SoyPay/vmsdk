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
/*! \mainpage Developer documentation
 *
 * \section 买家购买交易包
 *描述：买卖 双方商量好交易价格，超时时间,买家组合好交易包\n
 *函数：bool ProcessFirstContract(const FIRST_CONTRACT* pContract);\n


 *\section 买家确认包
 *描述：买家收到货，发送确认包\n
 *函数：bool ProcessSecondContract(const NEXT_CONTRACT* pContract);\n
 *.
 */

#include <string.h>
#include<stdlib.h>
#include"VmSdk.h"

#define ACCOUNT_ID_SIZE 6
#define MAX_ACCOUNT_LEN 20
#define HASH_SIZE	32


typedef struct  {
	unsigned char nType;					//!<类型
	ACCOUNT_ID 	buyer;						//!<买家ID（采用6字节的账户ID）
	ACCOUNT_ID seller;						//!<卖家ID（采用6字节的账户ID）
	int nHeight;							//!<超时绝对高度
	Int64 nPayMoney;						//!<买家向卖家支付的金额
} FIRST_CONTRACT;

typedef struct {
	unsigned char nType;				//!<交易类型
	unsigned char hash[HASH_SIZE];		//!<上一个交易包的哈希
} NEXT_CONTRACT;

/**
 * @brief   step1:block高度有效性\n
 * 			step2:买家和卖家地址是否有效\n
 *    		step3:买家和卖家是否有权限\n
 */
bool CheckfirstContact(const FIRST_CONTRACT* const pContract)
{
	return true;
}
/**
 * @brief   step1:从买家账户转namount钱到脚本账户\n
 * 			step2:从卖家账户转namount/2钱到脚本账户\n
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
* @brief 	step1:hash的有效性\n
* 			step2:从数据库中读取数据判断上一个交易是否已经处理（是否已经有其他卖家确认包，防止卖家重复确认），如果已经处理则此交易无效返回。\n
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
