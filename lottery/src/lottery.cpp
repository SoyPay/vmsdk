/*
 * lottery.cpp
 *
 *  Created on: Nov 17, 2014
 *      Author: spark.huang
 */
#include <string.h>
#include<stdlib.h>
#include"lottery.h"
#include<assert.h>

/**
 * @todo  建议 ： 把 u32 改成 u8 和 u16 减少空间
 */
typedef struct
{
 u32 m;
 u32 n;
 u32 result;
}MAP_M6;

__xdata static MAP_M6 ms6map[] = {
		{6,  6, 1},
		{7,  6, 7},
		{8,  6, 28},
		{9,  6, 84},
		{10, 6, 210},
		{11, 6, 462},
		{12, 6, 924},
		{13, 6, 1716},
		{14, 6, 3003},
		{15, 6, 5005},
		{5,  5, 1},
		{6,  5, 6},
		{7,  5, 21},
		{8,  5, 56},
		{9,  5, 126},
		{10, 5, 252},
		{11, 5, 462},
		{12, 5, 792},
		{13, 5, 1287},
		{14, 5, 2002},
		{15, 5, 3003},
		{4,  4, 1},
		{5,  4, 5},
		{6,  4, 15},
		{7,  4, 35},
		{8,  4, 70},
		{9,  4, 126},
		{10, 4, 210},
		{11, 4, 330},
		{12, 4, 495},
		{13, 4, 715},
		{14, 4, 1001},
		{15, 4, 1365},
		{3,  3, 1},
		{4,  3, 4},
		{5,  3, 10},
		{6,  3, 20},
		{7,  3, 35},
		{8,  3, 56},
		{9,  3, 84},
		{10, 3, 120},
		{11, 3, 165},
		{12, 3, 220},
		{13, 3, 286},
		{14, 3, 364},
		{15, 3, 455},
		{2,  2, 1},
		{3,  2, 3},
		{4,  2, 6},
		{5,  2, 10},
		{6,  2, 15},
		{7,  2, 21},
		{8,  2, 28},
		{9,  2, 36},
		{10, 2, 45},
		{11, 2, 55},
		{12, 2, 66},
		{13, 2, 78},
		{14, 2, 91},
		{15, 2, 105},
		{1,  1, 1},
		{2,  1, 2},
		{3,  1, 3},
		{4,  1, 4},
		{5,  1, 5},
		{6,  1, 6},
		{7,  1, 7},
		{8,  1, 8},
		{9,  1, 9},
		{10, 1, 10},
		{11, 1, 11},
		{12, 1, 12},
		{13, 1, 13},
		{14, 1, 14},
		{15, 1, 15},
};

static u32 factorial(u32 n) {
	if (n <= 1)
		return 1;
	else
		return n * factorial(n - 1);
}

u32 MselectN(u32 m, u32 n) {
//	LogPrint(&m, sizeof(m), HEX);
//	LogPrint(&n, sizeof(n), HEX);
	if(m < n)
	{
		return 0;
	}
	int size = sizeof(ms6map)/sizeof(MAP_M6);
//	LogPrint(&size, sizeof(size), HEX);
	for(int ii = 0; ii < size; ii++)
	{
		if(ms6map[ii].m == m && ms6map[ii].n == n)
		{
			return ms6map[ii].result;
		}
	}

	return 0;
}

static u32 GetTotalTicket(uchar selectsize)
{
	if(selectsize < LUCKYNUMSIZE || selectsize > TOTALNUMSIZE)
	{
		ErrorCheck(0);
		return 0;
	}
	return MselectN(selectsize, LUCKYNUMSIZE);
}

//static Int64 GetPricePerTick(uchar selectsize, const Int64 *pamount)
//{
//	if(selectsize < LUCKYNUMSIZE || selectsize > TOTALNUMSIZE || pamount == 0)
//	{
//		ErrorCheck(0);
//		return 0;
//	}
//
//	u32 tt = GetTotalTicket(selectsize);
//	Int64 price;
//	Int64 total;
//	memset(&price, 0, sizeof(price));
//	memset(&total, 0, sizeof(total));
//	memcpy(&total, &tt, sizeof(tt));
//	if(!Int64Div(pamount, &total, &price))
//	{
//		Int64 tmp;
//		memset(&tmp, 0, sizeof(tmp));
//		ErrorCheck(0);
//		return tmp;
//	}
//	return price;
//}

static REWARD_RESULT GetRewardTicket(const uchar *pdata, uchar datalen, const uchar luckynum[LUCKYNUMSIZE])
{
	REWARD_RESULT result;
	uchar luckycount = 0;

	result.top1 = 0;
	result.top2 = 0;
	result.top3 = 0;

	if(pdata == NULL || datalen < LUCKYNUMSIZE || datalen > TOTALNUMSIZE || luckynum == NULL)
	{
		ErrorCheck(0);
		return result;
	}

	for(uchar ii = 0; ii < datalen; ii++)
	{
		for(uchar mm = 0; mm < LUCKYNUMSIZE; mm++)
		{
			if(pdata[ii] == luckynum[mm])
			{
				luckycount++;
			}
		}
	}

	result.top1 = MselectN(luckycount, LUCKYNUMSIZE)*MselectN(datalen - luckycount, 0);
	result.top2 = MselectN(luckycount, LUCKYNUMSIZE - 1)*MselectN(datalen - luckycount, 1);
	result.top3 = MselectN(luckycount, LUCKYNUMSIZE - 2)*MselectN(datalen - luckycount, 2);

	return result;
}


//static REWARD_RESULT GetRewardResult(const uchar *pdata, uchar datalen, const uchar luckynum[LUCKYNUMSIZE])
//{
//	REWARD_RESULT result;
//	result.top1 = 0;
//	result.top2 = 0;
//	result.top3 = 0;
//
//	if(pdata == NULL || datalen < LUCKYNUMSIZE || datalen > TOTALNUMSIZE  || luckynum == NULL)
//	{
//		ErrorCheck(0);
//		return result;
//	}
//
////	u32 pptick = GetPricePerTick(datalen, pamount);
//
//	result = GetRewardTicket(pdata, datalen, luckynum);
//
////	result.top1 *= pptick;
////	result.top2 *= pptick;
////	result.top3 *= pptick;
//
//	return result;
//}

static bool IsDataIn(uchar data, const uchar *pbuf, uchar buflen)
{
	uchar ii = 0;

	if(pbuf == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	if(buflen == 0)
	{
		return false;
	}

	do
	{
		if(pbuf[ii++] == data)
		{
			return true;
		}
	}
	while(ii < buflen);

	return false;
}

static bool GetLuckyNum(const uchar *phash, uchar luckynum[LUCKYNUMSIZE])
{
	uchar luckysize = 0, ii = 0;
	uchar hash[HASHSIZE] = {0};

	if(phash == NULL || luckynum == NULL)
	{
		ErrorCheck(0);
		return false;
	}

	memcpy(hash, phash, HASHSIZE);

	do
	{
		do
		{
			if(hash[ii] < TOTALNUMSIZE)
			{
				if(!IsDataIn(hash[ii], (uchar *)luckynum, luckysize))
				{
					luckynum[luckysize++] = hash[ii];
				}
			}
			ii++;
		}
		while (luckysize < LUCKYNUMSIZE && ii < HASHSIZE);

		if(ii >= HASHSIZE && luckysize < LUCKYNUMSIZE)
		{
			luckysize = 0;
			ii = 0;
#if 1
			if(!SHA256(hash, HASHSIZE, hash))
			{
				ErrorCheck(0);
				return false;
			}
//			LogPrint(hash, sizeof(hash), HEX);
#else
			memcpy(hash, "\x01\x02\x03\x04\x05\x06", 6);
#endif
		}
	} while (luckysize < LUCKYNUMSIZE);

	return true;
}

REWARD_RESULT DrawLottery(const uchar *phash, const uchar *pdata, uchar datalen)
{
	REWARD_RESULT result;
	uchar luckynum[LUCKYNUMSIZE] = {0};
	result.top1 = 0;
	result.top2 = 0;
	result.top3 = 0;

	if(phash == NULL || pdata == NULL || datalen < LUCKYNUMSIZE || datalen > TOTALNUMSIZE)
	{
		ErrorCheck(0);
		return result;
	}

//	LogPrint(phash, 32, HEX);

	if(!GetLuckyNum(phash, luckynum))
	{
		ErrorCheck(0);
		return result;
	}

//	LogPrint(luckynum, sizeof(luckynum), HEX);

	result = GetRewardTicket(pdata, datalen, luckynum);

	return result;
}

//just for self test code
#if 0
static void errortest_GetTotalTicket(void)
{
	assert(GetTotalTicket(0) == 0);
	assert(GetTotalTicket(1) == 0);
	assert(GetTotalTicket(2) == 0);
	assert(GetTotalTicket(3) == 0);
	assert(GetTotalTicket(4) == 0);
	assert(GetTotalTicket(5) == 0);

	assert(GetTotalTicket(16) == 0);
	assert(GetTotalTicket(56) == 0);
	assert(GetTotalTicket(106) == 0);
}

static void normaltest_GetTotalTicket(void)
{
	assert(GetTotalTicket(6) == 1);
	assert(GetTotalTicket(7) == 7);
	assert(GetTotalTicket(8) == 28);
	assert(GetTotalTicket(9) == 84);
	assert(GetTotalTicket(10) == 210);
	assert(GetTotalTicket(11) == 462);
	assert(GetTotalTicket(12) == 924);
	assert(GetTotalTicket(13) == 1716);
	assert(GetTotalTicket(14) == 3003);
	assert(GetTotalTicket(15) == 5005);
}

static void errortest_GetPricePerTick(void)
{
	assert(GetPricePerTick(0, 0) == 0);
	assert(GetPricePerTick(0, 100) == 0);
	assert(GetPricePerTick(0, 1000) == 0);

	assert(GetPricePerTick(5, 0) == 0);
	assert(GetPricePerTick(5, 100) == 0);
	assert(GetPricePerTick(5, 1000) == 0);

	assert(GetPricePerTick(6, 0) == 0);
	assert(GetPricePerTick(10, 0) == 0);
	assert(GetPricePerTick(15, 0) == 0);

	assert(GetPricePerTick(16, 0) == 0);
	assert(GetPricePerTick(56, 100) == 0);
	assert(GetPricePerTick(106, 1000) == 0);
}

static void normaltest_GetPricePerTick(void)
{
	assert(GetPricePerTick(6, 1) == 1);
	assert(GetPricePerTick(7, 12) == (12/7));
	assert(GetPricePerTick(8, 123) == (123/28));
	assert(GetPricePerTick(9, 1234) == (1234/84));
	assert(GetPricePerTick(10, 12345) == (12345/210));
	assert(GetPricePerTick(11, 123456) == (123456/462));
	assert(GetPricePerTick(12, 1234567) == (1234567/924));
	assert(GetPricePerTick(13, 12345678) == (12345678/1716));
	assert(GetPricePerTick(14, 123456789) == (123456789/3003));
	assert(GetPricePerTick(15, 1234567899) == (1234567899/5005));
}

static void errorrewardresult(REWARD_RESULT *in)
{
	assert(in->top1 == 0);
	assert(in->top2 == 0);
	assert(in->top3 == 0);
}

static void errortest_GetRewardTicket(void)
{
	REWARD_RESULT result;
	uchar luckynum[LUCKYNUMSIZE] = {1, 2, 3, 4, 5, 6};
	uchar selectnum[LUCKYNUMSIZE] = {1, 2, 3, 4, 5, 6};

	result = GetRewardTicket(NULL, 9, luckynum);
	errorrewardresult(&result);

	result = GetRewardTicket(selectnum, 5, luckynum);
	errorrewardresult(&result);

	result = GetRewardTicket(selectnum, 16, luckynum);
	errorrewardresult(&result);

	result = GetRewardTicket(selectnum, 6, NULL);
	errorrewardresult(&result);
}

static void normalrewardresult(REWARD_RESULT *in)
{
	assert(in->top1 == 0 || in->top2 == 0 || in->top3 == 0 );
}

static void normaltest_GetRewardTicket(void)
{
	REWARD_RESULT result;
	uchar luckynum[LUCKYNUMSIZE] = {1, 2, 3, 4, 5, 6};
	uchar selectnum[LUCKYNUMSIZE] = {1, 2, 3, 4, 5, 6};

	result = GetRewardTicket(selectnum, 6, luckynum);
	normalrewardresult(&result);
}

static void errortest_GetRewardResult(void)
{
	REWARD_RESULT result;
	uchar luckynum[LUCKYNUMSIZE] = {1, 2, 3, 4, 5, 6};
	uchar selectnum[LUCKYNUMSIZE] = {1, 2, 3, 4, 5, 6};

	result = GetRewardResult(NULL, 9, luckynum, 99999);
	errorrewardresult(&result);

	result = GetRewardResult(selectnum, 5, luckynum, 99999);
	errorrewardresult(&result);

	result = GetRewardResult(selectnum, 16, luckynum, 99999);
	errorrewardresult(&result);

	result = GetRewardResult(selectnum, 6, NULL, 99999);
	errorrewardresult(&result);

	result = GetRewardResult(selectnum, 6, luckynum, 0);
	errorrewardresult(&result);
}

static void normaltest_GetRewardResult(void)
{
	REWARD_RESULT result;
	uchar luckynum[LUCKYNUMSIZE] = {1, 2, 3, 4, 5, 6};
	uchar selectnum[LUCKYNUMSIZE] = {1, 2, 3, 4, 5, 6};

	result = GetRewardResult(selectnum, 6, luckynum, 9999);
	normalrewardresult(&result);
}

static void errortest_IsDataIn(void)
{
	uchar xx[] = {1, 2, 3};

	assert(IsDataIn(5, NULL, 5) == false);
	assert(IsDataIn(5, xx, 0) == false);
}

static void normaltest_IsDataIn(void)
{
	uchar xx[] = {1, 2, 3, 4, 5};

	assert(IsDataIn(9, xx, sizeof(xx)) == false);
	assert(IsDataIn(3, xx, sizeof(xx)) == true);
}

static void errortest_GetLuckyNum(void)
{
	uchar hash[32] = {0};
	uchar luckynum[LUCKYNUMSIZE] = {0};

	assert(GetLuckyNum(NULL, luckynum) == false);
	assert(GetLuckyNum(hash, NULL) == false);
}

static void normaltest_GetLuckyNum(void)
{
	uchar hash[32] = {99, 63, 32, 13, 18, 14, 1, 2, 3, 4, 5, 6};
	uchar luckynum[LUCKYNUMSIZE] = {0};

	assert(GetLuckyNum(hash, luckynum) == true);
	assert(luckynum[0] == 13);
	assert(luckynum[1] == 14);
	assert(luckynum[2] == 1);
	assert(luckynum[3] == 2);
	assert(luckynum[4] == 3);
	assert(luckynum[5] == 4);
}


static void errortest_DrawLottery(void)
{
	REWARD_RESULT result;
	uchar hash[32] = {99, 63, 32, 13, 18, 14, 1, 2, 3, 4, 5, 6};
	uchar data[LUCKYNUMSIZE] = {13, 14, 1, 2, 3, 4};

	result = DrawLottery(NULL, data, sizeof(data), 99999);
	errorrewardresult(&result);

	result = DrawLottery(hash, NULL, sizeof(data), 99999);
	errorrewardresult(&result);

	result = DrawLottery(hash, data, 1, 99999);
	errorrewardresult(&result);

	result = DrawLottery(hash, data, 16, 99999);
	errorrewardresult(&result);

	result = DrawLottery(hash, data, sizeof(data), 0);
	errorrewardresult(&result);
}

static void normaltest_DrawLottery(void)
{
	REWARD_RESULT result;
	uchar hash[32] = {99, 63, 32, 13, 18, 14, 1, 2, 3, 4, 5, 6};
	uchar data[LUCKYNUMSIZE] = {13, 14, 1, 2, 3, 4};

	result = DrawLottery(hash, data, sizeof(data), 99999);
	normalrewardresult(&result);
}

static void ErrorParaTest(void)
{
	errortest_GetTotalTicket();
	errortest_GetPricePerTick();
	errortest_GetRewardTicket();
	errortest_GetRewardResult();
	errortest_IsDataIn();
	errortest_GetLuckyNum();
	errortest_DrawLottery();
}

static void NormalParaTest(void)
{
	normaltest_GetTotalTicket();
	normaltest_GetPricePerTick();
	normaltest_GetRewardTicket();
	normaltest_GetRewardResult();
	normaltest_IsDataIn();
	normaltest_GetLuckyNum();
	normaltest_DrawLottery();
}

int main(void)
{
	ErrorParaTest();
	NormalParaTest();
}

#endif






