#include"unit_test.h"

enum CALL_API_FUN {
	SUBMIT_ORDER= 0,
	CANCEL_ORDER,
};

bool CancelOrder(){
	char account[6] = {0};
	if (!GetCurTxAccount(account, 6)) {
		LogPrint("can account not exit", sizeof("can account not exit"), STRING);
		return false;
	}
	char scriptid[6] = {0};
	GetScriptID(scriptid);

	Int64 paymoney;
	Int64Inital(&paymoney,"\x00", 1);
	QueryAccountBalance((const unsigned char* )scriptid,&paymoney);

	VM_OPERATE ret;
	memcpy(ret.accountid,scriptid,sizeof(ret.accountid));
	ret.opeatortype = MINUS_FREE;
	ret.outheight = 10;
	memcpy(&ret.money,&paymoney,sizeof(Int64));
	WriteOutput(&ret,1);

	memcpy(ret.accountid,account,sizeof(account));
	ret.opeatortype = ADD_FREE;
	WriteOutput(&ret,1);

	return true;
}

bool ProcessTest(char*pcontact)
{
	switch(pcontact[0])
	{
		case 0x14:{
			break;
		}
		case 0x15:{
			if(!CancelOrder()){
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}
		default:
			{
				__VmExit(RUN_SCRIPT_DATA_ERR);
				break;
			}
	}
	__VmExit(RUN_SCRIPT_OK);
}
