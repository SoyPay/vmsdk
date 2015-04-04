#include"unit_test.h"


bool WriteAcct(){

	Int64 paymoey;
	Int64Inital(&paymoey,"\x00",1);

	unsigned long nheight = GetCurRunEnvHeight();
	char account[6]={0};
	if(!GetCurTxAccount(account,sizeof(account))){
		LogPrint("get aacount failed",strlen("get aacount failed")+1,STRING);
		return false;
	}
	 if(!GetCurPayAmount(&paymoey)){
		 LogPrint("get aacount money failed",strlen("get aacount money failed")+1,STRING);
		 return false;
	 }

	Int64 div,result;
	Int64Inital(&div,"\x05",1);
	Int64Inital(&result,"\x00",1);
	Int64Div(&paymoey, &div, &result);

	APP_ACC_OPERATE operatearray[5];
	APP_ACC_OPERATE useracc;
	S_APP_ID id;
	id.idlen = sizeof(account);
	memcpy(&id.ID,account,sizeof(account));

	memcpy(&useracc.AppAccID,&id,sizeof(S_APP_ID));

	memcpy(&useracc.mMoney,&result,sizeof(Int64));
	useracc.opeatortype = ADD_FREE_OP;

	/// add free
	operatearray[0] = useracc;

	useracc.opeatortype = ADD_FREEZED_OP;
	S_APP_ID tag;
	char ptag[4];
	tag.idlen = 4;
	useracc.outheight = nheight;

	for(int i = 1;i< 5;i++){
		sprintf(ptag,"%0d",i);
		memcpy(&tag.ID,ptag,sizeof(ptag));
		memcpy(&useracc.FundTag,&tag,sizeof(tag));
		useracc.outheight += 1;
		/// add freezed
		operatearray[i] = useracc;
	}

	WriteAppOperateOutput( operatearray, 5);

//	/// 验证写的数据是否正确
//	Int64 pRet;
//	Int64Inital(&pRet,"\x00",1);
//	GetUserAppAccFreeValue(&pRet,&id);
//	TestCheck(Int64Compare(&pRet,&result) == COMP_EQU);
//
//	useracc.outheight = nheight;
//	for(int i = 1;i< 5;i++){
//		sprintf(ptag,"%0d",i);
//		memcpy(&tag.ID,ptag,sizeof(ptag));
//		useracc.outheight += 1;
//		TestCheck(Int64Compare(&pRet,&result) == COMP_EQU);
//	}

	return true;
}

bool checkfreezedata(int count,Int64 paymoey){

	char account[6]={0};
	if(!GetCurTxAccount(account,sizeof(account))){
		LogPrint("get aacount failed",strlen("get aacount failed")+1,STRING);
		return false;
	}

	S_APP_ID id;
	APP_ACC_OPERATE pAppID;
	id.idlen = sizeof(account);
	memcpy(&id.ID,account,sizeof(account));
	memcpy(&pAppID.AppAccID,&id,sizeof(S_APP_ID));

	Int64 mul;
	memcpy(&mul,&count,sizeof(int));
	Int64Mul(&paymoey,&mul,&paymoey);

	Int64 pRet;
	Int64Inital(&pRet,"\x00",1);
	GetUserAppAccFreeValue(&pRet,&id);
	TestCheck(Int64Compare(&pRet,&paymoey) == COMP_EQU);

	unsigned long outheight = GetCurRunEnvHeight();

	S_APP_ID tag;
	char ptag[4];
	tag.idlen = 4;
	pAppID.outheight = outheight;

	for(int i = count;i <5;i++){
		sprintf(ptag,"%0d",i);
		memcpy(&tag.ID,ptag,sizeof(ptag));
		memcpy(&pAppID.FundTag,&tag,sizeof(tag));
		pAppID.outheight += 1;
		GetUserAppAccFoudWithTag(&pRet,&pAppID);
		TestCheck(Int64Compare(&pRet,&paymoey) == COMP_EQU);
	}

	return true;
}

bool minuxfreeValue(Int64 paymoey,bool flag){
	char account[6]={0};
		if(!GetCurTxAccount(account,sizeof(account))){
			LogPrint("get aacount failed",strlen("get aacount failed")+1,STRING);
			return false;
		}

		if(flag == true){
			Int64 mul;
			Int64Inital(&mul,"\x05",1);
			Int64Mul(&paymoey,&mul,&paymoey);
		}
		S_APP_ID id;
		APP_ACC_OPERATE pAppID;
		id.idlen = sizeof(account);
		memcpy(&id.ID,account,sizeof(account));
		memcpy(&pAppID.AppAccID,&id,sizeof(S_APP_ID));
		memcpy(&pAppID.mMoney,&paymoey,sizeof(Int64));
		pAppID.opeatortype = SUB_FREE_OP;

		WriteAppOperateOutput(&pAppID, 1);
		return true;
}

bool ProcessAppAcc(char*pcontact)
{
	switch(pcontact[0])
	{
		case 0x16:{
			LogPrint("16",strlen("16")+1,STRING);
			if(!WriteAcct()){
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}
		case 0x17:
		case 0x18:
		case 0x19:
		case 0x1a:
		{
			Int64 paymoey;
			int count;
			memcpy(&count,&pcontact[0],sizeof(char));
			memcpy(&paymoey,&pcontact[1],sizeof(Int64));
			if(!checkfreezedata((count-21),paymoey)){
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}
		case 0x1b:{
			Int64 paymoey;
			memcpy(&paymoey,&pcontact[1],sizeof(Int64));
			if(!minuxfreeValue(paymoey,false)){
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}
		case 0x1c:{
			Int64 paymoey;
			memcpy(&paymoey,&pcontact[1],sizeof(Int64));
			if(!minuxfreeValue(paymoey,true)){
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
