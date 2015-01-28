/*! \mainpage Developer documentation
 *
 * \section 买家购买交易包
 *描述：买卖 双方商量好交易价格，超时时间，裁决人等，买家组合好交易包\n
 *函数：bool ProcessContract1(const FIRST_CONTRACT* pContract);\n


 *\section 卖家确认包
 *描述：卖家接受买家的转账\n
 *函数：bool ProcessContract2(const NEXT_CONTRACT* pContract);\n
 *
 *
 *\section 买家申诉包
 *描述：卖家不发货，买家提出申诉\n
 *函数：bool ProcessContract3(const NEXT_CONTRACT* pContract);\n
 *
 *
 *\section 仲裁结果包
 *描述：仲裁判决结果\n
 *函数：bool ProcessContract3(const NEXT_CONTRACT* pContract);\n
 *
 *.
 */

#include <string.h>
#include<stdlib.h>
#include<stdio.h>
#include <stdarg.h>
#include"VmSdk.h"

#define ACCOUNT_ID_SIZE 6
#define MAX_ARBITRATOR 3
#define MAX_ACCOUNT_LEN 20
#define HASH_SIZE	32

typedef struct {
	unsigned char nType;					//!<类型
	unsigned char nArbitratorCount;			//!<仲裁者个数
	ACCOUNT_ID buyer;						//!<买家ID（采用6字节的账户ID）
	ACCOUNT_ID seller;						//!<卖家ID（采用6字节的账户ID）
	ACCOUNT_ID arbitrator[MAX_ARBITRATOR];	//!<仲裁者ID（采用6字节的账户ID）
	long nHeight;							//!<超时绝对高度
	Int64 nFineMoney;						//!<卖家违约后最大罚款金额
	Int64 nPayMoney;						//!<买家向卖家支付的金额
	Int64 nFee;								//!<仲裁手续费
	Int64 ndeposit;							//!<仲裁押金,申诉时从仲裁者扣除的押金(如果仲裁不响应钱归买家否则买家退还给仲裁者)
} FIRST_CONTRACT;

typedef struct {
	unsigned char nType;				//!<交易类型
	unsigned char hash[HASH_SIZE];		//!<上一个交易包的哈希
} NEXT_CONTRACT;

typedef struct {
	unsigned char nType;				//!<交易类型
	unsigned char hash[HASH_SIZE];		//!<上一个交易的哈希
	Int64 nMinus;
} ARBIT_RES_CONTRACT;

enum Role {
	BUYER = 1, SELLER, ARBITRATOR, UNDIFUNE_ROLE
};

void PrintString(const char *format, ...) {
	va_list arg;
	static char tmp[255];
	memset(tmp,0,sizeof(tmp));
	va_start(arg, format);
	vsprintf(tmp, format, arg);
	va_end(arg);

	LogPrint(tmp, strlen(tmp), STRING);
}

/**
 *
 * @param pContract 每个仲裁结果包的合约内容(第一个合约结构为当前合约内容，剩余2个为待填充内容)
 * @param pCount 有效合约结构内容的个数
 * @return
 */
bool FillArContract(ARBIT_RES_CONTRACT* pContract, unsigned char* pCount) {
	static char szData[512];
	if (!GetTxContacts((const unsigned char * const ) pContract[0].hash, (void* const ) szData, sizeof(szData)))
		return false;

	if (4 == szData[0]) {
		memcpy(&pContract[1], szData, sizeof(ARBIT_RES_CONTRACT));

		if (!GetTxContacts((const unsigned char * const ) pContract[1].hash, (void* const ) szData, sizeof(szData)))
			return false;

		if (4 == szData[0]) {
			memcpy(&pContract[2], szData, sizeof(ARBIT_RES_CONTRACT));

			if (!GetTxContacts((const unsigned char * const ) pContract[2].hash, (void* const ) szData, sizeof(szData)))
				return false;

			if (3 != szData[0]) {
				return false;
			}

			*pCount = 3;
		} else {
			*pCount = 2;
		}
	} else {
		*pCount = 1;
	}

	return true;
}

void Swap(ARBIT_RES_CONTRACT* pA, ARBIT_RES_CONTRACT* pB) {
	ARBIT_RES_CONTRACT tmp;
	memcpy(&tmp, pA, sizeof(ARBIT_RES_CONTRACT));
	memcpy(pA, pB, sizeof(ARBIT_RES_CONTRACT));
	memcpy(pB, &tmp, sizeof(ARBIT_RES_CONTRACT));
}

void SetArContactInOrder(ARBIT_RES_CONTRACT* pContract, unsigned char nCount) {
	ARBIT_RES_CONTRACT tmp;
	if (2 == nCount) {
		Swap(&pContract[0], &pContract[1]);
	}

	if (3 == nCount) {
		Swap(&pContract[0], &pContract[2]);
	}
}

Int64 GetFineMoney(ARBIT_RES_CONTRACT* pContract, unsigned char nCount)
{
	if (1 == nCount)
		return pContract[0].nMinus;

	Int64 nTotal;
	if (2 == nCount)
	{
		Int64Add(&pContract[0].nMinus,&pContract[1].nMinus,&nTotal);
	}

	if (3 == nCount)
	{
		Int64 tmp;
		Int64Add(&pContract[0].nMinus,&pContract[1].nMinus,&tmp);
		Int64Add(&tmp,&pContract[2].nMinus,&nTotal);
	}

	Int64 nRes;
	Int64 nDived;
	memset(&nDived,0,sizeof(nDived));
	memcpy(&nDived,&nCount,1);
	Int64Div(&nTotal,&nDived,&nRes);
	return nRes;
}

/**
 * @brief  	step1:block高度有效性\n
 * 			step2:买家和卖家地址是否有效\n
 *    		step3:仲裁者地址是否存在，仲裁者个数是否合法，仲裁者是否关联脚本\n
 * 			step4:卖家的自由金额是否足够赔偿\n
 * 			step5:权限自定义字段中的值与各角色是否匹配
 */
bool CheckContract1(const FIRST_CONTRACT* pContract) {
	//step1
	unsigned long nRunTimeHeight = GetCurRunEnvHeight();
	if (pContract->nHeight < 0) {
		PrintString("[Error]pContract->nHeight is %d",pContract->nHeight);
		return false;
	}

	//step2
	if (!IsRegID((const void* const ) pContract->buyer.accounid)
			|| !IsRegID((const void* const ) pContract->seller.accounid)) {
		PrintString("[Error]IsRegID step 2failed in CheckContract1");
		return false;
	}

	//step3
	for (int i = 0;i<pContract->nArbitratorCount;i++)
	{
		if (!IsRegID((const void*) pContract->arbitrator[0].accounid)) {
			PrintString("[Error]IsRegID step 3failed in CheckContract1");
			return false;
		}

	}

	//step4
	Int64 nTotalFreeMoney;
	if (!QueryAccountBalance((const unsigned char* const ) pContract->seller.accounid, &nTotalFreeMoney)) {
		PrintString("[Error]QueryAccountBalance failed in CheckContract1");
		return false;
	}


	COMP_RET ret = Int64Compare(&pContract->nFineMoney, &nTotalFreeMoney);
	if (COMP_ERR == ret || COMP_LARGER == ret) {
		PrintString("[Error]not enough money to fine in CheckContract1");
		return false;
	}


	//step5
	char szUserData[2] = { 0 };
	unsigned short nRes = GetAuthUserDefine((const void* const ) pContract->buyer.accounid, szUserData,
			sizeof(szUserData) / sizeof(szUserData[0]));

	if (!nRes || BUYER != szUserData[0]) {
		PrintString("[Error]check userdata in CheckContract1 failed");
		return false;
	}


	return true;
}

/**
 * @brief 	step1:hash的有效性\n
 * 			step2:从数据库中读取数据判断上一个交易是否已经处理（是否已经有其他卖家确认包，防止卖家重复确认），如果已经处理则此交易无效返回。\n
 * 			step3:如果买家提出申诉，卖家是否有足够的钱赔给买家。交易签名账户地址是否是第一个交易中卖家的地址\n
 * 			step4:用户自定义数据检测\n
 * @param pContract
 */

bool CheckContract2(const NEXT_CONTRACT* pContract, FIRST_CONTRACT* pContract1) {
	//step1
	if (!GetTxContacts((const unsigned char * const ) pContract->hash, (void* const ) pContract1,
			sizeof(FIRST_CONTRACT))) {
		PrintString("[Error]GetTxContacts failed in CheckContract2");
		return false;
	}

	//step2
	char szValue[2] = { 0 };
	unsigned short nRes = ReadDataValueDB(pContract->hash, sizeof(pContract->hash), szValue,
			sizeof(szValue) / sizeof(szValue[0]));
	if (nRes && 1 == szValue[0]) {
		PrintString("[Error]ReadDataValueDB failed in CheckContract2");
		return false;
	}

	//step3
	ACCOUNT_ID sellID;
	if (ACCOUNT_ID_SIZE != GetAccounts((const unsigned char *) pContract->hash, (void*) sellID.accounid, sizeof(sellID))
			|| !memcmp((const void*) sellID.accounid, (const void*) pContract1->seller.accounid, ACCOUNT_ID_SIZE)) {
		PrintString("[Error]GetAccounts failed in CheckContract2");
		return false;
	}

	//step4
	char szUserData[2] = { 0 };
	nRes = GetAuthUserDefine((const void* const ) pContract1->seller.accounid, szUserData,
			sizeof(szUserData) / sizeof(szUserData[0]));
	if (!nRes || SELLER != szUserData[0]) {
		PrintString("[Error]check userdata in CheckContract2 failed");
		return false;
	}

	return true;
}

/**
 * @brief	step1：hash的有效性\n
 * 			step2：上一个交易是否有处理过
 * 			step3:用户自定义数据检测\n
 * @param pContract
 */
bool CheckContract3(const NEXT_CONTRACT* pContract, FIRST_CONTRACT* pContract1, NEXT_CONTRACT* pContract2) {
	//step1
	if (!GetTxContacts((const unsigned char * const ) pContract->hash, (void* const ) pContract2,
			sizeof(NEXT_CONTRACT))) {
		PrintString("[Error]GetTxContacts2 failed in CheckContract3");
		return false;
	}

	if (!GetTxContacts((const unsigned char * const ) pContract2->hash, (void* const ) pContract1,
			sizeof(FIRST_CONTRACT))) {
		PrintString("[Error]GetTxContacts1 failed in CheckContract3");
		return false;
	}

	//step2
	char szValue[2] = { 0 };
	unsigned short nRes = ReadDataValueDB(pContract->hash, sizeof(pContract->hash), szValue,
			sizeof(szValue) / sizeof(szValue[0]));
	if (nRes && 1 == szValue[0]) {
		PrintString("[Error]ReadDataValueDB failed in CheckContract3");
		return false;
	}

	//step3
	char szUserData[2] = { 0 };
	nRes = GetAuthUserDefine((const void* const ) pContract1->buyer.accounid, szUserData,
			sizeof(szUserData) / sizeof(szUserData[0]));
	if (!nRes || BUYER != szUserData[0]) {
		PrintString("[Error]GetAuthUserDefine failed in CheckContract3");
		return false;
	}

	return true;
}

/**
 * @brief	step1：检查第三个包的hash有效性\n
 * 			step2；扣钱金额的有效性\n
 * 			step3：检测上一个交易是否有处理
 * 			step4:用户自定义数据检测\n
 * @param pContract
 */
bool CheckContract4(ARBIT_RES_CONTRACT* pContract, unsigned char* pArCountInArray, FIRST_CONTRACT* pContract1,
		NEXT_CONTRACT* pContract2) {
	//step1
	NEXT_CONTRACT contract3;
	if (!FillArContract(pContract, pArCountInArray)) {
		PrintString("[Error]FillArContract failed in CheckContract4");
		return false;
	}
	PrintString("11111");
	LogPrint(pContract1->arbitrator[0].accounid,6,HEX);

	SetArContactInOrder(pContract, *pArCountInArray);

	PrintString("2222");
	LogPrint(pContract1->arbitrator[0].accounid,6,HEX);
	if (!GetTxContacts((const unsigned char * const ) pContract[0].hash, (void* const ) &contract3, sizeof(contract3))) {
		PrintString("[Error]GetTxContacts3 failed in CheckContract4");
		return false;
	}


	if (!GetTxContacts((const unsigned char * const ) contract3.hash, (void* const ) pContract2, sizeof(NEXT_CONTRACT))) {
		PrintString("[Error]GetTxContacts3 failed in CheckContract4");
		return false;
	}


	if (!GetTxContacts((const unsigned char * const ) pContract2->hash, (void* const ) pContract1,
			sizeof(FIRST_CONTRACT))) {
		PrintString("[Error]GetTxContacts3 failed in CheckContract4");
		return false;
	}


	//step 2
	int nIndex = *pArCountInArray-1;
	COMP_RET ret = Int64Compare(&pContract[nIndex].nMinus, &pContract1->nFineMoney);
	if (COMP_ERR == ret || COMP_LARGER == ret) {
		PrintString("[Error]nMinus is biger than nFineMoney");
		return false;
	}

	//step3
	char szValue[2] = { 0 };
	unsigned short nRes = ReadDataValueDB(pContract[nIndex].hash, sizeof(pContract[nIndex].hash), szValue,
			sizeof(szValue) / sizeof(szValue[0]));
	if (nRes && 1 == szValue[0]) {
		PrintString("[Error]ReadDataValueDB failed in CheckContract4");
		return false;
	}

	//step4
	char szUserData[2] = { 0 };
	nRes = GetAuthUserDefine((const void* const ) pContract1->arbitrator[0].accounid, szUserData,
			sizeof(szUserData) / sizeof(szUserData[0]));
	if (!nRes || ARBITRATOR != szUserData[0]) {
		PrintString("[Error]GetAuthUserDefine failed in CheckContract4");
		return false;
	}

	return true;
}

/**
 * @brief step1:从买家的自由金额中扣除金额(应支付给卖家的钱)\n
 *        step2:把上一步操作的钱转入买家的冻结金额中
 * @param vmoper 操作账户的结构体数组
 * @param nVmSize 数组大小
 * @param pContract 第一个合约结构
 */
bool OperAccount1(VM_OPERATE *vmoper, unsigned char nVmSize, const void* pContract) {
	if (NULL == vmoper || NULL == pContract || nVmSize != 2)
		return false;

	FIRST_CONTRACT* pContractData = (FIRST_CONTRACT*) pContract;
	unsigned long nHeight = GetCurRunEnvHeight() + pContractData->nHeight;
	memcpy(vmoper[0].accountid, pContractData->buyer.accounid, ACCOUNT_ID_SIZE);
	vmoper[0].opeatortype = MINUS_FREE;
	vmoper[0].outheight = nHeight;
	vmoper[0].money = pContractData->nPayMoney;

	memcpy(vmoper[1].accountid, pContractData->buyer.accounid, ACCOUNT_ID_SIZE);
	vmoper[1].opeatortype = ADD_FREEZD;
	vmoper[1].outheight = nHeight;
	vmoper[1].money = pContractData->nPayMoney;
	return true;
}

/**
 * @brief step1:从买家的冻结转入卖家的冻结金额中\n
 * 		  step2:卖家自由金额中扣除最大罚款到冻结金额中
 * @param vmoper 操作账户的结构体数组
 * @param nVmSize 数组大小
 * @param pContract 第二个合约结构
 */
bool OperAccount2(VM_OPERATE *vmoper, unsigned short nVmSize, FIRST_CONTRACT* pContract1,
		const NEXT_CONTRACT* pContract2) {
	if (NULL == vmoper || NULL == pContract1 || nVmSize != 4)
	{
		PrintString("[ERROE]OperAccount2 failed");
		return false;
	}

	int nConfirmHeight = GetTxConFirmHeight(pContract2->hash);
	//step1
	unsigned long nHeight = nConfirmHeight + pContract1->nHeight;
	memcpy(vmoper[0].accountid, pContract1->buyer.accounid, ACCOUNT_ID_SIZE);
	vmoper[0].opeatortype = MINUS_FREEZD;
	vmoper[0].outheight = nHeight;
	vmoper[0].money = pContract1->nPayMoney;

	memcpy(vmoper[1].accountid, pContract1->seller.accounid, ACCOUNT_ID_SIZE);
	vmoper[1].opeatortype = ADD_FREEZD;
	vmoper[1].outheight = nHeight;
	vmoper[1].money = pContract1->nPayMoney;

	//step 2
	memcpy(vmoper[2].accountid, pContract1->seller.accounid, ACCOUNT_ID_SIZE);
	vmoper[2].opeatortype = MINUS_FREE;
	vmoper[2].outheight = nHeight;
	vmoper[2].money = pContract1->nFineMoney;

	memcpy(vmoper[3].accountid, pContract1->seller.accounid, ACCOUNT_ID_SIZE);
	vmoper[3].opeatortype = ADD_FREEZD;
	vmoper[3].outheight = nHeight;
	vmoper[3].money = pContract1->nFineMoney;
	return true;
}

/**
 *
 * @param vmoper
 * @param nVmSize
 * @param pContract
 */
bool OperAccount3(VM_OPERATE *vmoper, unsigned short nVmSize, FIRST_CONTRACT* pContract1, NEXT_CONTRACT* pContract2) {
	if (NULL == vmoper || NULL == pContract1 || NULL == pContract2)
		return false;

	int nConfirmHeight = GetTxConFirmHeight(pContract2->hash);
	unsigned long nHeight = nConfirmHeight + pContract1->nHeight;
	int nArCount = pContract1->nArbitratorCount;

	for(int i = 0;i<nArCount;i++)
	{
		memcpy(vmoper[2 * i].accountid, pContract1->arbitrator[i].accounid, ACCOUNT_ID_SIZE);
		vmoper[2 * i].opeatortype = MINUS_FREE;
		vmoper[2 * i].outheight = nHeight;
		vmoper[2 * i].money = pContract1->ndeposit;

		memcpy(vmoper[2 * i + 1].accountid, pContract1->buyer.accounid, ACCOUNT_ID_SIZE);
		vmoper[2 * i + 1].opeatortype = ADD_FREEZD;
		vmoper[2 * i + 1].outheight = nHeight;
		vmoper[2 * i + 1].money = pContract1->ndeposit;
	}

	return true;
}

/**
 *
 * @param vmoper
 * @param nVmSize
 * @param pContract
 */
bool OperAccount4(VM_OPERATE *vmoper, unsigned short nVmSize, FIRST_CONTRACT* pContract1, NEXT_CONTRACT* pContract2,
		ARBIT_RES_CONTRACT* pArContract, unsigned char nArCountInArray) {
	if (NULL == vmoper || NULL == pContract1 || NULL == pContract2 ||
			0 == nArCountInArray || 0 == pContract1->nArbitratorCount)
		return false;

	if (nArCountInArray!=pContract1->nArbitratorCount)
	{
		//最后一个中裁判决才操作账户
		return true;
	}

	int nConfirmHeight = GetTxConFirmHeight(pContract2->hash);
	unsigned long nHeight = nConfirmHeight + pContract1->nHeight;
	Int64 nFineMoney = GetFineMoney(pArContract,nArCountInArray);

	//step 1
	memcpy(vmoper[0].accountid, pContract1->seller.accounid, ACCOUNT_ID_SIZE);
	vmoper[0].opeatortype = MINUS_FREEZD;
	vmoper[0].outheight = nHeight;
	vmoper[0].money = nFineMoney;

	memcpy(vmoper[1].accountid, pContract1->buyer.accounid, ACCOUNT_ID_SIZE);
	vmoper[1].opeatortype = ADD_FREE;
	vmoper[1].outheight = nHeight;
	vmoper[1].money = nFineMoney;

	for(int i = 0;i<pContract1->nArbitratorCount;i++)
	{
		//step 2
		memcpy(vmoper[2+4*i].accountid, pContract1->buyer.accounid, ACCOUNT_ID_SIZE);
		vmoper[2+4*i].opeatortype = MINUS_FREEZD;
		vmoper[2+4*i].outheight = nHeight;
		vmoper[2+4*i].money = pContract1->ndeposit;

		memcpy(vmoper[3+4*i].accountid, pContract1->arbitrator[i].accounid, ACCOUNT_ID_SIZE);
		vmoper[3+4*i].opeatortype = ADD_FREE;
		vmoper[3+4*i].outheight = nHeight;
		vmoper[3+4*i].money = pContract1->ndeposit;

		//step 3
		memcpy(vmoper[4+4*i].accountid, pContract1->buyer.accounid, ACCOUNT_ID_SIZE);
		vmoper[4+4*i].opeatortype = MINUS_FREE;
		vmoper[4+4*i].outheight = nHeight;
		vmoper[4+4*i].money = pContract1->nFee;

		memcpy(vmoper[5+4*i].accountid, pContract1->arbitrator[i].accounid, ACCOUNT_ID_SIZE);
		vmoper[5+4*i].opeatortype = ADD_FREE;
		vmoper[5+4*i].outheight = nHeight;
		vmoper[5+4*i].money = pContract1->nFee;
	}

	return true;
}

/**
 * @brief 1.	从买家的自由金额转入买家的冻结中
 * @param data 操作账户的结构体指针
 * @param conter 操作次数
 * @return
 */
bool WriteOutput1(const VM_OPERATE* data, unsigned short conter) {
	return WriteOutput(data, conter);
}

/**
 * @brief	1.	从买家的输出冻结转到卖家的冻结。\n
 2.	从卖家的自由金额中扣款转到卖家的冻结中
 * @param data 操作账户的结构体指针
 * @param conter 操作次数
 * @return
 */
bool WriteOutput2(const VM_OPERATE* data, unsigned short conter) {
	return WriteOutput(data, conter);
}

/**
 * @brief	1.	从仲裁者自由金额中扣除少量费用到买家冻结金额中。
 * @param data 操作账户的结构体指针
 * @param conter 操作次数
 * @return
 */
bool WriteOutput3(const VM_OPERATE* data, unsigned short conter) {
	return WriteOutput(data, conter);
}

/**
 * @brief	1.	从仲裁者的冻结恢复一定金额到自由金额中\n
 2.	从卖家的冻结金额中扣除费用给买家\n
 3.	从卖家的自由金额中扣除仲裁费用给仲裁者
 * @param data 操作账户的结构体指针
 * @param conter 操作次数
 * @return
 */
bool WriteOutput4(const VM_OPERATE* data, unsigned short conter) {
	return WriteOutput(data, conter);
}

/**
 * @brief	将第一个交易的哈希作为key写入数据库
 * @return
 */
bool WriteDB2(const char* pKey, FIRST_CONTRACT* pFirstContract) {
	bool bFlag = true;
	return WriteDataDB((const void* const ) pKey, HASH_SIZE, &bFlag, 1, pFirstContract->nHeight);
}

/**
 * @brief	将第二个交易的哈希作为key写入数据库
 * @return
 */
bool WriteDB3(const char* pKey, FIRST_CONTRACT* pFirstContract) {
	bool bFlag = true;
	return WriteDataDB((const void* const ) pKey, 32, &bFlag, 1, pFirstContract->nHeight);
}

/**
 * @brief	将第三个交易的哈希作为key写入数据库
 * @return
 */
bool WriteDB4(const char* pKey, FIRST_CONTRACT* pFirstContract) {
	bool bFlag = true;
	return WriteDataDB((const void* const ) pKey, 32, &bFlag, 1, pFirstContract->nHeight);
}

/**
 * @brief 处理第一个交易
 * @param pContract
 * @return
 */
bool ProcessContract1(const FIRST_CONTRACT* pContract) {
	if (CheckContract1(pContract)) {
		VM_OPERATE vmOper[2];
		if (OperAccount1(vmOper, sizeof(vmOper) / sizeof(vmOper[0]), pContract))
			return WriteOutput1(vmOper, sizeof(vmOper) / sizeof(vmOper[0]));
	}
	return false;
}

/**
 * @brief 卖家确认
 * @param pContract 合约内容
 * @return
 */
bool ProcessContract2(const NEXT_CONTRACT* pContract) {
	VM_OPERATE vmOper[4];
	FIRST_CONTRACT firstcontract;
	if (CheckContract2(pContract, &firstcontract)) {
		if (OperAccount2(vmOper, sizeof(vmOper) / sizeof(vmOper[0]), &firstcontract, pContract)) {
			if (WriteDB2((const char*) pContract->hash, &firstcontract))
				return WriteOutput2(vmOper, sizeof(vmOper) / sizeof(vmOper[0]));
		}
	}
	PrintString("ProcessContract2 failed");
	return false;
}

/**
 * @brief 执行申诉
 * @param pContract 合约内容
 * @return
 */
bool ProcessContract3(const NEXT_CONTRACT* pContract) {
	VM_OPERATE vmOper[6];
	FIRST_CONTRACT firstContract;
	NEXT_CONTRACT secondContract;
	if (CheckContract3(pContract, &firstContract, &secondContract)) {
		if (OperAccount3(vmOper, sizeof(vmOper) / sizeof(vmOper[0]), &firstContract, &secondContract)) {
			if (WriteDB3((const char*) pContract->hash, &firstContract))
				return WriteOutput3(vmOper, 2*firstContract.nArbitratorCount);
		}
	}
	PrintString("ProcessContract3 failed");
	return false;
}

/**
 * @brief 执行仲裁结果
 * @param pContract 仲裁结果合约结构体
 * @return
 */
bool ProcessContract4(const ARBIT_RES_CONTRACT* pContract) {
	VM_OPERATE vmOper[14];
	FIRST_CONTRACT firstcontract;
	NEXT_CONTRACT secondContract;
	ARBIT_RES_CONTRACT arrayArContract[MAX_ARBITRATOR];
	arrayArContract[0] = *pContract;
	unsigned char nArCountInArray = 0;

	if (CheckContract4(arrayArContract, &nArCountInArray, &firstcontract, &secondContract)) {
		if (OperAccount4(vmOper, sizeof(vmOper) / sizeof(vmOper[0]), &firstcontract, &secondContract, arrayArContract,
				nArCountInArray)) {
			if (WriteDB4((const char*) pContract->hash, &firstcontract))
				if (nArCountInArray == firstcontract.nArbitratorCount)
					return WriteOutput4(vmOper, 2 + firstcontract.nArbitratorCount * 4);
				else
					return true;
		}
	}
	PrintString("ProcessContract4 failed");
	return false;
}

int main() {
	__xdata static char pContract[512];
	unsigned long len = 512;
	int nRet = GetCurTxContact(pContract, len);
	PrintString("nRet is %d", nRet);

#pragma data_alignment = 1
//	FIRST_CONTRACT firstcontract;
//	memcpy(firstcontract.buyer.accounid,"111111",ACCOUNT_ID_SIZE);
//	memcpy(firstcontract.seller.accounid,"222222",ACCOUNT_ID_SIZE);
//	memcpy(firstcontract.arbitrator[0].accounid,"333333",ACCOUNT_ID_SIZE);
//	firstcontract.nType = 1;
//	Int64Inital(&firstcontract.nFineMoney,"\x64",1);//100
//	Int64Inital(&firstcontract.nPayMoney,"\x50",1);//80
//	Int64Inital(&firstcontract.ndeposit,"\x14",1);//20
//	Int64Inital(&firstcontract.nFee,"\x64",1);//10
//	memcpy(pContract,&firstcontract,sizeof(FIRST_CONTRACT));

//	NEXT_CONTRACT secondcontract;
//	memcpy(secondcontract.hash,"11111111222222223333333344444444",32);
//	secondcontract.nType = 2;
//	memcpy(pContract,&secondcontract,sizeof(NEXT_CONTRACT));
	bool bProcessRet = false;
	switch (pContract[0]) {
	case 1:
		bProcessRet = ProcessContract1((FIRST_CONTRACT*) pContract);
		break;

	case 2:
		bProcessRet = ProcessContract2((NEXT_CONTRACT*) pContract);
		break;

	case 3:
		bProcessRet = ProcessContract3((NEXT_CONTRACT*) pContract);
		break;

	case 4:
		bProcessRet = ProcessContract4((ARBIT_RES_CONTRACT*) pContract);
		break;
	}

	if (bProcessRet) {
		PrintString("run ok");
		__exit(RUN_SCRIPT_OK);
	} else {
		PrintString("run failed");
		__exit(RUN_SCRIPT_DATA_ERR);
	}

	return 1;
}
