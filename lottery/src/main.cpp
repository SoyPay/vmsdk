/*
 * main.cpp
 *
 *  Created on: Nov 15, 2014
 *      Author: spark.huang
 */
#include <string.h>
#include<stdlib.h>
#include"VmSdk.h"

typedef unsigned char uchar;

/**
 * Lottery Operate Type
 */
enum OPERATETYPE {
	REG,  //!< REG
	ORDER,//!< ORDER
	OPEN  //!< OPEN
};

/**
 * lottery contract data handle
 */
typedef struct _DataHandle{
	OPERATETYPE type;
	uchar databuf[0];
}DataHandle;

/**
 *reg lottery data struct
 */
typedef struct _RegData{
	OPERATETYPE type;
	uchar regid[6];
	uchar money[8];
}RegData;

/**
 * order lottery data struct
 */
typedef struct _OrderData{
	OPERATETYPE type;
	uchar txhash[32];
	uchar regid[6];
	uchar num[15];
	uchar money[8];
}OrderData;

/**
 * open lottery data struct
 */
typedef struct _OpenData{
	OPERATETYPE type;
	uchar txhash[32];
}OpenData;

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

/**
 * @brief whether the account id have reg lottery in this script
 * @param pdata:reg data hanlde
 * @return
 */
static bool IsAccountIdUsed(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}
	return true;
}

/**
 * @brief check reg data
 * 1.check whether the account id have already used in this script
 * 2.check the account balance,whether the balance is enough to pay the reg amount
 * @param pdata:reg data hanlde
 * @return
 */
static bool RegDataCheck(const void *pdata)
{
	RegData *pregdata = (RegData *)pdata;
	Int64 balance, amount;

	if(pdata == NULL)
	{
		return false;
	}

	if(IsAccountIdUsed(pregdata->regid))
	{
		printf("the AccountId Already Reg in this Script!\r\n");
		return false;
	}

	if(!QueryAccountBalance(pregdata->regid, ACOUNT_ID, &balance))
	{
		printf("get account balance err, QueryAccountBalance return false!\r\n");
		return false;
	}

	Int64Inital(&amount, (char *)pregdata->money, sizeof(Int64));

	if(Int64Compare(&balance, &amount) != COMP_LARGER)
	{
		printf("account balance  <  reg amount!\r\n");
		return false;
	}

	return true;
}

/**
 * @brief operate account
 * @param pdata:data hanlde
 * @return
 */
static bool OperateAccount(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}
	return true;
}

/**
 * @brief record account reg status in script database
 * @param pdata:reg data hanlde
 * @return
 */
static bool RecordAccountRegStatus(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}
	return true;
}

/**
 * @brief reg lottery
 * 1.check the reg data
 * 2.According to the reg data operate account
 * 3.record the account reg status
 * @param pdata:reg data hanlde
 * @return
 */
static bool RegLottery(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}

	if(!RegDataCheck(pdata))
	{
		return false;
	}

	if(!OperateAccount(pdata))
	{
		return false;
	}

	if(!RecordAccountRegStatus(pdata))
	{
		return false;
	}

	return true;
}

/**
 * @brief check reg lottery tx contact
 * @param pdata:order data hanlde
 * @return
 */
static bool CheckRegLotteryTx(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}
	return true;
}

/**
 * @brief check order num and order amount
 * @param pdata:order data hanlde
 * @return
 */
static bool CheckOrderNumAndAmount(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}
	return true;
}

/**
 * @brief order data check
 * 1.check the reg tx's contact
 * 2.check account balance,is it enough to pay the order amount
 * 3.check whether the num and amount is legal
 * @param pdata:order data hanlde
 * @return
 */
static bool OrderDataCheck(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}
	OrderData *porderdata = (OrderData *)pdata;
	Int64 balance, amount;

	if(!CheckRegLotteryTx(porderdata->txhash))
	{
		return false;
	}

	if(!QueryAccountBalance(porderdata->regid, ACOUNT_ID, &balance))
	{
		printf("get account balance err, QueryAccountBalance return false!\r\n");
		return false;
	}

	Int64Inital(&amount, (char *)porderdata->money, sizeof(Int64));

	if(Int64Compare(&balance, &amount) != COMP_LARGER)
	{
		printf("account balance  <  order amount!\r\n");
		return false;
	}

	if(!CheckOrderNumAndAmount(pdata))
	{
		return false;
	}

	return true;
}

/**
 * @brief record account order status in the script database
 * @param pdata:order data hanlde
 * @return
 */
static bool RecordAccountOrderStatus(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}
	return true;
}

/**
 * @brief order lottery
 * 1.check order data
 * 2.according to the order data,operate the account
 * 3.record account order status in the script database
 * @param pdata
 * @return
 */
static bool OrderLottery(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}

	if(!OrderDataCheck(pdata))
	{
		return false;
	}

	if(!OperateAccount(pdata))
	{
		return false;
	}

	if(!RecordAccountOrderStatus(pdata))
	{
		return false;
	}

	return true;
}

/**
 * @brief get the hight of lottery open
 * @param pdata:open data handle
 * @return the hight
 */
static unsigned long GetOpenLotteryHight(const void *pdata)
{
	if(pdata == NULL)
	{
		return 0;
	}
	return 0;
}

/**
 * @brief check whether the current block hight is higher than open lottery hight
 * @param pdata:open data handle
 * @return
 */
static bool BlockHightCheck(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}

	unsigned long hight = GetCurRunEnvHeight();

	unsigned long openhight = GetOpenLotteryHight(pdata);
	return true;
}

/**
 * @brief get open lottery status from the script database
 * @param pdata:open data handle
 * @return
 */
static bool GetOpenLotteryStatus(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}
	return true;
}

/**
 * @brief open data check
 * 1.check the reg lottery tx's contact
 * 2.check the block hight,whether it is higher than the open hight
 * 3.check whether the lottery is opened
 * @param pdata:open data handle
 * @return
 */
static bool OpenDataCheck(const void *pdata)
{
	OpenData *popendata = (OpenData *)pdata;

	if(pdata == NULL)
	{
		return false;
	}

	if(!CheckRegLotteryTx(popendata->txhash))
	{
		return false;
	}

	if(!BlockHightCheck(pdata))
	{
		return false;
	}

	if(!GetOpenLotteryStatus(pdata))
	{
		return false;
	}

	return true;
}

/**
 * @brief record open lottery status in the script database
 * @param pdata:open data handle
 * @return
 */
static bool RecordOpenStatus(const void *pdata)
{
	if(pdata == NULL)
	{
		return false;
	}
	return true;
}

/**
 * @brief open lottery
 * 1.check open lottery data
 * 2.according to record of order lottery in database,open the lottery and operate account
 * 3.record open status
 * @param pdata
 * @return
 */
static bool OpenLottery(const void *pdata)
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

	if(!RecordOpenStatus(pdata))
	{
		return false;
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
	DataHandle *phandle = (DataHandle *)pdata;
	bool ret = false;

	if(pdata == NULL)
	{
		return false;
	}

	switch(phandle->type)
	{
	case REG:
		ret = RegLottery(pdata);
		break;

	case ORDER:
		ret = OrderLottery(pdata);
		break;

	case OPEN:
		ret = OpenLottery(pdata);
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















