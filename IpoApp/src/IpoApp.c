
#include <string.h>
#include<stdlib.h>
#include <stdio.h>
#include"VmSdk.h"

#define add_moth      1440l*30l
enum GETDAWEL{
	TX_REGID = 0x01,
	TX_BASE58 = 0x02,
};
typedef struct {
	unsigned char address[35];
	Int64 money;
	Int64 freemoney;
	Int64 freeMothmoney;
}IPO_USER;

const static char SendAccount[6]="\x00\x00\x00\x00\x14\x00";
bool WriteWithDrawal(const char *account,Int64 money){
	VM_OPERATE writeCode[2];
	VM_OPERATE ret;
	ret.type = BASE_58_ADDR;
	memcpy(ret.accountid,account,sizeof(ret.accountid));
	memcpy(&ret.money,&money,sizeof(Int64));

	ret.opeatortype = ADD_FREE;
	writeCode[0]=ret;

	char accountid[6] = {0};
	if(!GetScriptID(&accountid)){
		return false;
	}

	ret.opeatortype = MINUS_FREE;
	memcpy(ret.accountid,&accountid,sizeof(ret.accountid));
	writeCode[1]=ret;
	WriteOutput((VM_OPERATE*)&writeCode,2);
	return true;
}
bool WithDrawal(char type){

	if(type != TX_REGID&&type != TX_BASE58)
		return false;
	char account[6];
	if(!GetCurTxAccount(account,sizeof(account))){
		return false;
	}
	S_APP_ID id;
	if(type == TX_REGID){
		id.idlen = sizeof(account);
		memcpy(&id.ID,&account,sizeof(account));
	}else if(type == TX_BASE58){
		char dacrsaddr[34];
		if(!GetDacrsAddress(account,sizeof(account),dacrsaddr,sizeof(account)))
			return false;
		id.idlen = sizeof(dacrsaddr);
		memcpy(&id.ID,&dacrsaddr,sizeof(dacrsaddr));
	}
	Int64 pRet;
	if(!GetUserAppAccFreeValue(&pRet,&id)){
		return false;
	}
	Int64 compare;
	Int64Inital(&compare,"\x00",1);
	if(Int64Compare(&pRet, &compare) == COMP_EQU){
		   return false;
	}
  if(!WriteWithDrawal(account,pRet)){
	  return false;
  }
   return true;
}

//// 充值
bool Recharge(IPO_USER *ipouser)
{
	char account[6];
	if(!GetCurTxAccount(account,sizeof(account))){
		return false;
	}

	if(memcmp(account,SendAccount,sizeof(account)) != 0)
	{
		LogPrint("账户不正确",sizeof("账户不正确")+1,STRING);
		return false;
	}
	Int64 paymoey;
	Int64Inital(&paymoey,"\x00",1);
	 if(!GetCurTxPayAmount(&paymoey)){
		 return false;
	 }
	 if(Int64Compare(&ipouser->money, &paymoey) != COMP_EQU){
		 LogPrint("充值金额不正确",sizeof("充值金额不正确")+1,STRING);
	 	  return false;
	 }

	 VM_OPERATE ret;
	 ret.type = BASE_58_ADDR;
	 ret.opeatortype = ADD_FREE;

	 //// 打钱到自由金额
	 memcpy(ret.accountid,&ipouser->address,sizeof(ret.accountid));
	 memcpy(&ret.money,&ipouser->freemoney,sizeof(Int64));
	 WriteOutput(&ret,1);

	 unsigned long curheight = GetCurRunEnvHeight();

	S_APP_ID id;
	APP_ACC_OPERATE pAppID;
	id.idlen = sizeof(ipouser->address)-1;
	memcpy(&id.ID,&ipouser->address,sizeof(ipouser->address));
	memcpy(&pAppID.AppAccID,&id,sizeof(S_APP_ID));
	memcpy(&pAppID.mMoney,&ipouser->freeMothmoney,sizeof(Int64));
	pAppID.FundTag.idlen = 0 ;
	pAppID.opeatortype = ADD_FREEZED_OP;
	 for(char month = 0;month<36;month++)
	 {
		pAppID.outheight = curheight  + add_moth*(month+1);
		if(!WriteAppOperateOutput(&pAppID, 1)){
					return false;
				}
	 }

	char accountid[6] = {0};
	if(!GetScriptID(&accountid)){
		return false;
	}

	ret.type = REG_ID;
	ret.opeatortype = MINUS_FREE;
	memcpy(&ret.money,&ipouser->freemoney,sizeof(Int64));
	memcpy(ret.accountid,&accountid,sizeof(accountid));
	WriteOutput(&ret,1);
	return true;
}
 //__xdata static  char pcontact[100] ="\xff\x02\x64\x75\x4b\x66\x4e\x79\x71\x36\x7a\x73\x75\x79\x32\x43\x4d\x47\x4d\x46\x56\x72\x51\x4c\x75\x47\x69\x39\x35\x55\x43\x37\x77\x36\x44\x56\x10\x27\x00\x00\x00\x00\x00\x00\xc8\x00\x00\x00\x00\x00\x00\x00\x16\x00\x00\x00\x00\x00\x00\x00\x00";
int main( void )
{

	__xdata static  char pcontact[100] ;
	 unsigned long len = 100;
	GetCurTxContact(pcontact,len);
	switch(pcontact[0])
	{
		case  0xff:
		{
			if(pcontact[1] != 0x01 && pcontact[1] != 0x02){
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			if(pcontact[1] == 0x01)
			{
				LogPrint("WithDrawal",sizeof("WithDrawal"),STRING);
				if(!WithDrawal(pcontact[2]))
				{
					LogPrint("WithDrawal error",sizeof("WithDrawal error"),STRING);
					__VmExit(RUN_SCRIPT_DATA_ERR);
				}
			}
			else if(pcontact[1] == 0x02)
			{
				LogPrint("Recharge",sizeof("Recharge"),STRING);
				IPO_USER user;
				memcpy(&user,&pcontact[2],sizeof(IPO_USER));
				if(!Recharge((IPO_USER*)&user))
				{
					LogPrint("Recharge error",sizeof("Recharge error"),STRING);
					__VmExit(RUN_SCRIPT_DATA_ERR);
				}
			}
			break;
		}
		default:
		{
			LogPrint("tx format error",sizeof("tx format error"),STRING);
			__VmExit(RUN_SCRIPT_DATA_ERR);
			break;
		}
	}
	__VmExit(RUN_SCRIPT_OK);
	return 0;
}
