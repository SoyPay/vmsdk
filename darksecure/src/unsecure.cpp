#include <string.h>
#include<stdlib.h>
#include <stdio.h>
#include"VmSdk.h"


#define ACCOUNT_ID_SIZE 6
#define HASH_SIZE	32


typedef struct  {
	unsigned char nType;     //!<类型
	ACCOUNT_ID seller;       //!<卖家ID（采用6字节的账户ID）
} FIRST_CONTRACT;

typedef struct {
	unsigned char nType;				//!<交易类型
	unsigned char hash[HASH_SIZE];		//!<上一个交易包的哈希
}NEXT_CONTRACT;
typedef struct  {
	bool flag;
	ACCOUNT_ID 	buyer;						//!<买家ID（采用6字节的账户ID）
	ACCOUNT_ID seller;       //!<卖家ID（采用6字节的账户ID）
	Int64 nPayMoney;
} DATA_DB;
enum TXTYPE{
	TX_BUYTRADE = 0x01,
	TX_SELLERTRADE = 0x02,
	TX_BUYERCONFIM = 0x03,
	TX_BUYERCANCEL = 0x04,
};
bool CheckBuyerTradePackage(const FIRST_CONTRACT* const pContract){

	if(!IsRegID(&pContract->seller))
	{
		return false;
	}
	Int64 paymoey;
	Int64Inital(&paymoey,"\x00",1);

	 if(!GetCurPayAmount(&paymoey)){
		 return false;
	 }
	Int64 compare;
	Int64Inital(&compare,"\x00",1);
	if(Int64Compare(&compare,&paymoey) == COMP_EQU){
		return false;
	}
	return true;
}
bool BuyerTradePackage(const FIRST_CONTRACT* const pContract){
	if(!CheckBuyerTradePackage(pContract)){
		//LogPrint("check buyer trade package error",sizeof("check buyer trade package error")+1,STRING);
		return false;
	}

	Int64 paymoey;
	Int64Inital(&paymoey,"\x00",1);
	 if(!GetCurPayAmount(&paymoey)){
		 return false;
	 }

	char account[6];
	if(!GetCurTxAccount(account,sizeof(account))){
		return false;
	}
	char txhash[32];
	if( !GetCurTxHash(txhash)){
		return false;
	}

	DATA_DB dbdata;
	dbdata.flag =false;
	memcpy(&dbdata.seller,&pContract->seller,sizeof(pContract->seller));
	memcpy(&dbdata.buyer,&account,sizeof(dbdata.buyer));
	memcpy(&dbdata.nPayMoney,&paymoey,sizeof(paymoey));

	if( !WriteData(txhash,sizeof(txhash),&dbdata,sizeof(dbdata))){
		return false;
	}
	return true;
}
bool CheckSellerTradePackage(const DATA_DB* const Pdbdata){
	Int64 paymoey;
	Int64Inital(&paymoey,"\x00",1);
	if(!GetCurPayAmount(&paymoey)){
		 return false;
	 }

	Int64 div;
	Int64Inital(&div,"\x02",1);
	Int64 compare;
	Int64Div(&Pdbdata->nPayMoney,&div,&compare);
	/// 比较卖家抵押的金额是不是买家的一半
	if(Int64Compare(&compare,&paymoey) != COMP_EQU){
		return false;
	}

	char account[6];
	if(!GetCurTxAccount(account,sizeof(account))){
		return false;
	}
	// 检查是否是卖家发的包
	if(memcmp(account,&Pdbdata->seller,sizeof(account)) != 0){
		return false;
	}

	// 检查是此包是否被处理了
	if(Pdbdata->flag){
		return false;
	}

	return true;
}


bool SellerTradePackage(const NEXT_CONTRACT* const pContract){
	DATA_DB dbdata;
	LogPrint(pContract->hash,32,HEX);
	if(!ReadData(pContract->hash,sizeof(pContract->hash), &dbdata,sizeof(dbdata))){
		//LogPrint("SellerTradePackage read db error",sizeof("SellerTradePackage read db error")+1,STRING);
		return false;
	}
	if(!CheckSellerTradePackage(&dbdata)){
		//LogPrint("SellerTradePackage check error",sizeof("SellerTradePackage check error")+1,STRING);
		return false;
	}
	dbdata.flag = true;
	if(!ModifyData(pContract->hash,sizeof(pContract->hash),&dbdata,sizeof(dbdata))){
		return false;
	}

	return true;
}

bool WriteBuyerConfirmPackage(const DATA_DB* const pContract)
{
	Int64 div;
	Int64Inital(&div,"\x02", 1);
	Int64 pout;
	Int64Div(&pContract->nPayMoney,&div,&pout);
	Int64 result;
	Int64Add(&pContract->nPayMoney,&pout,&result);

	VM_OPERATE writeCode[3];
	VM_OPERATE ret;
	ret.type = REG_ID;
	memcpy(ret.accountid,&pContract->seller,sizeof(ret.accountid));
	memcpy(&ret.money,&pContract->nPayMoney,sizeof(Int64));
	ret.opeatortype = ADD_FREE;
	writeCode[0]=ret;


	memcpy(ret.accountid,&pContract->buyer,sizeof(ret.accountid));
	memcpy(&ret.money,&pout,sizeof(Int64));
	writeCode[1]=ret;

	char accountid[6] = {0};
	if(!GetScriptID(&accountid)){
		return false;
	}

	ret.opeatortype = MINUS_FREE;
	memcpy(ret.accountid,&accountid,sizeof(ret.accountid));
	memcpy(&ret.money,&result,sizeof(Int64));
	writeCode[2]=ret;
	WriteOutput((VM_OPERATE*)&writeCode,3);

	return true;
}
bool BuyerConfirmPackage(const NEXT_CONTRACT* const pContract){
	DATA_DB dbdata;
	if(!ReadData(pContract->hash,sizeof(pContract->hash), &dbdata,sizeof(dbdata))){
		//LogPrint("BuyerConfirmPackage read db error",sizeof("BuyerConfirmPackage read db error")+1,STRING);
		return false;
	}

	if(!dbdata.flag){
		return false;
	}
	if(!DeleteData(pContract->hash,sizeof(pContract->hash))){
		//LogPrint("BuyerConfirmPackage delete db error",sizeof("BuyerConfirmPackage delete db error")+1,STRING);
		return false;
	}
	if(!WriteBuyerConfirmPackage(&dbdata)){
		return false;
	}

	return true;

}
bool WriteBuyerCancelPackage(const DATA_DB* const pContract)
{

	VM_OPERATE writeCode[2];
	VM_OPERATE ret;
	ret.type = REG_ID;
	memcpy(ret.accountid,&pContract->buyer,sizeof(ret.accountid));
	memcpy(&ret.money,&pContract->nPayMoney,sizeof(Int64));
	ret.opeatortype = ADD_FREE;
	writeCode[0]=ret;

	char accountid[6] = {0};
	if(!GetScriptID(&accountid)){
		return false;
	}

	ret.opeatortype = MINUS_FREE;
	memcpy(ret.accountid,&accountid,sizeof(ret.accountid));
	memcpy(&ret.money,&pContract->nPayMoney,sizeof(Int64));
	writeCode[1]=ret;
	WriteOutput((VM_OPERATE*)&writeCode,2);

	return true;
}
bool BuyerCancelPackage(const NEXT_CONTRACT* const pContract){
	DATA_DB dbdata;
	if(!ReadData(pContract->hash,sizeof(pContract->hash), &dbdata,sizeof(dbdata))){
		//LogPrint("BuyerConfirmPackage read db error",sizeof("BuyerConfirmPackage read db error")+1,STRING);
		return false;
	}

	if(dbdata.flag){
		return false;
	}

	if(!WriteBuyerCancelPackage(&dbdata)){
		return false;
	}

	if(!DeleteData(pContract->hash,sizeof(pContract->hash))){
		//LogPrint("BuyerCancelPackage delete db error",sizeof("BuyerCancelPackage delete db error")+1,STRING);
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
		case TX_BUYTRADE:
		{
			//LogPrint("TX_BUYTRADE",sizeof("TX_BUYTRADE"),STRING);
			if(!BuyerTradePackage((FIRST_CONTRACT*)pcontact))
			{
				//LogPrint("TX_BUYTRADE error",sizeof("TX_BUYTRADE error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}
		case TX_SELLERTRADE:
		{
			//LogPrint("TX_SELLERTRADE",sizeof("TX_SELLERTRADE"),STRING);
			if(!SellerTradePackage((NEXT_CONTRACT*) pcontact))
			{
				//LogPrint("TX_SELLERTRADE error",sizeof("TX_SELLERTRADE error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}
		case  TX_BUYERCONFIM:
		{
			//LogPrint("TX_BUYERCONFIM",sizeof("TX_BUYERCONFIM"),STRING);
			if(!BuyerConfirmPackage((NEXT_CONTRACT*) pcontact))
			{
				//LogPrint("TX_BUYERCONFIM error",sizeof("TX_BUYERCONFIM error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}
		case  TX_BUYERCANCEL:
		{
			//LogPrint("TX_BUYERCANCEL",sizeof("TX_BUYERCANCEL"),STRING);
			if(!BuyerCancelPackage((NEXT_CONTRACT*) pcontact))
			{
				//LogPrint("TX_BUYERCANCEL error",sizeof("TX_BUYERCANCEL error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}
		default:
		{
			//LogPrint("tx format error",sizeof("tx format error"),STRING);
			__VmExit(RUN_SCRIPT_DATA_ERR);
			break;
		}
 	}
 	__VmExit(RUN_SCRIPT_OK);
    return 0;
}
