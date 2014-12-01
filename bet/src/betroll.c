/*
 * betroll.c
 *
 *  Created on: Sep 23, 2014
 *      Author: leo
 */


/*rule : actor A and B
 * A AND B both generate a rand number (a1 b1)(max 99)
 * then generate a secret rand number  (a2 b2)(max 99)  skey
 * s1 = a2^a1,s2 = b2^b1
 * rule result is (r = (a1 ^ b1)%6+1),B according to s1 and s2,guess result 0 or 1,
 * if(1<= r <=3) and B guess 0,B will win.
 * if(1<= r <=3) and B guess 1,A will win.
 * if(4<= r <=6) and B guess 0,A will win.
 * if(4<= r <=6) and B guess 1,B will win.
 *
 *a3=a1+a2 b3=b1+b2 (verify:this is just an example,the arithmetic is not enough secure) v
 *
 *
 *sys provide param:
 *cur_h  block height
 *con_h  contract confirm block height
 *i :index A and B,0:A  1:B
 *
 *A and B provide:
 *h :contract invalid heights
 *m :contract money
 *
 *
 * rule step 1 :contract transaction
 * s1 a3 s2 b3 r h m (r:B guess result)
 * step1 generate a transaction id: txid
 * the one who win publish result ,because tx need some fee.
 * who publish result,who win.
 * if one of them cheat,the other one can also publish the right result,then win the money.
 *
 * rule 2 :appeal transaction
 * txid a2 or b2
 *
 * rule 3 :appeal transaction
 * txid b2 or a2
 *
 *
 *rule step 1 excute data:
 *cur_h s1 a3 s2 b3 r h m
 *
 *rule step 2 excute data:
 *cur_h con_h s1 a3 s2 b3 r h m i a2(b2)
 *
 *rule step 3 excute data:
 *cur_h con_h s1 a3 s2 b3 r h m i a2(b2) i b2(a2)
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vmapi.cpp"
   
   
#define bool unsigned char
#define BOOL_CHECK(a) {if(!(a)) return 0;}
#define BOOL_EXIT(a) {if(!(a)) {__exit(0);return 0;}}
#define MAX_MLEN 20

/*
typedef struct _ContractHead {
	unsigned char obs;
	unsigned char actors;
	unsigned char appealtxs;
	unsigned char h1[4]; //current height
	unsigned char h2[4]; //confirm height
} ContractHead;
*/

/*
typedef struct _ContractOut {
	unsigned char txid;//tx sn
	unsigned char opt;//OperType
	unsigned char force;//1 : 0
	unsigned char i;//actor index
	unsigned char h[4];//valid heights
	unsigned char m[MAX_MLEN];//trade amount
}ContractOut;

typedef struct _ContractOutPair{
	ContractOut from;
	ContractOut to;
}ContractOutPair;
*/

typedef struct _BetRoll{
	unsigned char s[2];//A Bsecret data
	unsigned char v[2];//A Bverify data
	unsigned char r; //b guess rslt
	unsigned char h[4];//valid hegihts
	unsigned char m[MAX_MLEN];//bet money
}BetRoll;

typedef struct _BetRollAppeal{
	unsigned char txs;
	unsigned char i[2];
	unsigned char skey[2];
}BetRollAppeal;


bool CheckContractLen(int len)
{
	if(len == sizeof(BetRoll))
	{
		return 1;
	}
	return 0;
}

bool CheckContractHead(VM_HEAD const*pstCh)
{
	int h1 = 0,h2 = 0;
	BOOL_CHECK(pstCh->ArbitratorAccCount == 0);
	BOOL_CHECK(pstCh->vAccountCount == 2);
	BOOL_CHECK(pstCh->AppealTxCount <= 2);
	memcpy(&h1,pstCh->vCurrentH,sizeof(int));
	memcpy(&h2,pstCh->vSecureH,sizeof(int));
	BOOL_CHECK(h1 >= h2);
	return 1;
}

bool CheckM(unsigned char *pM,unsigned char mLen)
{
	unsigned char i = 0;
	BOOL_CHECK(mLen > 0 && mLen <= MAX_MLEN)
	BOOL_CHECK(pM[0] > '0' && pM[0] <= '9');
	i++;
	for(;i < mLen;i++)
	{
		if(pM[i] != 0x00)
		{
			BOOL_CHECK(pM[0] >= '0' && pM[0] <= '9');
		}
		else
		{
			i++;
			break;
		}
	}
	for(;i < mLen;i++)
	{
		BOOL_CHECK(pM[i] == 0x00);
	}
	return 1;
}
/*
 * pM1 > pM2 :1
 * pM1 = pM2 :0
 * pM1 < pM2:-1
 */
int CompareM(unsigned char *pM1,unsigned char m1Len,unsigned char *pM2,unsigned char m2Len)
{
	if(m1Len > m2Len)
	{
		return 1;
	}
	else if(m1Len < m2Len)
	{
		return -1;
	}
	return memcmp(pM1,pM2,m1Len);
}

bool CheckBetRoll(VM_HEAD const*pstCh,BetRoll *pstBr)
{
	int h = 0,mlen = 0;
        int ret = 0;
	//int h1 = 0,h2 = 0;

	unsigned char *pMaxM = (unsigned char *)"50000000000";
	unsigned char *pMinM = (unsigned char *)"100000000";

	BOOL_CHECK(pstBr->r < 2);//0 or 1
	memcpy(&h,pstBr->h,sizeof(int));
	BOOL_CHECK(h >= 50);//hs
	BOOL_CHECK(h <= 100);//hs

	BOOL_CHECK(CheckM(pstBr->m,sizeof(pstBr->m)));
	mlen = strlen((const char*)pstBr->m);
        ret = CompareM(pstBr->m,mlen,pMaxM,strlen((const char*)pMaxM));
	BOOL_CHECK(ret <= 0);
        ret = CompareM(pstBr->m,mlen,pMinM,strlen((const char*)pMinM));
	BOOL_CHECK(ret >= 0 );

	return 1;
}

bool CheckSignIndex(unsigned char const*pSignIndex,unsigned short len)
{
	BOOL_CHECK(len == 2);
	BOOL_CHECK(pSignIndex[0] == 0);
	BOOL_CHECK(pSignIndex[1] == 1);
	return 1;
}

bool CheckAppeal(BetRollAppeal *pstBra)
{
	BOOL_CHECK(pstBra->txs <= 2);//0 or 1 or 2
	if (pstBra->txs == 1)
	{
		BOOL_CHECK(pstBra->i[0] < 2);
		BOOL_CHECK(pstBra->skey[0] < 100);
	}
	else if (pstBra->txs == 2)
	{
		BOOL_CHECK(pstBra->i[0] < 2);
		BOOL_CHECK(pstBra->i[1] < 2);
		BOOL_CHECK(pstBra->i[0] != pstBra->i[1]);
		BOOL_CHECK(pstBra->skey[0] < 100);
		BOOL_CHECK(pstBra->skey[1] < 100);
	}
	return 1;
}
/*
 * case1 :contract transaction
 *
 */
unsigned char DealCase1(VM_HEAD const*pstCh,BetRoll *pstBr)
{
	VmOperate stCop;
	memset(&stCop,0,sizeof(stCop));

	stCop.muls.txid = 0;
	stCop.muls.opeatortype = MINUS_FREE;
	stCop.muls.ResultCheck = 1;
	stCop.muls.accountid = 0;
	memcpy(&stCop.muls.outheight,pstBr->h,sizeof(pstBr->h));
	memcpy(stCop.muls.money,pstBr->m,sizeof(pstBr->m));

	stCop.add.txid = 0;
	stCop.add.opeatortype = ADD_INPUT_FREEZD;
	stCop.add.ResultCheck = 1;
	stCop.add.accountid = 0;
	memcpy(&stCop.add.outheight, pstBr->h, sizeof(pstBr->h));
	memcpy(stCop.add.money, pstBr->m, sizeof(pstBr->m));
	OutPutOpratorAdd(&stCop,1);

	memset(&stCop,0,sizeof(stCop));
	stCop.muls.txid = 0;
	stCop.muls.opeatortype = MINUS_FREE;
	stCop.muls.ResultCheck = 1;
	stCop.muls.accountid = 1;
	memcpy(&stCop.muls.outheight, pstBr->h, sizeof(pstBr->h));
	memcpy(stCop.muls.money, pstBr->m, sizeof(pstBr->m));

	stCop.add.txid = 0;
	stCop.add.opeatortype = ADD_INPUT_FREEZD;
	stCop.add.ResultCheck = 1;
	stCop.add.accountid = 1;
	memcpy(&stCop.add.outheight, pstBr->h, sizeof(pstBr->h));
	memcpy(stCop.add.money, pstBr->m, sizeof(pstBr->m));
	OutPutOpratorAdd(&stCop,1);
	return 1;
}

//verify contract data
bool VerifyContract(unsigned char s,unsigned char v,unsigned char skey)
{
	unsigned char a = 0;
	a = skey^s; //
	BOOL_CHECK(v == a+skey);
	return 1;
}

/*
 * case2 :publish rslt
 *0:rslt connot change
 *1:rslt can change by case 3
 */
unsigned char  DealCase2(VM_HEAD const*pstCh,BetRoll *pstBr,BetRollAppeal *pstBra,bool bCheck)
{
	unsigned char addindex = 0,minusindex = 0;
	unsigned char ret = 0;

	if (VerifyContract(pstBr->s[pstBra->i[0]], pstBr->v[pstBra->i[0]], pstBra->skey[0]))
	{
		addindex = pstBra->i[0];
		minusindex = !pstBra->i[0];
		ret = 1;
	}
	else //cheat
	{
		addindex = !pstBra->i[0];
		minusindex = pstBra->i[0];
		ret = 0;
	}

	if(!bCheck)
	{
		VmOperate stCop;

		memset(&stCop, 0, sizeof(stCop));
		stCop.muls.txid = 0;
		stCop.muls.opeatortype = MINUS_INPUT;
		stCop.muls.ResultCheck = 1;
		stCop.muls.accountid = minusindex;
		memcpy(&stCop.muls.outheight, pstBr->h, sizeof(pstBr->h));
		memcpy(stCop.muls.money, pstBr->m, sizeof(pstBr->m));

		stCop.add.txid = 1;
		stCop.add.opeatortype = ADD_INPUT_FREEZD;
		if(ret == 0)
		{
			stCop.add.opeatortype = ADD_FREE;
		}
		stCop.add.ResultCheck = 1;
		stCop.add.accountid = addindex;
		memcpy(&stCop.add.outheight, pstBr->h, sizeof(pstBr->h));
		memcpy(stCop.add.money, pstBr->m, sizeof(pstBr->m));
		OutPutOpratorAdd(&stCop, 1);

		if(ret == 0)
		{
			memset(&stCop, 0, sizeof(stCop));
			stCop.muls.txid = 0;
			stCop.muls.opeatortype = MINUS_INPUT;
			stCop.muls.ResultCheck = 1;
			stCop.muls.accountid = addindex;
			memcpy(&stCop.muls.outheight, pstBr->h, sizeof(pstBr->h));
			memcpy(stCop.muls.money, pstBr->m, sizeof(pstBr->m));

			stCop.add.txid = 1;
			stCop.add.opeatortype = ADD_FREE;
			stCop.add.ResultCheck = 1;
			stCop.add.accountid = addindex;
			memcpy(&stCop.add.outheight, pstBr->h, sizeof(pstBr->h));
			memcpy(stCop.add.money, pstBr->m, sizeof(pstBr->m));
			OutPutOpratorAdd(&stCop, 1);
		}
	}

	return ret;
}

/*
 * case3 :final appeal
 * 0 fail
 * 1 succ
 *
 */
unsigned char DealCase3(VM_HEAD const*pstCh,BetRoll *pstBr,BetRollAppeal *pstBra)
{
	unsigned char win = 0,calwin = 0;
	unsigned char c1 = 0,c2 = 0,r = 0;

	BOOL_CHECK(DealCase2(pstCh,pstBr,pstBra,1));
	BOOL_CHECK(VerifyContract(pstBr->s[pstBra->i[1]], pstBr->v[pstBra->i[1]], pstBra->skey[1]));
	win = pstBra->i[0];
	c1 = pstBra->skey[0]^pstBr->s[0];
	c2 = pstBra->skey[1]^pstBr->s[1];
	r = ((c1^c2)%6)+1;
	if( (pstBr->r && r > 3) || ( !pstBr->r && r < 4))
	{
		calwin = 1;
	}

	if(calwin == win)//
	{
		return 0;
	}
	else
	{
		VmOperate stCop;

		memset(&stCop, 0, sizeof(stCop));
		stCop.muls.txid = 0;
		stCop.muls.opeatortype = MINUS_INPUT;
		stCop.muls.ResultCheck = 1;
		stCop.muls.accountid = win;
		memcpy(&stCop.muls.outheight, pstBr->h, sizeof(pstBr->h));
		memcpy(stCop.muls.money, pstBr->m, sizeof(pstBr->m));

		stCop.add.txid = 2;
		stCop.add.opeatortype = ADD_FREE;
		stCop.add.ResultCheck = 1;
		stCop.add.accountid = calwin;
		memcpy(&stCop.add.outheight, pstBr->h, sizeof(pstBr->h));
		memcpy(stCop.add.money, pstBr->m, sizeof(pstBr->m));
		OutPutOpratorAdd(&stCop, 1);

		memset(&stCop, 0, sizeof(stCop));
		stCop.muls.txid = 1;
		stCop.muls.opeatortype = MINUS_INPUT;
		stCop.muls.ResultCheck = 1;
		stCop.muls.accountid = win;
		memcpy(&stCop.muls.outheight, pstBr->h, sizeof(pstBr->h));
		memcpy(stCop.muls.money, pstBr->m, sizeof(pstBr->m));

		stCop.add.txid = 2;
		stCop.add.opeatortype = ADD_FREE;
		stCop.add.ResultCheck = 1;
		stCop.add.accountid = calwin;
		memcpy(&stCop.add.outheight, pstBr->h, sizeof(pstBr->h));
		memcpy(stCop.add.money, pstBr->m, sizeof(pstBr->m));
		OutPutOpratorAdd(&stCop, 1);
	}

	return 1;
}

unsigned char main()
{
	VM_HEAD const*pstVh;
	BetRoll *pstBr;
	unsigned short clen = 0,csignlen = 0,appeallen = 0,asignlen = 0;
	unsigned char const*pContract = NULL,*pCSignIndex = NULL;
	unsigned char const*pAppeal = NULL,*pAsignIndex = NULL;
	BetRollAppeal stAppeal = {0};

	pstVh = GetHead((unsigned char*)exchangdata);
	BOOL_EXIT(CheckContractHead(pstVh));

	pContract = ReadFistTxContact((unsigned char*)exchangdata,clen);
	pstBr =  (BetRoll *)pContract;
	BOOL_EXIT(CheckContractLen(clen));
	BOOL_EXIT(CheckBetRoll(pstVh,pstBr));

	pCSignIndex = GetFistTxContactSigID((unsigned char*)exchangdata,csignlen);
	BOOL_EXIT(CheckSignIndex(pCSignIndex,csignlen));


    stAppeal.txs = pstVh->AppealTxCount;
    if(stAppeal.txs == 0)
    {
    	DealCase1(pstVh,pstBr);
    	__exit(1);
    	return 1;
    }
    else if(stAppeal.txs == 1)
    {
    	pAppeal = GetAppealTx((unsigned char*)exchangdata,appeallen,0);
    	BOOL_EXIT(appeallen == 1);

    	pAsignIndex = GetAppealTxSignID((unsigned char*)exchangdata,asignlen,0);
    	BOOL_EXIT(asignlen == 1);

    	stAppeal.i[0] = pAsignIndex[0];
    	stAppeal.skey[0] = pAppeal[0];
    	BOOL_EXIT(CheckAppeal(&stAppeal));

    	DealCase2(pstVh,pstBr,&stAppeal,0);
    	__exit(1);
    	return 1;
    }

    else if(stAppeal.txs == 2)
    {
    	pAppeal = GetAppealTx((unsigned char*)exchangdata,appeallen,0);
    	BOOL_EXIT(appeallen == 1);

    	pAsignIndex = GetAppealTxSignID((unsigned char*)exchangdata,asignlen,0);
    	BOOL_EXIT(asignlen == 1);

    	stAppeal.i[0] = pAsignIndex[0];
    	stAppeal.skey[0] = pAppeal[0];

    	pAppeal = GetAppealTx((unsigned char*)exchangdata,appeallen,1);
		BOOL_EXIT(appeallen == 1);

		pAsignIndex = GetAppealTxSignID((unsigned char*) exchangdata, asignlen, 1);
		BOOL_EXIT(asignlen == 1);

		stAppeal.i[1] = pAsignIndex[0];
		stAppeal.skey[1] = pAppeal[0];
		BOOL_EXIT(CheckAppeal(&stAppeal));

		BOOL_EXIT(DealCase3(pstVh,pstBr,&stAppeal));
		__exit(1);
		return 1;
    }
	__exit(0);
	return 0;
}


