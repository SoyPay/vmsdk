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

typedef struct
{
 u32 m;
 u32 n;
 u32 result;
}MAP_M6;

const static MAP_M6 ms6map[] = {
		{6, 6, 1},
		{7, 6, 7},
		{8, 6, 28},
		{9, 6, 84},
		{10, 6, 210},
		{11, 6, 462},
		{12, 6, 924},
		{13, 6, 1716},
		{14, 6, 3003},
		{15, 6, 5005}
};

static u32 factorial(u32 n) {
	if (n <= 1)
		return 1;
	else
		return n * factorial(n - 1);
}

static u32 MselectN(u32 m, u32 n) {
#if 0
	if (n == 0)
		return 1;
	if (m < n)
		return 0;
	if (m < 1 || m == n)
		return 1;
	return factorial(m) / (factorial(m - n) * factorial(n));
#else
	return ms6map[m - 6].result;
#endif
}

static u32 GetTotalTicket(uchar selectsize)
{
	if(selectsize < LUCKYNUMSIZE || selectsize > TOTALNUMSIZE)
	{
		return 0;
	}
	return MselectN(selectsize, LUCKYNUMSIZE);
}

static u32 GetPricePerTick(uchar selectsize, u32 amount)
{
	if(selectsize < LUCKYNUMSIZE || selectsize > TOTALNUMSIZE || amount == 0)
	{
		return 0;
	}
	return (amount/GetTotalTicket(selectsize));
}

static REWARD_RESULT GetRewardTicket(const uchar *pdata, uchar datalen, const uchar luckynum[LUCKYNUMSIZE])
{
	REWARD_RESULT result;
	uchar luckycount = 0;

	result.top1 = 0;
	result.top2 = 0;
	result.top3 = 0;

	if(pdata == NULL || datalen < LUCKYNUMSIZE || datalen > TOTALNUMSIZE || luckynum == NULL)
	{
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


static REWARD_RESULT GetRewardResult(const uchar *pdata, uchar datalen, const uchar luckynum[LUCKYNUMSIZE], u32 amount)
{
	REWARD_RESULT result;
	result.top1 = 0;
	result.top2 = 0;
	result.top3 = 0;

	if(pdata == NULL || datalen < LUCKYNUMSIZE || datalen > TOTALNUMSIZE  || luckynum == NULL || amount == 0)
	{
		return result;
	}

	u32 pptick = GetPricePerTick(datalen, amount);

	result = GetRewardTicket(pdata, datalen, luckynum);

	result.top1 *= pptick;
	result.top2 *= pptick;
	result.top3 *= pptick;

	return result;
}

static bool IsDataIn(uchar data, const uchar *pbuf, uchar buflen)
{
	uchar ii = 0;

	if(pbuf == NULL || buflen == 0)
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
		while (luckysize < LUCKYNUMSIZE || ii < HASHSIZE);

		if(ii >= HASHSIZE && luckysize < LUCKYNUMSIZE)
		{
			luckysize = 0;
			ii = 0;
#if 0
			if(!SHA256(hash, HASHSIZE, hash))
			{
				return false;
			}
#else
			memcpy(hash, "\x01\x02\x03\x04\x05\x06", 6);
#endif
		}
	} while (luckysize < LUCKYNUMSIZE);

	return true;
}

REWARD_RESULT DrawLottery(const uchar *phash, const uchar *pdata, uchar datalen, u32 amount)
{
	REWARD_RESULT result;
	uchar luckynum[LUCKYNUMSIZE] = {0};
	result.top1 = 0;
	result.top2 = 0;
	result.top3 = 0;

	if(phash == NULL || pdata == NULL || datalen < LUCKYNUMSIZE || datalen > TOTALNUMSIZE || amount == 0)
	{
		return result;
	}

	if(!GetLuckyNum(phash, luckynum))
	{
		return result;
	}

	result = GetRewardResult(pdata, datalen, luckynum, amount);

	return result;
}

//just for self test code
#if 1
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






