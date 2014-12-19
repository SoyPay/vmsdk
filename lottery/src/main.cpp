/*
 * main.cpp
 *
 *  Created on: Nov 15, 2014
 *      Author: spark.huang
 */

#include <string.h>
#include<stdlib.h>
#include <stdio.h>
#include"lottery.h"

/**
 * Lottery Operate Type
 */
enum OPERATETYPE {
	REG,  //!< REG
	ORDER,//!< ORDER
	OPEN  //!< OPEN
};

enum LOTTOSTATUS {
	LOTTONONE,
	LOTTOUNOPEN,
	LOTTOOPEN
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
	uchar numlen;
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
	uchar scriptid[6];//current script id
}LOTTO_CTX;

//注册彩票需保存的数据
//key:SysRegLotto
typedef struct {
	uchar accid[6];
	Int64 amount;
	u32 reghight;
	u32 closehight;
}REG_SAVE_DATA;

//订购彩票需要保存的数据
//key:u32 term + u32 order
//data:tx hash
typedef struct {
	uchar accid[6];
	Int64 amount;
	uchar num[15];
	uchar numlen;
	u32 orderhight;
}ORDER_INFO;

//用一个关键字记录所有记录期数的状态，便于查找是否有开奖期数
//只保存5期的记录
//key:SysStatus
typedef struct {
	LOTTOSTATUS status[5];//0-no record;1-unopen;2-opened
	u32 term[5];//期数
	u32 openhight[5];//对应该期的开奖高度
	u32 totalorders[5];//对应总注数
}SYSLOTTO_STATUS;

//用另一个关键字记录当前期的投注状态
//key:CurretStatus
typedef struct {
	u32 term;//第几期
	Int64 amount;//奖池余额
	u32 totalorders;//当前投注量
	u32 openhight;//开奖高度
}CURRENT_STATUS;

#define CURRENT "CurretStatus"
#define SYSTERM "SysStatus"

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
		ErrorCheck(0);
		return false;
	}

	if(!GetTxContacts(pctxdata->txhash, gContractData, sizeof(gContractData)))
	{
		ErrorCheck(0);
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
 * @brief check reg hight,is it higher than the current hight
 * @param phight:the reg hight
 * @return
 */
static bool CheckRegHight(const u32 *phight)
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
 * @brief check reg data
 * 1.check whether the account id have already used in this script
 * 2.check the account balance,whether the balance is enough to pay the reg amount
 * @param pdata:reg data hanlde
 * @return
 */
static bool RegDataCheck(const LOTTO_CTX *pctxdata)
{
	REG_DATA *pregdata = (REG_DATA *)gContractData;
	Int64 balance;

	if(pctxdata == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(!QueryAccountBalance(pctxdata->accid, ACOUNT_ID, &balance))
	{
		ErrorCheck(0);
		return false;
	}

	if(Int64Compare(&balance, &pregdata->money) != COMP_LARGER)
	{
		ErrorCheck(0);
		return false;
	}

	if(!CheckMinimumAmount(&pregdata->money))
	{
		ErrorCheck(0);
		return false;
	}

	if(!CheckRegHight(&pregdata->hight))
	{
		ErrorCheck(0);
		return false;
	}

	return true;
}

static bool RegOperateAccount(const LOTTO_CTX *pbetctx)
{
	REG_DATA *pregdata = (REG_DATA *)gContractData;
	VM_OPERATE operate[2];

	if(pbetctx == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	memcpy(operate[0].accountid, pbetctx->accid, sizeof(pbetctx->accid));
	memcpy(&operate[0].money, &pregdata->money, sizeof(pregdata->money));
	operate[0].opeatortype = MINUS_FREE;
	operate[0].outheight = pregdata->hight;

	memcpy(operate[1].accountid, pbetctx->scriptid, sizeof(pbetctx->scriptid));
	memcpy(&operate[1].money, &pregdata->money, sizeof(pregdata->money));
	operate[1].opeatortype = ADD_FREE;
	operate[1].outheight = pregdata->hight;

    return WriteOutput(operate, 2);
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
	CURRENT_STATUS current;
	SYSLOTTO_STATUS sys;
	if(pctxdata == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	memset(&savedata, 0, sizeof(REG_SAVE_DATA));
	memcpy(savedata.accid ,pctxdata->accid, sizeof(pctxdata->accid));
	memcpy(&savedata.amount, &pregdata->money, sizeof(pregdata->money));
	savedata.reghight = GetCurRunEnvHeight();
	savedata.closehight = pregdata->hight;
	if(WriteDataDB("SysRegLotto", sizeof("SysRegLotto"), &savedata, sizeof(REG_SAVE_DATA), 12345))
	{
		ErrorCheck(0);
		return false;
	}

	memset(&current, 0, sizeof(CURRENT_STATUS));
	memcpy(&current.amount, &pregdata->money, sizeof(pregdata->money));
	current.term = 1;
	current.openhight = current.term + 10;
	current.totalorders = 0;
	if(WriteDataDB("CurretStatus", sizeof("CurretStatus"), &current, sizeof(CURRENT_STATUS), 12345))
	{
		ErrorCheck(0);
		return false;
	}

	memset(&sys, 0, sizeof(SYSLOTTO_STATUS));
	sys.status[0] = LOTTOUNOPEN;
	sys.term[0] = current.term;
	sys.openhight[0] = current.openhight;
	sys.totalorders[0] = 0;
	if(WriteDataDB("SysStatus", sizeof("SysStatus"), &sys, sizeof(SYSLOTTO_STATUS), 12345))
	{
		ErrorCheck(0);
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
		ErrorCheck(0);
		return false;
	}

	if(!RegDataCheck(pctxdata))
	{
		ErrorCheck(0);
		return false;
	}

	if(!RegOperateAccount(pctxdata))
	{
		ErrorCheck(0);
		return false;
	}

	if(!RecordRegLottoStatus(pctxdata))
	{
		ErrorCheck(0);
		return false;
	}

	return true;
}

static bool CheckOrderNum(const uchar *pdata, uchar datalen)
{
	uchar temp[15] = {0};
	uchar ii = 0;
	if(pdata == NULL || datalen == 0 || datalen > 15)
	{
		ErrorCheck(0);
		return false;
	}

	do
	{
		if(pdata[ii] > 14)
		{
			break;
		}
		temp[pdata[ii]]++;
		if(temp[pdata[ii]] > 1)
		{
			break;
		}
		ii++;
	}
	while(ii < datalen);

	return (ii == datalen)?(true):(false);
}

static bool CheckOrderAmount(const Int64 *pamount, uchar totalnum)
{
	Int64 apert, total64, min;
	u32 total = 0;
	if(pamount == NULL || totalnum < 6 || totalnum > 15)
	{
		ErrorCheck(0);
		return false;
	}

	total = MselectN(15, totalnum);
	memset(&total64, 0, sizeof(Int64));
	memcpy(&total64, &total, sizeof(total));
	if(!Int64Div(pamount, &total64, &apert))
	{
		ErrorCheck(0);
		return false;
	}
	Int64Inital(&min,"\x00\x00\x00\x00\x00\x01\x00\x00",8);
	if(Int64Compare(&apert, &min) != COMP_LARGER)
	{
		ErrorCheck(0);
		return false;
	}

	return true;
}

/**
 * @brief check order num and order amount
 * @param pdata:order data hanlde
 * @return
 */
static bool CheckOrderNumAndAmount(void)
{
	ORDER_DATA *porderdata = (ORDER_DATA *)gContractData;

	if(!CheckOrderNum(porderdata->num, porderdata->numlen))
	{
		ErrorCheck(0);
		return false;
	}

	if(!CheckOrderAmount(&porderdata->money, porderdata->numlen))
	{
		ErrorCheck(0);
		return false;
	}

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
		ErrorCheck(0);
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
	Int64 balance;

	if(pctxdata == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(!QueryAccountBalance(pctxdata->accid, ACOUNT_ID, &balance))
	{
		ErrorCheck(0);
		return false;
	}

	if(Int64Compare(&balance, &porderdata->money) != COMP_LARGER)
	{
		ErrorCheck(0);
		return false;
	}

	if(!CheckOrderNumAndAmount())
	{
		ErrorCheck(0);
		return false;
	}

	if(!CheckOrderHight())
	{
		ErrorCheck(0);
		return false;
	}

	return true;
}

static bool OrderOperateAccount(const LOTTO_CTX *pbetctx)
{
	ORDER_DATA *pregdata = (ORDER_DATA *)gContractData;
	VM_OPERATE operate[2];

	if(pbetctx == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	memcpy(operate[0].accountid, pbetctx->accid, sizeof(pbetctx->accid));
	memcpy(&operate[0].money, &pregdata->money, sizeof(pregdata->money));
	operate[0].opeatortype = MINUS_FREE;
	operate[0].outheight = 0;//???

	memcpy(operate[1].accountid, pbetctx->scriptid, sizeof(pbetctx->scriptid));
	memcpy(&operate[1].money, &pregdata->money, sizeof(pregdata->money));
	operate[1].opeatortype = ADD_FREE;
	operate[1].outheight = 0;//???

    return WriteOutput(operate, 2);
}

/**
 * @brief record account order status in the script database
 * @param pdata:order data hanlde
 * @return
 */
static bool RecordOrderLottoStatus(const LOTTO_CTX *pctxdata)
{
	CURRENT_STATUS current;
	uchar key[8] = {0};
	ORDER_DATA *pregdata = (ORDER_DATA *)gContractData;
	if(pctxdata == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(ReadDataValueDB("CurretStatus", sizeof("CurretStatus"), &current, sizeof(CURRENT_STATUS)))
	{
		ErrorCheck(0);
		return false;
	}
	current.totalorders++;

	memcpy(key, &current.term, sizeof(u32));
	memcpy(&key[4], &current.totalorders, sizeof(u32));
	if(WriteDataDB(key, sizeof(key), pctxdata->txhash, sizeof(pctxdata->txhash), 12345))
	{
		ErrorCheck(0);
		return false;
	}

	if(ModifyDataDBVavle("CurretStatus", sizeof("CurretStatus"), &current, sizeof(CURRENT_STATUS)))
	{
		ErrorCheck(0);
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
		ErrorCheck(0);
		return false;
	}

	if(!OrderDataCheck(pctxdata))
	{
		ErrorCheck(0);
		return false;
	}

	if(!OrderOperateAccount(pctxdata))
	{
		ErrorCheck(0);
		return false;
	}

	if(!RecordOrderLottoStatus(pctxdata))
	{
		ErrorCheck(0);
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

static bool IsHaveOpenTerm(void)
{
	SYSLOTTO_STATUS sys;
	u32 term = 0x7fffffff;
	bool flag = false;
	uchar kk = 0;
	u32 hight = GetCurRunEnvHeight();

	if(ReadDataValueDB("SysStatus", sizeof("SysStatus"), &sys, sizeof(SYSLOTTO_STATUS)))
	{
		ErrorCheck(0);
		return false;
	}

	for(uchar ii = 0; ii < 5; ii++)
	{
		if(sys.status[ii] == LOTTOUNOPEN)
		{
			if(term > sys.term[ii])
			{
				term = sys.term[ii];
				flag = true;
				kk = ii;
			}
		}
	}

	if(flag)
	{
		if(hight >= sys.openhight[kk])
		{
			return true;
		}
	}

	return flag;
}

/**
 * @brief open data check
 * 1.check the reg lottery tx's contact
 * 2.check the block hight,whether it is higher than the open hight
 * 3.check whether the lottery is opened
 * @param pdata:open data handle
 * @return
 */
static bool OpenDataCheck(const LOTTO_CTX *pctxdata)
{
	if(pctxdata == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(!IsHaveOpenTerm())
	{
		ErrorCheck(0);
		return false;
	}

	return true;
}

/**
 * @brief record open lottery status in the script database
 * @param pdata:open data handle
 * @return
 */
static bool RecordOpenStatus(const LOTTO_CTX *pctxdata)
{
	SYSLOTTO_STATUS sys;
	if(pctxdata == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(ReadDataValueDB("SysStatus", sizeof("SysStatus"), &sys, sizeof(SYSLOTTO_STATUS)))
	{
		ErrorCheck(0);
		return false;
	}

	sys.status[0] = LOTTOOPEN;

	if(ModifyDataDBVavle("SysStatus", sizeof("SysStatus"), &sys, sizeof(SYSLOTTO_STATUS)))
	{
		ErrorCheck(0);
		return false;
	}

	return true;
}

static bool OpenOperateAccount(const LOTTO_CTX *pbetctx)
{
	VM_OPERATE operate[2];
	SYSLOTTO_STATUS sys;
	uchar key[8] = {0};
	uchar txhash[32] = {0}, openhash[32] = {0};
	uchar accid[6] = {0};
	ORDER_INFO info;

	if(pbetctx == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(ReadDataValueDB("SysStatus", sizeof("SysStatus"), &sys, sizeof(SYSLOTTO_STATUS)))
	{
		ErrorCheck(0);
		return false;
	}

	for(u32 ii = 0; ii < sys.totalorders[0]; ii++)
	{
		memcpy(key, &sys.term[0], sizeof(u32));
		memcpy(&key[4], &ii, sizeof(u32));
		if(ReadDataValueDB(key, sizeof(key), &txhash, sizeof(txhash)))
		{
			ErrorCheck(0);
			return false;
		}

		//get info from tx hash
		GetAccounts(txhash, accid, sizeof(accid));
		if(!GetTxContacts(txhash, gContractData, sizeof(gContractData)))
		{
			ErrorCheck(0);
			return false;
		}

		if(!GetBlockHash(sys.openhight[0], openhash))
		{
			ErrorCheck(0);
			return false;
		}

		ORDER_DATA *pregdata = (ORDER_DATA *)gContractData;

		memcpy(info.accid, accid, sizeof(accid));
		memcpy(&info.amount, &pregdata->money, sizeof(pregdata->money));
		memcpy(info.num, pregdata->num, sizeof(pregdata->num));
		info.numlen = pregdata->numlen;

		REWARD_RESULT rslt = DrawLottery(openhash, info.num, info.numlen, &info.amount);

		{
			memcpy(operate[0].accountid, pbetctx->accid, sizeof(pbetctx->accid));
			u32 temp = rslt.top1 + rslt.top2 + rslt.top3;
			memcpy(&operate[0].money, &temp, sizeof(temp));
			operate[0].opeatortype = ADD_FREE;
			operate[0].outheight = 0;//???

			memcpy(operate[1].accountid, pbetctx->scriptid, sizeof(pbetctx->scriptid));
			memcpy(&operate[1].money, &temp, sizeof(temp));
			operate[1].opeatortype = MINUS_FREE;
			operate[1].outheight = 0;//???

			if(!WriteOutput(operate, 2))
			{
				ErrorCheck(0);
				return false;
			}
		}
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
static bool OpenLottery(const LOTTO_CTX *pctxdata)
{
	if(pctxdata == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(!OpenDataCheck(pctxdata))
	{
		ErrorCheck(0);
		return false;
	}

	if(!OpenOperateAccount(pctxdata))
	{
		ErrorCheck(0);
		return false;
	}

	if(!RecordOpenStatus(pctxdata))
	{
		ErrorCheck(0);
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
		ErrorCheck(0);
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
		ret = OpenLottery(pctxdata);
		break;

	default:
		ErrorCheck(0);
		ret = false;
		break;
	}

	return ret;
}

static bool InitCtxData(LOTTO_CTX const *pctxdata)
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

	if(GetAccounts(pctxdata->txhash, (void *)pctxdata->accid, sizeof(pctxdata->accid)) == 0)
	{
		ErrorCheck(0);
		return false;
	}

	if(!GetCurScritpAccount((void *)pctxdata->scriptid))
	{
		ErrorCheck(0);
		return false;
	}

	return true;
}

int main()
{
	__xdata __no_init static LOTTO_CTX LottoCtx;

	if(!InitCtxData(&LottoCtx))
	{
		ErrorCheck(0);
		return 0;
	}

	if(GetContractData(&LottoCtx))
	{
		ErrorCheck(0);
		return 0;
	}

	if(RunContractData(&LottoCtx))
	{
		ErrorCheck(0);
		return 0;
	}


	return 1;
}















