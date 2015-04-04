/*
 * main.cpp
 *
 *  Created on: Nov 26, 2014
 *      Author: spark.huang
 */
#include <string.h>
#include<stdlib.h>
#include <stdio.h>
#include"VmSdk.h"

#define min_out_height  10
typedef struct {
	unsigned char nType;
	unsigned char noperateType;
	Int64 money;
	unsigned int hight;
	unsigned char dhash[32];//32 + one byte checksum
}SEND_DATA;

typedef struct {
	unsigned char betstate;
	Int64 money;
	unsigned int hight;
	unsigned char dhash[32];//32 + one byte checksum
	unsigned char accepthash[32];//32 + one byte checksum
	ACCOUNT_ID sendbetid;
	ACCOUNT_ID acceptbetid;
	unsigned char acceptebetdata;

}DB_DATA;

typedef struct {
	unsigned char nType;
	unsigned char noperateType;
	Int64 money;
	unsigned char acceptdata;
	unsigned char txhash[32];
}ACCEPT_DATA;

typedef struct {
	unsigned char nType;
	unsigned char noperateType;
	unsigned char txhash[32];
	unsigned char senddata[33];
}OPEN_DATA;

enum TXTYPE{
	TX_SENDBET = 0x01,
	TX_ACCEPTBET = 0x02,
	TX_OPENBET = 0x03,
	TX_WITHDRAWAL = 0x04,
};
enum OPERATE{
	OP_SYSTEMACC = 0x00,
	OP_APPACC = 0x01,
	OP_NULL = 0x02,
};

bool CheckAccVaild(unsigned char type,Int64 money){

	Int64 paymoey;
	Int64Inital(&paymoey,"\x00",1);
	 if(!GetCurPayAmount(&paymoey)){
		 return false;
	 }
	 switch(type){
	    case OP_SYSTEMACC:{ //// 从脚本账户中扣金额 检查金额是否和合约中的金额一致
	    	 if( Int64Compare(&money, &paymoey) != COMP_EQU){
	    		 return false;
	    	 }
	    	break;
	    }
	    case OP_APPACC:{//// 从应用账户中扣金额 检查 检查应用账户中是否有足够的金额，已经是系统账户中没有扣钱
	    	char account[6];
	    	if(!GetCurTxAccount(account,sizeof(account))){
	    		return false;
	    	}
			Int64 compare;
			Int64Inital(&compare,"\x00",1);
			if(Int64Compare(&compare, &paymoey) != COMP_EQU){  /// 不能从系统账户扣钱
			 return false;
			}

			S_APP_ID id;
			id.idlen = sizeof(account);
			memcpy(&id.ID,account,sizeof(account));

			Int64 pRet;
			Int64Inital(&pRet,"\x00",1);
			GetUserAppAccFreeValue(&pRet,&id);
	    	 if(Int64Compare(&pRet, &money) == COMP_LESS){      //系统应用账户中要有足够的自由金额
	    		 return false;
	    	 }
	    	break;
	    }
	    case OP_NULL:{
	   	    	return false;
	   	    }
		default:
				{
					return false;
				}
	    }
	 return true;
}
bool CheckSendBetPackage(const SEND_DATA* const pContract){
	if(pContract->hight < min_out_height){
		return false;
	}

	if(!CheckAccVaild(pContract->noperateType,pContract->money)){
		return false;
	}
	return true;
}

bool WriteSendBetPakage(const DB_DATA* const pContract,const char*const txhash,char type){

	APP_ACC_OPERATE writecode[2];
	int count = type == OP_APPACC?2:1;
	S_APP_ID id;
	APP_ACC_OPERATE pAppID;
	id.idlen = sizeof(pContract->sendbetid);
	memcpy(&id.ID,&pContract->sendbetid,sizeof(pContract->sendbetid));
	memcpy(&pAppID.AppAccID,&id,sizeof(S_APP_ID));
	memcpy(&pAppID.mMoney,&pContract->money,sizeof(Int64));

	S_APP_ID tag;
	tag.idlen = 32;
	pAppID.outheight = GetCurRunEnvHeight() + pContract->hight;;
	memcpy(&tag.ID,txhash,32);
	memcpy(&pAppID.FundTag,&tag,sizeof(S_APP_ID));

	pAppID.opeatortype = ADD_FREEZED_OP;
	writecode[0] = pAppID;
	if(type == OP_APPACC){
		pAppID.opeatortype=SUB_FREE_OP;
		writecode[1] = pAppID;
	}
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)&writecode, count)){
		return false;
	}

	return true;
}
bool SendBetPackage(const SEND_DATA* const pContract){

	if(!CheckSendBetPackage(pContract)){
		LogPrint("check SendBetPackage error",sizeof("check SendBetPackage error")+1,STRING);
				return false;
	}

	char account[6];
	if(!GetCurTxAccount(account,sizeof(account))){
		return false;
	}

	char txhash[32];
	if(!GetCurTxHash(txhash)){
		return false;
	}
	DB_DATA dbdata;
	dbdata.betstate = 0x00;
	memcpy(&dbdata.money,&pContract->money,sizeof(dbdata.money));
	memcpy(&dbdata.dhash,&pContract->dhash,sizeof(dbdata.dhash));
	memcpy(&dbdata.sendbetid,&account,sizeof(dbdata.sendbetid));
	dbdata.hight = GetCurRunEnvHeight() + pContract->hight;
	if( !WriteData(txhash,sizeof(txhash),&dbdata,sizeof(dbdata))){
		return false;
	}
	if(!WriteSendBetPakage(&dbdata,txhash,pContract->noperateType)){
		return false;
	}

	return true;
}

bool CheckAcceptBetPackage(const ACCEPT_DATA* const pContract,Int64 sendmoney,unsigned char betsate){
	if(!CheckAccVaild(pContract->noperateType,pContract->money)){
		return false;
	}
	Int64 paymoey;
	Int64Inital(&paymoey,"\x00",1);
	 if(!GetCurPayAmount(&paymoey)){
		 return false;
	 }
	 if(Int64Compare(&paymoey, &sendmoney) != COMP_EQU){
		 return false;
	 }
	 if(pContract->acceptdata != 0x00 && pContract->acceptdata != 0x01){
		 return false;
	 }
     if(betsate&0x01 == 0x01){
    	 return  false;      //已经被接赌
     }
	 return true;
}

bool WritAcceptBetPakage(const DB_DATA* const pContract,const char*const txhash,unsigned char type){

	APP_ACC_OPERATE writecode[3];
	int count = type == OP_APPACC?3:2;
	S_APP_ID id;
	APP_ACC_OPERATE pAppID;
	id.idlen = sizeof(pContract->sendbetid);
	memcpy(&id.ID,&pContract->sendbetid,sizeof(pContract->sendbetid));
	memcpy(&pAppID.AppAccID,&id,sizeof(S_APP_ID));
	memcpy(&pAppID.mMoney,&pContract->money,sizeof(Int64));

	S_APP_ID tag;
	tag.idlen = 32;
	pAppID.outheight = GetTxConFirmHeight(txhash)  + pContract->hight;
	memcpy(&tag.ID,txhash,32);
	memcpy(&pAppID.FundTag,&tag,sizeof(tag));
	pAppID.opeatortype = SUB_FREEZED_OP;

	writecode[0]=pAppID;

	memcpy(&id.ID,&pContract->acceptbetid,sizeof(pContract->acceptbetid));
	memcpy(&pAppID.AppAccID,&id,sizeof(S_APP_ID));
	pAppID.outheight = GetCurRunEnvHeight()  + pContract->hight;
	Int64 mul;
	Int64Inital(&mul,"\x02",1);
	Int64Mul(&pAppID.mMoney, &mul,&pAppID.mMoney);
	pAppID.opeatortype = ADD_FREEZED_OP;

	writecode[1]=pAppID;
	if(type == OP_APPACC){
		pAppID.opeatortype=SUB_FREE_OP;
		writecode[2]=pAppID;
	}
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)&writecode, count)){
		return false;
	}

	return true;
}

bool AcceptBetPackage(const ACCEPT_DATA* const pContract){

	DB_DATA dbdata;
	if(!ReadData(pContract->txhash,sizeof(pContract->txhash), &dbdata,sizeof(dbdata))){
		LogPrint("AcceptBetPackage read db error",sizeof("AcceptBetPackage read db error")+1,STRING);
		LogPrint(pContract->txhash,32,HEX);
		return false;
	}
	char account[6];
	if(!GetCurTxAccount(account,sizeof(account))){

		return false;
	}

	char acceptehash[32];
	if(!GetCurTxHash(acceptehash)){
		return false;
	}
	memcpy(&dbdata.acceptbetid,&account,sizeof(dbdata.acceptbetid));
	memcpy(&dbdata.accepthash,&acceptehash,sizeof(dbdata.accepthash));
	dbdata.acceptebetdata = pContract->acceptdata;

	if(!CheckAcceptBetPackage(pContract,dbdata.money,dbdata.betstate)){
		return false;
	}
	if(!WritAcceptBetPakage((DB_DATA*)&dbdata,(char*)pContract->txhash,pContract->noperateType)){
		return false;
	}
	///表示接赌
	dbdata.acceptebetdata = 0x01;
	if( !WriteData(pContract->txhash,sizeof(pContract->txhash),&dbdata,sizeof(dbdata))){
		LogPrint(pContract->txhash,32,HEX);
		return false;
	}
	return true;
}
bool CheckOpenBetPackage(const OPEN_DATA* const pContract,const DB_DATA* const dbdata){
	char account[6];
	if(!GetCurTxAccount(account,sizeof(account))){
		return false;
	}

	if(strcmp(account,(char*)&dbdata->sendbetid) != 0){
		 LogPrint("263",sizeof("188")+1,STRING);
		return false;
	}

	unsigned char checkhash[32];
	unsigned char senddata = pContract->senddata[32];
	//开奖数字在1-6之间
	if(senddata > 6){
		 LogPrint("271",sizeof("271")+1,STRING);
		return false;
	}
	SHA256(pContract->senddata, sizeof(pContract->senddata), checkhash);
	if(memcmp(checkhash,dbdata->dhash,sizeof(dbdata->dhash)) != 0){
		return false;
	}
    if(dbdata->betstate&0x01 != 0x01){
   	 return  false;      //未被接赌
    }

    if((GetTxConFirmHeight(dbdata->accepthash) + dbdata->hight)<GetCurRunEnvHeight()){ /// 已经超时不能揭赌
    	return false;
    }
	return true;
}
bool WriteOpenBetAppAcc(const OPEN_DATA* const pContract,const DB_DATA *const dbdata,bool flag){
	APP_ACC_OPERATE writecode[2];
	S_APP_ID id;
	APP_ACC_OPERATE pAppID;
	id.idlen = sizeof(dbdata->sendbetid);
	memcpy(&id.ID,&dbdata->sendbetid,sizeof(dbdata->sendbetid));
	memcpy(&pAppID.AppAccID,&id,sizeof(S_APP_ID));
	Int64 mul;
	Int64Inital(&mul,"\x02",1);
	Int64Mul(&dbdata->money, &mul,&pAppID.mMoney);

	S_APP_ID tag;
	tag.idlen = 32;
	pAppID.outheight = GetTxConFirmHeight(dbdata->accepthash) + dbdata->hight;
	memcpy(&tag.ID,pContract->txhash,sizeof(pContract->txhash));
	memcpy(&pAppID.FundTag,&tag,sizeof(tag));
	if(pContract->noperateType == OP_APPACC){
		if(flag){  /// 从接赌约的冻结应用账户打钱到发赌约的账户
			pAppID.opeatortype = ADD_FREE_OP;
			writecode[0]=pAppID;
			memcpy(&id.ID,&dbdata->acceptbetid,sizeof(dbdata->acceptbetid));
			memcpy(&pAppID.AppAccID,&id,sizeof(S_APP_ID));
			pAppID.opeatortype = SUB_FREEZED_OP;
			writecode[1]=pAppID;
			}else{
				memcpy(&id.ID,&dbdata->acceptbetid,sizeof(dbdata->acceptbetid));  /// 接赌的人赢
				memcpy(&pAppID.AppAccID,&id,sizeof(S_APP_ID));
				pAppID.opeatortype = ADD_FREE_OP;
				writecode[0]=pAppID;
				pAppID.opeatortype = SUB_FREEZED_OP;
				writecode[1]=pAppID;
			}
		if(!WriteAppOperateOutput((APP_ACC_OPERATE*)&writecode, 2)){
			return false;
		}
	}else if(pContract->noperateType == OP_SYSTEMACC){
		memcpy(&id.ID,&dbdata->acceptbetid,sizeof(dbdata->acceptbetid));
		memcpy(&pAppID.AppAccID,&id,sizeof(S_APP_ID));
		pAppID.opeatortype = SUB_FREEZED_OP;
		if(!WriteAppOperateOutput((APP_ACC_OPERATE*)&pAppID, 1)){
					return false;
		}
	}

	return true;
}
bool WriteOpenBetSysAcc(const OPEN_DATA* const pContract,const DB_DATA *const dbdata,bool flag){

	VM_OPERATE writeCode[2];
	VM_OPERATE ret;
	if(flag){
		memcpy(ret.accountid,&dbdata->sendbetid,sizeof(ret.accountid));
	}else{
		memcpy(ret.accountid,&dbdata->acceptbetid,sizeof(ret.accountid));
	}
	memcpy(&ret.money,&dbdata->money,sizeof(ret.money));
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
bool WriteOpenBetPackage(const OPEN_DATA* const pContract,const DB_DATA *const dbdata,bool flag){
	/// 表示发赌约的人赢

	if(pContract->noperateType ==OP_APPACC){
		if(WriteOpenBetAppAcc(pContract,dbdata,flag))
			return true;
	}else if(pContract->noperateType ==OP_SYSTEMACC){
		if(!WriteOpenBetAppAcc(pContract,dbdata,flag))
			return false;
		if(WriteOpenBetSysAcc(pContract,dbdata,flag)){
			return true;
		}
	}

	return false;
}
bool OpenBetPackage(const OPEN_DATA* const pContract){

	DB_DATA dbdata;
	if(!ReadData(pContract->txhash,sizeof(pContract->txhash), &dbdata,sizeof(dbdata))){
		LogPrint("OpenBetPackage read db error",sizeof("OpenBetPackage read db error")+1,STRING);
		LogPrint(pContract->txhash,32,HEX);
		return false;
	}

	if(!CheckOpenBetPackage(pContract,(DB_DATA*)&dbdata)){
		return false;
	}
	char winnum = pContract->senddata[32];
	LogPrint("winnum:",sizeof("winnum:"),STRING);
	LogPrint(&winnum,1,HEX);
    unsigned char flagbig = 0x01;   /// 初始表示数字是大
	if(0<winnum && winnum<=3){
		flagbig= 0x00;          // 表示猜小是赢
	}
	bool wflag  =true;             ///初始表示发赌约者赢
	if(dbdata.acceptebetdata == flagbig){
		wflag  =false;         //// 表示猜赌的人赢
		LogPrint("winb:",sizeof("winb:"),STRING);
		LogPrint(&dbdata.acceptebetdata,1,HEX);
		LogPrint(&dbdata.acceptbetid,sizeof(dbdata.acceptbetid),HEX);
	}else{
		LogPrint("winA:",sizeof("winA:"),STRING);
		LogPrint(&dbdata.sendbetid,sizeof(dbdata.sendbetid),HEX);
	}

	if(!WriteOpenBetPackage(pContract,&dbdata,wflag)){
		return false;
	}
	if(!DeleteData(pContract->txhash,sizeof(pContract->txhash))){
		return false;
	}
	return true;
}
bool WriteWithDrawal(const char *account,Int64 money){

	VM_OPERATE writeCode[2];
	VM_OPERATE ret;
	memcpy(ret.accountid,&account,sizeof(ret.accountid));
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
bool WithDrawal(){
	char account[6];
	if(!GetCurTxAccount(account,sizeof(account))){
		return false;
	}
	S_APP_ID id;
	id.idlen = sizeof(account);
	memcpy(&id.ID,&account,sizeof(account));
	Int64 pRet;
	if(!GetUserAppAccFreeValue(&pRet,&id)){
		return false;
	}
	Int64 compare;
	Int64Inital(&compare,"\x00",1);
	if(Int64Compare(&pRet, &pRet) == COMP_EQU){
		   return false;
	}
        
        return true;
}
int main()
{
	 __xdata static  char pcontact[100];
		unsigned long len = 100;
		GetCurTxContact(pcontact,len);
	 	switch(pcontact[0])
	 	{
			case TX_SENDBET:
			{
				LogPrint("SendBetPackage",sizeof("SendBetPackage"),STRING);
				if(!SendBetPackage((SEND_DATA*)pcontact))
				{
					LogPrint("SendBetPackage error",sizeof("SendBetPackage error"),STRING);
					__exit(RUN_SCRIPT_DATA_ERR);
				}
				LogPrint("SendBetPackage end",sizeof("SendBetPackage end"),STRING);
				break;
			}
			case TX_ACCEPTBET:
			{
				LogPrint("AcceptBetPackage",sizeof("AcceptBetPackage"),STRING);
				if(!AcceptBetPackage((ACCEPT_DATA*) pcontact))
				{
					LogPrint("AcceptBetPackage error",sizeof("AcceptBetPackage error"),STRING);
					__exit(RUN_SCRIPT_DATA_ERR);
				}
				break;
			}
			case  TX_OPENBET:
			{
				LogPrint("OpenBetPackage",sizeof("OpenBetPackage"),STRING);
				if(!OpenBetPackage((OPEN_DATA* )pcontact))
				{
					LogPrint("OpenBetPackage error",sizeof("OpenBetPackage error"),STRING);
					__exit(RUN_SCRIPT_DATA_ERR);
				}
				break;
			}
			case  TX_WITHDRAWAL:
			{
				LogPrint("WithDrawal",sizeof("WithDrawal"),STRING);
				if(!WithDrawal())
				{
					LogPrint("WithDrawal error",sizeof("WithDrawal error"),STRING);
					__exit(RUN_SCRIPT_DATA_ERR);
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

