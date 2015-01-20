
#include "unit_test.h"
void processTx(unsigned char*account,OperType type)
{
	VM_OPERATE ret;
	memcpy(ret.accountid,account,sizeof(ret.accountid));
	ret.opeatortype = type;
	ret.outheight = 10;
	Int64Inital(&ret.money,"\x27\x10",2);
	WriteOutput(&ret,1);

	char accountid[6] = {0};
	GetCurTxAccount(accountid,6);
	memcpy(ret.accountid,accountid,sizeof(accountid));
	if(type == ADD_FREE)
	{
		ret.opeatortype = MINUS_FREE;
	}
	else{
		ret.opeatortype = ADD_FREE;
	}
	WriteOutput(&ret,1);
}

bool ProcessScript(char*pcontact)
{
	char account[6];
	memcpy(&account,&pcontact[1],sizeof(account));

	switch(pcontact[0])
	{
		case 0x0d:
				{
					processTx((unsigned char*)account,ADD_FREE);
					break;
				}
		case 0x0e:
				{
					processTx((unsigned char*)account,MINUS_FREE);
					break;
				}

	}
	__exit(RUN_SCRIPT_OK);
	  return true;
}
