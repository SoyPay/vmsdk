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
#include <stdio.h>
#include"VmSdk.h"

#define ACCOUNT_ID_SIZE 6
#define MAX_ARBITRATOR 3
#define MAX_ACCOUNT_LEN 20
#define HASH_SIZE	32


typedef struct  {
	unsigned char nType;					//!<����
	unsigned char nArbitratorCount;			//!<�ٲ��߸���
	ACCOUNT_ID 	buyer;						//!<���ID������6�ֽڵ��˻�ID��
	ACCOUNT_ID seller;						//!<����ID������6�ֽڵ��˻�ID��
	ACCOUNT_ID arbitrator[MAX_ARBITRATOR];	//!<�ٲ���ID������6�ֽڵ��˻�ID��
	int nHeight;							//!<��ʱ���Ը߶�
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
}ARBIT_RES_CONTRACT;

enum Role {
	BUYER = 1,
	SELLER,
	ARBITRATOR,
	UNDIFUNE_ROLE
};
/**
 * @brief   step1:block�߶���Ч��\n
 * 			step2:��Һ����ҵ�ַ�Ƿ���Ч\n
 *    		step3:�ٲ��ߵ�ַ�Ƿ���ڣ��ٲ��߸����Ƿ�Ϸ����ٲ����Ƿ�����ű�\n
 * 			step4:���ҵ����ɽ���Ƿ��㹻�⳥\n
 * 			step5:Ȩ���Զ����ֶ��е�ֵ�����ɫ�Ƿ�ƥ��
 */

bool CheckContract1(const FIRST_CONTRACT* pContract) {
	//step1
	unsigned long nRunTimeHeight = GetCurRunEnvHeight();
	if ((unsigned long)pContract->nHeight>nRunTimeHeight)
		return false;

	//step2
	if (!IsRegID((const void* const)pContract->buyer.accounid) ||
			! IsRegID((const void* const)pContract->seller.accounid))
		return false;

	//step3
	if (1 != pContract->nArbitratorCount || IsRegID((const void*)pContract->arbitrator[0].accounid) )
		return false;

	//step4
	Int64	nTotalFreeMoney;
	if (!QueryAccountBalance((const unsigned char* const)pContract->seller.accounid,ACOUNT_ID,&nTotalFreeMoney))
		return false;
	COMP_RET ret = Int64Compare(&pContract->nFineMoney,&nTotalFreeMoney);
	if (COMP_ERR == ret || COMP_LARGER == ret)
		return false;

	//step5
	char szRole[10] = {0};
	if (GetAuthUserDefine((const void* const ) pContract->buyer.accounid, szRole, sizeof(szRole) / sizeof(szRole[0]))
			|| BUYER != szRole[0])
		return false;
	if (GetAuthUserDefine((const void* const ) pContract->seller.accounid, szRole, sizeof(szRole) / sizeof(szRole[0]))
			|| SELLER != szRole[0])
		return false;
	if (GetAuthUserDefine((const void* const ) pContract->arbitrator[0].accounid, szRole, sizeof(szRole) / sizeof(szRole[0]))
			|| ARBITRATOR != szRole[0])
		return false;

	return true;
}

/**
 * @brief 	step1:hash����Ч��\n
 * 			step2:�����ݿ��ж�ȡ�����ж���һ�������Ƿ��Ѿ������Ƿ��Ѿ�����������ȷ�ϰ�����ֹ�����ظ�ȷ�ϣ�������Ѿ�������˽�����Ч���ء�\n
 * 			step3:����ǩ���˻���ַ�Ƿ��ǵ�һ�����������ҵĵ�ַ\n
 * @param pContract
 */

bool CheckContract2(const NEXT_CONTRACT* pContract,FIRST_CONTRACT* pFirstContract) {
	//step1
	if (!GetTxContacts((const unsigned char * const)pContract->hash,(void* const)pFirstContract,sizeof(FIRST_CONTRACT)))
		return false;

	//step2
	char szValue[2] = {0};
	if (!ReadDataValueDB(pContract->hash,sizeof(pContract->hash),szValue,sizeof(szValue)/sizeof(szValue[0])))
		return false;

	//step3
	ACCOUNT_ID sellID;
	if (ACCOUNT_ID_SIZE != GetAccounts((const unsigned char *)pContract->hash,(void*)sellID.accounid,sizeof(sellID))
			||!memcmp((const void*)sellID.accounid,(const void*)pFirstContract->seller.accounid,ACCOUNT_ID_SIZE) )
		return false;
	return true;
}

/**
 * @brief	step1��hash����Ч��\n
 * 			step2����һ�������Ƿ��д����
 * @param pContract
 */
bool CheckContract3(const NEXT_CONTRACT* pContract, FIRST_CONTRACT* pFirstContract) {
	//step1
	NEXT_CONTRACT contract2;
	if (!GetTxContacts((const unsigned char * const ) pContract->hash, (void* const ) &contract2, sizeof(NEXT_CONTRACT)))
		return false;

	if (!GetTxContacts((const unsigned char * const ) contract2.hash, (void* const ) pFirstContract,
			sizeof(FIRST_CONTRACT)))
		return false;

	//step2
	char szValue[2] = { 0 };
	if (!ReadDataValueDB(pContract->hash, sizeof(pContract->hash), szValue, sizeof(szValue) / sizeof(szValue[0])))
		return false;

	return true;
}

/**
 * @brief	step1��������������hash��Ч��\n
 * 			step2����Ǯ������Ч��\n
 * 			step3�������һ�������Ƿ��д���
 * @param pContract
 */
bool CheckContract4(const ARBIT_RES_CONTRACT* pContract, FIRST_CONTRACT* pFirstContract) {
	//step1
	NEXT_CONTRACT contract3;
	if (!GetTxContacts((const unsigned char * const ) pContract->hash, (void* const ) &contract3, sizeof(contract3)))
		return false;

	NEXT_CONTRACT contract2;
	if (!GetTxContacts((const unsigned char * const ) pContract->hash, (void* const ) &contract2, sizeof(NEXT_CONTRACT)))
		return false;

	if (!GetTxContacts((const unsigned char * const ) contract2.hash, (void* const ) pFirstContract,
			sizeof(FIRST_CONTRACT)))
		return false;

	//step 2
	COMP_RET ret = Int64Compare(&pContract->nMinus, &pFirstContract->nFineMoney);
	if (COMP_ERR == ret || COMP_LARGER == ret)
		return false;

	//step3
	char szValue[2] = { 0 };
	if (!ReadDataValueDB(pContract->hash, sizeof(pContract->hash), szValue, sizeof(szValue) / sizeof(szValue[0])))
		return false;
	return true;
}
/**
 * @brief step1:����ҵ����ɽ���п۳����(Ӧ֧�������ҵ�Ǯ)\n
 *        step2:����һ��������Ǯת����ҵĶ�������
 * @param vmoper �����˻��Ľṹ������
 * @param nVmSize �����С
 * @param pContract ��һ����Լ�ṹ
 */
bool OperAccount1(VM_OPERATE *vmoper,unsigned char nVmSize,const void* pContract) {
	if (NULL == vmoper || NULL == pContract || nVmSize!=2)
		return false;

	FIRST_CONTRACT* pContractData = (FIRST_CONTRACT*)pContract;
	vmoper[0].TYPE = ACOUNT_ID;
	vmoper[0].accountid = pContractData->buyer;
	vmoper[0].opeatortype = MINUS_FREE;
	vmoper[0].outheight = pContractData->nHeight;
	vmoper[0].money = pContractData->nPayMoney;

	vmoper[1].TYPE = ACOUNT_ID;
	vmoper[1].accountid = pContractData->buyer;
	vmoper[1].opeatortype = ADD_FREEZD;
	vmoper[1].outheight = pContractData->nHeight;
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
bool OperAccount2(VM_OPERATE *vmoper, unsigned short nVmSize,FIRST_CONTRACT* pFirstContract) {
	if (NULL == vmoper || NULL == pFirstContract || nVmSize != 4)
		return false;

	//step1
	vmoper[0].TYPE = ACOUNT_ID;
	vmoper[0].accountid = pFirstContract->buyer;
	vmoper[0].opeatortype = MINUS_FREEZD;
	vmoper[0].outheight = pFirstContract->nHeight;
	vmoper[0].money = pFirstContract->nPayMoney;

	vmoper[1].TYPE = ACOUNT_ID;
	vmoper[1].accountid = pFirstContract->seller;
	vmoper[1].opeatortype = ADD_FREEZD;
	vmoper[1].outheight = pFirstContract->nHeight;
	vmoper[1].money = pFirstContract->nPayMoney;

	//step 2
	vmoper[2].TYPE = ACOUNT_ID;
	vmoper[2].accountid = pFirstContract->buyer;
	vmoper[2].opeatortype = MINUS_FREE;
	vmoper[2].outheight = pFirstContract->nHeight;
	vmoper[2].money = pFirstContract->nFineMoney;

	vmoper[3].TYPE = ACOUNT_ID;
	vmoper[3].accountid = pFirstContract->seller;
	vmoper[3].opeatortype = ADD_FREEZD;
	vmoper[3].outheight = pFirstContract->nHeight;
	vmoper[3].money = pFirstContract->nFineMoney;
	return true;
}

/**
 * @brief 	���ٲ������ɽ���п۳�ָ�����õ���Ҷ������С�
 * @param vmoper
 * @param nVmSize
 * @param pContract
 */
bool OperAccount3(VM_OPERATE *vmoper, unsigned short nVmSize,FIRST_CONTRACT* pFirstContract) {
	if (NULL == vmoper || NULL == pFirstContract || nVmSize != 2)
		return false;

	vmoper[0].TYPE = ACOUNT_ID;
	vmoper[0].accountid = pFirstContract->arbitrator[0];
	vmoper[0].opeatortype = MINUS_FREE;
	vmoper[0].outheight = pFirstContract->nHeight;
	vmoper[0].money = pFirstContract->ndeposit;

	vmoper[1].TYPE = ACOUNT_ID;
	vmoper[1].accountid = pFirstContract->buyer;
	vmoper[1].opeatortype = ADD_FREEZD;
	vmoper[1].outheight = pFirstContract->nHeight;
	vmoper[1].money = pFirstContract->ndeposit;
	return true;
}

/**
 * @brief 	1.	�����ҵĶ������п۳������ҵ����ɽ��
			2.	����ҵĶ������з����ٲ��߿۸���ҵ�Ǯ
			3.	��������ɽ���п۳��ٲ������Ѹ��ٲ���
 * @param vmoper
 * @param nVmSize
 * @param pContract
 */
bool OperAccount4(VM_OPERATE *vmoper, unsigned short nVmSize,FIRST_CONTRACT* pFirstContract) {
	if (NULL == vmoper || NULL == pFirstContract || nVmSize != 2)
		return false;

	//step 1
	vmoper[0].TYPE = ACOUNT_ID;
	vmoper[0].accountid = pFirstContract->seller;
	vmoper[0].opeatortype = MINUS_FREEZD;
	vmoper[0].outheight = pFirstContract->nHeight;
	vmoper[0].money = pFirstContract->nFineMoney;

	vmoper[1].TYPE = ACOUNT_ID;
	vmoper[1].accountid = pFirstContract->buyer;
	vmoper[1].opeatortype = ADD_FREE;
	vmoper[1].outheight = pFirstContract->nHeight;
	vmoper[1].money = pFirstContract->nFineMoney;

	//step 2
	vmoper[2].TYPE = ACOUNT_ID;
	vmoper[2].accountid = pFirstContract->buyer;
	vmoper[2].opeatortype = MINUS_FREEZD;
	vmoper[2].outheight = pFirstContract->nHeight;
	vmoper[2].money = pFirstContract->ndeposit;

	vmoper[3].TYPE = ACOUNT_ID;
	vmoper[3].accountid = pFirstContract->arbitrator[0];
	vmoper[3].opeatortype = ADD_FREE;
	vmoper[3].outheight = pFirstContract->nHeight;
	vmoper[3].money = pFirstContract->ndeposit;

	//step 3
	vmoper[4].TYPE = ACOUNT_ID;
	vmoper[4].accountid = pFirstContract->buyer;
	vmoper[4].opeatortype = MINUS_FREE;
	vmoper[4].outheight = pFirstContract->nHeight;
	vmoper[4].money = pFirstContract->nFee;

	vmoper[5].TYPE = ACOUNT_ID;
	vmoper[5].accountid = pFirstContract->arbitrator[0];
	vmoper[5].opeatortype = ADD_FREE;
	vmoper[5].outheight = pFirstContract->nHeight;
	vmoper[5].money = pFirstContract->nFee;
	return true;
}

/**
 * @brief 1.	����ҵ����ɽ��ת����ҵĶ�����
 * @param data �����˻��Ľṹ��ָ��
 * @param conter ��������
 * @return
 */
bool WriteOutput1( const VM_OPERATE* data, unsigned short conter){
	return WriteOutput(data,conter);
}

/**
 * @brief	1.	����ҵ��������ת�����ҵĶ��ᡣ\n
			2.	�����ҵ����ɽ���пۿ�ת�����ҵĶ�����
 * @param data �����˻��Ľṹ��ָ��
 * @param conter ��������
 * @return
 */
bool WriteOutput2( const VM_OPERATE* data, unsigned short conter){
	return WriteOutput(data,conter);
}

/**
 * @brief	1.	���ٲ������ɽ���п۳��������õ���Ҷ������С�
 * @param data �����˻��Ľṹ��ָ��
 * @param conter ��������
 * @return
 */
bool WriteOutput3( const VM_OPERATE* data, unsigned short conter){
	return WriteOutput(data,conter);
}

/**
 * @brief	1.	���ٲ��ߵĶ���ָ�һ�������ɽ����\n
			2.	�����ҵĶ������п۳����ø����\n
			3.	�����ҵ����ɽ���п۳��ٲ÷��ø��ٲ���
 * @param data �����˻��Ľṹ��ָ��
 * @param conter ��������
 * @return
 */
bool WriteOutput4( const VM_OPERATE* data, unsigned short conter){
	return WriteOutput(data,conter);
}

/**
 * @brief	����һ�����׵Ĺ�ϣ��Ϊkeyд�����ݿ�
 * @return
 */
bool WriteDB2(const char* pKey,FIRST_CONTRACT* pFirstContract) {
	bool bFlag = true;
	return WriteDataDB((const void* const)pKey,32,&bFlag,1,pFirstContract->nHeight);
}

/**
 * @brief	���ڶ������׵Ĺ�ϣ��Ϊkeyд�����ݿ�
 * @return
 */
bool WriteDB3(const char* pKey,FIRST_CONTRACT* pFirstContract) {
	bool bFlag = true;
	return WriteDataDB((const void* const)pKey,32,&bFlag,1,pFirstContract->nHeight);
}

/**
 * @brief	�����������׵Ĺ�ϣ��Ϊkeyд�����ݿ�
 * @return
 */
bool WriteDB4(const char* pKey,FIRST_CONTRACT* pFirstContract) {
	bool bFlag = true;
	return WriteDataDB((const void* const)pKey,32,&bFlag,1,pFirstContract->nHeight);
}
/**
 * @brief �����һ������
 * @param pContract
 * @return
 */
bool ProcessContract1(const FIRST_CONTRACT* pContract) {
	if (!CheckContract1(pContract)) {
		VM_OPERATE vmOper[2];
		if (OperAccount1(vmOper, sizeof(vmOper) / sizeof(vmOper[0]), pContract))
			return WriteOutput1(vmOper, sizeof(vmOper)/sizeof(vmOper[0]));
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
	if (!CheckContract2(pContract,&firstcontract)) {
		if (OperAccount2(vmOper, sizeof(vmOper) / sizeof(vmOper[0]), &firstcontract)) {
			if (WriteDB2((const char*)pContract->hash,&firstcontract))
				return WriteOutput2(vmOper, sizeof(vmOper)/sizeof(vmOper[0]));
		}
	}
	return false;
}

/**
 * @brief ִ������
 * @param pContract ��Լ����
 * @return
 */
bool ProcessContract3(const NEXT_CONTRACT* pContract) {
	VM_OPERATE vmOper[2];
	FIRST_CONTRACT firstcontract;
	if (CheckContract3(pContract,&firstcontract)) {
		if (OperAccount3(vmOper, sizeof(vmOper) / sizeof(vmOper[0]), &firstcontract)) {
			if (WriteDB3((const char*)pContract->hash,&firstcontract))
				return WriteOutput3(vmOper, sizeof(vmOper)/sizeof(vmOper[0]));
		}
	}
	return false;
}

/**
 * @brief ִ���ٲý��
 * @param pContract �ٲý����Լ�ṹ��
 * @return
 */
bool ProcessContract4(const ARBIT_RES_CONTRACT* pContract) {
	VM_OPERATE vmOper[6];
	FIRST_CONTRACT firstcontract;
	if (CheckContract4(pContract,&firstcontract)) {
		if (OperAccount4(vmOper, sizeof(vmOper) / sizeof(vmOper[0]), &firstcontract)) {
			if (WriteDB4((const char*)pContract->hash,&firstcontract))
				return WriteOutput4(vmOper, sizeof(vmOper)/sizeof(vmOper[0]));
		}
	}
	return false;
}

int main() {
	__xdata static  char pContract[512];
	unsigned long len = 512;
	GetMemeroyData(pContract,len);

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
	switch (pContract[0]) {
	case 1:
		ProcessContract1((FIRST_CONTRACT*)pContract);
		break;

	case 2:
		ProcessContract2((NEXT_CONTRACT*)pContract);
		break;

	case 3:
		ProcessContract3((NEXT_CONTRACT*)pContract);
		break;

	case 4:
		ProcessContract4((ARBIT_RES_CONTRACT*)pContract);
		break;
	}
}










