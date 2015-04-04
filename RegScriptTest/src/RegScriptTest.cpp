#include <string.h>
#include<stdlib.h>
#include<stdio.h>
#include <stdarg.h>
#include"VmSdk.h"

#define ACCOUNT_ID_SIZE 6

enum {
	ID1_FREE_TO_ID2_FREE = 1,
	ID2_FREE_TO_ID3_FREE,
	ID3_FREE_TO_ID3_SELF,
	ID3_SELF_TO_ID2_FREE,
	ID3_FREE_TO_ID2_FREE,
	UNDEFINED_OPER
};

enum {
	ID1 = 0,
	ID2 = 1,
	ID3 = 2,
	UNDEFINE_ROLE,
};
typedef struct {
	unsigned char nType;
	ACCOUNT_ID vregID[3];
	long nHeight;
	Int64 nPay;
} CONTRACT_DATA;

void PrintString(const char *format, ...) {
	va_list arg;
	static char tmp[255];
	va_start(arg, format);
	vsprintf(tmp, format, arg);
	va_end(arg);

	LogPrint(tmp, strlen(tmp), STRING);
}

bool OperMoney(CONTRACT_DATA* pData,VM_OPERATE* pOperate,unsigned char nFrom,
		OperType emFromType,unsigned char nTo,OperType emToType) {
	if (nFrom>=UNDEFINE_ROLE || nTo>=UNDEFINE_ROLE || NULL == pData || NULL == pOperate
			|| emFromType>=UNDEFINED_OPER || emToType >=UNDEFINED_OPER) {
		PrintString("ERROR..............");
		return false;
	}

	unsigned long nHeight = GetCurRunEnvHeight() + pData->nHeight;
	memcpy(pOperate[0].accountid, pData->vregID[nFrom].accounid, ACCOUNT_ID_SIZE);
	pOperate[0].opeatortype = emFromType;
	pOperate[0].outheight = nHeight;
	pOperate[0].money = pData->nPay;

	memcpy(pOperate[1].accountid, pData->vregID[nTo].accounid, ACCOUNT_ID_SIZE);
	pOperate[1].opeatortype = emToType;
	pOperate[1].outheight = nHeight;
	pOperate[1].money = pData->nPay;
	PrintString("Money in OperMoney:");
	PrintString("from is %d,to is %d:",nFrom,nTo);
	LogPrint(pOperate[0].accountid,6,HEX);
	LogPrint(pOperate[1].accountid,6,HEX);
	LogPrint(pData->nPay.data,8,HEX);
	return true;
}

int main() {
	__xdata static char pContract[512];
	unsigned long len = 512;
	int nRet = GetCurTxContact(pContract, len);
	PrintString("nRet is %d,data size is %d", nRet,sizeof(CONTRACT_DATA));

	CONTRACT_DATA* pData = (CONTRACT_DATA*)pContract;

	if (!IsRegID((const void* const ) pData->vregID[0].accounid)
			|| !IsRegID((const void* const ) pData->vregID[1].accounid)
			|| !IsRegID((const void* const ) pData->vregID[1].accounid))
		return false;

//	char szUserData[2] = { 0 };
//	nRes = GetAuthUserDefine((const void* const ) pData->vregID[0].accounid, szUserData,
//			sizeof(szUserData) / sizeof(szUserData[0]));
//	if (!nRes || 1 != szUserData[0])
//		return false;
//	else
//
//
//	nRes = GetAuthUserDefine((const void* const ) pData->vregID[2].accounid, szUserData,
//			sizeof(szUserData) / sizeof(szUserData[0]));
//	if (!nRes || 3 != szUserData[0])
//		return false;


	bool bRes = false;
	VM_OPERATE vmoper[2] = { 0 };
	switch (pData->nType) {
	case 1: {
		PrintString("222");
		if (!OperMoney(pData,vmoper,ID1,MINUS_FREE,ID2,ADD_FREE)) {
			break;
		}
		bRes = WriteOutput(vmoper, sizeof(vmoper) / sizeof(vmoper[0]));
	}
		break;

	case 2: {
		PrintString("333");
		if (!OperMoney(pData, vmoper, ID2, MINUS_FREE, ID3, ADD_FREE)) {
			break;
		}
		bRes = WriteOutput(vmoper, sizeof(vmoper) / sizeof(vmoper[0]));
	}
		break;

	case 3: {
		PrintString("444");
		if (!OperMoney(pData, vmoper, ID3, MINUS_FREE, ID3, ADD_SELF)) {
			break;
		}
		bRes = WriteOutput(vmoper, sizeof(vmoper) / sizeof(vmoper[0]));
	}
		break;

	case 4: {
		PrintString("555");
		if (!OperMoney(pData, vmoper, ID3, MINUS_SELF, ID2, ADD_FREE)) {
			break;
		}
		bRes = WriteOutput(vmoper, sizeof(vmoper) / sizeof(vmoper[0]));
	}
		break;

	case 5: {
		PrintString("666");
		if (!OperMoney(pData, vmoper, ID3, MINUS_FREE, ID2, ADD_FREE)) {
			break;
		}
		bRes = WriteOutput(vmoper, sizeof(vmoper) / sizeof(vmoper[0]));
	}
	default:
		break;
	}

	PrintString("777.....res is %d",bRes);

	if (bRes) {
		__VmExit(RUN_SCRIPT_OK);
	} else {
		__VmExit(RUN_SCRIPT_DATA_ERR);
	}
	return 0;
}

















