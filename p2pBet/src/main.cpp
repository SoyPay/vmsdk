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
typedef unsigned int u16;
typedef unsigned long u32;

/**
 * p2pBet Operate Type
 */
enum OPERATETYPE {
	SEND,  //!< SEND
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
	uchar dhash[33];//32 + one byte checksum
}SEND_DATA;

/**
 * accept bet data struct
 */
typedef struct {
	OPERATETYPE type;
	Int64 money;
	uchar dhash[33];//32 + one byte checksum
}ACCEPT_DATA;

/**
 * open bet data struct
 */
typedef struct {
	OPERATETYPE type;
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
typedef struct {
	BETSTATUS status;
	uchar sendid[6];//send bet account id
	uchar acceptid[6];//accept bet account id
	Int64 money;//bet amount
	uchar shash[33];//send data hash
	uchar ahash[33];//accept data hash
	uchar sdata[5];//send data
	uchar adata[5];//accept data
}SAVE_DATA;


/**
 * @brief get contract data
 * @param pdata:out data point
 * @return
 */
static bool GetContractData(void const *pdata, u16 bufsize)
{
	uchar txhash[32] = {0};
	if(pdata == NULL || bufsize == 0)
	{
		return false;
	}

	if(!GetCurTxHash(txhash))
	{
		return false;
	}

	if(!GetTxContacts(txhash, (void* const)pdata, bufsize))
	{
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
		return false;
	}

	Int64Inital(&minamount,"\x00\x00\x00\x00\x00\x01\x00\x00",8);

	if(Int64Compare(pdata, &minamount) != COMP_LARGER)
	{
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
		return false;
	}

	if(*phight <= GetCurRunEnvHeight())
	{
		return false;
	}

	return true;
}

/**
 * @brief check data hash
 * @param pdata:the input data
 * @return
 */
static bool CheckDataHash(const void *pdata)
{
	uchar checksum = 0;
	if(pdata == NULL)
	{
		return false;
	}

	for(uchar ii = 0; ii < 32; ii++)
	{
		checksum += pdata[ii];
	}

	if(checksum != pdata[32])
	{
		return false;
	}

	return true;
}

/**
 * @brief send bet data check
 * @param pdata:the send bet data
 * @return
 */
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

/**
 * @brief operate account
 * @param pdata:the contract data
 * @return
 */
static bool OperateAccount(const void *pdata)
{
	OPEN_DATA *phandle = (OPEN_DATA *)pdata;
	bool ret = false;

	switch(phandle->type)
	{
	case SEND:
		{
			VM_OPERATE operate;
			uchar txhash[32] = {0};
			SEND_DATA *psdata = (SEND_DATA *)pdata;

			if(pdata == NULL)
			{
				return false;
			}

			if(!GetCurTxHash(txhash))
			{
				return false;
			}

			GetAccounts(txhash, operate.accountid, sizeof(operate.accountid));

			operate.money = psdata->money;
			operate.opeatortype = MINUS_FREE;
			operate.outheight = psdata->hight;

			WriteOutput(&operate, 1);

			ret = true;
		}
		break;

	case ACCEPT:
		{
			VM_OPERATE operate;
			uchar txhash[32] = {0};
			ACCEPT_DATA *psdata = (ACCEPT_DATA *)pdata;

			if(pdata == NULL)
			{
				return false;
			}

			if(!GetCurTxHash(txhash))
			{
				return false;
			}

			GetAccounts(txhash, operate.accountid, sizeof(operate.accountid));

			operate.money = psdata->money;
			operate.opeatortype = MINUS_FREE;
			operate.outheight = 100;//???

			WriteOutput(&operate, 1);

			ret = true;
		}
		break;

	case OPEN:
		{
			SAVE_DATA savedata;
			if(ReadDataValueDB("test", sizeof("test"), &savedata, sizeof(SAVE_DATA)))
			{
				return false;
			}

			if(savedata.status == BETACCEPT)//fisrt open
			{

			}
			else if(savedata.status == BETOPEN1)//second open
			{

			}
			else//error status
			{
				ret = false;
				break;
			}


			ret = true;
		}
		break;

	default:
		{
			ret = false;
		}
		break;
	}

	return ret;
}

/**
 * @brief record send bet status
 * @param pdata:the contract data
 * @return
 */
static bool RecordSendBetStatus(const void *pdata)
{
	SEND_DATA *psdata = (SEND_DATA *)pdata;
	SAVE_DATA savedata;

	if(pdata == NULL)
	{
		return false;
	}

	memset(&savedata, 0, sizeof(SAVE_DATA));
	savedata.status = BETSEND;
	savedata.money = psdata->money;
	savedata.shash = psdata->dhash;

	{
		uchar txhash[32] = {0};
		if(!GetCurTxHash(txhash))
		{
			return false;
		}

		GetAccounts(txhash, savedata.sendid, sizeof(savedata.sendid));
	}

	if(WriteDataDB("test", sizeof("test"), &savedata, sizeof(SAVE_DATA), 12345))
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

/**
 * @brief check if the database have someone send a bet
 * @return
 */
static bool IsHaveSendBet(void)
{
	SAVE_DATA savedata;

	if(ReadDataValueDB("test", sizeof("test"), &savedata, sizeof(SAVE_DATA)))
	{
		return false;
	}

	if(savedata.status != BETSEND)
	{
		return false;
	}

	return true;
}

static bool IsEqualSendAmount(const Int64 *pdata)
{
	SAVE_DATA savedata;
	if(pdata == NULL)
	{
		return false;
	}

	if(ReadDataValueDB("test", sizeof("test"), &savedata, sizeof(SAVE_DATA)))
	{
		return false;
	}

	if(Int64Compare(pdata, &savedata.money) != COMP_LARGER)
	{
		return false;
	}

	return true;
}

/**
 * @brief accept bet data check
 * @param pdata:accept bet contract data
 * @return
 */
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

	if(!IsEqualSendAmount(&psdata->money))
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

/**
 * @brief record accept bet status
 * @param pdata:the contract data
 * @return
 */
static bool RecordAcceptBetStatus(const void *pdata)
{
	SAVE_DATA savedata;
	ACCEPT_DATA *psdata = (ACCEPT_DATA *)pdata;
	if(pdata == NULL)
	{
		return false;
	}

	if(ReadDataValueDB("test", sizeof("test"), &savedata, sizeof(SAVE_DATA)))
	{
		return false;
	}

	savedata.status = ACCEPT;
	savedata.ahash = psdata->dhash;

	{
		uchar txhash[32] = {0};
		if(!GetCurTxHash(txhash))
		{
			return false;
		}

		GetAccounts(txhash, savedata.acceptid, sizeof(savedata.sendid));
	}

	if(ModifyDataDBVavle("test", sizeof("test"), &savedata, sizeof(SAVE_DATA)))
	{
		return false;
	}

	return true;
}

/**
 * @brief accept a bet
 * @param pdata:the contract data
 * @return
 */
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

/**
 * @brief check if the database have a full bet
 * @return
 */
static bool IsHaveP2PBet(void)
{
	SAVE_DATA savedata;
	if(ReadDataValueDB("test", sizeof("test"), &savedata, sizeof(SAVE_DATA)))
	{
		return false;
	}

	if(savedata.status != BETACCEPT || savedata.status != BETOPEN1)
	{
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
static bool IsDataVaild(const void *pdata, u32 len)
{
	uchar checkhash[33] = {0};

	if(pdata == NULL || len == 0)
	{
		return false;
	}

	SHA256(pdata, len, checkhash);

	for(uchar ii = 0; ii < 32; ii++)
	{
		checkhash[32] += checkhash[ii];
	}

	{
		SAVE_DATA savedata;
		if(ReadDataValueDB("test", sizeof("test"), &savedata, sizeof(SAVE_DATA)))
		{
			return false;
		}

		uchar txhash[32] = {0}, accid[6] = {0};
		if(!GetCurTxHash(txhash))
		{
			return false;
		}

		GetAccounts(txhash, accid, sizeof(accid));

		if(memcmp(accid, savedata.acceptid, sizeof(accid)) == 0)
		{
			if(memcmp(checkhash, savedata.ahash, sizeof(checkhash)) != 0)
			{
				return false;
			}
		}
		else if(memcmp(accid, savedata.sendid, sizeof(accid)) == 0)
		{
			if(memcmp(checkhash, savedata.shash, sizeof(checkhash)) != 0)
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}

/**
 * @brief open bet data check
 * @param pdata:the open bet contract data
 * @return
 */
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

	if(!IsDataVaild(podata->dhash, sizeof(podata->dhash)))
	{
		return false;
	}

	return true;
}

/**
 * @brief record open bet status
 * @param pdata:the contract data
 * @return
 */
static bool RecordOpenBetStatus(const void *pdata)
{
	SAVE_DATA savedata;
	if(pdata == NULL)
	{
		return false;
	}

	if(ReadDataValueDB("test", sizeof("test"), &savedata, sizeof(SAVE_DATA)))
	{
		return false;
	}

	savedata.status = OPEN;

	if(ModifyDataDBVavle("test", sizeof("test"), &savedata, sizeof(SAVE_DATA)))
	{
		return false;
	}

	return true;
}

/**
 * @brief open a bet
 * @param pdata:the open bet contract data
 * @return
 */
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
	uchar contractbuf[256] = {0};

	if(!GetContractData(contractbuf, sizeof(contractbuf)))
	{
		printf("get contract err, GetContractData return false!\r\n");
		return 0;
	}

	if(RunContractData(contractbuf))
	{
		printf("run contract data err, RunContractData return false!\r\n");
		return 0;
	}

	return 1;
}

