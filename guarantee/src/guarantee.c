#include <string.h>
#include<stdlib.h>
#include <stdio.h>
#include"VmSdk.h"
__root __code static const char checkaccount[]@0x0014 ={0x01};  //0x01 打开，0x00 关闭

#define OVERTIME_HEIGHT  (1440 * 7)    //!<各个环节的超时最大高度

#define ARBITRATION_MIN_HEIGHT (1440 * 1)  //!<判决期限时间

//#define COIN     100000000
//#define CENT     1000000

//#define JUDGE_REGISTER_FEE 10000     //!<仲裁者C 注册的担保押金
//!<对应的字节流               "\x00\x00\x00\xE8\xD4\xA5\x10\x00"   10000 * COIN

//#define SEND_ACCOUNT_FEE   10         //!<挂单 固定付10个币
//!<对应的字节流      "\x00\x00\x00\x00\x3B\x9A\xCA\x00"   10 * COIN
const static char DeveloperAccount[6]="\xC0\x52\x01\x00\x01\x00";//!<开发者地址ID  86720-1 生产网络
//const static char DeveloperAccount[6]="\x00\x00\x00\x00\x14\x00";//!<开发者地址ID
//string regAddr="dk2NNjraSvquD9b4SQbysVRQeFikA55HLi";   //RegID = "0-20"



enum TRADE_STATE{   //!<担保交易的各种状态
	STATE_SEND = 0x01,    //挂单
	STATE_ACCEPT = 0x02,  //接单
	STATE_DELIVERY = 0x03, //发货
	STATE_ARBITER = 0x04  //申请仲裁
};

enum TXTYPE{
	TX_REGISTER = 0x01,     //注册仲裁账户
	TX_MODIFYREGISTER =0x02,// 修改仲裁者注册信息
	TX_ARBIT_ON    = 0x03, //仲裁开启
	TX_ARBIT_OFF   = 0x04, //仲裁暂停
	TX_UNREGISTER = 0x05,  //注销仲裁账户
	TX_SEND = 0x06,        //挂单
	TX_CANCEL = 0x07,      //取消挂单
	TX_ACCEPT = 0x08,      //接单
	TX_DELIVERY = 0x09,    //发货
	TX_BUYERCONFIRM = 0x0a,//买家确认收货
	TX_ARBITRATION = 0x0b, //申请仲裁
	TX_FINALRESULT = 0x0c, //裁决结果
};

typedef struct  {
	Int64 arbiterMoneyX;             //!<仲裁费用X
	Int64 overtimeMoneyYmax;  //!<超时未判决的最大赔偿费用Y
	Int64 configMoneyZ;              //!<无争议裁决费用Z
	unsigned long  overtimeheightT;  //!<判决期限时间T
	char  comment[220];             //!<备注说明 字符串以\0结束，长度不足后补0
} DB_DATA_ARBITER_REGISTER;         //!<仲裁者注册的信息

typedef struct {
	enum TXTYPE nType;            //!<交易类型
	DB_DATA_ARBITER_REGISTER db_data;        //!<仲裁者注册的信息
}TX_REGISTER_CONTRACT;  //!<注册仲裁账户

#define  ARBITER_ON   0xaa   //!<仲裁开启
#define  ARBITER_OFF  0x55   //!<仲裁暂停

//保存的 仲裁者注册后的数据结构
typedef struct{
  unsigned char arbitState;        //!<0xaa,仲裁开启;0x55,仲裁暂停
  DB_DATA_ARBITER_REGISTER db_data;        //!<仲裁者注册的信息
}DB_DATA_ARBITER;

enum SEND_TYPE{      //!<挂单的状态
	SEND_TYPE_BUY = 0x00,   //!<挂单 买
	SEND_TYPE_SELL = 0x01,  //!<挂单 卖
};
typedef struct{
	enum SEND_TYPE sendType;         //!<挂单类型:0 买  1卖
	ACCOUNT_ID arbitationID;        //!<仲裁者ID（采用6字节的账户ID）
	Int64 moneyM;                   //!<交易金额
	unsigned long height;           //!<每个交易环节的超时高度
	char goods[20];               //!<商品信息 字符串以\0结束，长度不足后补0
    char  comment[200];             //!<备注说明 字符串以\0结束，长度不足后补0
}TX_SEND_PUBLICDATA;
typedef struct {
	enum TXTYPE nType;            //!<交易类型
	TX_SEND_PUBLICDATA tradeInfo;   //!<交易相关信息
}TX_SNED_CONTRACT;                  //!<挂单

typedef struct  {
	enum TRADE_STATE  state;         //!<标示交易状态
	TX_SEND_PUBLICDATA tradeInfo;    //!<交易相关信息
	Int64 arbiterMoneyX;             //!<仲裁费用X
	Int64 arbiterovertimeMoneyY;     //!<超时未判决的赔偿费用Y
	Int64 configMoneyZ;              //!<无争议裁决费用Z
	unsigned long  overtimeheightT;  //!<判决期限时间T

    //下述可选项，UI根据state来读取
	ACCOUNT_ID 	buyer;      	     //!<买家ID（采用6字节的账户ID）可选项
	ACCOUNT_ID 	seller;      	     //!<卖家ID（采用6字节的账户ID）可选项
	unsigned char arbiterType;       //!<申请仲裁类型 0：买家发起，1:卖家发起
	unsigned char arbitHash[32];     //!<申请仲裁的hash
} DB_DATA;    //!<需要保存的数据结构，交易过程中


typedef struct {
	enum TXTYPE nType;            //!<交易类型
	unsigned char txhash[32];       //!<交易hash
	unsigned long height;          //!<每个交易环节的超时高度
} TX_CONTRACT;

typedef struct {
	enum TXTYPE nType;            //!<交易类型
	unsigned char txhash[32];       //!<接单交易hash
	unsigned long height;          //!<每个交易环节的超时高度
	char  arbitationID[6];       //!<仲裁者ID（采用6字节的账户ID）
} TX_Arbitration;                //!<申请仲裁

typedef struct {
	enum TXTYPE nType;            //!<交易类型
	unsigned char arbitHash[32];     //!<申请仲裁的交易hash
	unsigned long overtimeheightT;//!<判决期限时间T
	ACCOUNT_ID 	winner;      	//!<赢家ID（采用6字节的账户ID）
	Int64 winnerMoney;            //!<最终获得的金额
	ACCOUNT_ID  loser;       //!<输家ID（采用6字节的账户ID）
	Int64 loserMoney;            //!<最终获得的金额
}TX_FINALRESULT_CONTRACT;        //!<最终裁决


void PrintAppOperate(APP_ACC_OPERATE* pAppID,unsigned char num){
// 打印应用账户操作前的必要信息，
  unsigned char i = 0;

  LogPrint("PrintAppOperate start",sizeof("PrintAppOperate start")+1,STRING);
	for(i = 0; i < num; i++)
	{
		LogPrint(&i,1,HEX);
		LogPrint("AppAccID",sizeof("AppAccID")+1,STRING);
		LogPrint(&pAppID[i].AppAccID.ID[0],pAppID[i].AppAccID.idlen,HEX);
		LogPrint("tag",sizeof("tag")+1,STRING);
		LogPrint(&pAppID[i].FundTag.ID[0],pAppID[i].FundTag.idlen,HEX);

		LogPrint("opeatortype",sizeof("opeatortype")+1,STRING);
		LogPrint(&pAppID[i].opeatortype,sizeof(pAppID[i].opeatortype),HEX);
		LogPrint("outheight",sizeof("outheight")+1,STRING);
		LogPrint(&pAppID[i].outheight,sizeof(pAppID[i].outheight),HEX);
		LogPrint("mMoney",sizeof("mMoney")+1,STRING);
		LogPrint(&pAppID[i].mMoney,sizeof(Int64),HEX);
	}
	LogPrint("PrintAppOperate end",sizeof("PrintAppOperate end")+1,STRING);
}



void revert(char*pchar)
{

	char temp = pchar[0];
	(pchar)[0] = (pchar)[3];;
	(pchar)[3] = temp;
	temp =(pchar)[1];
	(pchar)[1] = (pchar)[2];
	(pchar)[2] = temp;
}


bool CheckDeveloperAccWithdraw(Int64 withMoney){
	//!<检测提现，开发者不能提 注册仲裁者总的担保押金
	unsigned char key[20]={0};
	unsigned long num = 0;
	Int64 money,cmpMoney,refundMoney;

	LogPrint("CheckDeveloperAccWithdraw",sizeof("CheckDeveloperAccWithdraw")+1,STRING);
	memcpy(key,"arbitrationnum",14);
	ReadData(key,14, &num,sizeof(num));
	if(num)
	{
		Int64Inital(&money,"\x00\x00\x00\xE8\xD4\xA5\x10\x00",8);//!<JUDGE_REGISTER_FEE * COIN
		Int64Inital(&cmpMoney,"\x00\x00\x00\x00\x00\x00\x00\x02",8);
		if(!Int64Div(&money,&cmpMoney,&refundMoney)){
			 LogPrint("Int64Div err",sizeof("Int64Div err")+1,STRING);
			 return false;
		}
		revert((char *)&num);
		Int64Inital(&cmpMoney,(char *)&num,sizeof(num));
//		LogPrint(&cmpMoney,sizeof(cmpMoney),HEX);  //debug
		if(Int64Mul(&refundMoney,&cmpMoney,&money))  //!<注销退一半的押金
		{
//			LogPrint(&money,sizeof(money),HEX);
			 if((Int64Compare(&money,&refundMoney) == COMP_LESS) ||
				   (Int64Compare(&money,&cmpMoney) == COMP_LESS)){
				 LogPrint("Int64Mul0 err",sizeof("Int64Mul0 err")+1,STRING);
				  return false;
			 }
		}else{
			 LogPrint("Int64Mul1 err",sizeof("Int64Mul1 err")+1,STRING);
			 return false;
		}
	}else{
		return true; //无仲裁者注册 的担保押金,直接返回
	}
	Int64Inital(&cmpMoney,"\x01",1);  //!< 不能小于1
    if(Int64Compare(&withMoney,&cmpMoney) == COMP_LESS)
    {  //!<withMoney为0,表示全额提现，不允许
		 LogPrint("input0 err",sizeof("input0 err")+1,STRING);
		 return false;
    }
	Int64Add(&money,&withMoney,&cmpMoney);

	S_APP_ID id;
	id.idlen = 6;
	memcpy(id.ID,&DeveloperAccount[0],MAX_ACCOUNT_LEN);

	Int64 pRet;
	Int64Inital(&pRet,"\x00",1);
	GetUserAppAccFreeValue(&pRet,&id);
//	LogPrint(&pRet,sizeof(pRet),HEX);
//	LogPrint(&cmpMoney,sizeof(cmpMoney),HEX);
	 if(Int64Compare(&pRet, &cmpMoney) == COMP_LESS){      //系统应用账户中要有足够的自由金额
		 LogPrint("not enough money",sizeof("not enough money")+1,STRING);
		 return false;
	 }
	 return true;
}

bool OpRegisterAccount(const char*const pAccount,unsigned char type){
   //!<仲裁者注册注销时，账户操作
	APP_ACC_OPERATE writecode[2];

	S_APP_ID id;
	APP_ACC_OPERATE appID;

	memcpy(id.ID,pAccount,MAX_ACCOUNT_LEN);
	id.idlen = MAX_ACCOUNT_LEN;
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));

	Int64Inital(&appID.mMoney,"\x00\x00\x00\xE8\xD4\xA5\x10\x00",8);//!<  JUDGE_REGISTER_FEE * COIN
	// type 1:注册， 0： 注销
	if(type)
	{
		//!<扣仲裁者账户JUDGE_REGISTER_FEE * COIN 币担保押金到开发者账户
		 appID.opeatortype = SUB_FREE_OP;
	}else{
		//!<退还扣仲裁者账户担保押金一半
		Int64 cmpMoney;
		Int64Inital(&cmpMoney,"\x00\x00\x00\x00\x00\x00\x00\x02",8);
		if(!Int64Div(&appID.mMoney,&cmpMoney,&appID.mMoney)){
			LogPrint("OpRegisterAccount 0 error",sizeof("OpRegisterAccount 0 error")+1,STRING);
			return false;
		}
		appID.opeatortype = ADD_FREE_OP;
	}
	writecode[0] = appID;

	memcpy(id.ID,&DeveloperAccount[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
    if(type)
    {  	//注册
	    appID.opeatortype = ADD_FREE_OP;
    }else{
    	appID.opeatortype = SUB_FREE_OP;
    }
	writecode[1] = appID;
	PrintAppOperate((APP_ACC_OPERATE*)writecode, 2);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, 2)){
		return false;
	}
	return true;
}

bool Register(const TX_REGISTER_CONTRACT* const pContract){
	//!< 仲裁者注册信息
	LogPrint((char *)pContract,sizeof(TX_REGISTER_CONTRACT),HEX);

	Int64 compare;
	Int64Inital(&compare,"\x00",1);
	if((Int64Compare(&pContract->db_data.arbiterMoneyX,&compare) == COMP_LESS) ||
		 (Int64Compare(&pContract->db_data.overtimeMoneyYmax,&compare) == COMP_LESS) ||
		 (Int64Compare(&pContract->db_data.configMoneyZ,&compare) == COMP_LESS) ||
		 (pContract->db_data.overtimeheightT < ARBITRATION_MIN_HEIGHT)){
		LogPrint("Register input error",sizeof("Register input error"),STRING);
		return false;
	}

 	char account[6];
	if(!GetCurTxAccount(account,sizeof(account)) || (!memcmp(DeveloperAccount,account,sizeof(account)))){
		LogPrint("Register GetCurTxAccount error",sizeof("Register GetCurTxAccount error"),STRING);
		return false;
	}
     //!<保存仲裁者注册信息
	DB_DATA_ARBITER dbdata;
	unsigned char key[20] = {0};
	memcpy(key,"judge",5);
	memcpy(&key[5],account,sizeof(account));
	LogPrint(key,11,HEX);
	if(ReadData(key,11, &dbdata,sizeof(dbdata))){
		 //!<已注册 ，不允许重复注册
		LogPrint("Register repeat error",sizeof("Register repeat error"),STRING);
		return false;
	}
	else{
		//!<未注册
		dbdata.arbitState = ARBITER_ON;  //!<默认开启
		memcpy(&dbdata.db_data,&pContract->db_data,sizeof(pContract->db_data));
		if(!WriteData(key,11,&dbdata,sizeof(dbdata))){
			LogPrint("Register WriteData error",sizeof("Register WriteData error"),STRING);
			return false;
		}
	}

	//!<注册时，扣仲裁者账户10000个币担保押金到开发者账户。修改时，不必付押金。
	if(!OpRegisterAccount(account,1)){
		LogPrint("Register OpRegisterAccount error",sizeof("Register OpRegisterAccount error")+1,STRING);
		return false;
	}

 //!<账户操作成功后，再统计注册仲裁者的总个数
	unsigned long num = 0;
	memcpy(key,"arbitrationnum",14);
	if(ReadData(key,14, &num,sizeof(num))){
		num++;
		if(!ModifyData(key,14, &num,sizeof(num))){
			LogPrint("Register ModifyData error",sizeof("Register ModifyData error"),STRING);
			return false;
		}
	}
	else{
		num++;
		if(!WriteData(key,14,&num,sizeof(num))){
			LogPrint("Register WriteData error",sizeof("Register WriteData error"),STRING);
			return false;
		}
	}

	return true;
}

bool ModifyRegister(const TX_REGISTER_CONTRACT* const pContract){
	//!< 修改仲裁者注册信息
	LogPrint((char *)pContract,sizeof(TX_REGISTER_CONTRACT),HEX);

	Int64 compare;
	Int64Inital(&compare,"\x00",1);
	if((Int64Compare(&pContract->db_data.arbiterMoneyX,&compare) == COMP_LESS) ||
		 (Int64Compare(&pContract->db_data.overtimeMoneyYmax,&compare) == COMP_LESS) ||
		 (Int64Compare(&pContract->db_data.configMoneyZ,&compare) == COMP_LESS) ||
		 (pContract->db_data.overtimeheightT < ARBITRATION_MIN_HEIGHT)){
		LogPrint("ModifyRegister input error",sizeof("ModifyRegister input error"),STRING);
		return false;
	}
    //!<保存仲裁者注册信息
	DB_DATA_ARBITER dbdata;
	unsigned char key[20] = {0};
	memcpy(key,"judge",5);
 	char account[6];
	if(!GetCurTxAccount(account,sizeof(account))){
		LogPrint("ModifyRegister GetCurTxAccount error",sizeof("ModifyRegister GetCurTxAccount error"),STRING);
		return false;
	}
	memcpy(&key[5],account,sizeof(account));
	LogPrint(key,11,HEX);
	if(ReadData(key,11, &dbdata,sizeof(dbdata))){
	  //!<只有注册了，才能去修改
		memcpy(&dbdata.db_data,&pContract->db_data,sizeof(pContract->db_data));
		if(!ModifyData(key,11, &dbdata,sizeof(dbdata))){
			LogPrint("ModifyRegister ModifyData error",sizeof("ModifyRegister ModifyData error"),STRING);
			return false;
		}
	}
	else{
		//!<未注册 ，返回
		LogPrint("ModifyRegister ReadData error",sizeof("ModifyRegister ReadData error"),STRING);
		return false;
	}
	return true;
}

bool ArbitOnOrOff(const unsigned char* const pContract){
	//!< 修改仲裁者开启或暂停
	DB_DATA_ARBITER dbdata;
	unsigned char key[20] = {0};
	memcpy(key,"judge",5);
 	char account[6];
	if(!GetCurTxAccount(account,sizeof(account))){
		LogPrint("ArbitOnOrOff GetCurTxAccount error",sizeof("ArbitOnOrOff GetCurTxAccount error"),STRING);
		return false;
	}
	memcpy(&key[5],account,sizeof(account));
	LogPrint(key,11,HEX);
	if(ReadData(key,11, &dbdata,sizeof(dbdata))){
	  //!<只有注册了，才能去修改
	   if(pContract[0] == TX_ARBIT_ON)
	   {
		   dbdata.arbitState = ARBITER_ON;
	   }else{
		   dbdata.arbitState = ARBITER_OFF;
	   }
		if(!ModifyData(key,11, &dbdata,sizeof(dbdata))){
			LogPrint("ArbitOnOrOff ModifyData error",sizeof("ArbitOnOrOff ModifyData error"),STRING);
			return false;
		}
	}
	else{
		//!<未注册 ，返回
		LogPrint("ArbitOnOrOff ReadData error",sizeof("ArbitOnOrOff ReadData error"),STRING);
		return false;
	}
	return true;
}

bool UnRegister(void){
    //!<注销仲裁者
	unsigned char key[20] = {0};
 	char account[6];
	if(!GetCurTxAccount(account,sizeof(account))){
		LogPrint("UnRegister GetCurTxAccount error",sizeof("UnRegister GetCurTxAccount error"),STRING);
		return false;
	}

	memcpy(key,"judge",5);
	memcpy(&key[5],account,sizeof(account));
	LogPrint(key,11,HEX);
	if(!DeleteData(key,11)){  //!<不允许重复注销
		LogPrint("UnRegister DeleteData error",sizeof("UnRegister DeleteData error"),STRING);
		return false;
	}
	//!<退还一半的押金
	if(!OpRegisterAccount(account,0)){
		LogPrint("UnRegister OpRegisterAccount error",sizeof("UnRegister OpRegisterAccount error")+1,STRING);
		return false;
	}

	//!<账户操作成功后，再更新注册仲裁者的总个数
	unsigned long num = 0;
	memcpy(key,"arbitrationnum",14);
	LogPrint(key,14,HEX);
	if(ReadData(key,14, &num,sizeof(num))){
		LogPrint(&num,sizeof(num),HEX); //debug
		if(num)
		{
		   num--;
		}
		if(!ModifyData(key,14, &num,sizeof(num))){
			LogPrint("UnRegister ModifyData error",sizeof("UnRegister ModifyData error"),STRING);
			return false;
		}
	}
	else{
		LogPrint("UnRegister ReadData error",sizeof("UnRegister ReadData error"),STRING);
		return false;
	}

	return true;
}


bool CheckTradeIsOverTime(unsigned long height)
{
  return (GetCurRunEnvHeight() > height);
}


bool OpWriteSendAccount(const DB_DATA* const pDbData,const unsigned char*const pTxhash){
	APP_ACC_OPERATE writecode[3];

	S_APP_ID id;
	APP_ACC_OPERATE appID;
    //!<冻结买家A账户交易金额M和可能要付的仲裁费Xa,付给仲裁者无争议裁决费Z
    //!<或冻结卖家B账户可能要付的仲裁费Xb,付给仲裁者无争议裁决费Z
	memcpy(&appID.mMoney,&pDbData->arbiterMoneyX,sizeof(appID.mMoney));
	if(SEND_TYPE_SELL == pDbData->tradeInfo.sendType)
	{ //!<卖     Xb
		memcpy(id.ID,&pDbData->seller.accounid[0],MAX_ACCOUNT_LEN);
	}else{
		//!<买 M,Xa
		memcpy(id.ID,&pDbData->buyer.accounid[0],MAX_ACCOUNT_LEN);
		Int64Add(&appID.mMoney,&pDbData->tradeInfo.moneyM,&appID.mMoney);
	}
	id.idlen = MAX_ACCOUNT_LEN;
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));

	S_APP_ID tag;
	tag.idlen = 32;
	appID.outheight = GetCurRunEnvHeight() + pDbData->tradeInfo.height;//freeze_height
	memcpy(tag.ID,pTxhash,32);
	memcpy(&appID.FundTag,&tag,sizeof(S_APP_ID));

	//!< 从自由金额转到冻结金额
	appID.opeatortype = ADD_FREEZED_OP;
	writecode[0] = appID;

	Int64Add(&appID.mMoney,&pDbData->configMoneyZ,&appID.mMoney);//!<减掉 付给仲裁者无争议裁决费Z
	appID.opeatortype=SUB_FREE_OP;
	writecode[1] = appID;
	PrintAppOperate((APP_ACC_OPERATE*)writecode, 2);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, 2)){
		return false;
	}

    //!< 挂单付给 开发者账户10个币
	Int64Inital(&appID.mMoney,"\x00\x00\x00\x00\x3B\x9A\xCA\x00",8); //!< SEND_ACCOUNT_FEE * COIN
	//!<冻结挂单费用10个币到开发者账户
	appID.opeatortype = SUB_FREE_OP;
	writecode[0] = appID;

	memcpy(id.ID,&DeveloperAccount[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	appID.opeatortype = ADD_FREEZED_OP;
	writecode[1] = appID;
	PrintAppOperate((APP_ACC_OPERATE*)writecode, 2);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, 2)){
		return false;
	}

	//!<仲裁者C 账户，接收 无争议裁决费Z，冻结超时判决赔付金额Y
    memcpy(id.ID,&pDbData->tradeInfo.arbitationID.accounid[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	memcpy(&appID.mMoney,&pDbData->configMoneyZ,sizeof(appID.mMoney));
	appID.opeatortype = ADD_FREE_OP;
	writecode[0] = appID;

	memcpy(&appID.mMoney,&pDbData->arbiterovertimeMoneyY,sizeof(appID.mMoney));
	appID.opeatortype = SUB_FREE_OP;
	writecode[1] = appID;

	appID.opeatortype = ADD_FREEZED_OP;
	writecode[2] = appID;
	PrintAppOperate((APP_ACC_OPERATE*)writecode, 3);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, 3)){
		return false;
	}
	return true;
}

bool SendStartTrade(const TX_SNED_CONTRACT* const pContract){
//!< 挂单
	LogPrint((char *)pContract,sizeof(TX_SNED_CONTRACT),HEX);

    //!<检测发送包
	char account[6];
	if(!GetCurTxAccount(account,sizeof(account))){
		LogPrint("SendStartTrade GetCurTxAccount error",sizeof("SendStartTrade GetCurTxAccount error")+1,STRING);
		return false;
	}
	//!<挂单类型 ,交易金额检测,仲裁者ID不能与挂单者相同,不能是开发者账户ID
	Int64 compare;
	Int64Inital(&compare,"\x01",1);
	if((pContract->tradeInfo.sendType > SEND_TYPE_SELL) ||
			(Int64Compare(&pContract->tradeInfo.moneyM,&compare) == COMP_LESS) ||
			(!memcmp(account,&pContract->tradeInfo.arbitationID.accounid[0],sizeof(account))) ||
			(!memcmp(DeveloperAccount,account,sizeof(account)))){
		LogPrint("SendStartTrade input0 error",sizeof("SendStartTrade input0 error")+1,STRING);
		return false;
	}
	//!<仲裁者是否 开启仲裁
	DB_DATA_ARBITER arbitData;
	unsigned char key[36] = {0};
	memcpy(key,"judge",5);
	memcpy(&key[5],&pContract->tradeInfo.arbitationID.accounid[0],MAX_ACCOUNT_LEN);
	if((!ReadData(key,11, &arbitData,sizeof(arbitData))) || (arbitData.arbitState != ARBITER_ON)){
	  LogPrint("SendStartTrade input1 error",sizeof("SendStartTrade input1 error")+1,STRING);
	  return false;
	}
	 //!<超时高度是否小于等于最大超时高度,是否大于等于判决期限时间T
	if((pContract->tradeInfo.height > OVERTIME_HEIGHT) ||
			(pContract->tradeInfo.height < arbitData.db_data.overtimeheightT)){
		LogPrint("SendStartTrade input2 error",sizeof("SendStartTrade input2 error")+1,STRING);
		return false;
	}

	unsigned char txhash[32];
	if(!GetCurTxHash(txhash)){
		LogPrint("SendStartTrade GetCurTxHash error",sizeof("SendStartTrade GetCurTxHash error")+1,STRING);
		return false;
	}
	//!<填写必要的信息，并保存
	DB_DATA dbdata;

	memset(&dbdata,0,sizeof(dbdata));
    dbdata.state = STATE_SEND;
	memcpy(&dbdata.tradeInfo,&pContract->tradeInfo,sizeof(TX_SEND_PUBLICDATA));
	memcpy(&dbdata.arbiterMoneyX,&arbitData.db_data.arbiterMoneyX,sizeof(Int64));
	if(Int64Compare(&pContract->tradeInfo.moneyM,&arbitData.db_data.overtimeMoneyYmax) == COMP_LESS)
	{  //!<如果交易金额M小于 Y，则只赔偿金额M
		memcpy(&dbdata.arbiterovertimeMoneyY,&pContract->tradeInfo.moneyM,sizeof(Int64));
	}else
	{  //!<如果交易金额M 大于等于Y,则最多只赔偿Y;
	    memcpy(&dbdata.arbiterovertimeMoneyY,&arbitData.db_data.overtimeMoneyYmax,sizeof(Int64));
	}
	memcpy(&dbdata.configMoneyZ,&arbitData.db_data.configMoneyZ,sizeof(Int64));
	dbdata.overtimeheightT = arbitData.db_data.overtimeheightT;
	///LVLV
	if(SEND_TYPE_SELL == pContract->tradeInfo.sendType)
	{ //卖
		memcpy(&dbdata.seller.accounid[0],&account[0],sizeof(account));
	}else{
	  memcpy(&dbdata.buyer.accounid[0],&account[0],sizeof(account));
	}

	unsigned long outheight = GetCurRunEnvHeight()  +  pContract->tradeInfo.height; //freeze_height
//	unsigned long outheight = GetCurRunEnvHeight()  + OVERTIME_HEIGHT;
	revert((char*)&outheight);
	memcpy(key,&outheight,sizeof(outheight));
	memcpy(&key[4],txhash,sizeof(txhash));
	LogPrint(key,36,HEX);
	LogPrint((char *)&dbdata,sizeof(dbdata),HEX);
	if( !WriteData(key,36,&dbdata,sizeof(dbdata))){
		LogPrint("SendStartTrade WriteData error",sizeof("SendStartTrade WriteData error")+1,STRING);
		return false;
	}
    //!<挂单者账户操作
	if(!OpWriteSendAccount(&dbdata,txhash)){
		LogPrint("SendStartTrade WriteSendPakage error",sizeof("SendStartTrade WriteSendPakage error")+1,STRING);
		return false;
	}
	return true;
}

unsigned long GetSptDbData(const unsigned char* const pTxhash,unsigned long height,unsigned char *pKey,DB_DATA* pDbData){
	//!<取挂单交易时，保存的DB_DATA中间结果

	unsigned long outheight = GetTxConFirmHeight(pTxhash)+ height; //freeze_height
//	unsigned long outheight = GetTxConFirmHeight(pTxhash)+ OVERTIME_HEIGHT;
	revert((char*)&outheight);
	memcpy(pKey,&outheight,sizeof(outheight));
	memcpy(&pKey[4],pTxhash,32);
	if(!ReadData(pKey,36, pDbData,sizeof(DB_DATA))){
		LogPrint("GetSptDbData ReadData error",sizeof("GetSptDbData ReadData error")+1,STRING);
		return 0;
	}else{
		LogPrint((char *)pDbData,sizeof(DB_DATA),HEX);
        return outheight;
	}
}
bool OpWriteSendCancelAccount(const DB_DATA* const pDbData,const unsigned char*const pTxhash){
           //!< type 1:挂单 0:取消
	APP_ACC_OPERATE writecode[3];

	S_APP_ID id;
	APP_ACC_OPERATE appID;
    //!<解冻买家A账户交易金额M和可能要付的仲裁费Xa
    //!<或解冻卖家B账户可能要付的仲裁费Xb
	memcpy(&appID.mMoney,&pDbData->arbiterMoneyX,sizeof(appID.mMoney));
	if(SEND_TYPE_SELL == pDbData->tradeInfo.sendType)
	{ //!<卖     Xb
		memcpy(id.ID,&pDbData->seller.accounid[0],MAX_ACCOUNT_LEN);
	}else{
		//!<买 M,Xa
		memcpy(id.ID,&pDbData->buyer.accounid[0],MAX_ACCOUNT_LEN);
		Int64Add(&appID.mMoney,&pDbData->tradeInfo.moneyM,&appID.mMoney);
	}
	id.idlen = MAX_ACCOUNT_LEN;
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));

	S_APP_ID tag;
	tag.idlen = 32;
	appID.outheight = GetTxConFirmHeight(pTxhash) + pDbData->tradeInfo.height;//freeze_height
	memcpy(tag.ID,pTxhash,32);
	memcpy(&appID.FundTag,&tag,sizeof(S_APP_ID));

	//!< 从冻结金额转到自由金额
	appID.opeatortype = SUB_FREEZED_OP;
	writecode[0] = appID;

	appID.opeatortype = ADD_FREE_OP;
	writecode[1] = appID;
	PrintAppOperate((APP_ACC_OPERATE*)writecode, 2);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, 2)){
		return false;
	}

    //!< 挂单付给 开发者账户10个币
	Int64Inital(&appID.mMoney,"\x00\x00\x00\x00\x3B\x9A\xCA\x00",8); //!< SEND_ACCOUNT_FEE * COIN
	Int64 compare;
	//!<解冻开发者账户挂单费5个币退回给挂单者,5个币给开发者自由账户
	Int64Inital(&compare,"\x00\x00\x00\x00\x00\x00\x00\x02",8);
	Int64Div(&appID.mMoney,&compare,&appID.mMoney);
	appID.opeatortype = ADD_FREE_OP;//SUB_FREE_OP;
	writecode[0] = appID;

	memcpy(id.ID,&DeveloperAccount[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	appID.opeatortype = ADD_FREE_OP;
	writecode[1] = appID;

//	appID.outheight = GetTxConFirmHeight(pTxhash) + pDbData->tradeInfo.height;//freeze_height
	Int64Inital(&appID.mMoney,"\x00\x00\x00\x00\x3B\x9A\xCA\x00",8); //!< SEND_ACCOUNT_FEE * COIN
	appID.opeatortype = SUB_FREEZED_OP;//ADD_FREEZED_OP;
	writecode[2] = appID;
	PrintAppOperate((APP_ACC_OPERATE*)writecode, 3);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, 3)){
		return false;
	}

	//!<仲裁者C 账户，解冻超时判决赔付金额Y
    memcpy(id.ID,&pDbData->tradeInfo.arbitationID.accounid[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	memcpy(&appID.mMoney,&pDbData->arbiterovertimeMoneyY,sizeof(appID.mMoney));
	appID.opeatortype = SUB_FREEZED_OP;
	writecode[0] = appID;

	appID.opeatortype = ADD_FREE_OP;
	writecode[1] = appID;
	PrintAppOperate((APP_ACC_OPERATE*)writecode, 2);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, 2)){
		return false;
	}
	return true;
}
bool SendCancelTrade(const TX_CONTRACT* const pContract){
	//!<挂单取消

	LogPrint((char *)pContract,sizeof(TX_CONTRACT),HEX);
	//////key
	unsigned char key[36];
	DB_DATA dbdata;
	unsigned long outheight = GetSptDbData(pContract->txhash,pContract->height,key,&dbdata);
	if((!outheight) || (pContract->height != dbdata.tradeInfo.height)){
		LogPrint("SendCancelTrade input_height error",sizeof("SendCancelTrade input_height error")+1,STRING);
		return false;
	}

    //!<接单以后 ,超时 不能取消
	if((dbdata.state != STATE_SEND) || (CheckTradeIsOverTime(outheight))){
		LogPrint("SendCancelTrade input0 error",sizeof("SendCancelTrade input0 error")+1,STRING);
		return false;
	}
	char account[6];
	if(!GetCurTxAccount(account,sizeof(account))){
		LogPrint("SendCancelTrade GetCurTxAccount error",sizeof("SendCancelTrade GetCurTxAccount error")+1,STRING);
		return false;
	}
	if(SEND_TYPE_SELL == dbdata.tradeInfo.sendType)
	{
      //!<只能挂单者自己取消
      if(memcmp(account,dbdata.seller.accounid,sizeof(account)))
      {
    	LogPrint("SendCancelTrade input1 error",sizeof("SendCancelTrade input1 error")+1,STRING);
   	    return false;
      }
	}else{
	  //!<只能挂单者自己取消
	  if(memcmp(account,dbdata.buyer.accounid,sizeof(account)))
	  {
		  LogPrint("SendCancelTrade input2 error",sizeof("SendCancelTrade input2 error")+1,STRING);
		return false;
	  }
	}
	if(!OpWriteSendCancelAccount(&dbdata,pContract->txhash)){
		return false;
	}
	if(!DeleteData(key,36)){
		LogPrint("SendCancelTrade DeleteData error",sizeof("SendCancelTrade DeleteData error")+1,STRING);
		return false;
	}
	return true;
}

/*
 * 更新指定账户的冻结项超时高度*/
bool UpdateFreezeOverheight(const char *pAccount,const unsigned long height,const char*const pLasttxhash,const char*const pCurtxhash){
	APP_ACC_OPERATE writecode[2];

	S_APP_ID id;
	APP_ACC_OPERATE appID;
	 //!<更新账户的冻结项 超时高度
	//!<首先原高度的冻结项全删除
	S_APP_ID tag;
	tag.idlen = 32;
	appID.outheight = GetTxConFirmHeight(pLasttxhash)  + height;
	memcpy(tag.ID,pLasttxhash,32);
	memcpy(&appID.FundTag,&tag,sizeof(tag));

    id.idlen = MAX_ACCOUNT_LEN;
    memcpy(id.ID,pAccount,MAX_ACCOUNT_LEN);
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	GetUserAppAccFoudWithTag(&appID.mMoney,&appID);

	appID.opeatortype = SUB_FREEZED_OP;
	writecode[0]=appID;
	//!<其次, 更新账户冻结项超时高度
	appID.outheight = GetCurRunEnvHeight()  + height;
	memcpy(tag.ID,pCurtxhash,32);
	memcpy(&appID.FundTag,&tag,sizeof(tag));
	appID.opeatortype = ADD_FREEZED_OP;
	writecode[1]=appID;
	PrintAppOperate((APP_ACC_OPERATE*)writecode, 2);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, 2)){
		return false;
	}

	return true;
}
bool WritAcceptBuyPakage(const DB_DATA* const pDbData,const char*const pLasttxhash,const char*const pCurtxhash){
   //!<卖家 来接买家的挂单
	APP_ACC_OPERATE writecode[2];

	S_APP_ID id;
	APP_ACC_OPERATE appID;

	 //!<更新买家A账户的冻结项
	if(!UpdateFreezeOverheight(&pDbData->buyer.accounid[0],pDbData->tradeInfo.height,pLasttxhash,pCurtxhash)){
		return false;
	}
    //!<冻结卖家B账户可能要付的仲裁费Xb;
	S_APP_ID tag;
	tag.idlen = 32;
	id.idlen = MAX_ACCOUNT_LEN;
	memcpy(id.ID,&pDbData->seller.accounid[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
    memcpy(&appID.mMoney,&pDbData->arbiterMoneyX,sizeof(Int64));
	appID.outheight = GetCurRunEnvHeight()  + pDbData->tradeInfo.height;
	memcpy(tag.ID,pCurtxhash,32);
	memcpy(&appID.FundTag,&tag,sizeof(tag));

	appID.opeatortype = SUB_FREE_OP;
	writecode[0]=appID;

	appID.opeatortype = ADD_FREEZED_OP;
	writecode[1]=appID;
	PrintAppOperate((APP_ACC_OPERATE*)writecode, 2);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, 2)){
		return false;
	}
	return true;
}
bool WritAcceptSellPakage(const DB_DATA* const pDbData,const char*const pLasttxhash,const char*const pCurtxhash){
   //!<买家 来接卖家的挂单
	APP_ACC_OPERATE writecode[2];

	S_APP_ID id;
	APP_ACC_OPERATE appID;
	//!<冻结买家交易金额M,可能要付的仲裁费Xa。
	memcpy(id.ID,&pDbData->buyer.accounid[0],MAX_ACCOUNT_LEN);
	id.idlen = MAX_ACCOUNT_LEN;
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	Int64Add(&pDbData->tradeInfo.moneyM,&pDbData->arbiterMoneyX,&appID.mMoney);

	appID.opeatortype = SUB_FREE_OP;
	writecode[0]=appID;

	S_APP_ID tag;
	tag.idlen = 32;
	appID.outheight = GetCurRunEnvHeight()  + pDbData->tradeInfo.height;
	memcpy(tag.ID,pCurtxhash,32);
	memcpy(&appID.FundTag,&tag,sizeof(tag));

	appID.opeatortype = ADD_FREEZED_OP;
	writecode[1]=appID;
	PrintAppOperate((APP_ACC_OPERATE*)writecode, 2);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, 2)){
		return false;
	}

	//!<更新卖家B冻结账户超时高度
	if(!UpdateFreezeOverheight(&pDbData->seller.accounid[0],pDbData->tradeInfo.height,pLasttxhash,pCurtxhash)){
		return false;
	}
	return true;
}
bool WritAcceptPakage(const DB_DATA* const pDbData,const char*const pLasttxhash,const char*const pCurtxhash){

	APP_ACC_OPERATE writecode[2];

	S_APP_ID id;
	APP_ACC_OPERATE appID;
 	if(SEND_TYPE_SELL == pDbData->tradeInfo.sendType)
	{ //!<卖
		if(!WritAcceptSellPakage(pDbData,pLasttxhash,pCurtxhash)){
			return false;
		}
	}else{
		if(!WritAcceptBuyPakage(pDbData,pLasttxhash,pCurtxhash)){
			return false;
		}
	}
    //!<更新仲裁者账户的冻结项
	if(!UpdateFreezeOverheight(&pDbData->tradeInfo.arbitationID.accounid[0],pDbData->tradeInfo.height,pLasttxhash,pCurtxhash)){
		return false;
	}

	//!<解冻开发者账户挂单费10个币
	S_APP_ID tag;
	tag.idlen = 32;
	appID.outheight = GetTxConFirmHeight(pLasttxhash)  + pDbData->tradeInfo.height;
	memcpy(tag.ID,pLasttxhash,32);
	memcpy(&appID.FundTag,&tag,sizeof(tag));
	Int64Inital(&appID.mMoney,"\x00\x00\x00\x00\x3B\x9A\xCA\x00",8); //!< SEND_ACCOUNT_FEE * COIN
	memcpy(id.ID,&DeveloperAccount[0],MAX_ACCOUNT_LEN);
	id.idlen = MAX_ACCOUNT_LEN;
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));

	appID.opeatortype = SUB_FREEZED_OP;
	writecode[0] = appID;

	appID.opeatortype = ADD_FREE_OP;
	writecode[1] = appID;
	PrintAppOperate((APP_ACC_OPERATE*)writecode, 2);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, 2)){
		return false;
	}
	return true;
}



bool AcceptTrade(const TX_CONTRACT* const pContract){
   //!< 接单
	LogPrint((char *)pContract,sizeof(TX_CONTRACT),HEX);
	////key
	unsigned char key[36];
	DB_DATA dbdata;
	unsigned long outheight = GetSptDbData(pContract->txhash,pContract->height,key,&dbdata);
	if((!outheight) || (pContract->height != dbdata.tradeInfo.height)){
		LogPrint("AcceptTrade input_height error",sizeof("AcceptTrade input_height error")+1,STRING);
		return false;
	}

	//!<合法性校验
	 if((dbdata.state != STATE_SEND) || (CheckTradeIsOverTime(outheight))){
		 LogPrint("AcceptTrade input0 error",sizeof("AcceptTrade input0 error")+1,STRING);
	   	 return  false;
	 }
	//!<校验是否能接 ,不允许接单者既是买家又是卖家,且不能与仲裁者ID相同,且不能是开发者账户ID
	char account[6];
	if(!GetCurTxAccount(account,sizeof(account)) ||
			(!memcmp(account,dbdata.tradeInfo.arbitationID.accounid,sizeof(account))) ||
			(!memcmp(DeveloperAccount,account,sizeof(account)))){
		LogPrint("AcceptTrade input1 error",sizeof("AcceptTrade input1 error")+1,STRING);
		return false;
	}
	if(SEND_TYPE_SELL == dbdata.tradeInfo.sendType)
	{ //!<卖
		if(!memcmp(account,dbdata.seller.accounid,sizeof(account))){
			LogPrint("AcceptTrade input2 error",sizeof("AcceptTrade input2 error")+1,STRING);
			return false;
		}
        memcpy(&dbdata.buyer.accounid[0],account,sizeof(account));
	}else{
		if(!memcmp(account,dbdata.buyer.accounid,sizeof(account))){
			LogPrint("AcceptTrade input3 error",sizeof("AcceptTrade input3 error")+1,STRING);
			return false;
		}
		memcpy(&dbdata.seller.accounid[0],account,sizeof(account));
	}

	char curtxhash[32];
	if(!GetCurTxHash(curtxhash)){
		LogPrint("AcceptTrade GetCurTxHash error",sizeof("AcceptTrade GetCurTxHash error")+1,STRING);
		return false;
	}
	if(!WritAcceptPakage((DB_DATA*)&dbdata,(char*)pContract->txhash,curtxhash)){
		return false;
	}
	//!<表示接单
	dbdata.state = STATE_ACCEPT;
	if(!DeleteData(key,36))
	{
		LogPrint("AcceptTrade DeleteData error",sizeof("AcceptTrade DeleteData error")+1,STRING);
		return false;
	}
	outheight =GetCurRunEnvHeight()  + pContract->height;
//	outheight =GetCurRunEnvHeight()  + OVERTIME_HEIGHT;
	revert((char*)&outheight);
	memcpy(key,&outheight,sizeof(outheight));
	memcpy(&key[4],curtxhash,sizeof(curtxhash));
	LogPrint(key,36,HEX);
	LogPrint((char *)&dbdata,sizeof(dbdata),HEX);

	if( !WriteData(key,36,&dbdata,sizeof(dbdata))){
		LogPrint("AcceptTrade WriteData error",sizeof("AcceptTrade WriteData error")+1,STRING);
		return false;
	}

	return true;
}
bool WriteDeliveryPakage(const DB_DATA* const pDbData,const char*const pLasttxhash,const char*const pCurtxhash)
{
	APP_ACC_OPERATE writecode[3];

	S_APP_ID id;
	APP_ACC_OPERATE appID;
 //!<解冻买家A账户交易金额M,仲裁费Xa,转入卖家B账户冻结项;
	S_APP_ID tag;
	tag.idlen = 32;
	appID.outheight = GetTxConFirmHeight(pLasttxhash)  + pDbData->tradeInfo.height;//freeze_height;
	memcpy(tag.ID,pLasttxhash,32);
	memcpy(&appID.FundTag,&tag,sizeof(tag));

	id.idlen = MAX_ACCOUNT_LEN;
	memcpy(id.ID,&pDbData->buyer.accounid[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	GetUserAppAccFoudWithTag(&appID.mMoney,&appID);

	appID.opeatortype = SUB_FREEZED_OP;
	writecode[0]=appID;
	//!<其次, 卖家B账户更新冻结项超时高度, 接收 M，Xa,
	memcpy(id.ID,&pDbData->seller.accounid[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	GetUserAppAccFoudWithTag(&appID.mMoney,&appID);
	appID.opeatortype = SUB_FREEZED_OP;
	writecode[1]=appID;

	Int64Add(&appID.mMoney,&pDbData->tradeInfo.moneyM,&appID.mMoney);
	Int64Add(&appID.mMoney,&pDbData->arbiterMoneyX,&appID.mMoney);
	appID.outheight = GetCurRunEnvHeight()  + pDbData->tradeInfo.height;
	memcpy(tag.ID,pCurtxhash,32);
	memcpy(&appID.FundTag,&tag,sizeof(tag));
	appID.opeatortype = ADD_FREEZED_OP;
	writecode[2]=appID;
	PrintAppOperate((APP_ACC_OPERATE*)writecode, 3);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, 3)){
		return false;
	}

    //!<更新仲裁者账户的冻结项
	if(!UpdateFreezeOverheight(&pDbData->tradeInfo.arbitationID.accounid[0],pDbData->tradeInfo.height,pLasttxhash,pCurtxhash)){
		return false;
	}
	return true;
}

bool DeliveryTrade(const TX_CONTRACT* const pContract){
   //!< 发货
	LogPrint((char *)pContract,sizeof(TX_CONTRACT),HEX);
	////key
	unsigned char key[36];
	DB_DATA dbdata;
	unsigned long outheight = GetSptDbData(pContract->txhash,pContract->height,key,&dbdata);
	if((!outheight) || (pContract->height != dbdata.tradeInfo.height)){
		LogPrint("DeliveryTrade input_height error",sizeof("DeliveryTrade input_height error")+1,STRING);
		return false;
	}

	//!<合法性校验
	 if((dbdata.state != STATE_ACCEPT) || (CheckTradeIsOverTime(outheight))){
		 LogPrint("DeliveryTrade input0 error",sizeof("DeliveryTrade input0 error")+1,STRING);
	   	 return  false;
	 }
	//!<校验是否为卖家 发货
	char account[6];
	if(!GetCurTxAccount(account,sizeof(account)) || (memcmp(account,dbdata.seller.accounid,sizeof(account)))){
		LogPrint("DeliveryTrade input1 error",sizeof("DeliveryTrade input1 error")+1,STRING);
		return false;
	}
	char curtxhash[32];
	if(!GetCurTxHash(curtxhash)){
		LogPrint("DeliveryTrade GetCurTxHash error",sizeof("DeliveryTrade GetCurTxHash error")+1,STRING);
		return false;
	}
	if(!WriteDeliveryPakage((DB_DATA*)&dbdata,(char*)pContract->txhash,curtxhash)){
		return false;
	}
	//!<表示发货
	dbdata.state = STATE_DELIVERY;
	if(!DeleteData(key,36))
	{
		LogPrint("DeliveryTrade DeleteData error",sizeof("DeliveryTrade DeleteData error")+1,STRING);
		return false;
	}
	outheight =GetCurRunEnvHeight()  + pContract->height;
//	outheight =GetCurRunEnvHeight()  + OVERTIME_HEIGHT;
	revert((char*)&outheight);
	memcpy(key,&outheight,sizeof(outheight));
	memcpy(&key[4],curtxhash,sizeof(curtxhash));
	LogPrint(key,36,HEX);
	LogPrint((char *)&dbdata,sizeof(dbdata),HEX);

	if( !WriteData(key,36,&dbdata,sizeof(dbdata))){
		LogPrint("DeliveryTrade WriteData error",sizeof("DeliveryTrade WriteData error")+1,STRING);
		return false;
	}

	return true;
}

bool WriteBuyerConfirm(const DB_DATA* const pDbData,const unsigned char*const pTxhash)
{
	APP_ACC_OPERATE writecode[3];

	S_APP_ID id;
	APP_ACC_OPERATE appID;
	//!<退还买家A账户仲裁费Xa;
	memcpy(id.ID,&pDbData->buyer.accounid[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.mMoney,&pDbData->arbiterMoneyX,sizeof(Int64));
	id.idlen = MAX_ACCOUNT_LEN;
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));

	appID.opeatortype = ADD_FREE_OP;
	writecode[0]=appID;

	//!<解冻卖家B账户的交易金额M和仲裁费Xb,Xa退还给买家A账户;
	S_APP_ID tag;
	tag.idlen = 32;
	appID.outheight = GetTxConFirmHeight(pTxhash)  + pDbData->tradeInfo.height;//freeze_height;
	memcpy(tag.ID,pTxhash,32);
	memcpy(&appID.FundTag,&tag,sizeof(tag));

	memcpy(id.ID,&pDbData->seller.accounid[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	GetUserAppAccFoudWithTag(&appID.mMoney,&appID);
	appID.opeatortype = SUB_FREEZED_OP;
	writecode[1]=appID;

	Int64Add(&pDbData->tradeInfo.moneyM,&pDbData->arbiterMoneyX,&appID.mMoney);
	appID.opeatortype = ADD_FREE_OP;
	writecode[2]=appID;
	PrintAppOperate((APP_ACC_OPERATE*)writecode, 3);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, 3)){
		return false;
	}
  //!<解冻仲裁者C的超时未判决的赔偿费用Y。
	memcpy(id.ID,&pDbData->tradeInfo.arbitationID.accounid[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	memcpy(&appID.mMoney,&pDbData->arbiterovertimeMoneyY,sizeof(Int64));

	appID.opeatortype = SUB_FREEZED_OP;
	writecode[0]=appID;

	appID.opeatortype = ADD_FREE_OP;
	writecode[1]=appID;
	PrintAppOperate((APP_ACC_OPERATE*)writecode, 2);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, 2)){
		return false;
	}
	return true;
}

bool BuyerConfirm(const TX_CONTRACT* const pContract){
    //!<买家确认收货
	LogPrint((char *)pContract,sizeof(TX_CONTRACT),HEX);

	////key
	unsigned char key[36];
	DB_DATA dbdata;
	unsigned long outheight = GetSptDbData(pContract->txhash,pContract->height,key,&dbdata);
	if((!outheight) || (pContract->height != dbdata.tradeInfo.height)){
		LogPrint("BuyerConfirm input_height error",sizeof("BuyerConfirm input_height error")+1,STRING);
		return false;
	}

	//!<合法性校验
	 if((dbdata.state != STATE_DELIVERY) || (CheckTradeIsOverTime(outheight))){
    	 LogPrint("BuyerConfirm input0 error",sizeof("BuyerConfirm input0 error")+1,STRING);
	   	 return  false;
	 }
	//!<校验是否为买家确认
	char account[6];
	if(!GetCurTxAccount(account,sizeof(account)) || (memcmp(account,dbdata.buyer.accounid,sizeof(account)))){
		LogPrint("BuyerConfirm input1 error",sizeof("BuyerConfirm input1 error")+1,STRING);
		return false;
	}

	if(!WriteBuyerConfirm(&dbdata,pContract->txhash)){
		return false;
	}
	//!<删除中间记录
	if(!DeleteData(key,36)){
		LogPrint("BuyerConfirm DeleteData error",sizeof("BuyerConfirm DeleteData error")+1,STRING);
		return false;
	}
	return true;

}

bool WriteArbitrationPackage(const DB_DATA* const pDbData,const  char*const pLasttxhash,const char*const pCurtxhash)
{   //!< 申请仲裁的 账户操作
	APP_ACC_OPERATE writecode[3];

	S_APP_ID id;
	APP_ACC_OPERATE appID;
	unsigned char num = 0;

	//!<从卖家B冻结账户，扣仲裁费Xa或Xb付给仲裁者C;并将仲裁者C冻结账户的超时未判决的赔偿费用Y转入仲裁发起者的冻结账户。
	S_APP_ID tag;
	tag.idlen = 32;
	appID.outheight = GetTxConFirmHeight(pLasttxhash)  + pDbData->tradeInfo.height;
	memcpy(tag.ID,pLasttxhash,32);
	memcpy(&appID.FundTag,&tag,sizeof(tag));
	id.idlen = MAX_ACCOUNT_LEN;

	//!<首先，删除原高度的冻结项
	memcpy(id.ID,&pDbData->seller.accounid[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	GetUserAppAccFoudWithTag(&appID.mMoney,&appID);
	appID.opeatortype = SUB_FREEZED_OP;
	writecode[0]=appID;

	//!<其次，再更新高度
	appID.outheight = GetCurRunEnvHeight()  +  pDbData->overtimeheightT;//pDbData->tradeInfo.height;
	memcpy(tag.ID,pCurtxhash,32);
	memcpy(&appID.FundTag,&tag,sizeof(tag));
	Int64Add(&pDbData->tradeInfo.moneyM,&pDbData->arbiterMoneyX,&appID.mMoney);

    if(pDbData->arbiterType)
    {  //!<卖家B 申请仲裁 ,接收C付的超时未判决的费用Y
      Int64Add(&appID.mMoney,&pDbData->arbiterovertimeMoneyY,&appID.mMoney);
    }
	appID.opeatortype = ADD_FREEZED_OP;
	writecode[1]=appID;
	num = 2;

	if(pDbData->arbiterType == 0)
	{
		//!<如果是买家B 申请仲裁，接收C付的超时未判决的费用Y
		memcpy(id.ID,&pDbData->buyer.accounid[0],MAX_ACCOUNT_LEN);
		memcpy(&appID.mMoney,&pDbData->arbiterovertimeMoneyY,sizeof(Int64));
		memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));

		appID.opeatortype = ADD_FREEZED_OP;
		writecode[2]=appID;
		num++;
	}
	PrintAppOperate((APP_ACC_OPERATE*)writecode, num);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, num)){
		return false;
	}
  //!<将仲裁者C冻结账户的超时未判决的赔偿费用Y转入仲裁发起者的冻结账户,接收发起者付的仲裁费X。
	appID.outheight = GetTxConFirmHeight(pLasttxhash)  + pDbData->tradeInfo.height;
	memcpy(tag.ID,pLasttxhash,32);
	memcpy(&appID.FundTag,&tag,sizeof(tag));
	memcpy(id.ID,&pDbData->tradeInfo.arbitationID.accounid[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	memcpy(&appID.mMoney,&pDbData->arbiterovertimeMoneyY,sizeof(Int64));

	appID.opeatortype = SUB_FREEZED_OP;
	writecode[0]=appID;

	memcpy(&appID.mMoney,&pDbData->arbiterMoneyX,sizeof(Int64));
	appID.opeatortype = ADD_FREE_OP;
	writecode[1]=appID;
	PrintAppOperate((APP_ACC_OPERATE*)writecode, 2);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, 2)){
		return false;
	}
	return true;
}

bool Arbitration(const TX_Arbitration* const pContract){
   //!<申请仲裁
	LogPrint((char *)pContract,sizeof(TX_Arbitration),HEX);

	////key
	unsigned char key[36];
	DB_DATA dbdata;
	unsigned long outheight = GetSptDbData(pContract->txhash,pContract->height,key,&dbdata);
	if((!outheight) || (pContract->height != dbdata.tradeInfo.height)){
		LogPrint("Arbitration input_height error",sizeof("Arbitration input_height error")+1,STRING);
		return false;
	}
	//!<合法性校验
	 if((dbdata.state != STATE_DELIVERY) || (CheckTradeIsOverTime(outheight)) ||
			 (memcmp(pContract->arbitationID,dbdata.tradeInfo.arbitationID.accounid,6))){
		 LogPrint("Arbitration input0 error",sizeof("Arbitration input0 error")+1,STRING);
	   	 return  false;
	 }
	//!<校验是否是买家或卖家
	char account[6];
	if(!GetCurTxAccount(account,sizeof(account))){
		 LogPrint("Arbitration input1 error",sizeof("Arbitration input1 error")+1,STRING);
		return false;
	}
	if(!memcmp(account,dbdata.buyer.accounid,sizeof(account))) {
		dbdata.arbiterType = 0 ;
	}else if(!memcmp(account,dbdata.seller.accounid,sizeof(account))){
		dbdata.arbiterType = 1 ;
	}else{
		 LogPrint("Arbitration input2 error",sizeof("Arbitration input2 error")+1,STRING);
		return false;
	}
	char curtxhash[32];
	if(!GetCurTxHash(curtxhash)){
		LogPrint("SendStartTrade GetCurTxHash error",sizeof("SendStartTrade GetCurTxHash error")+1,STRING);
		return false;
	}
	if(!WriteArbitrationPackage(&dbdata,(char*)pContract->txhash,curtxhash)){
		return false;
	}
	//!<表示申请仲裁
	dbdata.state = STATE_ARBITER;
    memcpy(dbdata.arbitHash,curtxhash,32);
	if(!DeleteData(key,36))
	{
		LogPrint("Arbitration DeleteData error",sizeof("Arbitration DeleteData error")+1,STRING);
		return false;
	}
	//!<其次，再更新高度
	outheight =GetCurRunEnvHeight()  + dbdata.overtimeheightT;//dbdata.tradeInfo.height;
//	outheight =GetCurRunEnvHeight()  + OVERTIME_HEIGHT;
	revert((char*)&outheight);
	memcpy(key,&outheight,sizeof(outheight));
	memcpy(&key[4],curtxhash,sizeof(curtxhash));
	LogPrint(key,36,HEX);
	LogPrint(&dbdata,sizeof(dbdata),HEX);
	if( !WriteData(key,36,&dbdata,sizeof(dbdata))){
		LogPrint("Arbitration WriteData error",sizeof("Arbitration WriteData error")+1,STRING);
		return false;
	}
	return true;
}

bool WriteFinalResultPakage(const TX_FINALRESULT_CONTRACT* const pContract,const DB_DATA* const pDbData,char type){
	   //!< 执行最终裁决的 账户操作
	APP_ACC_OPERATE writecode[3];

	S_APP_ID id;
	APP_ACC_OPERATE appID;

	S_APP_ID tag;
	tag.idlen = 32;
	appID.outheight = GetTxConFirmHeight(pContract->arbitHash)  + pDbData->overtimeheightT;//pDbData->tradeInfo.height;
	memcpy(tag.ID,pContract->arbitHash,32);
	memcpy(&appID.FundTag,&tag,sizeof(tag));

	//!<解冻卖家B账户里的交易金额M，付给买家A指定的金额，付给卖家B指定的金额。
	memcpy(id.ID,&pDbData->seller.accounid[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.mMoney,&pDbData->tradeInfo.moneyM,sizeof(Int64));
	id.idlen = MAX_ACCOUNT_LEN;
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));

	appID.opeatortype = SUB_FREEZED_OP;
	writecode[0]=appID;

	if(type)
	{ //!<卖家是胜诉方
	    memcpy(&appID.mMoney,&pContract->winnerMoney,sizeof(Int64));
	}else{
		memcpy(&appID.mMoney,&pContract->loserMoney,sizeof(Int64));
	}
	appID.opeatortype = ADD_FREE_OP;
	writecode[1]=appID;

	memcpy(id.ID,&pDbData->buyer.accounid[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	if(type == 0)
	{ //!<买家是胜诉方
	    memcpy(&appID.mMoney,&pContract->winnerMoney,sizeof(Int64));
	}else{
		memcpy(&appID.mMoney,&pContract->loserMoney,sizeof(Int64));
	}
	appID.opeatortype = ADD_FREE_OP;
	writecode[2]=appID;
	PrintAppOperate((APP_ACC_OPERATE*)writecode, 3);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, 3)){
		return false;
	}
  //!<将申请仲裁者冻结账户里的超时未判决的赔偿费用Y，解冻给仲裁者C
    if(pDbData->arbiterType)
    { //!< 1:卖家发起
    	memcpy(id.ID,&pDbData->seller.accounid[0],MAX_ACCOUNT_LEN);
    }else{
    	memcpy(id.ID,&pDbData->buyer.accounid[0],MAX_ACCOUNT_LEN);
    }
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	memcpy(&appID.mMoney,&pDbData->arbiterovertimeMoneyY,sizeof(Int64));
	appID.opeatortype = SUB_FREEZED_OP;
	writecode[0]=appID;

	memcpy(id.ID,&pDbData->tradeInfo.arbitationID.accounid[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	appID.opeatortype = ADD_FREE_OP;
	writecode[1]=appID;
	PrintAppOperate((APP_ACC_OPERATE*)writecode, 2);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, 2)){
		return false;
	}
	//!<最终仲裁费，由败诉方承担
	memcpy(id.ID,&pDbData->seller.accounid[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	memcpy(&appID.mMoney,&pDbData->arbiterMoneyX,sizeof(Int64));
    appID.opeatortype = SUB_FREEZED_OP;
	writecode[0]=appID;

	if(type == 0)
	{ //!<买家是胜诉方则从卖家冻结账户拿回仲裁费Xa
		memcpy(id.ID,&pDbData->buyer.accounid[0],MAX_ACCOUNT_LEN);
	}else{
//		memcpy(id.ID,&pDbData->seller.accounid[0],MAX_ACCOUNT_LEN);
	}
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	appID.opeatortype = ADD_FREE_OP;
	writecode[1]=appID;
	PrintAppOperate((APP_ACC_OPERATE*)writecode, 2);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, 2)){
		return false;
	}
    return true;
}
bool RunFinalResult(const TX_FINALRESULT_CONTRACT* const pContract){
   //!<通知最终裁决结果，并执行
	LogPrint((char *)pContract,sizeof(TX_FINALRESULT_CONTRACT),HEX);

	////key
	unsigned char key[36];
	DB_DATA dbdata;
	unsigned long outheight = GetSptDbData(pContract->arbitHash,pContract->overtimeheightT,key,&dbdata);
	if((!outheight) || (pContract->overtimeheightT != dbdata.overtimeheightT)){
		LogPrint("RunFinalResult input_height error",sizeof("RunFinalResult input_height error")+1,STRING);
		return false;
	}
	//!<合法性校验
	 if((dbdata.state != STATE_ARBITER) || (CheckTradeIsOverTime(outheight))){
		 LogPrint("RunFinalResult input0 error",sizeof("RunFinalResult input0 error")+1,STRING);
		 return  false;
	 }

	//!<校验 裁决者
	char account[6];
	if(!GetCurTxAccount(account,sizeof(account)) || (memcmp(account,dbdata.tradeInfo.arbitationID.accounid,sizeof(account)))){
		LogPrint("RunFinalResult input1 error",sizeof("RunFinalResult input1 error")+1,STRING);
		return false;
	}
     char type = 0; //!<输赢类型
	//!<校验判决结果
	if((!memcmp(pContract->winner.accounid,dbdata.buyer.accounid,6)) &&
			(!memcmp(pContract->loser.accounid,dbdata.seller.accounid,6))){
		type = 0;
	}
	else if((!memcmp(pContract->winner.accounid,dbdata.seller.accounid,6)) &&
			  (!memcmp(pContract->loser.accounid,dbdata.buyer.accounid,6))){
		type = 1;
	}
	else{
		LogPrint("RunFinalResult input2 error",sizeof("RunFinalResult input2 error")+1,STRING);
		return false;
	}
	//!<输赢家ID 不能相同
	if(0 == memcmp(pContract->loser.accounid,pContract->winner.accounid,6)){
		LogPrint("RunFinalResult input3 error",sizeof("RunFinalResult input3 error")+1,STRING);
		return false;
	}

	//!<校验 判决的金额分配
	Int64 compare;
	Int64Inital(&compare,"\x00",1);
	if((Int64Compare(&pContract->winnerMoney,&compare) == COMP_LESS) ||
		 (Int64Compare(&pContract->loserMoney,&compare) == COMP_LESS)){
		LogPrint("RunFinalResult input4 error",sizeof("RunFinalResult input4 error")+1,STRING);
		return false;
	}
	Int64Add(&pContract->winnerMoney,&pContract->loserMoney,&compare);
    if(Int64Compare(&compare,&dbdata.tradeInfo.moneyM) != COMP_EQU){
    	LogPrint("RunFinalResult input5 error",sizeof("RunFinalResult input5 error")+1,STRING);
    	return false;
    }

    //!<账户操作
	if(!WriteFinalResultPakage(pContract,&dbdata,type)){
		return false;
	}
	//!<删除记录
	if(!DeleteData(key,36)){
		LogPrint("RunFinalResult DeleteData error",sizeof("RunFinalResult DeleteData error")+1,STRING);
		return false;
	}
	return true;
}

typedef struct {
	unsigned char type;
	Int64 money;
}DAWEL_DATA_SOMEMONEY;

int main()
{
  __xdata static unsigned char contact[300];  // 100
	unsigned long len = 300; //100
	GetCurTxContact(contact,len);

 	switch(contact[0])
 	{
 	    case TX_REGISTER:  //注册仲裁账户
		{
			LogPrint("TX_REGISTER",sizeof("TX_REGISTER"),STRING);
			if(!Register((TX_REGISTER_CONTRACT*)contact))
			{
				LogPrint("TX_REGISTER error",sizeof("TX_REGISTER error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}
 	    case TX_MODIFYREGISTER:  // 修改仲裁者注册信息
		{
			LogPrint("TX_MODIFYREGISTER",sizeof("TX_MODIFYREGISTER"),STRING);
			if(!ModifyRegister((TX_REGISTER_CONTRACT*)contact))
			{
				LogPrint("TX_MODIFYREGISTER error",sizeof("TX_MODIFYREGISTER error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}

 		case TX_ARBIT_ON:  //仲裁开启
 		case TX_ARBIT_OFF: //仲裁暂停
		{
			LogPrint("TX_ARBIT_ON",sizeof("TX_ARBIT_ON"),STRING);
			if(!ArbitOnOrOff((unsigned char *) contact))
			{
				LogPrint("TX_ARBIT_ON error",sizeof("TX_ARBIT_ON error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}

 		case  TX_UNREGISTER: //注销仲裁账户
 		{
 			LogPrint("TX_UNREGISTER",sizeof("TX_UNREGISTER"),STRING);
 			if(!UnRegister())
 			{
 				LogPrint("TX_UNREGISTER error",sizeof("TX_UNREGISTER error"),STRING);
 				__VmExit(RUN_SCRIPT_DATA_ERR);
 			}
 			break;
 		}
		case TX_SEND: //挂单
		{
			LogPrint("TX_SEND",sizeof("TX_SEND"),STRING);
			if(!SendStartTrade((TX_SNED_CONTRACT*) contact))
			{
				LogPrint("TX_SEND error",sizeof("TX_SEND error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}
 		case  TX_CANCEL: //挂单取消
 		{
 			LogPrint("TX_CANCEL",sizeof("TX_CANCEL"),STRING);
 			if(!SendCancelTrade((TX_CONTRACT*) contact))
 			{
 				LogPrint("TX_CANCEL error",sizeof("TX_CANCEL error"),STRING);
 				__VmExit(RUN_SCRIPT_DATA_ERR);
 			}
 			break;
 		}
		case TX_ACCEPT: //接单
		{
			LogPrint("TX_ACCEPT",sizeof("TX_ACCEPT"),STRING);
			if(!AcceptTrade((TX_CONTRACT*) contact))
			{
				LogPrint("TX_ACCEPT error",sizeof("TX_ACCEPT error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}

		case TX_DELIVERY:  //发货
		{
			LogPrint("TX_DELIVERY",sizeof("TX_DELIVERY"),STRING);
			if(!DeliveryTrade((TX_CONTRACT*) contact))
			{
				LogPrint("TX_DELIVERY error",sizeof("TX_DELIVERY error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}

		case  TX_BUYERCONFIRM: //买家确认交易
		{
			LogPrint("TX_BUYERCONFIRM",sizeof("TX_BUYERCONFIRM"),STRING);
			if(!BuyerConfirm((TX_CONTRACT*) contact))
			{
				LogPrint("TX_BUYERCONFIRM error",sizeof("TX_BUYERCONFIRM error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}

		case  TX_ARBITRATION://申请仲裁
		{
			LogPrint("TX_ARBITRATION",sizeof("TX_ARBITRATION"),STRING);
			if(!Arbitration((TX_Arbitration*) contact))
			{
				LogPrint("TX_ARBITRATION error",sizeof("TX_ARBITRATION error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}
		case  TX_FINALRESULT://通知裁决结果，并执行
		{
			LogPrint("TX_FINALRESULT",sizeof("TX_FINALRESULT"),STRING);
			if(!RunFinalResult((TX_FINALRESULT_CONTRACT*) contact))
			{
				LogPrint("TX_FINALRESULT error",sizeof("TX_FINALRESULT error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}


		case 0xff:
 		{
 		 	char account[6];
 			if(!GetCurTxAccount(account,sizeof(account))){
 				LogPrint("RechargeWithdraw GetCurTxAccount error",sizeof("RechargeWithdraw GetCurTxAccount error"),STRING);
 				return false;
 			}
 			//!<开发者账户提现时，必须冻结所有仲裁者的注册担保押金。
 			if(!memcmp(account,&DeveloperAccount[0],6))
 			{
				if(contact[1] == 0x01){
					Int64 withMoney;
					Int64Inital(&withMoney,"\x00",sizeof(Int64));
					if(!CheckDeveloperAccWithdraw(withMoney)){
						__VmExit(RUN_SCRIPT_DATA_ERR);
					}
				}
				else if(contact[1] == 0x03){
					DAWEL_DATA_SOMEMONEY *pBuf= NULL;
					char buffer[12];
					memcpy(&buffer[0], &contact[2], 12);

					pBuf = (DAWEL_DATA_SOMEMONEY*)(buffer);
//					LogPrint(&pBuf->money,sizeof(Int64),HEX);
					if(!CheckDeveloperAccWithdraw(pBuf->money)){
						__VmExit(RUN_SCRIPT_DATA_ERR);
					}
				}
 			}

			if(!RechargeWithdraw(contact))
			{
			 	__VmExit(RUN_SCRIPT_DATA_ERR);
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

