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

typedef unsigned char uchar;
typedef unsigned int u16;
typedef unsigned long u32;

#define BET_MIN    ("\xA0\x86\x01\x00\x00\x00\x00\x00")//100000
#define REMAINHIGHT (10)
#define ErrorCheck(a) {if(!(a)) {PrintfFileAndLine(__LINE__, __FILE__);}}

/**
 * p2pBet Operate Type
 */
enum OPERATETYPE {
	SEND = 1,  //!< SEND
	ACCEPT,//!< ACCEPT
	OPEN   //!< OPEN
};

/**
 * send bet data struct
 */
typedef struct {
	OPERATETYPE type;
	Int64 money;
	u32 hight;
	uchar dhash[32];//32 + one byte checksum
}SEND_DATA;

/**
 * accept bet data struct
 */
typedef struct {
	OPERATETYPE type;
	Int64 money;
	uchar targetkey[32];//发起对赌的哈希，也是对赌数据的关键字
//	uchar dhash[32];//32 + one byte checksum
	uchar data[5];
}ACCEPT_DATA;

/**
 * open bet data struct
 */
typedef struct {
	OPERATETYPE type;
	uchar targetkey[32];//发起对赌的哈希，也是对赌数据的关键字
	uchar data[5];
}OPEN_DATA;

enum BETSTATUS {
	BETSEND,  //!< SEND
	BETACCEPT,//!< ACCEPT
	BETOPEN   //!< OPEN
};

/**
 * the data struct saved in database
 */
//关键字暂时定为accid + current hight
typedef struct {
	BETSTATUS status;
	uchar sendid[6];//send bet account id
	uchar acceptid[6];//accept bet account id
	Int64 money;//bet amount
	u32 hight;//上次冻结高度
	u32 delyhight;
	uchar shash[32];//send data hash
//	uchar ahash[32];//accept data hash
	uchar sdata[5];//send data
	uchar adata[5];//accept data
}SAVE_DATA;

typedef struct {
	uchar txhash[32];//current tx hash
	uchar accid[6];//current account id
}BET_CTX;

__xdata __no_init static uchar gContractData[256];
#pragma data_alignment = 1
__xdata __no_init static SAVE_DATA gSaveData;
__xdata __no_init u32 curhigh;



/**
 * @brief get contract data
 * @param pdata:out data point
 * @return
 */
static bool GetContractData(const BET_CTX *pctxdata)
{
//	if(pctxdata == NULL)
//	{
////		ErrorCheck(0);
//		return false;
//	}

	if(GetCurTxContact(gContractData, sizeof(gContractData)) == 0)
	{
//		ErrorCheck(0);
		return false;
	}

	return true;
}

/**
 * @brief check the script min amount
 * @param pdata:the input amount
 * @return
 */
static bool CheckMinimumAmount(const Int64 *pdata)
{
#pragma data_alignment = 1
	Int64 minamount;

//	if(pdata == NULL)
//	{
//		ErrorCheck(0);
//		return false;
//	}

	Int64Inital(&minamount, BET_MIN, 8);

	if(Int64Compare(pdata, &minamount) != COMP_LARGER)
	{
//		ErrorCheck(0);
		return false;
	}

	return true;
}

/**
 * @brief check send bet hight
 * @param phight:input hight
 * @return
 */
//static bool CheckSendBetHight(const u32 *phight)
//{
////	if(phight == NULL)
////	{
////		ErrorCheck(0);
////		return false;
////	}
//
//	if(*phight <= GetCurRunEnvHeight())
//	{
//		ErrorCheck(0);
//		return false;
//	}
//
//	return true;
//}

/**
 * @brief check data hash
 * @param pdata:the input data
 * @return
 */
//static bool CheckDataHash(const void *pdata)
//{
//	uchar checksum = 0;
//	if(pdata == NULL)
//	{
//		return false;
//	}
//
//	for(uchar ii = 0; ii < 32; ii++)
//	{
//		checksum += pdata[ii];
//	}
//
//	if(checksum != pdata[32])
//	{
//		return false;
//	}
//
//	return true;
//}

/**
 * @brief send bet data check
 * @param pdata:the send bet data
 * @return
 */
static bool SendDataCheck(const BET_CTX *pbetctx)
{
#pragma data_alignment = 1
	SEND_DATA *psdata = (SEND_DATA *)gContractData;
	Int64 balance;

//	if(pbetctx == NULL)
//	{
//		ErrorCheck(0);
//		return false;
//	}

	if(!CheckMinimumAmount(&psdata->money))
	{
//		ErrorCheck(0);
		return false;
	}

	if(!QueryAccountBalance(pbetctx->accid, &balance))
	{
//		ErrorCheck(0);
		return false;
	}

	if(Int64Compare(&balance, &psdata->money) != COMP_LARGER)
	{
//		ErrorCheck(0);
		return false;
	}

	if(psdata->hight < 10)
	{
//		ErrorCheck(0);
		return false;
	}

//	if(!CheckDataHash(psdata->dhash))
//	{
//		return false;
//	}

	return true;
}

static bool SendOperateAccount(const BET_CTX *pbetctx)
{
#pragma data_alignment = 1
	SEND_DATA *psdata = (SEND_DATA *)gContractData;
#pragma data_alignment = 1
	VM_OPERATE operate[2];

//	if(pbetctx == NULL)
//	{
//		ErrorCheck(0);
//		return false;
//	}

	memcpy(operate[0].accountid, pbetctx->accid, sizeof(pbetctx->accid));
	memcpy(&operate[0].money, &psdata->money, sizeof(psdata->money));
	operate[0].opeatortype = MINUS_FREE;
	operate[0].outheight =  curhigh+psdata->hight;

	memcpy(operate[1].accountid, pbetctx->accid, sizeof(pbetctx->accid));
	memcpy(&operate[1].money, &psdata->money, sizeof(psdata->money));
	operate[1].opeatortype = ADD_FREEZD;
	operate[1].outheight = curhigh+psdata->hight;

    return WriteOutput(operate, 2);
}

static bool AcceptOperateAccount(const BET_CTX *pbetctx)
{
#pragma data_alignment = 1
	ACCEPT_DATA *psdata = (ACCEPT_DATA *)gContractData;
#pragma data_alignment = 1
	VM_OPERATE operate[4];

//	if(pbetctx == NULL)
//	{
//		ErrorCheck(0);
//		return false;
//	}

	//先把自己的钱冻结起来
	memcpy(operate[0].accountid, pbetctx->accid, sizeof(pbetctx->accid));
	memcpy(&operate[0].money, &psdata->money, sizeof(psdata->money));
	operate[0].opeatortype = MINUS_FREE;
	operate[0].outheight = gSaveData.hight;

	memcpy(operate[1].accountid, pbetctx->accid, sizeof(pbetctx->accid));
	memcpy(&operate[1].money, &psdata->money, sizeof(psdata->money));
	operate[1].opeatortype = ADD_FREEZD;
	operate[1].outheight = curhigh+gSaveData.delyhight;

	memcpy(operate[2].accountid, gSaveData.sendid, sizeof(gSaveData.sendid));
	memcpy(&operate[2].money, &gSaveData.money, sizeof(psdata->money));
	operate[2].opeatortype = MINUS_FREEZD;
	operate[2].outheight = gSaveData.hight;

	memcpy(operate[3].accountid, pbetctx->accid, sizeof(pbetctx->accid));
	memcpy(&operate[3].money, &gSaveData.money, sizeof(psdata->money));
	operate[3].opeatortype = ADD_FREEZD;
	operate[3].outheight = curhigh+gSaveData.delyhight;;

	return WriteOutput(operate, 4);
}

static bool OpenOperateAccount(const BET_CTX *pbetctx)
{
#pragma data_alignment = 1
	VM_OPERATE operate[2];
#pragma data_alignment = 1
	OPEN_DATA *psdata = (OPEN_DATA *)gContractData;

	uchar rslt = 0;
	for(uchar ii = 0; ii < 5; ii++)
	{
//		LogPrint(&psdata->data[ii], sizeof(uchar), HEX);
		rslt += psdata->data[ii];
	}

	for(uchar kk = 0; kk < 5; kk++)
	{
//		LogPrint(&gSaveData.adata[kk], sizeof(uchar), HEX);
		rslt += gSaveData.adata[kk];
	}

//	LogPrint(&rslt, sizeof(uchar), HEX);
	memcpy(operate[0].accountid, gSaveData.sendid, sizeof(gSaveData.sendid));
	Int64Add(&gSaveData.money, &gSaveData.money, &operate[0].money);
	operate[0].outheight = gSaveData.hight;

	memcpy(operate[1].accountid, gSaveData.acceptid, sizeof(gSaveData.acceptid));
//		Int64Add(&gSaveData.money, &gSaveData.money, &operate[1].money);
	memcpy(&operate[1].money, &operate[0].money, sizeof(operate[0].money));
	operate[1].outheight = gSaveData.hight;
	if(rslt%2)//juge result,send win
	{
//		LogPrint("A WIN", sizeof("A WIN"), STRING);
		operate[0].opeatortype = ADD_FREE;
		operate[1].opeatortype = MINUS_FREEZD;
	}
	else//accept win
	{
//		LogPrint("B WIN", sizeof("B WIN"), STRING);
		memcpy(operate[0].accountid, gSaveData.acceptid, sizeof(gSaveData.acceptid));
		operate[0].opeatortype = MINUS_FREEZD;
		operate[1].opeatortype = ADD_FREE;
	}

	return WriteOutput(operate, 2);
}

/**
 * @brief record send bet status
 * @param pdata:the contract data
 * @return
 */
static bool RecordSendBetStatus(const BET_CTX *pbetctx)
{
#pragma data_alignment = 1
	SEND_DATA *psdata = (SEND_DATA *)gContractData;
#pragma data_alignment = 1
	SAVE_DATA savedata;

//	if(pbetctx == NULL)
//	{
//		ErrorCheck(0);
//		return false;
//	}

	memset(&savedata, 0, sizeof(savedata));
	savedata.status = BETSEND;
	memcpy(&savedata.money, &psdata->money, sizeof(psdata->money));
	memcpy(&savedata.shash, &psdata->dhash, sizeof(psdata->dhash));
	savedata.hight = curhigh+psdata->hight;
	savedata.delyhight = psdata->hight;
	memcpy(savedata.sendid, pbetctx->accid, sizeof(pbetctx->accid));


//	LogPrint(&savedata,sizeof(savedata),HEX);

//	LogPrint(psdata,sizeof(SEND_DATA),HEX);

	if(!WriteDataDB(pbetctx->txhash, sizeof(pbetctx->txhash), &savedata, sizeof(savedata), savedata.hight))
	{
//		ErrorCheck(0);
		return false;
	}

	return true;
}

/**
 * @brief send a p2p bet
 * @param pdata:contract data
 * @return
 */
static bool SendP2PBet(const BET_CTX *pbetctx)
{
//	if(pbetctx == NULL)
//	{
//		ErrorCheck(0);
//		return false;
//	}

	if(!SendDataCheck(pbetctx))
	{
//		ErrorCheck(0);
		return false;
	}

	if(!SendOperateAccount(pbetctx))
	{
//		ErrorCheck(0);
		return false;
	}

	if(!RecordSendBetStatus(pbetctx))
	{
//		ErrorCheck(0);
		return false;
	}

	return true;
}

/**
 * @brief check if the database have someone send a bet
 * @return
 */
static bool IsHaveSendBet(const void * phash)
{
//	if(phash == NULL)
//	{
//		ErrorCheck(0);
//		return false;
//	}
	if(!ReadData(phash, 32, &gSaveData, sizeof(SAVE_DATA)))
	{
//		ErrorCheck(0);
		return false;
	}
	return (gSaveData.status == BETSEND)?(true):(false);
}

static bool IsEqualSendAmount(const Int64 *pdata)
{
//	if(pdata == NULL)
//	{
//		ErrorCheck(0);
//		return false;
//	}

	if(Int64Compare(pdata, &gSaveData.money) != COMP_EQU)
	{
//		ErrorCheck(0);
		return false;
	}

	return true;
}

/**
 * @brief accept bet data check
 * @param pdata:accept bet contract data
 * @return
 */
static bool AcceptDataCheck(const BET_CTX *pbetctx)
{
#pragma data_alignment = 1
	ACCEPT_DATA *psdata = (ACCEPT_DATA *)gContractData;
#pragma data_alignment = 1
	Int64 balance;

//	if(pbetctx == NULL)
//	{
////		ErrorCheck(0);
//		return false;
//	}

	if(!IsHaveSendBet(psdata->targetkey))
	{
//		ErrorCheck(0);
		return false;
	}

	if(!IsEqualSendAmount(&psdata->money))
	{
//		ErrorCheck(0);
		return false;
	}

	if(!QueryAccountBalance(pbetctx->accid, &balance))
	{
//		ErrorCheck(0);
		return false;
	}

	if(Int64Compare(&balance, &psdata->money) != COMP_LARGER)
	{
//		ErrorCheck(0);
		return false;
	}

//	if(gSaveData.hight - GetCurRunEnvHeight() < REMAINHIGHT)
//	{
//		ErrorCheck(0);
//		return false;
//	}

//	if(!CheckDataHash(psdata->dhash))
//	{
//		return false;
//	}

	return true;
}

/**
 * @brief record accept bet status
 * @param pdata:the contract data
 * @return
 */
static bool RecordAcceptBetStatus(const BET_CTX *pbetctx)
{
#pragma data_alignment = 1
	ACCEPT_DATA *psdata = (ACCEPT_DATA *)gContractData;
//	if(pbetctx == NULL)
//	{
//		ErrorCheck(0);
//		return false;
//	}

	gSaveData.status = BETACCEPT;
	memcpy(gSaveData.adata, psdata->data, sizeof(psdata->data));
	memcpy(gSaveData.acceptid, pbetctx->accid, sizeof(pbetctx->accid));
	gSaveData.hight = curhigh+gSaveData.delyhight;
//	if(!WriteDataDB(pbetctx->txhash, sizeof(pbetctx->txhash), &savedata, sizeof(SAVE_DATA), savedata.hight))
	if(!WriteDataDB(psdata->targetkey, sizeof(psdata->targetkey), &gSaveData, sizeof(SAVE_DATA),gSaveData.hight))
	{
//		ErrorCheck(0);
		return false;
	}

	return true;
}

/**
 * @brief accept a bet
 * @param pdata:the contract data
 * @return
 */
static bool AcceptP2PBet(const BET_CTX *pbetctx)
{
//	if(pbetctx == NULL)
//	{
//		ErrorCheck(0);
//		return false;
//	}

	if(!AcceptDataCheck(pbetctx))
	{
//		ErrorCheck(0);
		return false;
	}

	if(!AcceptOperateAccount(pbetctx))
	{
//		ErrorCheck(0);
		return false;
	}

	if(!RecordAcceptBetStatus(pbetctx))
	{
//		ErrorCheck(0);
		return false;
	}

	return true;
}

/**
 * @brief check if the database have a full bet
 * @return
 */
static bool IsHaveP2PBet(const void * phash)
{
//	if(phash == NULL)
//	{
//		ErrorCheck(0);
//		return false;
//	}

	if(!ReadData(phash, 32, &gSaveData, sizeof(SAVE_DATA)))
	{
//		ErrorCheck(0);
		return false;
	}

//	LogPrint(&gSaveData.status, sizeof(gSaveData.status), HEX);

	if(gSaveData.status != BETACCEPT)
	{
//		ErrorCheck(0);
		return false;
	}

	return true;
}

/**
 * @brief check if the open data is meet with hashdata
 * @param pdata:input data
 * @param len:input len
 * @return
 */
static bool IsDataVaild(const BET_CTX *pbetctx)
{
	uchar checkhash[32] = {0};
#pragma data_alignment = 1
	OPEN_DATA *podata = (OPEN_DATA *)gContractData;

//	if(pbetctx == NULL)
//	{
//		ErrorCheck(0);
//		return false;
//	}

	SHA256(podata->data, sizeof(podata->data), checkhash);

	if(memcmp(checkhash, gSaveData.shash, sizeof(checkhash)) != 0)
	{
//		ErrorCheck(0);
		return false;
	}

	return true;
}

/**
 * @brief open bet data check
 * @param pdata:the open bet contract data
 * @return
 */
static bool OpenDataCheck(const BET_CTX *pbetctx)
{
#pragma data_alignment = 1
	OPEN_DATA *podata = (OPEN_DATA *)gContractData;

//	if(pbetctx == NULL)
//	{
//		ErrorCheck(0);
//		return false;
//	}

	if(!IsHaveP2PBet(podata->targetkey))
	{
//		ErrorCheck(0);
		return false;
	}

	if(!IsDataVaild(pbetctx))
	{
//		ErrorCheck(0);
		return false;
	}

	return true;
}

/**
 * @brief record open bet status
 * @param pdata:the contract data
 * @return
 */
static bool RecordOpenBetStatus(const BET_CTX *pdata)
{
#pragma data_alignment = 1
	OPEN_DATA *podata = (OPEN_DATA *)gContractData;
//	if(pdata == NULL)
//	{
//		ErrorCheck(0);
//		return false;
//	}

	gSaveData.status = BETOPEN;

	memcpy(gSaveData.sdata, podata->data, sizeof(podata->data));
	gSaveData.hight += gSaveData.delyhight;
	if(!WriteDataDB(podata->targetkey, sizeof(podata->targetkey), &gSaveData, sizeof(SAVE_DATA),gSaveData.hight))
	{
		ErrorCheck(0);
		return false;
	}

	return true;
}

/**
 * @brief open a bet
 * @param pdata:the open bet contract data
 * @return
 */
static bool OpenP2PBet(const BET_CTX *pbetctx)
{
//	if(pbetctx == NULL)
//	{
//		ErrorCheck(0);
//		return false;
//	}

	if(!OpenDataCheck(pbetctx))
	{
//		ErrorCheck(0);
		return false;
	}

	if(!OpenOperateAccount(pbetctx))
	{
//		ErrorCheck(0);
		return false;
	}

	if(!RecordOpenBetStatus(pbetctx))
	{
//		ErrorCheck(0);
		return true;
	}

	return true;
}

/**
 * @brief according to the contract data,run the script
 * @param pdata:contract data handle
 * @return
 */
static bool RunContractData(const BET_CTX *pbetctx)
{
#pragma data_alignment = 1
	OPEN_DATA *phandle = (OPEN_DATA *)gContractData;
	bool ret = false;

//	if(pbetctx == NULL)
//	{
//		ErrorCheck(0);
//		return false;
//	}

	switch(phandle->type)
	{
	case SEND:
		ret = SendP2PBet(pbetctx);
		break;

	case ACCEPT:
		ret = AcceptP2PBet(pbetctx);
		break;

	case OPEN:
		ret = OpenP2PBet(pbetctx);
		break;

	default:
//		ErrorCheck(0);
		ret = false;
		break;
	}

	return ret;
}

static bool InitCtxData(BET_CTX const *pctxdata)
{
//	if(pctxdata == NULL)
//	{
//		ErrorCheck(0);
//		return false;
//	}

	if(!GetCurTxHash((void *)pctxdata->txhash))
	{
//		ErrorCheck(0);
		return false;
	}

	if(GetCurTxAccount((void *)pctxdata->accid, sizeof(pctxdata->accid)) == 0)
	{
//		ErrorCheck(0);
		return false;
	}

	return true;
}

int main()
{
	__xdata __no_init static BET_CTX BetCtx;
//	RecordSendBetStatus(NULL);
	if(!InitCtxData(&BetCtx))
	{
//		ErrorCheck(0);
		__exit(RUN_SCRIPT_DATA_ERR);
	}

	if(!GetContractData(&BetCtx))
	{
//		ErrorCheck(0);
		__exit(RUN_SCRIPT_DATA_ERR);
	}

	curhigh = GetCurRunEnvHeight() ;

	if(!RunContractData(&BetCtx))
	{
//		ErrorCheck(0);
		__exit(RUN_SCRIPT_DATA_ERR);
	}

	__exit(RUN_SCRIPT_OK);
	return 1;
}

