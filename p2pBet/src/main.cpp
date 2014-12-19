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
	uchar dhash[32];//32 + one byte checksum
}ACCEPT_DATA;

/**
 * open bet data struct
 */
typedef struct {
	OPERATETYPE type;
	uchar targetkey[32];//发起对赌的哈希，也是对赌数据的关键字
	uchar dhash[5];
}OPEN_DATA;

enum BETSTATUS {
	BETSEND,  //!< SEND
	BETACCEPT,//!< ACCEPT
	BETOPEN1,//!< ONE OPEN
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
	u32 hight;//解冻高度
	uchar shash[32];//send data hash
	uchar ahash[32];//accept data hash
	uchar sdata[5];//send data
	uchar adata[5];//accept data
}SAVE_DATA;

typedef struct {
	uchar txhash[32];//current tx hash
	uchar accid[6];//current account id
}BET_CTX;

__xdata __no_init static uchar gContractData[256];
__xdata __no_init static SAVE_DATA gSaveData;



/**
 * @brief get contract data
 * @param pdata:out data point
 * @return
 */
static bool GetContractData(const BET_CTX *pctxdata)
{
	if(pctxdata == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(GetCurTxContact(gContractData, sizeof(gContractData)) == 0)
	{
		ErrorCheck(0);
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
	Int64 minamount;

	if(pdata == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	Int64Inital(&minamount,"\x00\x00\x00\x00\x00\x01\x00\x00",8);

	if(Int64Compare(pdata, &minamount) != COMP_LARGER)
	{
		ErrorCheck(0);
		return false;
	}

	return true;
}

/**
 * @brief check send bet hight
 * @param phight:input hight
 * @return
 */
static bool CheckSendBetHight(const u32 *phight)
{
	if(phight == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(*phight <= GetCurRunEnvHeight())
	{
		ErrorCheck(0);
		return false;
	}

	return true;
}

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
	SEND_DATA *psdata = (SEND_DATA *)gContractData;
	Int64 balance;

	if(pbetctx == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(!CheckMinimumAmount(&psdata->money))
	{
		ErrorCheck(0);
		return false;
	}

	if(!QueryAccountBalance(pbetctx->accid, ACOUNT_ID, &balance))
	{
		ErrorCheck(0);
		return false;
	}

	if(Int64Compare(&balance, &psdata->money) != COMP_LARGER)
	{
		ErrorCheck(0);
		return false;
	}

	if(!CheckSendBetHight(&psdata->hight))
	{
		ErrorCheck(0);
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
	SEND_DATA *psdata = (SEND_DATA *)gContractData;
	VM_OPERATE operate[2];

	if(pbetctx == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	memcpy(operate[0].accountid, pbetctx->accid, sizeof(pbetctx->accid));
	memcpy(&operate[0].money, &psdata->money, sizeof(psdata->money));
	operate[0].opeatortype = MINUS_FREE;
	operate[0].outheight = psdata->hight;

	memcpy(operate[1].accountid, pbetctx->accid, sizeof(pbetctx->accid));
	memcpy(&operate[1].money, &psdata->money, sizeof(psdata->money));
	operate[1].opeatortype = ADD_FREEZD;
	operate[1].outheight = psdata->hight;

    return WriteOutput(operate, 2);
}

static bool AcceptOperateAccount(const BET_CTX *pbetctx)
{
	ACCEPT_DATA *psdata = (ACCEPT_DATA *)gContractData;
	VM_OPERATE operate[2];

	if(pbetctx == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	memcpy(operate[0].accountid, pbetctx->accid, sizeof(pbetctx->accid));
	memcpy(&operate[0].money, &psdata->money, sizeof(psdata->money));
	operate[0].opeatortype = MINUS_FREE;
	operate[0].outheight = gSaveData.hight;

	memcpy(operate[1].accountid, pbetctx->accid, sizeof(pbetctx->accid));
	memcpy(&operate[1].money, &psdata->money, sizeof(psdata->money));
	operate[1].opeatortype = ADD_FREEZD;
	operate[1].outheight = gSaveData.hight;

	return WriteOutput(operate, 2);
}

static bool OpenOperateAccount(const BET_CTX *pbetctx)
{
	VM_OPERATE operate[2];
	OPEN_DATA *psdata = (OPEN_DATA *)gContractData;

	if(gSaveData.status == BETACCEPT)//fisrt open
	{
		memcpy(operate[0].accountid, gSaveData.sendid, sizeof(gSaveData.sendid));
		memcpy(&operate[0].money, &gSaveData.money, sizeof(gSaveData.money));
		operate[0].outheight = gSaveData.hight;

		memcpy(&operate[1].accountid, &gSaveData.acceptid, sizeof(gSaveData.acceptid));
		memcpy(&operate[1].money, &gSaveData.money, sizeof(gSaveData.money));
		operate[1].outheight = gSaveData.hight;

		if(memcmp(pbetctx->accid, gSaveData.sendid, sizeof(pbetctx->accid)) == 0)//send first
		{
			operate[0].opeatortype = ADD_FREEZD;
			operate[1].opeatortype = MINUS_FREEZD;
		}
		else if(memcmp(pbetctx->accid, gSaveData.acceptid, sizeof(pbetctx->accid)) == 0)//accept first
		{
			operate[0].opeatortype = MINUS_FREEZD;
			operate[1].opeatortype = ADD_FREEZD;
		}
		else
		{
			ErrorCheck(0);
			return false;
		}
	}
	else if(gSaveData.status == BETOPEN1)//second open
	{
		uchar rslt = 0;
		for(uchar ii = 0; ii < 5; ii++)
		{
			LogPrint(&psdata->dhash[ii], sizeof(uchar), HEX);
			rslt += psdata->dhash[ii];
		}

		if(memcmp(pbetctx->accid, gSaveData.acceptid, sizeof(pbetctx->accid)) == 0)
		{
			for(uchar kk = 0; kk < 5; kk++)
			{
				LogPrint(&gSaveData.sdata[kk], sizeof(uchar), HEX);
				rslt += gSaveData.sdata[kk];
			}
		}
		else if(memcmp(pbetctx->accid, gSaveData.sendid, sizeof(pbetctx->accid)) == 0)
		{
			for(uchar kk = 0; kk < 5; kk++)
			{
				LogPrint(&gSaveData.adata[kk], sizeof(uchar), HEX);
				rslt += gSaveData.adata[kk];
			}
		}
		else
		{
			ErrorCheck(0);
			return false;
		}
		LogPrint(&rslt, sizeof(uchar), HEX);
		memcpy(operate[0].accountid, gSaveData.sendid, sizeof(gSaveData.sendid));
		Int64Add(&gSaveData.money, &gSaveData.money, &operate[0].money);
		operate[0].outheight = gSaveData.hight;

		memcpy(operate[1].accountid, gSaveData.acceptid, sizeof(gSaveData.acceptid));
//		Int64Add(&gSaveData.money, &gSaveData.money, &operate[1].money);
		memcpy(&operate[1].money, &operate[0].money, sizeof(operate[0].money));
		operate[1].outheight = gSaveData.hight;
		if(rslt%2)//juge result,send win
		{
			LogPrint("A WIN", sizeof("A WIN"), STRING);
			if(memcmp(pbetctx->accid, gSaveData.sendid, sizeof(pbetctx->accid)) == 0)//accept first
			{
				operate[0].opeatortype = ADD_FREE;
				operate[1].opeatortype = MINUS_FREEZD;
			}
			else if(memcmp(pbetctx->accid, gSaveData.acceptid, sizeof(pbetctx->accid)) == 0)//send first
			{
				operate[0].opeatortype = ADD_FREE;
				operate[1].opeatortype = MINUS_FREEZD;
				memcpy(operate[1].accountid, gSaveData.sendid, sizeof(gSaveData.sendid));
			}
			else
			{
				ErrorCheck(0);
				return false;
			}
		}
		else//accept win
		{
			LogPrint("B WIN", sizeof("B WIN"), STRING);
			if(memcmp(pbetctx->accid, gSaveData.sendid, sizeof(pbetctx->accid)) == 0)//accept first
			{
				operate[0].opeatortype = MINUS_FREEZD;
				operate[1].opeatortype = ADD_FREE;
				memcpy(operate[0].accountid, gSaveData.acceptid, sizeof(gSaveData.acceptid));
			}
			else if(memcmp(pbetctx->accid, gSaveData.acceptid, sizeof(pbetctx->accid)) == 0)//send first
			{
				operate[0].opeatortype = MINUS_FREEZD;
				operate[1].opeatortype = ADD_FREE;
			}
			else
			{
				ErrorCheck(0);
				return false;
			}
		}

	}
	else//error status
	{
		ErrorCheck(0);
		return false;
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
	SEND_DATA *psdata = (SEND_DATA *)gContractData;
	SAVE_DATA savedata;

	if(pbetctx == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	memset(&savedata, 0, sizeof(SAVE_DATA));
	savedata.status = BETSEND;
	memcpy(&savedata.money, &psdata->money, sizeof(psdata->money));
	memcpy(&savedata.shash, &psdata->dhash, sizeof(psdata->dhash));
	savedata.hight = psdata->hight;
	memcpy(savedata.sendid, pbetctx->accid, sizeof(pbetctx->accid));

	if(!WriteDataDB(pbetctx->txhash, sizeof(pbetctx->txhash), &savedata, sizeof(SAVE_DATA), 12345))
	{
		ErrorCheck(0);
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
	if(pbetctx == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(!SendDataCheck(pbetctx))
	{
		ErrorCheck(0);
		return false;
	}

	if(!SendOperateAccount(pbetctx))
	{
		ErrorCheck(0);
		return false;
	}

	if(!RecordSendBetStatus(pbetctx))
	{
		ErrorCheck(0);
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
	if(phash == NULL)
	{
		ErrorCheck(0);
		return false;
	}
	if(!ReadDataValueDB(phash, 32, &gSaveData, sizeof(SAVE_DATA)))
	{
		ErrorCheck(0);
		return false;
	}
	return (gSaveData.status == BETSEND)?(true):(false);
}

static bool IsEqualSendAmount(const Int64 *pdata)
{
	if(pdata == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(Int64Compare(pdata, &gSaveData.money) != COMP_EQU)
	{
		ErrorCheck(0);
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
	ACCEPT_DATA *psdata = (ACCEPT_DATA *)gContractData;
	Int64 balance;

	if(pbetctx == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(!IsHaveSendBet(psdata->targetkey))
	{
		ErrorCheck(0);
		return false;
	}

	if(!IsEqualSendAmount(&psdata->money))
	{
		ErrorCheck(0);
		return false;
	}

	if(!QueryAccountBalance(pbetctx->accid, ACOUNT_ID, &balance))
	{
		ErrorCheck(0);
		return false;
	}

	if(Int64Compare(&balance, &psdata->money) != COMP_LARGER)
	{
		ErrorCheck(0);
		return false;
	}

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
	ACCEPT_DATA *psdata = (ACCEPT_DATA *)gContractData;
	if(pbetctx == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	gSaveData.status = BETACCEPT;
	memcpy(gSaveData.ahash, psdata->dhash, sizeof(psdata->dhash));
	memcpy(gSaveData.acceptid, pbetctx->accid, sizeof(pbetctx->accid));

	if(!ModifyDataDBVavle(psdata->targetkey, sizeof(psdata->targetkey), &gSaveData, sizeof(SAVE_DATA)))
	{
		ErrorCheck(0);
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
	if(pbetctx == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(!AcceptDataCheck(pbetctx))
	{
		ErrorCheck(0);
		return false;
	}

	if(!AcceptOperateAccount(pbetctx))
	{
		ErrorCheck(0);
		return false;
	}

	if(!RecordAcceptBetStatus(pbetctx))
	{
		ErrorCheck(0);
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
	if(phash == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(!ReadDataValueDB(phash, 32, &gSaveData, sizeof(SAVE_DATA)))
	{
		ErrorCheck(0);
		return false;
	}

//	LogPrint(&gSaveData.status, sizeof(gSaveData.status), HEX);

	if(gSaveData.status != BETACCEPT && gSaveData.status != BETOPEN1)
	{
		ErrorCheck(0);
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
	OPEN_DATA *podata = (OPEN_DATA *)gContractData;

	if(pbetctx == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	SHA256(podata->dhash, sizeof(podata->dhash), checkhash);

//	for(uchar ii = 0; ii < 32; ii++)
//	{
//		checkhash[32] += checkhash[ii];
//	}

	if(memcmp(pbetctx->accid, gSaveData.acceptid, sizeof(pbetctx->accid)) == 0)
	{
		if(memcmp(checkhash, gSaveData.ahash, sizeof(checkhash)) != 0)
		{
			ErrorCheck(0);
			return false;
		}
	}
	else if(memcmp(pbetctx->accid, gSaveData.sendid, sizeof(pbetctx->accid)) == 0)
	{
		if(memcmp(checkhash, gSaveData.shash, sizeof(checkhash)) != 0)
		{
			ErrorCheck(0);
			return false;
		}
	}
	else
	{
		ErrorCheck(0);
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
	OPEN_DATA *podata = (OPEN_DATA *)gContractData;

	if(pbetctx == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(!IsHaveP2PBet(podata->targetkey))
	{
		ErrorCheck(0);
		return false;
	}

	if(!IsDataVaild(pbetctx))
	{
		ErrorCheck(0);
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
	OPEN_DATA *podata = (OPEN_DATA *)gContractData;
	if(pdata == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	gSaveData.status = (gSaveData.status == BETACCEPT)?(BETOPEN1):(BETOPEN);

	if(memcmp(pdata->accid, gSaveData.sendid, sizeof(pdata->accid)) == 0)
	{
		memcpy(gSaveData.sdata, podata->dhash, sizeof(podata->dhash));
	}
	else if(memcmp(pdata->accid, gSaveData.acceptid, sizeof(pdata->accid)) == 0)
	{
		memcpy(gSaveData.adata, podata->dhash, sizeof(podata->dhash));
	}
	else
	{
		ErrorCheck(0);
		return false;
	}

	if(!ModifyDataDBVavle(podata->targetkey, sizeof(podata->targetkey), &gSaveData, sizeof(SAVE_DATA)))
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
	if(pbetctx == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(!OpenDataCheck(pbetctx))
	{
		ErrorCheck(0);
		return false;
	}

	if(!OpenOperateAccount(pbetctx))
	{
		ErrorCheck(0);
		return false;
	}

	if(!RecordOpenBetStatus(pbetctx))
	{
		ErrorCheck(0);
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
	OPEN_DATA *phandle = (OPEN_DATA *)gContractData;
	bool ret = false;

	if(pbetctx == NULL)
	{
		ErrorCheck(0);
		return false;
	}

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
		ErrorCheck(0);
		ret = false;
		break;
	}

	return ret;
}

static bool InitCtxData(BET_CTX const *pctxdata)
{
	if(pctxdata == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(!GetCurTxHash((void *)pctxdata->txhash))
	{
		ErrorCheck(0);
		return false;
	}

	if(GetCurTxAccount((void *)pctxdata->accid, sizeof(pctxdata->accid)) == 0)
	{
		ErrorCheck(0);
		return false;
	}

	return true;
}

int main()
{
	__xdata __no_init static BET_CTX BetCtx;

	if(!InitCtxData(&BetCtx))
	{
		ErrorCheck(0);
		__exit(RUN_SCRIPT_DATA_ERR);
	}

	if(!GetContractData(&BetCtx))
	{
		ErrorCheck(0);
		__exit(RUN_SCRIPT_DATA_ERR);
	}
#if 0
	{
		LogPrint(BetCtx.txhash, sizeof(BetCtx.txhash), HEX);
		LogPrint(BetCtx.accid, sizeof(BetCtx.accid), HEX);
		LogPrint(gContractData, sizeof(gContractData), HEX);

		SEND_DATA *phandle1 = (SEND_DATA *)gContractData;
		LogPrint(&phandle1->type, sizeof(phandle1->type), HEX);
		LogPrint(&phandle1->money, sizeof(phandle1->money), HEX);
		LogPrint(&phandle1->hight, sizeof(phandle1->hight), HEX);
		LogPrint(phandle1->dhash, sizeof(phandle1->dhash), HEX);

		ACCEPT_DATA *phandle2 = (ACCEPT_DATA *)gContractData;
		LogPrint(&phandle2->type, sizeof(phandle2->type), HEX);
		LogPrint(&phandle2->money, sizeof(phandle2->money), HEX);
		LogPrint(&phandle2->targetkey, sizeof(phandle2->targetkey), HEX);
		LogPrint(&phandle2->dhash, sizeof(phandle2->dhash), HEX);

		OPEN_DATA *phandle3 = (OPEN_DATA *)gContractData;
		LogPrint(&phandle3->type, sizeof(phandle3->type), HEX);
		LogPrint(&phandle3->targetkey, sizeof(phandle3->targetkey), HEX);
		LogPrint(&phandle3->dhash, sizeof(phandle3->dhash), HEX);
	}
#endif

	if(!RunContractData(&BetCtx))
	{
		ErrorCheck(0);
		__exit(RUN_SCRIPT_DATA_ERR);
	}

	__exit(RUN_SCRIPT_OK);
	return 1;
}

