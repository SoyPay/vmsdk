/*! \mainpage Developer documentation
 *
 * \section ��ҹ����װ�
 *���������� ˫�������ý��׼۸񣬳�ʱʱ�䣬�þ��˵ȣ������Ϻý��װ�\n
 *������bool ProcessContract1(const FIRST_CONTRACT* pContract);\n


 *\section ����ȷ�ϰ�
 *���������ҽ�����ҵ�ת��\n
 *������bool ProcessContract2(const NEXT_CONTRACT* pContract);\n
 *
 *
 *\section ������߰�
 *���������Ҳ�����������������\n
 *������bool ProcessContract3(const NEXT_CONTRACT* pContract);\n
 *
 *
 *\section �ٲý����
 *�������ٲ��о����\n
 *������bool ProcessContract3(const NEXT_CONTRACT* pContract);\n
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
	unsigned char nType;					//!<����
	unsigned char nArbitratorCount;			//!<�ٲ��߸���
	ACCOUNT_ID buyer;						//!<���ID������6�ֽڵ��˻�ID��
	ACCOUNT_ID seller;						//!<����ID������6�ֽڵ��˻�ID��
	ACCOUNT_ID arbitrator[MAX_ARBITRATOR];	//!<�ٲ���ID������6�ֽڵ��˻�ID��
	long nHeight;							//!<��ʱ���Ը߶�
	Int64 nFineMoney;						//!<����ΥԼ����󷣿���
	Int64 nPayMoney;						//!<���������֧���Ľ��
	Int64 nFee;								//!<�ٲ�������
	Int64 ndeposit;							//!<�ٲ�Ѻ��,����ʱ���ٲ��߿۳���Ѻ��(����ٲò���ӦǮ����ҷ�������˻����ٲ���)
} FIRST_CONTRACT;

typedef struct {
	unsigned char nType;				//!<��������
	unsigned char hash[HASH_SIZE];		//!<��һ�����װ��Ĺ�ϣ
} NEXT_CONTRACT;

typedef struct {
	unsigned char nType;				//!<��������
	unsigned char hash[HASH_SIZE];		//!<��һ�����׵Ĺ�ϣ
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
 * @param pContract ÿ���ٲý�����ĺ�Լ����(��һ����Լ�ṹΪ��ǰ��Լ���ݣ�ʣ��2��Ϊ���������)
 * @param pCount ��Ч��Լ�ṹ���ݵĸ���
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
 * @brief  	step1:block�߶���Ч��\n
 * 			step2:��Һ����ҵ�ַ�Ƿ���Ч\n
 *    		step3:�ٲ��ߵ�ַ�Ƿ���ڣ��ٲ��߸����Ƿ�Ϸ����ٲ����Ƿ�����ű�\n
 * 			step4:���ҵ����ɽ���Ƿ��㹻�⳥\n
 * 			step5:Ȩ���Զ����ֶ��е�ֵ�����ɫ�Ƿ�ƥ��
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
 * @brief 	step1:hash����Ч��\n
 * 			step2:�����ݿ��ж�ȡ�����ж���һ�������Ƿ��Ѿ������Ƿ��Ѿ�����������ȷ�ϰ�����ֹ�����ظ�ȷ�ϣ�������Ѿ�������˽�����Ч���ء�\n
 * 			step3:������������ߣ������Ƿ����㹻��Ǯ�����ҡ�����ǩ���˻���ַ�Ƿ��ǵ�һ�����������ҵĵ�ַ\n
 * 			step4:�û��Զ������ݼ��\n
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
 * @brief	step1��hash����Ч��\n
 * 			step2����һ�������Ƿ��д����
 * 			step3:�û��Զ������ݼ��\n
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
 * @brief	step1��������������hash��Ч��\n
 * 			step2����Ǯ������Ч��\n
 * 			step3�������һ�������Ƿ��д���
 * 			step4:�û��Զ������ݼ��\n
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
 * @brief step1:����ҵ����ɽ���п۳����(Ӧ֧�������ҵ�Ǯ)\n
 *        step2:����һ��������Ǯת����ҵĶ�������
 * @param vmoper �����˻��Ľṹ������
 * @param nVmSize �����С
 * @param pContract ��һ����Լ�ṹ
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
 * @brief step1:����ҵĶ���ת�����ҵĶ�������\n
 * 		  step2:�������ɽ���п۳���󷣿��������
 * @param vmoper �����˻��Ľṹ������
 * @param nVmSize �����С
 * @param pContract �ڶ�����Լ�ṹ
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
		//���һ���в��о��Ų����˻�
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
 * @brief 1.	����ҵ����ɽ��ת����ҵĶ�����
 * @param data �����˻��Ľṹ��ָ��
 * @param conter ��������
 * @return
 */
bool WriteOutput1(const VM_OPERATE* data, unsigned short conter) {
	return WriteOutput(data, conter);
}

/**
 * @brief	1.	����ҵ��������ת�����ҵĶ��ᡣ\n
 2.	�����ҵ����ɽ���пۿ�ת�����ҵĶ�����
 * @param data �����˻��Ľṹ��ָ��
 * @param conter ��������
 * @return
 */
bool WriteOutput2(const VM_OPERATE* data, unsigned short conter) {
	return WriteOutput(data, conter);
}

/**
 * @brief	1.	���ٲ������ɽ���п۳��������õ���Ҷ������С�
 * @param data �����˻��Ľṹ��ָ��
 * @param conter ��������
 * @return
 */
bool WriteOutput3(const VM_OPERATE* data, unsigned short conter) {
	return WriteOutput(data, conter);
}

/**
 * @brief	1.	���ٲ��ߵĶ���ָ�һ�������ɽ����\n
 2.	�����ҵĶ������п۳����ø����\n
 3.	�����ҵ����ɽ���п۳��ٲ÷��ø��ٲ���
 * @param data �����˻��Ľṹ��ָ��
 * @param conter ��������
 * @return
 */
bool WriteOutput4(const VM_OPERATE* data, unsigned short conter) {
	return WriteOutput(data, conter);
}

/**
 * @brief	����һ�����׵Ĺ�ϣ��Ϊkeyд�����ݿ�
 * @return
 */
bool WriteDB2(const char* pKey, FIRST_CONTRACT* pFirstContract) {
	bool bFlag = true;
	return WriteDataDB((const void* const ) pKey, HASH_SIZE, &bFlag, 1, pFirstContract->nHeight);
}

/**
 * @brief	���ڶ������׵Ĺ�ϣ��Ϊkeyд�����ݿ�
 * @return
 */
bool WriteDB3(const char* pKey, FIRST_CONTRACT* pFirstContract) {
	bool bFlag = true;
	return WriteDataDB((const void* const ) pKey, 32, &bFlag, 1, pFirstContract->nHeight);
}

/**
 * @brief	�����������׵Ĺ�ϣ��Ϊkeyд�����ݿ�
 * @return
 */
bool WriteDB4(const char* pKey, FIRST_CONTRACT* pFirstContract) {
	bool bFlag = true;
	return WriteDataDB((const void* const ) pKey, 32, &bFlag, 1, pFirstContract->nHeight);
}

/**
 * @brief �����һ������
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
 * @brief ����ȷ��
 * @param pContract ��Լ����
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
 * @brief ִ������
 * @param pContract ��Լ����
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
 * @brief ִ���ٲý��
 * @param pContract �ٲý����Լ�ṹ��
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
