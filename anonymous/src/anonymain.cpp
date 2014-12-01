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
	int nHeight;							//!<超时绝对高度
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
	return true;
}
void WriteOperate()
{

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
	WriteOperate();
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
