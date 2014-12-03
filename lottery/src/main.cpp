/*
 * main.cpp
 *
 *  Created on: Nov 15, 2014
 *      Author: spark.huang
 */
#include <string.h>
#include<stdlib.h>
#include"VmSdk.h"
#include"lottery.h"

/**
 * Lottery Operate Type
 */
enum OPERATETYPE {
	REG,  //!< REG
	ORDER,//!< ORDER
	OPEN  //!< OPEN
};

/**
 *reg lottery data struct
 */
typedef struct {
	OPERATETYPE type;
	Int64 money;
	u32 hight;
}REG_DATA;

/**
 * order lottery data struct
 */
typedef struct {
	OPERATETYPE type;
	uchar num[15];
	Int64 money;
}ORDER_DATA;

/**
 * open lottery data struct
 */
typedef struct {
	OPERATETYPE type;
}OPEN_DATA;

typedef struct {
	uchar txhash[32];//current tx hash
	uchar accid[6];//current account id
}LOTTO_CTX;

typedef struct {
	uchar accid[6];
	Int64 amount;
	u32 reghight;
	u32 closehight;
}REG_SAVE_DATA;

__xdata __no_init static uchar gContractData[256];
__xdata __no_init static REG_SAVE_DATA gRegSaveData;

/**
 * @brief get contract data
 * @param pdata:out data point
 * @return
 */
static bool GetContractData(const LOTTO_CTX *pctxdata)
{
	if(pctxdata == NULL)
	{
		return false;
	}

	if(!GetTxContacts(pctxdata->txhash, gContractData, sizeof(gContractData)))
	{
		return false;
	}

	return true;
}

/**
 * @brief check whether the reg amount is more than the min amount
 * @param pdata:the reg amount
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
 * @brief check reg hight,is it higher than the current hight
 * @param phight:the reg hight
 * @return
 */
static bool CheckRegHight(const u32 *phight)
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
 * @brief check reg data
 * 1.check whether the account id have already used in this script
 * 2.check the account balance,whether the balance is enough to pay the reg amount
 * @param pdata:reg data hanlde
 * @return
 */
static bool RegDataCheck(const LOTTO_CTX *pctxdata)
{
	REG_DATA *pregdata = (REG_DATA *)gContractData;
	Int64 balance, amount;

	if(pctxdata == NULL)
	{
		return false;
	}

	if(!QueryAccountBalance(pctxdata->accid, ACOUNT_ID, &balance))
	{
		printf("get account balance err, QueryAccountBalance return false!\r\n");
		return false;
	}

	if(Int64Compare(&balance, &pregdata->money) != COMP_LARGER)
	{
		printf("account balance  <  reg amount!\r\n");
		return false;
	}

	if(!CheckMinimumAmount(&pregdata->money))
	{
		return false;
	}

	if(!CheckRegHight(&pregdata->hight))
	{
		return false;
	}

	return true;
}

static bool RegOperateAccount(const LOTTO_CTX *pbetctx)
{
	REG_DATA *pregdata = (REG_DATA *)gContractData;
	VM_OPERATE operate;

	if(pbetctx == NULL)
	{
		return false;
	}

	operate.accountid = pbetctx->accid;
	operate.money = pregdata->money;
	operate.opeatortype = MINUS_FREE;
	operate.outheight = pregdata->hight;

    return WriteOutput(&operate, 1);
}

/**
 * @brief record account reg status in script database
 * @param pdata:reg data hanlde
 * @return
 */
static bool RecordRegLottoStatus(const LOTTO_CTX *pctxdata)
{
	REG_DATA *pregdata = (REG_DATA *)gContractData;
	REG_SAVE_DATA savedata;
	if(pctxdata == NULL)
	{
		return false;
	}

	memset(&savedata, 0, sizeof(REG_SAVE_DATA));
	savedata.accid = pctxdata->accid;
	savedata.amount = pregdata->money;
	savedata.reghight = GetCurRunEnvHeight();
	savedata.closehight = pregdata->hight;

	if(WriteDataDB("reglotto", sizeof("reglotto"), &savedata, sizeof(REG_SAVE_DATA), 12345))
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
static bool RegLottery(const LOTTO_CTX *pctxdata)
{
	if(pctxdata == NULL)
	{
		return false;
	}

	if(!RegDataCheck(pctxdata))
	{
		return false;
	}

	if(!RegOperateAccount(pctxdata))
	{
		return false;
	}

	if(!RecordRegLottoStatus(pctxdata))
	{
		return false;
	}

	return true;
}

static bool CheckOrderNum(const uchar *pdata, uchar datalen)
{
	uchar ii = 0;
	if(pdata == NULL || datalen == 0 || datalen > 15)
	{
		return false;
	}

	do
	{
		if(pdata[ii] > 14)
		{
			break;
		}
	}
	while(ii < datalen);
}


/**
 * @brief check order num and order amount
 * @param pdata:order data hanlde
 * @return
 */
static bool CheckOrderNumAndAmount(void)
{
	ORDER_DATA *porderdata = (ORDER_DATA *)gContractData;



	return true;
}

/**
 * @brief check order hight
 * @param phight:order hight
 * @return
 */
static bool CheckOrderHight(void)
{
	if(GetCurRunEnvHeight() > gRegSaveData.closehight)
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
static bool OrderDataCheck(const LOTTO_CTX *pctxdata)
{
	ORDER_DATA *porderdata = (ORDER_DATA *)gContractData;
	Int64 balance, amount;

	if(pctxdata == NULL)
	{
		return false;
	}

	if(!QueryAccountBalance(pctxdata->accid, ACOUNT_ID, &balance))
	{
		printf("get account balance err, QueryAccountBalance return false!\r\n");
		return false;
	}

	if(Int64Compare(&balance, &porderdata->money) != COMP_LARGER)
	{
		printf("account balance  <  order amount!\r\n");
		return false;
	}

	if(!CheckOrderNumAndAmount())
	{
		return false;
	}

	if(!CheckOrderHight())
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
static bool OrderLottery(const LOTTO_CTX *pctxdata)
{
	if(pctxdata == NULL)
	{
		return false;
	}

	if(!OrderDataCheck(pctxdata))
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
 * @return the hight
 */
static unsigned long GetOpenLotteryHight(void)
{
	return 0;
}

/**
 * @brief check whether the current block hight is higher than open lottery hight
 * @return
 */
static bool CheckOpenHight(void)
{
	unsigned long hight = GetCurRunEnvHeight();

	unsigned long openhight = GetOpenLotteryHight();
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
	OPEN_DATA *popendata = (OPEN_DATA *)pdata;

	if(pdata == NULL)
	{
		return false;
	}

	if(!CheckOpenHight())
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
static bool RunContractData(const LOTTO_CTX *pctxdata)
{
	OPEN_DATA *phandle = (OPEN_DATA *)gContractData;
	bool ret = false;

	if(pctxdata == NULL)
	{
		return false;
	}

	switch(phandle->type)
	{
	case REG:
		ret = RegLottery(pctxdata);
		break;

	case ORDER:
		ret = OrderLottery(pctxdata);
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

static bool InitCtxData(LOTTO_CTX const *pctxdata)
{
	if(pctxdata == NULL)
	{
		return false;
	}

	if(!GetCurTxHash((void *)pctxdata->txhash))
	{
		return false;
	}

	if(GetAccounts(pctxdata->txhash, (void *)pctxdata->accid, sizeof(pctxdata->accid)) > 0)
	{
		return true;
	}

	return false;
}

int main()
{
	__xdata __no_init static LOTTO_CTX LottoCtx;

	if(!InitCtxData(&LottoCtx))
	{
		return 0;
	}

	if(GetContractData(&LottoCtx))
	{
		printf("get contract err, GetContractData return false!\r\n");
		return 0;
	}

	if(RunContractData(&LottoCtx))
	{
		printf("run contract data err, RunContractData return false!\r\n");
		return 0;
	}


	return 1;
}















