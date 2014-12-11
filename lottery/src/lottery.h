/*
 * lottery.h
 *
 *  Created on: Nov 17, 2014
 *      Author: spark.huang
 */

#ifndef LOTTERY_H_
#define LOTTERY_H_

#include"VmSdk.h"

typedef unsigned char uchar;
typedef unsigned long u32;


#define TOTALNUMSIZE (15)
#define LUCKYNUMSIZE (6)
#define HASHSIZE (32)

#define ErrorCheck(a) {if(!(a)) {PrintfFileAndLine(__LINE__, __FILE__);}}

typedef struct
{
	u32 top1;
	u32 top2;
	u32 top3;
}REWARD_RESULT;

u32 MselectN(u32 m, u32 n);

REWARD_RESULT DrawLottery(const uchar *phash, const uchar *pdata, uchar datalen, const Int64 *pamount);

#endif /* LOTTERY_H_ */
