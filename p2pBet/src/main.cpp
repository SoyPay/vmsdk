/*
 * main.cpp
 *
 *  Created on: Nov 26, 2014
 *      Author: spark.huang
 */
#include <string.h>
#include<stdlib.h>
#include"VmSdk.h"

typedef unsigned char uchar;
typedef unsigned long u32;

/**
 * p2pBet Operate Type
 */
enum OPERATETYPE {
	SEND,  //!< SEND
	ACCEPT,//!< ACCEPT
	OPEN   //!< OPEN
};

typedef struct {
	OPERATETYPE type;
	Int64 money;
	u32 hight;
	uchar dhash[32];
}SEND_DATA;

typedef struct {
	OPERATETYPE type;
	Int64 money;
	uchar dhash[32];
}ACCEPT_DATA;

typedef struct {
	OPERATETYPE type;
	uchar dhash[5];
}OPEN_DATA;

/**
 * @brief get contract data
 * @param pdata:out data point
 * @return
 */
static bool GetContractData(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}
	return true;
}

static bool CheckMinimumAmount(const Int64 *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}
	return true;
}

static bool CheckSendBetHight(const u32 *phight)
{
	if(phight == NULL)
	{
		return false;
	}

	if(*phight <= GetCurRunEnvHeight())
	{
		return false;
	}

	return true;
}

static bool CheckDataHash(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}

	return true;
}

static bool SendDataCheck(const void *pdata)
{
	SEND_DATA *psdata = (SEND_DATA *)pdata;
	Int64 balance;
	uchar txhash[32] = {0}, accid[6] = {0};

	if(pdata == NULL)
	{
		return false;
	}

	if(!CheckMinimumAmount(&psdata->money))
	{
		return false;
	}

	if(!GetCurTxHash(txhash))
	{
		return false;
	}

	GetAccounts(txhash, accid, sizeof(accid));

	if(!QueryAccountBalance(accid, ACOUNT_ID, &balance))
	{
		printf("get account balance err, QueryAccountBalance return false!\r\n");
		return false;
	}

	if(Int64Compare(&balance, &psdata->money) != COMP_LARGER)
	{
		printf("account balance  <  reg amount!\r\n");
		return false;
	}

	if(!CheckSendBetHight(&psdata->hight))
	{
		return false;
	}

	if(!CheckDataHash(psdata->dhash))
	{
		return false;
	}

	return true;
}

static bool OperateAccount(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}
	return true;
}

static bool RecordSendBetStatus(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}
	return true;
}

/**
 * @brief send a p2p bet
 * @param pdata:contract data
 * @return
 */
static bool SendP2PBet(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}

	if(!SendDataCheck(pdata))
	{
		return false;
	}

	if(!OperateAccount(pdata))
	{
		return false;
	}

	if(!RecordSendBetStatus(pdata))
	{
		return false;
	}

	return true;
}

static bool IsHaveSendBet(void)
{
	return true;
}

static bool AcceptDataCheck(const void *pdata)
{
	SEND_DATA *psdata = (SEND_DATA *)pdata;
	Int64 balance;
	uchar txhash[32] = {0}, accid[6] = {0};

	if(pdata == NULL)
	{
		return false;
	}

	if(!IsHaveSendBet())
	{
		return false;
	}

	if(!CheckMinimumAmount(&psdata->money))
	{
		return false;
	}

	if(!GetCurTxHash(txhash))
	{
		return false;
	}

	GetAccounts(txhash, accid, sizeof(accid));

	if(!QueryAccountBalance(accid, ACOUNT_ID, &balance))
	{
		printf("get account balance err, QueryAccountBalance return false!\r\n");
		return false;
	}

	if(Int64Compare(&balance, &psdata->money) != COMP_LARGER)
	{
		printf("account balance  <  reg amount!\r\n");
		return false;
	}

	if(!CheckDataHash(psdata->dhash))
	{
		return false;
	}

	return true;
}

static bool RecordAcceptBetStatus(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}
	return true;
}

static bool AcceptP2PBet(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}

	if(!AcceptDataCheck(pdata))
	{
		return false;
	}

	if(!OperateAccount(pdata))
	{
		return false;
	}

	if(!RecordAcceptBetStatus(pdata))
	{
		return false;
	}

	return true;
}

static bool IsHaveP2PBet(void)
{
	return true;
}

static bool IsAlreadyOpen(void)
{
	return true;
}



static bool IsDataVaild(const void *pdata, u32 len)
{
	uchar checkhash[32] = {0};

	if(pdata == NULL || len == 0)
	{
		return false;
	}

	SHA256(pdata, len, checkhash);

	return true;
}

static bool OpenDataCheck(const void *pdata)
{
	OPEN_DATA *podata = (OPEN_DATA *)pdata;

	if(pdata == NULL)
	{
		return false;
	}

	if(!IsHaveP2PBet())
	{
		return false;
	}

	if(!IsAlreadyOpen())
	{
		return false;
	}

	if(!IsDataVaild(podata->dhash, sizeof(podata->dhash)))
	{
		return false;
	}

	return true;
}

static bool RecordOpenBetStatus(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}
	return true;
}

static bool OpenP2PBet(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}

	if(!OpenDataCheck(pdata))
	{
		return false;
	}

	if(!OperateAccount(pdata))
	{
		return false;
	}

	if(!RecordOpenBetStatus(pdata))
	{
		return true;
	}

	return true;
}

/**
 * @brief according to the contract data,run the script
 * @param pdata:contract data handle
 * @return
 */
static bool RunContractData(const void *pdata)
{
	OPEN_DATA *phandle = (OPEN_DATA *)pdata;
	bool ret = false;

	if(pdata == NULL)
	{
		return false;
	}

	switch(phandle->type)
	{
	case SEND:
		ret = SendP2PBet(pdata);
		break;

	case ACCEPT:
		ret = AcceptP2PBet(pdata);
		break;

	case OPEN:
		ret = OpenP2PBet(pdata);
		break;

	default:
		ret = false;
		break;
	}

	return ret;
}


int main()
{
	unsigned char *pcontract = NULL;

	if(GetContractData(pcontract))
	{
		printf("get contract err, GetContractData return false!\r\n");
		return 0;
	}

	if(pcontract == NULL)
	{
		printf("get contract err, pcontract == NULL!\r\n");
		return 0;
	}

	if(RunContractData(pcontract))
	{
		printf("run contract data err, RunContractData return false!\r\n");
		return 0;
	}

	return 1;
}

