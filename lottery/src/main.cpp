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
	REG = 0,  //!< REG
	ORDER,//!< ORDER
	OPEN  //!< OPEN
};

enum LOTTOSTATUS {
	LOTTONONE,
	LOTTONEW,
	LOTTOUNOPEN,
	LOTTOOPEN
};

typedef struct {
	unsigned char type;
	unsigned char hash[32];
	unsigned char num[15];
	unsigned char numlen;
}TEST_DATA;

/**
 *reg lottery data struct
 */
typedef struct {
	uchar type;
	Int64 money;
	u32 hight;
}REG_DATA;

/**
 * order lottery data struct
 */
typedef struct {
	uchar type;
	uchar num[15];
	uchar numlen;
	Int64 money;//单注价格
}ORDER_DATA;

/**
 * open lottery data struct
 */
typedef struct {
	uchar type;
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
	Int64 amount;//单注价格
	uchar num[15];
	uchar numlen;
	u32 orderhight;
}ORDER_INFO;

////用一个关键字记录所有记录期数的状态，便于查找是否有开奖期数
////只保存5期的记录
////key:SysStatus
//typedef struct {
//	LOTTOSTATUS status[5];//0-no record;1-unopen;2-opened
//	u32 term[5];//期数
//	u32 openhight[5];//对应该期的开奖高度
//	u32 totalorders[5];//对应总注数
//}SYSLOTTO_STATUS;

//用另一个关键字记录当前期的投注状态
//key:CurretStatus
typedef struct {
	u32 term;//第几期
	Int64 amount;//奖池余额
	u32 totalorders;//当前投注量
	u32 openhight;//开奖高度
	LOTTOSTATUS status;//开奖状态
}CURRENT_STATUS;

#define CURRENT "CurretStatus"
#define SYSTERM "SysStatus"

#define REWARD_NO1 3000
#define REWARD_NO2 17
#define REWARD_NO3 2
//5000 000 000->0x01 2A 05 F2 00<-->对应C51中数据->0x00 f2 05 2a 01 00 00 00

#define ORDER_PRICE    ("\xA0\x86\x01\x00\x00\x00\x00\x00")//100000
#define REG_MIN_AMOUNT ("\x00\xf2\x05\x2a\x01\x00\x00\x00")//50 00 000 000

__xdata __no_init static uchar gContractData[256];
__xdata __no_init static REG_SAVE_DATA gRegSaveData;
__xdata __no_init static CURRENT_STATUS gCurrentStatus;

static bool Uint64ToU32(const Int64 * p64, u32 * p32)
{
	memcpy(p32, p64, sizeof(u32));
	return true;
}

static bool U32ToUint64(const u32 * p32, Int64 * p64)
{
	memcpy(p64, p32, sizeof(u32));
	return true;
}

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

	if(GetCurTxContact(gContractData, sizeof(gContractData)) == 0)
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

//	Int64Inital(&minamount, REG_MIN_AMOUNT, sizeof(REG_MIN_AMOUNT));
	memcpy(&minamount, REG_MIN_AMOUNT, sizeof(minamount));

	if(Int64Compare(pdata, &minamount) == COMP_LESS)
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
	u32 xx = GetCurRunEnvHeight();
	LogPrint(phight, sizeof(u32), HEX);
	LogPrint(&xx, sizeof(xx), HEX);

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
	Int64 balance;
	REG_DATA *pregdata = (REG_DATA *)gContractData;
//	{
//		LogPrint("hello", sizeof("hello"), STRING);
//		LogPrint(&pregdata->type, sizeof(pregdata->type), HEX);
//		LogPrint(&pregdata->money, sizeof(pregdata->money), HEX);
//		short offset = (short)((&((REG_DATA *)0)->hight));
//		LogPrint(&offset, sizeof(offset), HEX);
//		LogPrint(&pregdata->hight, sizeof(pregdata->hight), HEX);
//	}

	if(pctxdata == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(!QueryAccountBalance(pctxdata->accid, &balance))
	{
		ErrorCheck(0);
		return false;
	}
//
//	LogPrint(&balance, sizeof(balance), HEX);
//	LogPrint(&pregdata->money, sizeof(pregdata->money), HEX);

	if(Int64Compare(&balance, &pregdata->money) != COMP_LARGER)
	{
		ErrorCheck(0);
		return false;
	}
//	LogPrint(pregdata, sizeof(gContractData), HEX);
//	LogPrint("helloSHIT", sizeof("helloSHIT"), STRING);
//	LogPrint(&pregdata->money, sizeof(pregdata->money), HEX);
	if(!CheckMinimumAmount(&pregdata->money))
	{
		ErrorCheck(0);
		return false;
	}

//	pregdata->hight = 100;
//	LogPrint(pregdata, sizeof(gContractData), HEX);
//	LogPrint(&pregdata->hight, sizeof(u32), HEX);
//	short offset = (char)(&((REG_DATA *)0)->hight);
//	LogPrint(&offset, sizeof(offset), HEX);
//	LogPrint(&offset, sizeof(offset), HEX);
//	LogPrint(&pregdata->hight, sizeof(pregdata->hight), HEX);
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

	memcpy(operate[1].accountid, pbetctx->accid, sizeof(pbetctx->accid));
	memcpy(&operate[1].money, &pregdata->money, sizeof(pregdata->money));
	operate[1].opeatortype = ADD_FREEZD;
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
	if(!WriteDataDB(SYSTERM, sizeof(SYSTERM), &savedata, sizeof(REG_SAVE_DATA), 12345))
	{
		ErrorCheck(0);
		return false;
	}

	memset(&current, 0, sizeof(CURRENT_STATUS));
	memcpy(&current.amount, &pregdata->money, sizeof(pregdata->money));
	current.term = 1;
	current.openhight = savedata.reghight + current.term*10;//注册后10个高度开之前的奖
	current.totalorders = 0;
	current.status = LOTTOUNOPEN;
	if(!WriteDataDB(CURRENT, sizeof(CURRENT), &current, sizeof(CURRENT_STATUS), 12345))
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

static bool CheckOrderAmount(const Int64 *pamount)
{
	Int64 min;
	u32 total = 0;
	if(pamount == NULL)
	{
		ErrorCheck(0);
		return false;
	}



//	total = MselectN(totalnum, 6);
//	memset(&total64, 0, sizeof(Int64));
//	memcpy(&total64, &total, sizeof(total));
//	if(!Int64Div(pamount, &total64, &apert))
//	{
//		ErrorCheck(0);
//		return false;
//	}

//	LogPrint(pamount, sizeof(total64), HEX);
//	LogPrint(&total64, sizeof(total64), HEX);
//	LogPrint(&apert, sizeof(apert), HEX);

//	Int64Inital(&min, ORDER_PRICE, sizeof(ORDER_PRICE));
	memcpy(&min, ORDER_PRICE, sizeof(min));

	if(Int64Compare(pamount, &min) != COMP_EQU)
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
//	LogPrint("CheckOrderNum", sizeof("CheckOrderNum"), STRING);
	if(!CheckOrderNum(porderdata->num, porderdata->numlen))
	{
		ErrorCheck(0);
		return false;
	}
//	LogPrint("CheckOrderAmount", sizeof("CheckOrderAmount"), STRING);
	if(!CheckOrderAmount(&porderdata->money))
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
//	CURRENT_STATUS current;
//
//	if(!ReadDataValueDB("CurretStatus", sizeof("CurretStatus"), &current, sizeof(CURRENT_STATUS)))
//	{
//		ErrorCheck(0);
//		return false;
//	}

	//判断是否到期，如果到期没人开奖，则投注失败。直到有人开奖才可以投注
	if(gCurrentStatus.openhight < GetCurRunEnvHeight())
	{
		ErrorCheck(0);
		return false;
	}


	if(GetCurRunEnvHeight() > gRegSaveData.closehight)
	{
		ErrorCheck(0);
		return false;
	}

	if(gCurrentStatus.openhight > gRegSaveData.closehight)//最后一期的彩票开奖前就解冻，则拒绝投注该期
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
	Int64 balance, orderamount;

	if(pctxdata == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(!CheckOrderNumAndAmount())
	{
		ErrorCheck(0);
		return false;
	}

	if(!QueryAccountBalance(pctxdata->accid, &balance))
	{
		ErrorCheck(0);
		return false;
	}

	{
		Int64 total64;
		u32 total = MselectN(porderdata->numlen, 6);
		memset(&total64, 0, sizeof(total64));
		memcpy(&total64, &total, sizeof(total));
		if(!Int64Mul(&porderdata->money, &total64, &orderamount))
		{
			ErrorCheck(0);
			return false;
		}
	}

	if(Int64Compare(&balance, &orderamount) != COMP_LARGER)
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
	Int64 amount;

	if(pbetctx == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	{
		Int64 tmp64;
		u32 total = MselectN(pregdata->numlen, 6);
		memset(&tmp64, 0, sizeof(tmp64));
		memcpy(&tmp64, &total, sizeof(total));
		if(!Int64Mul(&pregdata->money, &tmp64, &amount))
		{
			ErrorCheck(0);
			return false;
		}
//		LogPrint(&pregdata->numlen, sizeof(pregdata->numlen), HEX);
//		LogPrint(&total, sizeof(total), HEX);
//		LogPrint(&tmp64, sizeof(tmp64), HEX);
//		LogPrint(&pregdata->money, sizeof(pregdata->money), HEX);
//		LogPrint(&amount, sizeof(amount), HEX);
	}

	memcpy(operate[0].accountid, pbetctx->accid, sizeof(pbetctx->accid));
	memcpy(&operate[0].money, &amount, sizeof(amount));
	operate[0].opeatortype = MINUS_FREE;
	operate[0].outheight = 0x7fffffff;//???

	memcpy(operate[1].accountid, gRegSaveData.accid, sizeof(gRegSaveData.accid));
	memcpy(&operate[1].money, &amount, sizeof(amount));
	operate[1].opeatortype = ADD_FREEZD;
	operate[1].outheight = gRegSaveData.closehight;//???

    return WriteOutput(operate, 2);
}

/**
 * @brief record account order status in the script database
 * @param pdata:order data hanlde
 * @return
 */
static bool RecordOrderLottoStatus(const LOTTO_CTX *pctxdata)
{
//	CURRENT_STATUS current;
	uchar key[8] = {0};
	ORDER_DATA *pregdata = (ORDER_DATA *)gContractData;
	if(pctxdata == NULL)
	{
		ErrorCheck(0);
		return false;
	}

//	if(!ReadDataValueDB("CurretStatus", sizeof("CurretStatus"), &current, sizeof(CURRENT_STATUS)))
//	{
//		ErrorCheck(0);
//		return false;
//	}

//	//判断是否到期，如果到期没人开奖，则投注失败。直到有人开奖才可以投注
//	if(current.openhight > GetCurRunEnvHeight() && current.status == LOTTOUNOPEN)
//	{
//
//	}
//	current.status = (current.status == LOTTONEW)?(LOTTOUNOPEN):();


	memcpy(key, &gCurrentStatus.term, sizeof(u32));
	memcpy(&key[4], &gCurrentStatus.totalorders, sizeof(u32));
	if(!WriteDataDB(key, sizeof(key), pctxdata->txhash, sizeof(pctxdata->txhash), 12345))
	{
		ErrorCheck(0);
		return false;
	}

	gCurrentStatus.totalorders++;
	if(!Int64Add(&pregdata->money, &gCurrentStatus.amount, &gCurrentStatus.amount))
	{
		ErrorCheck(0);
		return false;
	}

	if(!ModifyDataDBVavle(CURRENT, sizeof(CURRENT), &gCurrentStatus, sizeof(CURRENT_STATUS)))
	{
		ErrorCheck(0);
		return false;
	}


	return true;
}

static bool ReadGlobalPara(void)
{
	if(!ReadDataValueDB(CURRENT, sizeof(CURRENT), &gCurrentStatus, sizeof(CURRENT_STATUS)))
	{
		ErrorCheck(0);
		return false;
	}

	if(!ReadDataValueDB(SYSTERM, sizeof(SYSTERM), &gRegSaveData, sizeof(REG_SAVE_DATA)))
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

	if (!ReadGlobalPara()) {
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
//	SYSLOTTO_STATUS sys;
//	u32 term = 0x7fffffff;
//	bool flag = false;
//	uchar kk = 0;
//	u32 hight = GetCurRunEnvHeight();
//	CURRENT_STATUS current;
//
//	if(!ReadDataValueDB("CurretStatus", sizeof("CurretStatus"), &current, sizeof(CURRENT_STATUS)))
//	{
//		ErrorCheck(0);
//		return false;
//	}

	//判断是否到期，如果到期没人开奖，则投注失败。直到有人开奖才可以投注
	if(gCurrentStatus.openhight > GetCurRunEnvHeight())
	{
		ErrorCheck(0);
		return false;
	}

	return true;

//	if(!ReadDataValueDB("SysStatus", sizeof("SysStatus"), &sys, sizeof(SYSLOTTO_STATUS)))
//	{
//		ErrorCheck(0);
//		return false;
//	}
//
//	for(uchar ii = 0; ii < 5; ii++)
//	{
//		if(sys.status[ii] == LOTTOUNOPEN)
//		{
//			if(term > sys.term[ii])
//			{
//				term = sys.term[ii];
//				flag = true;
//				kk = ii;
//			}
//		}
//	}
//
//	if(flag)
//	{
//		if(hight >= sys.openhight[kk])
//		{
//			return true;
//		}
//	}

//	return flag;
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
//	SYSLOTTO_STATUS sys;
	if(pctxdata == NULL)
	{
		ErrorCheck(0);
		return false;
	}

//	if(!ReadDataValueDB("SysStatus", sizeof("SysStatus"), &sys, sizeof(SYSLOTTO_STATUS)))
//	{
//		ErrorCheck(0);
//		return false;
//	}
//
//	sys.status[0] = LOTTOOPEN;
//
//	if(!ModifyDataDBVavle("SysStatus", sizeof("SysStatus"), &sys, sizeof(SYSLOTTO_STATUS)))
//	{
//		ErrorCheck(0);
//		return false;
//	}

	return true;
}

static Int64 GetRewardForOpen(void)
{
	Int64 reward;
	memset(&reward, 0, sizeof(reward));
	return reward;
}

static bool OpenOperateAccount(const LOTTO_CTX *pbetctx)
{
	VM_OPERATE operate[2];
//	SYSLOTTO_STATUS sys;
	uchar key[8] = {0};
	uchar txhash[32] = {0}, openhash[32] = {0};
	uchar accid[6] = {0};
	ORDER_INFO info;
	uchar luckynum[LUCKYNUMSIZE] = {0};
	Int64 treward, orderprice, top1_80;

	u32 nttop1 = 0, nttop2 = 0, nttop3 = 0, utop1_80 = 0;

	if(pbetctx == NULL)
	{
		ErrorCheck(0);
		return false;
	}

//	Int64Inital(&orderprice, ORDER_PRICE, sizeof(ORDER_PRICE));
	memcpy(&orderprice, ORDER_PRICE, sizeof(orderprice));

//	CURRENT_STATUS current;
//
//	if(!ReadDataValueDB("CurretStatus", sizeof("CurretStatus"), &current, sizeof(CURRENT_STATUS)))
//	{
//		ErrorCheck(0);
//		return false;
//	}

	if(!GetBlockHash(gCurrentStatus.openhight, openhash))
	{
		ErrorCheck(0);
		return false;
	}

	if(!GetLuckyNum(openhash, luckynum))
	{
		ErrorCheck(0);
		return false;
	}

	{
		for (u32 ii = 0; ii < gCurrentStatus.totalorders; ii++) {
			memcpy(key, &gCurrentStatus.term, sizeof(u32));
			memcpy(&key[4], &ii, sizeof(u32));
			if (!ReadDataValueDB(key, sizeof(key), &txhash, sizeof(txhash))) {
				ErrorCheck(0);
				return false;
			}

			if (GetAccounts(txhash, accid, sizeof(accid)) == 0) {
				ErrorCheck(0);
				return false;
			}

			if (!GetTxContacts(txhash, gContractData, sizeof(gContractData))) {
				ErrorCheck(0);
				return false;
			}

			ORDER_DATA *pregdata = (ORDER_DATA *) gContractData;

			memcpy(info.accid, accid, sizeof(accid));
			memcpy(&info.amount, &pregdata->money, sizeof(pregdata->money));
			memcpy(info.num, pregdata->num, sizeof(pregdata->num));
			info.numlen = pregdata->numlen;

			REWARD_RESULT rslt = DrawLottery(luckynum, info.num, info.numlen);
			{
				LogPrint(info.accid, sizeof(info.accid), HEX);
				LogPrint(&rslt.top1, sizeof(u32), HEX);
				LogPrint(&rslt.top2, sizeof(u32), HEX);
				LogPrint(&rslt.top3, sizeof(u32), HEX);
			}

			if (rslt.top1 == 0 && rslt.top2 == 0 && rslt.top3 == 0) {
				continue;
			} else {
				nttop1 += rslt.top1;
				nttop2 += rslt.top2;
				nttop3 += rslt.top3;
			}

//			{
//				Int64 tt64, price;
//				u32 total = MselectN(info.numlen, 6);
//				memset(&tt64, 0, sizeof(Int64));
//				memcpy(&tt64, &total, sizeof(total));
//				if (!Int64Div(&info.amount, &tt64, &price)) {
//					ErrorCheck(0);
//					return false;
//				}
//				{
//					LogPrint(&total, sizeof(u32), HEX);
//					LogPrint(&price, sizeof(price), HEX);
//				}
//
//				if (!Int64Mul(&ntop1, &price, &tamount)) {
//					ErrorCheck(0);
//					return false;
//				}
//				{
//					LogPrint(&tamount, sizeof(tamount), HEX);
//				}
//
//				if(!Int64Add(&tamount, &treward, &treward))
//				{
//					ErrorCheck(0);
//					return false;
//				}
		}
	}

	{
		Int64 tmpreward, tmpamount, xreward, xamount, tmp64;
		u32 tmp = nttop1*REWARD_NO1 + nttop2*REWARD_NO2 + nttop3*REWARD_NO3;
		memset(&tmp64, 0, sizeof(tmp64));
		memcpy(&tmp64, &tmp, sizeof(u32));

		if(!Int64Mul(&tmp64, &orderprice, &treward))
		{
			ErrorCheck(0);
			return false;
		}

//		Int64Inital(&xreward,"\x0a", 1);
//		Int64Inital(&xamount,"\x08", 1);
		memcpy(&xreward,"\x0a\x00\x00\x00\x00\x00\x00\x00", 8);
		memcpy(&xamount,"\x08\x00\x00\x00\x00\x00\x00\x00", 8);

		if(!Int64Mul(&gCurrentStatus.amount, &xamount, &tmpamount))
		{
			ErrorCheck(0);
			return false;
		}

		if(!Int64Mul(&treward, &xreward, &tmpreward))
		{
			ErrorCheck(0);
			return false;
		}

		if(Int64Compare(&tmpamount, &tmpreward) != COMP_LARGER)//是否开奖金额大于奖池余额：分发80%奖池金额
		{
			Int64 actreward, tmp23, amount23, amount1, top1_64;
			u32 utmp23;
			if(!Int64Div(&tmpamount, &xreward, &actreward))//得到80%的奖池金额
			{
				ErrorCheck(0);
				return false;
			}

			utmp23 = nttop2*REWARD_NO2 + nttop3*REWARD_NO3;

			memset(&tmp23, 0, sizeof(tmp23));
			memcpy(&tmp23, &utmp23, sizeof(u32));

			if(!Int64Mul(&tmp23, &orderprice, &amount23))
			{
				ErrorCheck(0);
				return false;
			}

			if(!Int64Sub(&actreward, &amount23, &amount1))
			{
				ErrorCheck(0);
				return false;
			}

			memset(&top1_64, 0, sizeof(top1_64));
			memcpy(&top1_64, &nttop1, sizeof(nttop1));

			if(!Int64Div(&amount1, &top1_64, &top1_80))//得到一等奖的倍数
			{
				ErrorCheck(0);
				return false;
			}

			memcpy(&utop1_80, &top1_80, sizeof(u32));

			if(utop1_80 > REWARD_NO1)//校验得到的结果
			{
				ErrorCheck(0);
				return false;
			}
		}
	}

#if 1
	for(u32 ii = 0; ii < gCurrentStatus.totalorders; ii++)
	{
		memcpy(key, &gCurrentStatus.term, sizeof(u32));
		memcpy(&key[4], &ii, sizeof(u32));
//		LogPrint(key, sizeof(key), HEX);
		if(!ReadDataValueDB(key, sizeof(key), &txhash, sizeof(txhash)))
		{
			ErrorCheck(0);
			return false;
		}
//		LogPrint(txhash, sizeof(txhash), HEX);
		//get info from tx hash
		GetAccounts(txhash, accid, sizeof(accid));
//		LogPrint(accid, sizeof(accid), HEX);
		if(!GetTxContacts(txhash, gContractData, sizeof(gContractData)))
		{
			ErrorCheck(0);
			return false;
		}
//		LogPrint(&current.openhight, sizeof(current.openhight), HEX);
//		if(!GetBlockHash(gCurrentStatus.openhight, openhash))
//		{
//			ErrorCheck(0);
//			return false;
//		}

		ORDER_DATA *pregdata = (ORDER_DATA *)gContractData;

		memcpy(info.accid, accid, sizeof(accid));
		memcpy(&info.amount, &pregdata->money, sizeof(pregdata->money));
		memcpy(info.num, pregdata->num, sizeof(pregdata->num));
		info.numlen = pregdata->numlen;

		REWARD_RESULT rslt = DrawLottery(luckynum, info.num, info.numlen);
		{
			LogPrint(info.accid, sizeof(info.accid), HEX);
			LogPrint(&rslt.top1, sizeof(u32), HEX);
			LogPrint(&rslt.top2, sizeof(u32), HEX);
			LogPrint(&rslt.top3, sizeof(u32), HEX);
		}


		Int64 ntop1, ntop2, ntop3, tamount;

		if (rslt.top1 == 0 && rslt.top2 == 0 && rslt.top3 == 0) {
			continue;
		}
		else
		{
			u32 ttop = rslt.top1*((utop1_80 != 0)?(utop1_80):(REWARD_NO1)) + rslt.top2*REWARD_NO2 + rslt.top3*REWARD_NO3;
			memset(&ntop1, 0, sizeof(ntop1));
			memcpy(&ntop1, &ttop, sizeof(ttop));
		}

		{
//			Int64 tt64, price;
//		 	u32 total = MselectN(info.numlen, 6);
//			memset(&tt64, 0, sizeof(Int64));
//			memcpy(&tt64, &total, sizeof(total));
//			if (!Int64Div(&info.amount, &tt64, &price)) {
//				ErrorCheck(0);
//				return false;
//			}
//			{
//				LogPrint(&total, sizeof(u32), HEX);
//				LogPrint(&price, sizeof(price), HEX);
//			}

			if (!Int64Mul(&ntop1, &orderprice, &tamount)) {
				ErrorCheck(0);
				return false;
			}

//			if (!Int64Mul(&ntop2, &price, &ntop2)) {
//				ErrorCheck(0);
//				return false;
//			}
//
//			if (!Int64Mul(&ntop3, &price, &ntop3)) {
//				ErrorCheck(0);
//				return false;
//			}
//
//			if(!Int64Add(&tamount, &ntop1, &tamount))
//			{
//				ErrorCheck(0);
//				return false;
//			}
//
//			if (!Int64Add(&tamount, &ntop2, &tamount)) {
//				ErrorCheck(0);
//				return false;
//			}
//
//			if (!Int64Add(&tamount, &ntop3, &tamount)) {
//				ErrorCheck(0);
//				return false;
//			}

			{
				LogPrint(&tamount, sizeof(tamount), HEX);
			}

			memcpy(operate[0].accountid, info.accid, sizeof(info.accid));
			memcpy(&operate[0].money, &tamount, sizeof(tamount));
			operate[0].opeatortype = ADD_FREE;
			operate[0].outheight = 0;//???

			memcpy(operate[1].accountid, gRegSaveData.accid, sizeof(gRegSaveData.accid));
			memcpy(&operate[1].money, &tamount, sizeof(tamount));
			operate[1].opeatortype = MINUS_FREEZD;
			operate[1].outheight = gRegSaveData.closehight;//???

			if(!WriteOutput(operate, 2))
			{
				ErrorCheck(0);
				return false;
			}
		}
	}
	//增加对开奖账户的奖励金额，目前奖励金额为零
	{
		Int64 openreward = GetRewardForOpen();
		memcpy(operate[0].accountid, pbetctx->accid, sizeof(pbetctx->accid));
		memcpy(&operate[0].money, &openreward, sizeof(openreward));
		operate[0].opeatortype = ADD_FREE;
		operate[0].outheight = 0;//???

		memcpy(operate[1].accountid, gRegSaveData.accid, sizeof(gRegSaveData.accid));
		memcpy(&operate[1].money, &openreward, sizeof(openreward));
		operate[1].opeatortype = MINUS_FREEZD;
		operate[1].outheight = gRegSaveData.closehight;//???

		if(!WriteOutput(operate, 2))
		{
			ErrorCheck(0);
			return false;
		}
	}
#endif

	{
//		REG_SAVE_DATA savedata;
//		if(!ReadDataValueDB("SysRegLotto", sizeof("SysRegLotto"), &savedata, sizeof(REG_SAVE_DATA)))
//		{
//			ErrorCheck(0);
//			return false;
//		}

		gCurrentStatus.term++;
		gCurrentStatus.openhight = gRegSaveData.reghight + gCurrentStatus.term*10;
		gCurrentStatus.status = LOTTONEW;
		gCurrentStatus.totalorders = 0;
		if(!Int64Sub(&gCurrentStatus.amount, &treward, &gCurrentStatus.amount))
		{
			ErrorCheck(0);
			return false;
		}

		if(!ModifyDataDBVavle(CURRENT, sizeof(CURRENT), &gCurrentStatus, sizeof(CURRENT_STATUS)))
		{
			ErrorCheck(0);
			return false;
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

//	if (!ReadDataValueDB("SysRegLotto", sizeof("SysRegLotto"), &gRegSaveData, sizeof(REG_SAVE_DATA))) {
//		ErrorCheck(0);
//		return false;
//	}
	if (!ReadGlobalPara()) {
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
//		TEST_DATA *phandletest = (TEST_DATA *)gContractData;
//
//		REWARD_RESULT rslt = DrawLottery(phandletest->hash, phandletest->num, phandletest->numlen);
//		{
//			LogPrint(&rslt.top1, sizeof(rslt.top1), HEX);
//			LogPrint(&rslt.top2, sizeof(rslt.top2), HEX);
//			LogPrint(&rslt.top3, sizeof(rslt.top3), HEX);
//		}
//		ret = true;
//		break;
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

	if(GetCurTxAccount((void *)pctxdata->accid, sizeof(pctxdata->accid)) == 0)
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
		__VmExit(RUN_SCRIPT_DATA_ERR);
	}

	if(!GetContractData(&LottoCtx))
	{
		ErrorCheck(0);
		__VmExit(RUN_SCRIPT_DATA_ERR);
	}

#if 1
	{
		LogPrint(LottoCtx.txhash, sizeof(LottoCtx.txhash), HEX);
		LogPrint(LottoCtx.accid, sizeof(LottoCtx.accid), HEX);
		LogPrint(LottoCtx.scriptid, sizeof(LottoCtx.scriptid), HEX);
		LogPrint(gContractData, sizeof(gContractData), HEX);

		REG_DATA *phandle1 = (REG_DATA *)gContractData;
		LogPrint(&phandle1->type, sizeof(phandle1->type), HEX);
		LogPrint(&phandle1->money, sizeof(phandle1->money), HEX);
		LogPrint(&phandle1->hight, sizeof(phandle1->hight), HEX);

		ORDER_DATA *phandle2 = (ORDER_DATA *)gContractData;
		LogPrint(&phandle2->type, sizeof(phandle2->type), HEX);
		LogPrint(&phandle2->money, sizeof(phandle2->money), HEX);
		LogPrint(&phandle2->num, sizeof(phandle2->num), HEX);
		LogPrint(&phandle2->numlen, sizeof(phandle2->numlen), HEX);

		OPEN_DATA *phandle3 = (OPEN_DATA *)gContractData;
		LogPrint(&phandle3->type, sizeof(phandle3->type), HEX);
	}
#else

//	{
//		TEST_DATA *phandletest = (TEST_DATA *)gContractData;
//		LogPrint(&phandletest->type, sizeof(phandletest->type), HEX);
//		LogPrint(&phandletest->hash, sizeof(phandletest->hash), HEX);
//		LogPrint(&phandletest->num, sizeof(phandletest->num), HEX);
//		LogPrint(&phandletest->numlen, sizeof(phandletest->numlen), HEX);
//	}

#endif


	if(!RunContractData(&LottoCtx))
	{
		ErrorCheck(0);
		__VmExit(RUN_SCRIPT_DATA_ERR);
	}

	__VmExit(RUN_SCRIPT_OK);
	return 1;
}













