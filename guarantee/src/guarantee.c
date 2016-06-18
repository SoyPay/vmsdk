#include <string.h>
#include<stdlib.h>
#include <stdio.h>
#include"VmSdk.h"
__root __code static const char checkaccount[]@0x0014 ={0x01};  //0x01 �򿪣�0x00 �ر�

#define OVERTIME_HEIGHT  (1440 * 7)    //!<�������ڵĳ�ʱ���߶�

#define ARBITRATION_MIN_HEIGHT (1440 * 1)  //!<�о�����ʱ��

//#define COIN     100000000
//#define CENT     1000000

//#define JUDGE_REGISTER_FEE 10000     //!<�ٲ���C ע��ĵ���Ѻ��
//!<��Ӧ���ֽ���               "\x00\x00\x00\xE8\xD4\xA5\x10\x00"   10000 * COIN

//#define SEND_ACCOUNT_FEE   10         //!<�ҵ� �̶���10����
//!<��Ӧ���ֽ���      "\x00\x00\x00\x00\x3B\x9A\xCA\x00"   10 * COIN
const static char DeveloperAccount[6]="\xC0\x52\x01\x00\x01\x00";//!<�����ߵ�ַID  86720-1 ��������
//const static char DeveloperAccount[6]="\x00\x00\x00\x00\x14\x00";//!<�����ߵ�ַID
//string regAddr="dk2NNjraSvquD9b4SQbysVRQeFikA55HLi";   //RegID = "0-20"



enum TRADE_STATE{   //!<�������׵ĸ���״̬
	STATE_SEND = 0x01,    //�ҵ�
	STATE_ACCEPT = 0x02,  //�ӵ�
	STATE_DELIVERY = 0x03, //����
	STATE_ARBITER = 0x04  //�����ٲ�
};

enum TXTYPE{
	TX_REGISTER = 0x01,     //ע���ٲ��˻�
	TX_MODIFYREGISTER =0x02,// �޸��ٲ���ע����Ϣ
	TX_ARBIT_ON    = 0x03, //�ٲÿ���
	TX_ARBIT_OFF   = 0x04, //�ٲ���ͣ
	TX_UNREGISTER = 0x05,  //ע���ٲ��˻�
	TX_SEND = 0x06,        //�ҵ�
	TX_CANCEL = 0x07,      //ȡ���ҵ�
	TX_ACCEPT = 0x08,      //�ӵ�
	TX_DELIVERY = 0x09,    //����
	TX_BUYERCONFIRM = 0x0a,//���ȷ���ջ�
	TX_ARBITRATION = 0x0b, //�����ٲ�
	TX_FINALRESULT = 0x0c, //�þ����
};

typedef struct  {
	Int64 arbiterMoneyX;             //!<�ٲ÷���X
	Int64 overtimeMoneyYmax;  //!<��ʱδ�о�������⳥����Y
	Int64 configMoneyZ;              //!<������þ�����Z
	unsigned long  overtimeheightT;  //!<�о�����ʱ��T
	char  comment[220];             //!<��ע˵�� �ַ�����\0���������Ȳ����0
} DB_DATA_ARBITER_REGISTER;         //!<�ٲ���ע�����Ϣ

typedef struct {
	enum TXTYPE nType;            //!<��������
	DB_DATA_ARBITER_REGISTER db_data;        //!<�ٲ���ע�����Ϣ
}TX_REGISTER_CONTRACT;  //!<ע���ٲ��˻�

#define  ARBITER_ON   0xaa   //!<�ٲÿ���
#define  ARBITER_OFF  0x55   //!<�ٲ���ͣ

//����� �ٲ���ע�������ݽṹ
typedef struct{
  unsigned char arbitState;        //!<0xaa,�ٲÿ���;0x55,�ٲ���ͣ
  DB_DATA_ARBITER_REGISTER db_data;        //!<�ٲ���ע�����Ϣ
}DB_DATA_ARBITER;

enum SEND_TYPE{      //!<�ҵ���״̬
	SEND_TYPE_BUY = 0x00,   //!<�ҵ� ��
	SEND_TYPE_SELL = 0x01,  //!<�ҵ� ��
};
typedef struct{
	enum SEND_TYPE sendType;         //!<�ҵ�����:0 ��  1��
	ACCOUNT_ID arbitationID;        //!<�ٲ���ID������6�ֽڵ��˻�ID��
	Int64 moneyM;                   //!<���׽��
	unsigned long height;           //!<ÿ�����׻��ڵĳ�ʱ�߶�
	char goods[20];               //!<��Ʒ��Ϣ �ַ�����\0���������Ȳ����0
    char  comment[200];             //!<��ע˵�� �ַ�����\0���������Ȳ����0
}TX_SEND_PUBLICDATA;
typedef struct {
	enum TXTYPE nType;            //!<��������
	TX_SEND_PUBLICDATA tradeInfo;   //!<���������Ϣ
}TX_SNED_CONTRACT;                  //!<�ҵ�

typedef struct  {
	enum TRADE_STATE  state;         //!<��ʾ����״̬
	TX_SEND_PUBLICDATA tradeInfo;    //!<���������Ϣ
	Int64 arbiterMoneyX;             //!<�ٲ÷���X
	Int64 arbiterovertimeMoneyY;     //!<��ʱδ�о����⳥����Y
	Int64 configMoneyZ;              //!<������þ�����Z
	unsigned long  overtimeheightT;  //!<�о�����ʱ��T

    //������ѡ�UI����state����ȡ
	ACCOUNT_ID 	buyer;      	     //!<���ID������6�ֽڵ��˻�ID����ѡ��
	ACCOUNT_ID 	seller;      	     //!<����ID������6�ֽڵ��˻�ID����ѡ��
	unsigned char arbiterType;       //!<�����ٲ����� 0����ҷ���1:���ҷ���
	unsigned char arbitHash[32];     //!<�����ٲõ�hash
} DB_DATA;    //!<��Ҫ��������ݽṹ�����׹�����


typedef struct {
	enum TXTYPE nType;            //!<��������
	unsigned char txhash[32];       //!<����hash
	unsigned long height;          //!<ÿ�����׻��ڵĳ�ʱ�߶�
} TX_CONTRACT;

typedef struct {
	enum TXTYPE nType;            //!<��������
	unsigned char txhash[32];       //!<�ӵ�����hash
	unsigned long height;          //!<ÿ�����׻��ڵĳ�ʱ�߶�
	char  arbitationID[6];       //!<�ٲ���ID������6�ֽڵ��˻�ID��
} TX_Arbitration;                //!<�����ٲ�

typedef struct {
	enum TXTYPE nType;            //!<��������
	unsigned char arbitHash[32];     //!<�����ٲõĽ���hash
	unsigned long overtimeheightT;//!<�о�����ʱ��T
	ACCOUNT_ID 	winner;      	//!<Ӯ��ID������6�ֽڵ��˻�ID��
	Int64 winnerMoney;            //!<���ջ�õĽ��
	ACCOUNT_ID  loser;       //!<���ID������6�ֽڵ��˻�ID��
	Int64 loserMoney;            //!<���ջ�õĽ��
}TX_FINALRESULT_CONTRACT;        //!<���ղþ�


void PrintAppOperate(APP_ACC_OPERATE* pAppID,unsigned char num){
// ��ӡӦ���˻�����ǰ�ı�Ҫ��Ϣ��
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
	//!<������֣������߲����� ע���ٲ����ܵĵ���Ѻ��
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
		if(Int64Mul(&refundMoney,&cmpMoney,&money))  //!<ע����һ���Ѻ��
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
		return true; //���ٲ���ע�� �ĵ���Ѻ��,ֱ�ӷ���
	}
	Int64Inital(&cmpMoney,"\x01",1);  //!< ����С��1
    if(Int64Compare(&withMoney,&cmpMoney) == COMP_LESS)
    {  //!<withMoneyΪ0,��ʾȫ�����֣�������
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
	 if(Int64Compare(&pRet, &cmpMoney) == COMP_LESS){      //ϵͳӦ���˻���Ҫ���㹻�����ɽ��
		 LogPrint("not enough money",sizeof("not enough money")+1,STRING);
		 return false;
	 }
	 return true;
}

bool OpRegisterAccount(const char*const pAccount,unsigned char type){
   //!<�ٲ���ע��ע��ʱ���˻�����
	APP_ACC_OPERATE writecode[2];

	S_APP_ID id;
	APP_ACC_OPERATE appID;

	memcpy(id.ID,pAccount,MAX_ACCOUNT_LEN);
	id.idlen = MAX_ACCOUNT_LEN;
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));

	Int64Inital(&appID.mMoney,"\x00\x00\x00\xE8\xD4\xA5\x10\x00",8);//!<  JUDGE_REGISTER_FEE * COIN
	// type 1:ע�ᣬ 0�� ע��
	if(type)
	{
		//!<���ٲ����˻�JUDGE_REGISTER_FEE * COIN �ҵ���Ѻ�𵽿������˻�
		 appID.opeatortype = SUB_FREE_OP;
	}else{
		//!<�˻����ٲ����˻�����Ѻ��һ��
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
    {  	//ע��
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
	//!< �ٲ���ע����Ϣ
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
     //!<�����ٲ���ע����Ϣ
	DB_DATA_ARBITER dbdata;
	unsigned char key[20] = {0};
	memcpy(key,"judge",5);
	memcpy(&key[5],account,sizeof(account));
	LogPrint(key,11,HEX);
	if(ReadData(key,11, &dbdata,sizeof(dbdata))){
		 //!<��ע�� ���������ظ�ע��
		LogPrint("Register repeat error",sizeof("Register repeat error"),STRING);
		return false;
	}
	else{
		//!<δע��
		dbdata.arbitState = ARBITER_ON;  //!<Ĭ�Ͽ���
		memcpy(&dbdata.db_data,&pContract->db_data,sizeof(pContract->db_data));
		if(!WriteData(key,11,&dbdata,sizeof(dbdata))){
			LogPrint("Register WriteData error",sizeof("Register WriteData error"),STRING);
			return false;
		}
	}

	//!<ע��ʱ�����ٲ����˻�10000���ҵ���Ѻ�𵽿������˻����޸�ʱ�����ظ�Ѻ��
	if(!OpRegisterAccount(account,1)){
		LogPrint("Register OpRegisterAccount error",sizeof("Register OpRegisterAccount error")+1,STRING);
		return false;
	}

 //!<�˻������ɹ�����ͳ��ע���ٲ��ߵ��ܸ���
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
	//!< �޸��ٲ���ע����Ϣ
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
    //!<�����ٲ���ע����Ϣ
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
	  //!<ֻ��ע���ˣ�����ȥ�޸�
		memcpy(&dbdata.db_data,&pContract->db_data,sizeof(pContract->db_data));
		if(!ModifyData(key,11, &dbdata,sizeof(dbdata))){
			LogPrint("ModifyRegister ModifyData error",sizeof("ModifyRegister ModifyData error"),STRING);
			return false;
		}
	}
	else{
		//!<δע�� ������
		LogPrint("ModifyRegister ReadData error",sizeof("ModifyRegister ReadData error"),STRING);
		return false;
	}
	return true;
}

bool ArbitOnOrOff(const unsigned char* const pContract){
	//!< �޸��ٲ��߿�������ͣ
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
	  //!<ֻ��ע���ˣ�����ȥ�޸�
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
		//!<δע�� ������
		LogPrint("ArbitOnOrOff ReadData error",sizeof("ArbitOnOrOff ReadData error"),STRING);
		return false;
	}
	return true;
}

bool UnRegister(void){
    //!<ע���ٲ���
	unsigned char key[20] = {0};
 	char account[6];
	if(!GetCurTxAccount(account,sizeof(account))){
		LogPrint("UnRegister GetCurTxAccount error",sizeof("UnRegister GetCurTxAccount error"),STRING);
		return false;
	}

	memcpy(key,"judge",5);
	memcpy(&key[5],account,sizeof(account));
	LogPrint(key,11,HEX);
	if(!DeleteData(key,11)){  //!<�������ظ�ע��
		LogPrint("UnRegister DeleteData error",sizeof("UnRegister DeleteData error"),STRING);
		return false;
	}
	//!<�˻�һ���Ѻ��
	if(!OpRegisterAccount(account,0)){
		LogPrint("UnRegister OpRegisterAccount error",sizeof("UnRegister OpRegisterAccount error")+1,STRING);
		return false;
	}

	//!<�˻������ɹ����ٸ���ע���ٲ��ߵ��ܸ���
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
    //!<�������A�˻����׽��M�Ϳ���Ҫ�����ٲ÷�Xa,�����ٲ���������þ���Z
    //!<�򶳽�����B�˻�����Ҫ�����ٲ÷�Xb,�����ٲ���������þ���Z
	memcpy(&appID.mMoney,&pDbData->arbiterMoneyX,sizeof(appID.mMoney));
	if(SEND_TYPE_SELL == pDbData->tradeInfo.sendType)
	{ //!<��     Xb
		memcpy(id.ID,&pDbData->seller.accounid[0],MAX_ACCOUNT_LEN);
	}else{
		//!<�� M,Xa
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

	//!< �����ɽ��ת��������
	appID.opeatortype = ADD_FREEZED_OP;
	writecode[0] = appID;

	Int64Add(&appID.mMoney,&pDbData->configMoneyZ,&appID.mMoney);//!<���� �����ٲ���������þ���Z
	appID.opeatortype=SUB_FREE_OP;
	writecode[1] = appID;
	PrintAppOperate((APP_ACC_OPERATE*)writecode, 2);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, 2)){
		return false;
	}

    //!< �ҵ����� �������˻�10����
	Int64Inital(&appID.mMoney,"\x00\x00\x00\x00\x3B\x9A\xCA\x00",8); //!< SEND_ACCOUNT_FEE * COIN
	//!<����ҵ�����10���ҵ��������˻�
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

	//!<�ٲ���C �˻������� ������þ���Z�����ᳬʱ�о��⸶���Y
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
//!< �ҵ�
	LogPrint((char *)pContract,sizeof(TX_SNED_CONTRACT),HEX);

    //!<��ⷢ�Ͱ�
	char account[6];
	if(!GetCurTxAccount(account,sizeof(account))){
		LogPrint("SendStartTrade GetCurTxAccount error",sizeof("SendStartTrade GetCurTxAccount error")+1,STRING);
		return false;
	}
	//!<�ҵ����� ,���׽����,�ٲ���ID������ҵ�����ͬ,�����ǿ������˻�ID
	Int64 compare;
	Int64Inital(&compare,"\x01",1);
	if((pContract->tradeInfo.sendType > SEND_TYPE_SELL) ||
			(Int64Compare(&pContract->tradeInfo.moneyM,&compare) == COMP_LESS) ||
			(!memcmp(account,&pContract->tradeInfo.arbitationID.accounid[0],sizeof(account))) ||
			(!memcmp(DeveloperAccount,account,sizeof(account)))){
		LogPrint("SendStartTrade input0 error",sizeof("SendStartTrade input0 error")+1,STRING);
		return false;
	}
	//!<�ٲ����Ƿ� �����ٲ�
	DB_DATA_ARBITER arbitData;
	unsigned char key[36] = {0};
	memcpy(key,"judge",5);
	memcpy(&key[5],&pContract->tradeInfo.arbitationID.accounid[0],MAX_ACCOUNT_LEN);
	if((!ReadData(key,11, &arbitData,sizeof(arbitData))) || (arbitData.arbitState != ARBITER_ON)){
	  LogPrint("SendStartTrade input1 error",sizeof("SendStartTrade input1 error")+1,STRING);
	  return false;
	}
	 //!<��ʱ�߶��Ƿ�С�ڵ������ʱ�߶�,�Ƿ���ڵ����о�����ʱ��T
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
	//!<��д��Ҫ����Ϣ��������
	DB_DATA dbdata;

	memset(&dbdata,0,sizeof(dbdata));
    dbdata.state = STATE_SEND;
	memcpy(&dbdata.tradeInfo,&pContract->tradeInfo,sizeof(TX_SEND_PUBLICDATA));
	memcpy(&dbdata.arbiterMoneyX,&arbitData.db_data.arbiterMoneyX,sizeof(Int64));
	if(Int64Compare(&pContract->tradeInfo.moneyM,&arbitData.db_data.overtimeMoneyYmax) == COMP_LESS)
	{  //!<������׽��MС�� Y����ֻ�⳥���M
		memcpy(&dbdata.arbiterovertimeMoneyY,&pContract->tradeInfo.moneyM,sizeof(Int64));
	}else
	{  //!<������׽��M ���ڵ���Y,�����ֻ�⳥Y;
	    memcpy(&dbdata.arbiterovertimeMoneyY,&arbitData.db_data.overtimeMoneyYmax,sizeof(Int64));
	}
	memcpy(&dbdata.configMoneyZ,&arbitData.db_data.configMoneyZ,sizeof(Int64));
	dbdata.overtimeheightT = arbitData.db_data.overtimeheightT;
	///LVLV
	if(SEND_TYPE_SELL == pContract->tradeInfo.sendType)
	{ //��
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
    //!<�ҵ����˻�����
	if(!OpWriteSendAccount(&dbdata,txhash)){
		LogPrint("SendStartTrade WriteSendPakage error",sizeof("SendStartTrade WriteSendPakage error")+1,STRING);
		return false;
	}
	return true;
}

unsigned long GetSptDbData(const unsigned char* const pTxhash,unsigned long height,unsigned char *pKey,DB_DATA* pDbData){
	//!<ȡ�ҵ�����ʱ�������DB_DATA�м���

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
           //!< type 1:�ҵ� 0:ȡ��
	APP_ACC_OPERATE writecode[3];

	S_APP_ID id;
	APP_ACC_OPERATE appID;
    //!<�ⶳ���A�˻����׽��M�Ϳ���Ҫ�����ٲ÷�Xa
    //!<��ⶳ����B�˻�����Ҫ�����ٲ÷�Xb
	memcpy(&appID.mMoney,&pDbData->arbiterMoneyX,sizeof(appID.mMoney));
	if(SEND_TYPE_SELL == pDbData->tradeInfo.sendType)
	{ //!<��     Xb
		memcpy(id.ID,&pDbData->seller.accounid[0],MAX_ACCOUNT_LEN);
	}else{
		//!<�� M,Xa
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

	//!< �Ӷ�����ת�����ɽ��
	appID.opeatortype = SUB_FREEZED_OP;
	writecode[0] = appID;

	appID.opeatortype = ADD_FREE_OP;
	writecode[1] = appID;
	PrintAppOperate((APP_ACC_OPERATE*)writecode, 2);
	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)writecode, 2)){
		return false;
	}

    //!< �ҵ����� �������˻�10����
	Int64Inital(&appID.mMoney,"\x00\x00\x00\x00\x3B\x9A\xCA\x00",8); //!< SEND_ACCOUNT_FEE * COIN
	Int64 compare;
	//!<�ⶳ�������˻��ҵ���5�����˻ظ��ҵ���,5���Ҹ������������˻�
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

	//!<�ٲ���C �˻����ⶳ��ʱ�о��⸶���Y
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
	//!<�ҵ�ȡ��

	LogPrint((char *)pContract,sizeof(TX_CONTRACT),HEX);
	//////key
	unsigned char key[36];
	DB_DATA dbdata;
	unsigned long outheight = GetSptDbData(pContract->txhash,pContract->height,key,&dbdata);
	if((!outheight) || (pContract->height != dbdata.tradeInfo.height)){
		LogPrint("SendCancelTrade input_height error",sizeof("SendCancelTrade input_height error")+1,STRING);
		return false;
	}

    //!<�ӵ��Ժ� ,��ʱ ����ȡ��
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
      //!<ֻ�ܹҵ����Լ�ȡ��
      if(memcmp(account,dbdata.seller.accounid,sizeof(account)))
      {
    	LogPrint("SendCancelTrade input1 error",sizeof("SendCancelTrade input1 error")+1,STRING);
   	    return false;
      }
	}else{
	  //!<ֻ�ܹҵ����Լ�ȡ��
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
 * ����ָ���˻��Ķ����ʱ�߶�*/
bool UpdateFreezeOverheight(const char *pAccount,const unsigned long height,const char*const pLasttxhash,const char*const pCurtxhash){
	APP_ACC_OPERATE writecode[2];

	S_APP_ID id;
	APP_ACC_OPERATE appID;
	 //!<�����˻��Ķ����� ��ʱ�߶�
	//!<����ԭ�߶ȵĶ�����ȫɾ��
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
	//!<���, �����˻������ʱ�߶�
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
   //!<���� ������ҵĹҵ�
	APP_ACC_OPERATE writecode[2];

	S_APP_ID id;
	APP_ACC_OPERATE appID;

	 //!<�������A�˻��Ķ�����
	if(!UpdateFreezeOverheight(&pDbData->buyer.accounid[0],pDbData->tradeInfo.height,pLasttxhash,pCurtxhash)){
		return false;
	}
    //!<��������B�˻�����Ҫ�����ٲ÷�Xb;
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
   //!<��� �������ҵĹҵ�
	APP_ACC_OPERATE writecode[2];

	S_APP_ID id;
	APP_ACC_OPERATE appID;
	//!<������ҽ��׽��M,����Ҫ�����ٲ÷�Xa��
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

	//!<��������B�����˻���ʱ�߶�
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
	{ //!<��
		if(!WritAcceptSellPakage(pDbData,pLasttxhash,pCurtxhash)){
			return false;
		}
	}else{
		if(!WritAcceptBuyPakage(pDbData,pLasttxhash,pCurtxhash)){
			return false;
		}
	}
    //!<�����ٲ����˻��Ķ�����
	if(!UpdateFreezeOverheight(&pDbData->tradeInfo.arbitationID.accounid[0],pDbData->tradeInfo.height,pLasttxhash,pCurtxhash)){
		return false;
	}

	//!<�ⶳ�������˻��ҵ���10����
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
   //!< �ӵ�
	LogPrint((char *)pContract,sizeof(TX_CONTRACT),HEX);
	////key
	unsigned char key[36];
	DB_DATA dbdata;
	unsigned long outheight = GetSptDbData(pContract->txhash,pContract->height,key,&dbdata);
	if((!outheight) || (pContract->height != dbdata.tradeInfo.height)){
		LogPrint("AcceptTrade input_height error",sizeof("AcceptTrade input_height error")+1,STRING);
		return false;
	}

	//!<�Ϸ���У��
	 if((dbdata.state != STATE_SEND) || (CheckTradeIsOverTime(outheight))){
		 LogPrint("AcceptTrade input0 error",sizeof("AcceptTrade input0 error")+1,STRING);
	   	 return  false;
	 }
	//!<У���Ƿ��ܽ� ,������ӵ��߼��������������,�Ҳ������ٲ���ID��ͬ,�Ҳ����ǿ������˻�ID
	char account[6];
	if(!GetCurTxAccount(account,sizeof(account)) ||
			(!memcmp(account,dbdata.tradeInfo.arbitationID.accounid,sizeof(account))) ||
			(!memcmp(DeveloperAccount,account,sizeof(account)))){
		LogPrint("AcceptTrade input1 error",sizeof("AcceptTrade input1 error")+1,STRING);
		return false;
	}
	if(SEND_TYPE_SELL == dbdata.tradeInfo.sendType)
	{ //!<��
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
	//!<��ʾ�ӵ�
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
 //!<�ⶳ���A�˻����׽��M,�ٲ÷�Xa,ת������B�˻�������;
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
	//!<���, ����B�˻����¶����ʱ�߶�, ���� M��Xa,
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

    //!<�����ٲ����˻��Ķ�����
	if(!UpdateFreezeOverheight(&pDbData->tradeInfo.arbitationID.accounid[0],pDbData->tradeInfo.height,pLasttxhash,pCurtxhash)){
		return false;
	}
	return true;
}

bool DeliveryTrade(const TX_CONTRACT* const pContract){
   //!< ����
	LogPrint((char *)pContract,sizeof(TX_CONTRACT),HEX);
	////key
	unsigned char key[36];
	DB_DATA dbdata;
	unsigned long outheight = GetSptDbData(pContract->txhash,pContract->height,key,&dbdata);
	if((!outheight) || (pContract->height != dbdata.tradeInfo.height)){
		LogPrint("DeliveryTrade input_height error",sizeof("DeliveryTrade input_height error")+1,STRING);
		return false;
	}

	//!<�Ϸ���У��
	 if((dbdata.state != STATE_ACCEPT) || (CheckTradeIsOverTime(outheight))){
		 LogPrint("DeliveryTrade input0 error",sizeof("DeliveryTrade input0 error")+1,STRING);
	   	 return  false;
	 }
	//!<У���Ƿ�Ϊ���� ����
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
	//!<��ʾ����
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
	//!<�˻����A�˻��ٲ÷�Xa;
	memcpy(id.ID,&pDbData->buyer.accounid[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.mMoney,&pDbData->arbiterMoneyX,sizeof(Int64));
	id.idlen = MAX_ACCOUNT_LEN;
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));

	appID.opeatortype = ADD_FREE_OP;
	writecode[0]=appID;

	//!<�ⶳ����B�˻��Ľ��׽��M���ٲ÷�Xb,Xa�˻������A�˻�;
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
  //!<�ⶳ�ٲ���C�ĳ�ʱδ�о����⳥����Y��
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
    //!<���ȷ���ջ�
	LogPrint((char *)pContract,sizeof(TX_CONTRACT),HEX);

	////key
	unsigned char key[36];
	DB_DATA dbdata;
	unsigned long outheight = GetSptDbData(pContract->txhash,pContract->height,key,&dbdata);
	if((!outheight) || (pContract->height != dbdata.tradeInfo.height)){
		LogPrint("BuyerConfirm input_height error",sizeof("BuyerConfirm input_height error")+1,STRING);
		return false;
	}

	//!<�Ϸ���У��
	 if((dbdata.state != STATE_DELIVERY) || (CheckTradeIsOverTime(outheight))){
    	 LogPrint("BuyerConfirm input0 error",sizeof("BuyerConfirm input0 error")+1,STRING);
	   	 return  false;
	 }
	//!<У���Ƿ�Ϊ���ȷ��
	char account[6];
	if(!GetCurTxAccount(account,sizeof(account)) || (memcmp(account,dbdata.buyer.accounid,sizeof(account)))){
		LogPrint("BuyerConfirm input1 error",sizeof("BuyerConfirm input1 error")+1,STRING);
		return false;
	}

	if(!WriteBuyerConfirm(&dbdata,pContract->txhash)){
		return false;
	}
	//!<ɾ���м��¼
	if(!DeleteData(key,36)){
		LogPrint("BuyerConfirm DeleteData error",sizeof("BuyerConfirm DeleteData error")+1,STRING);
		return false;
	}
	return true;

}

bool WriteArbitrationPackage(const DB_DATA* const pDbData,const  char*const pLasttxhash,const char*const pCurtxhash)
{   //!< �����ٲõ� �˻�����
	APP_ACC_OPERATE writecode[3];

	S_APP_ID id;
	APP_ACC_OPERATE appID;
	unsigned char num = 0;

	//!<������B�����˻������ٲ÷�Xa��Xb�����ٲ���C;�����ٲ���C�����˻��ĳ�ʱδ�о����⳥����Yת���ٲ÷����ߵĶ����˻���
	S_APP_ID tag;
	tag.idlen = 32;
	appID.outheight = GetTxConFirmHeight(pLasttxhash)  + pDbData->tradeInfo.height;
	memcpy(tag.ID,pLasttxhash,32);
	memcpy(&appID.FundTag,&tag,sizeof(tag));
	id.idlen = MAX_ACCOUNT_LEN;

	//!<���ȣ�ɾ��ԭ�߶ȵĶ�����
	memcpy(id.ID,&pDbData->seller.accounid[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	GetUserAppAccFoudWithTag(&appID.mMoney,&appID);
	appID.opeatortype = SUB_FREEZED_OP;
	writecode[0]=appID;

	//!<��Σ��ٸ��¸߶�
	appID.outheight = GetCurRunEnvHeight()  +  pDbData->overtimeheightT;//pDbData->tradeInfo.height;
	memcpy(tag.ID,pCurtxhash,32);
	memcpy(&appID.FundTag,&tag,sizeof(tag));
	Int64Add(&pDbData->tradeInfo.moneyM,&pDbData->arbiterMoneyX,&appID.mMoney);

    if(pDbData->arbiterType)
    {  //!<����B �����ٲ� ,����C���ĳ�ʱδ�о��ķ���Y
      Int64Add(&appID.mMoney,&pDbData->arbiterovertimeMoneyY,&appID.mMoney);
    }
	appID.opeatortype = ADD_FREEZED_OP;
	writecode[1]=appID;
	num = 2;

	if(pDbData->arbiterType == 0)
	{
		//!<��������B �����ٲã�����C���ĳ�ʱδ�о��ķ���Y
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
  //!<���ٲ���C�����˻��ĳ�ʱδ�о����⳥����Yת���ٲ÷����ߵĶ����˻�,���շ����߸����ٲ÷�X��
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
   //!<�����ٲ�
	LogPrint((char *)pContract,sizeof(TX_Arbitration),HEX);

	////key
	unsigned char key[36];
	DB_DATA dbdata;
	unsigned long outheight = GetSptDbData(pContract->txhash,pContract->height,key,&dbdata);
	if((!outheight) || (pContract->height != dbdata.tradeInfo.height)){
		LogPrint("Arbitration input_height error",sizeof("Arbitration input_height error")+1,STRING);
		return false;
	}
	//!<�Ϸ���У��
	 if((dbdata.state != STATE_DELIVERY) || (CheckTradeIsOverTime(outheight)) ||
			 (memcmp(pContract->arbitationID,dbdata.tradeInfo.arbitationID.accounid,6))){
		 LogPrint("Arbitration input0 error",sizeof("Arbitration input0 error")+1,STRING);
	   	 return  false;
	 }
	//!<У���Ƿ�����һ�����
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
	//!<��ʾ�����ٲ�
	dbdata.state = STATE_ARBITER;
    memcpy(dbdata.arbitHash,curtxhash,32);
	if(!DeleteData(key,36))
	{
		LogPrint("Arbitration DeleteData error",sizeof("Arbitration DeleteData error")+1,STRING);
		return false;
	}
	//!<��Σ��ٸ��¸߶�
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
	   //!< ִ�����ղþ��� �˻�����
	APP_ACC_OPERATE writecode[3];

	S_APP_ID id;
	APP_ACC_OPERATE appID;

	S_APP_ID tag;
	tag.idlen = 32;
	appID.outheight = GetTxConFirmHeight(pContract->arbitHash)  + pDbData->overtimeheightT;//pDbData->tradeInfo.height;
	memcpy(tag.ID,pContract->arbitHash,32);
	memcpy(&appID.FundTag,&tag,sizeof(tag));

	//!<�ⶳ����B�˻���Ľ��׽��M���������Aָ���Ľ���������Bָ���Ľ�
	memcpy(id.ID,&pDbData->seller.accounid[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.mMoney,&pDbData->tradeInfo.moneyM,sizeof(Int64));
	id.idlen = MAX_ACCOUNT_LEN;
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));

	appID.opeatortype = SUB_FREEZED_OP;
	writecode[0]=appID;

	if(type)
	{ //!<������ʤ�߷�
	    memcpy(&appID.mMoney,&pContract->winnerMoney,sizeof(Int64));
	}else{
		memcpy(&appID.mMoney,&pContract->loserMoney,sizeof(Int64));
	}
	appID.opeatortype = ADD_FREE_OP;
	writecode[1]=appID;

	memcpy(id.ID,&pDbData->buyer.accounid[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	if(type == 0)
	{ //!<�����ʤ�߷�
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
  //!<�������ٲ��߶����˻���ĳ�ʱδ�о����⳥����Y���ⶳ���ٲ���C
    if(pDbData->arbiterType)
    { //!< 1:���ҷ���
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
	//!<�����ٲ÷ѣ��ɰ��߷��е�
	memcpy(id.ID,&pDbData->seller.accounid[0],MAX_ACCOUNT_LEN);
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	memcpy(&appID.mMoney,&pDbData->arbiterMoneyX,sizeof(Int64));
    appID.opeatortype = SUB_FREEZED_OP;
	writecode[0]=appID;

	if(type == 0)
	{ //!<�����ʤ�߷�������Ҷ����˻��û��ٲ÷�Xa
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
   //!<֪ͨ���ղþ��������ִ��
	LogPrint((char *)pContract,sizeof(TX_FINALRESULT_CONTRACT),HEX);

	////key
	unsigned char key[36];
	DB_DATA dbdata;
	unsigned long outheight = GetSptDbData(pContract->arbitHash,pContract->overtimeheightT,key,&dbdata);
	if((!outheight) || (pContract->overtimeheightT != dbdata.overtimeheightT)){
		LogPrint("RunFinalResult input_height error",sizeof("RunFinalResult input_height error")+1,STRING);
		return false;
	}
	//!<�Ϸ���У��
	 if((dbdata.state != STATE_ARBITER) || (CheckTradeIsOverTime(outheight))){
		 LogPrint("RunFinalResult input0 error",sizeof("RunFinalResult input0 error")+1,STRING);
		 return  false;
	 }

	//!<У�� �þ���
	char account[6];
	if(!GetCurTxAccount(account,sizeof(account)) || (memcmp(account,dbdata.tradeInfo.arbitationID.accounid,sizeof(account)))){
		LogPrint("RunFinalResult input1 error",sizeof("RunFinalResult input1 error")+1,STRING);
		return false;
	}
     char type = 0; //!<��Ӯ����
	//!<У���о����
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
	//!<��Ӯ��ID ������ͬ
	if(0 == memcmp(pContract->loser.accounid,pContract->winner.accounid,6)){
		LogPrint("RunFinalResult input3 error",sizeof("RunFinalResult input3 error")+1,STRING);
		return false;
	}

	//!<У�� �о��Ľ�����
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

    //!<�˻�����
	if(!WriteFinalResultPakage(pContract,&dbdata,type)){
		return false;
	}
	//!<ɾ����¼
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
 	    case TX_REGISTER:  //ע���ٲ��˻�
		{
			LogPrint("TX_REGISTER",sizeof("TX_REGISTER"),STRING);
			if(!Register((TX_REGISTER_CONTRACT*)contact))
			{
				LogPrint("TX_REGISTER error",sizeof("TX_REGISTER error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}
 	    case TX_MODIFYREGISTER:  // �޸��ٲ���ע����Ϣ
		{
			LogPrint("TX_MODIFYREGISTER",sizeof("TX_MODIFYREGISTER"),STRING);
			if(!ModifyRegister((TX_REGISTER_CONTRACT*)contact))
			{
				LogPrint("TX_MODIFYREGISTER error",sizeof("TX_MODIFYREGISTER error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}

 		case TX_ARBIT_ON:  //�ٲÿ���
 		case TX_ARBIT_OFF: //�ٲ���ͣ
		{
			LogPrint("TX_ARBIT_ON",sizeof("TX_ARBIT_ON"),STRING);
			if(!ArbitOnOrOff((unsigned char *) contact))
			{
				LogPrint("TX_ARBIT_ON error",sizeof("TX_ARBIT_ON error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}

 		case  TX_UNREGISTER: //ע���ٲ��˻�
 		{
 			LogPrint("TX_UNREGISTER",sizeof("TX_UNREGISTER"),STRING);
 			if(!UnRegister())
 			{
 				LogPrint("TX_UNREGISTER error",sizeof("TX_UNREGISTER error"),STRING);
 				__VmExit(RUN_SCRIPT_DATA_ERR);
 			}
 			break;
 		}
		case TX_SEND: //�ҵ�
		{
			LogPrint("TX_SEND",sizeof("TX_SEND"),STRING);
			if(!SendStartTrade((TX_SNED_CONTRACT*) contact))
			{
				LogPrint("TX_SEND error",sizeof("TX_SEND error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}
 		case  TX_CANCEL: //�ҵ�ȡ��
 		{
 			LogPrint("TX_CANCEL",sizeof("TX_CANCEL"),STRING);
 			if(!SendCancelTrade((TX_CONTRACT*) contact))
 			{
 				LogPrint("TX_CANCEL error",sizeof("TX_CANCEL error"),STRING);
 				__VmExit(RUN_SCRIPT_DATA_ERR);
 			}
 			break;
 		}
		case TX_ACCEPT: //�ӵ�
		{
			LogPrint("TX_ACCEPT",sizeof("TX_ACCEPT"),STRING);
			if(!AcceptTrade((TX_CONTRACT*) contact))
			{
				LogPrint("TX_ACCEPT error",sizeof("TX_ACCEPT error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}

		case TX_DELIVERY:  //����
		{
			LogPrint("TX_DELIVERY",sizeof("TX_DELIVERY"),STRING);
			if(!DeliveryTrade((TX_CONTRACT*) contact))
			{
				LogPrint("TX_DELIVERY error",sizeof("TX_DELIVERY error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}

		case  TX_BUYERCONFIRM: //���ȷ�Ͻ���
		{
			LogPrint("TX_BUYERCONFIRM",sizeof("TX_BUYERCONFIRM"),STRING);
			if(!BuyerConfirm((TX_CONTRACT*) contact))
			{
				LogPrint("TX_BUYERCONFIRM error",sizeof("TX_BUYERCONFIRM error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}

		case  TX_ARBITRATION://�����ٲ�
		{
			LogPrint("TX_ARBITRATION",sizeof("TX_ARBITRATION"),STRING);
			if(!Arbitration((TX_Arbitration*) contact))
			{
				LogPrint("TX_ARBITRATION error",sizeof("TX_ARBITRATION error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			break;
		}
		case  TX_FINALRESULT://֪ͨ�þ��������ִ��
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
 			//!<�������˻�����ʱ�����붳�������ٲ��ߵ�ע�ᵣ��Ѻ��
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

