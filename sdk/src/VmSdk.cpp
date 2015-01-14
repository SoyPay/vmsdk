#include <string.h>
#include<stdlib.h>
#include <stdio.h>
#include"VmSdk.h"

/***
 *  @brief 虚拟机调用系统功能的函数指令
 */
enum CALL_API_FUN {
	COMP_FUNC = 0,            //!< COMP_FUNC
	MULL_MONEY ,              //!< MULL_MONEY
	ADD_MONEY ,               //!< ADD_MONEY
	SUB_MONEY,                //!< SUB_MONEY
	DIV_MONEY,                //!< DIV_MONEY
	SHA256_FUNC,              //!< SHA256_FUNC
	DES_FUNC,                 //!< DES_FUNC
	VERFIY_SIGNATURE_FUNC,    //!< VERFIY_SIGNATURE_FUNC
	SIGNATURE_FUNC,           //!< SIGNATURE_FUNC
	PRINT_FUNC,               //!< PRINT_FUNC
	GETTX_CONTRACT_FUNC,      //!< GETTX_CONTRACT_FUNC
	GETTX_ACCOUNT_FUNC,       //!< GETTX_ACCOUNT_FUNC
	GETACCPUB_FUNC,           //!< GETACCPUB_FUNC
	QUEYACCBALANCE_FUNC,      //!< QUEYACCBALANCE_FUNC
	GETTXCONFIRH_FUNC,        //!< GETTXCONFIRH_FUNC
	GETTIPH_FUNC,             //!< GETTIPH_FUNC
	GETBLOCKHASH_FUNC,        //!< GETBLOCKHASH_FUNC
	ISAUTHORIT_FUNC,          //!<ISAUTHORIT
	GETAUTHORITDEFINE_FUNC,	  //!<GETAUTHORITDEFINE_FUNC


	//// tx api
	GETCTXCONFIRMH_FUNC,//!< GETCTXCONFIRMH_FUNC
	WRITEDB_FUNC,       //!< WRITEDB_FUNC
	DELETEDB_FUNC,      //!< DELETEDB_FUNC
	READDB_FUNC,        //!< READDB_FUNC
	MODIFYDB_FUNC,      //!< MODIFYDB_FUNC
	GETDBSIZE_FUNC,     //!< GETDBSIZE_FUNC
	GETDBVALUE_FUNC,    //!< GETDBVALUE_FUNC
	GetCURTXHASH_FUNC,  //!< GetCURTXHASH_FUNC
	READDBTIME_FUNC,     //!< READDBTIME_FUNC
	MODIFYDBTIME_FUNC,  //!< MODIFYDBTIME_FUNC
	MODIFYDBVALUE_FUNC ,  //!< MODIFYDBVALUE_FUNC
	WRITEOUTPUT_FUNC,     //!<WRITEOUTPUT_FUNC
	GETSCRIPTDATA_FUNC,		  //!<GETSCRIPTDATA_FUNC
	GETSCRIPTID_FUNC,		  //!<GETSCRIPTID_FUNC
	GETCURTXACCOUNT_FUNC,		  //!<GETSCRIPTID_FUNC
	GETCURTXCONTACT_FUNC,		 //!<GETCURTXCONTACT_FUNC
};

__root __code static const char aa[]@0x0008 = {0x22,0x22}; //{0,0,0,0x80,0xFB};
__root __code static const char a12[]@0x0012 = {0x22,0x22};
__root __xdata __no_init static  unsigned char Communicate[4*1024]@0xEFFF;
__root __xdata __no_init static unsigned char Result[2]@0xEFFD;

__noreturn __root void __exit(EXIT_CODE tep) {
	Result[0] = tep;
	((void (*)(void)) (0x0008))();
}

__root void __CallApi(CALL_API_FUN tep) {
	Result[0] = (unsigned char) (tep & 0xFF);
	Result[1] = (unsigned char) ((tep >> 8) & 0xFF);
	((void (*)(void)) (0x0012))();
}

typedef struct {
unsigned short len ;
unsigned char buffer[1];
} FUN_RET_DATA;

#define  GetInterflowP()   ((FUN_RET_DATA *)Communicate)
//FUN_RET_DATA inline * GetInterflowP() {
//	return (FUN_RET_DATA *)Communicate;
//}

#define  ClearParaSpace()   {((unsigned char * )GetInterflowP())[0] = 0 ;((unsigned char * )GetInterflowP())[1] = 0;}

static void InsertOutData(void const* pfrist, unsigned short len);
unsigned short GetVecLen(unsigned char const * data) {
	return *((unsigned short*)data);
}




//static unsigned short GetInterflowLen(void) {
//	return sizeof(Communicate);
//}

static bool IsOverShortFlow(unsigned short len1, unsigned short len2) {
	unsigned short len3 = len1 + len2;
	return (len3 < len1 || len3 < len2);
}

void InsertOutData(void const* pfrist, unsigned short len) {
	unsigned char * pbuffer = (unsigned char *) GetInterflowP();
	unsigned short len2 = GetVecLen(pbuffer);
	unsigned short len3 = len2 + (len + sizeof(len));
//                bool flag = IsOverShortFlow(len2,(len + sizeof(len)));
//                bool flag1 = IsOverShortFlow(len,sizeof(len));
	if (IsOverShortFlow(len2, (len + sizeof(len))) || IsOverShortFlow(len, sizeof(len))) {
		__exit(RUN_SCRIPT_DATA_ERR);
	}
	memcpy(pbuffer, &len3, sizeof(len3));
	memcpy(&pbuffer[len2 + 2], &len, sizeof(len));
	memcpy(&pbuffer[len2 + 4], pfrist, len);
}

bool Int64Inital(Int64 * p64, char const *pdata, unsigned char len) {
	memset(p64, 0, sizeof(*p64));

	if (len <= sizeof(*p64)) {
		while (len--) {
			((char *) p64)[len] = *(pdata++);
		}
		return true;
	}
	return false;

}

COMP_RET Int64Compare(const Int64* const pM1, const Int64* const pM2) {
	ClearParaSpace()
	;
	InsertOutData(pM1, sizeof(*pM1));
	InsertOutData(pM2, sizeof(*pM2));
	__CallApi(COMP_FUNC);
	FUN_RET_DATA *retdata = NULL;
	retdata = GetInterflowP();
	if (retdata->len == 1) {
		return (COMP_RET) retdata->buffer[0];
	}
//	__exit(RUN_SCRIPT_DATA_ERR);
	 return COMP_ERR;
}

static bool CALC_64(const Int64* pM1, const Int64* pM2, Int64* pOutM, CALL_API_FUN type) {
	ClearParaSpace();
	InsertOutData(pM1, sizeof(*pM1));
	InsertOutData(pM2, sizeof(*pM2));
	__CallApi(type);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len == sizeof(Int64)) {
		memset(pOutM, 0, sizeof(*pOutM));
		memcpy(pOutM, retdata->buffer, retdata->len);
		return true;
	}
	//__exit(RUN_SCRIPT_DATA_ERR);
	 return false;
}

bool Int64Mul(const Int64* const pM1, const Int64* const pM2, Int64* const pOutM) {
	return CALC_64(pM1, pM2, pOutM, MULL_MONEY);
}
bool Int64Add(const Int64* const pM1, const Int64* const pM2, Int64* const pOutM) {
	return CALC_64(pM1, pM2, pOutM, ADD_MONEY);
}
bool Int64Sub(const Int64* const pM1, const Int64* const pM2, Int64* const pOutM) {
	return CALC_64(pM1, pM2, pOutM, SUB_MONEY);
}
bool Int64Div(const Int64* const pM1, const Int64* const pM2, Int64* const pOutM) {
	return CALC_64(pM1, pM2, pOutM, DIV_MONEY);
}

bool SHA256(void const* pfrist, const unsigned short len, void * const pout) {
	ClearParaSpace()
	;
//	if(len <= 0)
//	{
//		return false;
//	}
	InsertOutData(pfrist, len);
	__CallApi(SHA256_FUNC);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len == 32) {
		memcpy(pout, retdata->buffer, 32);
		return true;
	}
//	__exit(RUN_SCRIPT_DATA_ERR);
	 return false;
}

bool SignatureVerify(void const* data, unsigned short datalen, void const* key, unsigned short keylen,
		void const* phash, unsigned short hashlen) {
	ClearParaSpace()
	;
//	if(datalen <=0 || keylen <=0)
//	{
//		return false;
//	}
	InsertOutData(data, datalen);
	InsertOutData(key, keylen);
	InsertOutData(phash, hashlen);
	__CallApi(VERFIY_SIGNATURE_FUNC);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len == 1) {
		return retdata->buffer[0];
	}
//	__exit(RUN_SCRIPT_DATA_ERR);
	 return false;
}

unsigned short Des(void const* pdata, unsigned short len, void const* pkey, unsigned short keylen, bool IsEn, void * const pOut,unsigned short poutlen) {
	ClearParaSpace()
	;
//	if(len <= 0 || keylen <= 0)
//	{
//		return false;
//	}
	InsertOutData(pdata, len);
	InsertOutData(pkey, keylen);
	InsertOutData(&IsEn, sizeof(IsEn));
	__CallApi(DES_FUNC);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len > 0 && retdata->len <= poutlen) {
		memcpy(pOut, retdata->buffer, retdata->len);
		return retdata->len;
	}
//	LogPrint("Des error",sizeof("Des error")+1,STRING);
//	__exit(RUN_SCRIPT_DATA_ERR);
	 return 0;
}

void LogPrint(const void *pdata, const unsigned short datalen,PRINT_FORMAT flag ) {
	ClearParaSpace()
	;
	InsertOutData(&flag, sizeof(PRINT_FORMAT));
	InsertOutData(pdata, datalen);
	__CallApi(PRINT_FUNC);
}

bool WriteOutput( const VM_OPERATE* data, const unsigned short conter) {
	ClearParaSpace()
	;

	InsertOutData(data, conter * sizeof(*data));
	__CallApi(WRITEOUTPUT_FUNC);
	return true;
}


bool GetTxContacts(const unsigned char * const txhash,void* const pcotact,const unsigned short maxLen) {
	ClearParaSpace()
	;
	InsertOutData(txhash, 32);
	__CallApi(GETTX_CONTRACT_FUNC);
	#pragma data_alignment = 1
	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len > maxLen || retdata->len <= 0) {
		return false;
	}
	memcpy(pcotact, retdata->buffer, retdata->len);
	return true;
}



unsigned short GetAccounts(const unsigned char *txhash,void* const paccoutn,unsigned short maxlen) {
	ClearParaSpace()
	;
	InsertOutData(txhash, 32);
	__CallApi(GETTX_ACCOUNT_FUNC);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len == 0 || retdata->len > maxlen) {
		return 0;
	}
//	if((retdata->len % sizeof(ACCOUNT_RET))!= 0)
//	{
//		return 0;
//	}
	memcpy(paccoutn, retdata->buffer, retdata->len);
	return retdata->len;
}

unsigned short GetAccountPublickey(const void* const accounid,void * const pubkey,const unsigned short maxlength) {
	ClearParaSpace()
	;

	InsertOutData(accounid, 6);
	__CallApi(GETACCPUB_FUNC);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len > maxlength || !(retdata->len == MAX_PUK_LEN || retdata->len == 33) || retdata->len <= 0) {
		return 0;
	}
	memcpy(pubkey, retdata->buffer, retdata->len);
	return retdata->len;
}
bool QueryAccountBalance(const unsigned char* const account,Int64* const pBalance){
	ClearParaSpace()
	;
	InsertOutData(account,  6 );
	__CallApi(QUEYACCBALANCE_FUNC);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len != sizeof(Int64)) {
		return false;
	}
	memcpy(pBalance, retdata->buffer, retdata->len);
	return true;
}
unsigned long GetTxConFirmHeight(const void * const txhash) {
	ClearParaSpace()
	;
	InsertOutData(txhash, 32);
	__CallApi(GETTXCONFIRH_FUNC);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len != sizeof(long)) {
		return 0;
	}

	unsigned long height = 0;
	memcpy(&height, retdata->buffer, retdata->len);
	return height;
}
bool WriteDataDB(const void* const key,const unsigned char keylen,const void * const value,const unsigned short valuelen,const unsigned long time) {
	ClearParaSpace();
//	if(keylen <=0 || valuelen<=0)
//	{
//		return false;
//	}
	InsertOutData(key, keylen);
	InsertOutData(value, valuelen);
	InsertOutData(&time, 4);
	__CallApi(WRITEDB_FUNC);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len != 1) {
		return false;
	}
	bool ret = false;
	memcpy(&ret, retdata->buffer, 1);
	return ret;

}
bool DeleteDataDB(const void* const key,const unsigned char keylen) {
	ClearParaSpace();
//	if(keylen <= 0)
//		return false;
	InsertOutData(key, keylen);

	__CallApi(DELETEDB_FUNC);

#pragma data_alignment = 1
	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len != 1) {
		return false;
	}
//	bool falg = false;
//	memcpy(&falg,retdata->buffer,1);
//	char buffer[10] = {0};
//	memcpy(buffer,retdata,10);
//	LogPrint(buffer,10,HEX);
	return retdata->buffer[0];
	//return falg;
}

unsigned short ReadDataValueDB(const void* const key,const unsigned char keylen, void* const value,unsigned short const maxbuffer) {
	ClearParaSpace();
//	if(keylen <= 0 || maxbuffer <= 0)
//		return 0;
	InsertOutData(key, keylen);
	__CallApi(READDB_FUNC);



#pragma data_alignment = 1
	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len > maxbuffer || retdata->len <= 0) {
		return 0;
	}
	memcpy(value, retdata->buffer, retdata->len);
	unsigned short size = retdata->len;
	return size;
}
bool ModifyDataDB(const void* const key,const unsigned char keylen, const void* const pvalue,const unsigned short valuelen,const unsigned long ptime) {
	ClearParaSpace();
//	if(keylen <= 0 || valuelen <= 0)
//		return false;
	InsertOutData(key, keylen);
	InsertOutData(pvalue, valuelen);
	InsertOutData(&ptime, sizeof(ptime));
	__CallApi(MODIFYDB_FUNC);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len != 1) {
		return false;
	}
	bool ret = false;
	memcpy(&ret, retdata->buffer, 1);
	return ret;
}
unsigned long GetDBSize() {
	__CallApi(GETDBSIZE_FUNC);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len != sizeof(long)) {
		return false;
	}
	unsigned long size = 0;
	memcpy(&size, retdata->buffer, sizeof(size));
	return size;
}
bool GetDBValue(const unsigned long index,void* const key,unsigned char * const keylen,unsigned short maxkeylen,void* const value,unsigned short* const maxbuffer, unsigned long* const ptime)
 {

	ClearParaSpace();
//	if(*keylen <=0 || *maxbuffer <=0)
//	{
//		return false;
//	}
	InsertOutData(&index, sizeof(index));
	if(index == 1)
	{
		InsertOutData(key, *keylen);
	}
	__CallApi(GETDBVALUE_FUNC);

   #pragma data_alignment = 1
	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len <= 0 || retdata->len > maxkeylen) {
		return false;
	}

//	memcpy(key,retdata->buffer, retdata->len);
	memcpy(key,retdata->buffer, retdata->len);
	*keylen = retdata->len;
   #pragma data_alignment = 1
	retdata = (FUN_RET_DATA *) ((unsigned char*) retdata->buffer + retdata->len);
	if (retdata->len > *maxbuffer) {
		return false;
	}
	memcpy(value, retdata->buffer, retdata->len);
	*maxbuffer = retdata->len;

   #pragma data_alignment = 1
	retdata = (FUN_RET_DATA *) ((unsigned char*) retdata->buffer + retdata->len);
	if (retdata->len != sizeof(*ptime)) {
		return false;
	}
	memcpy((char*) ptime, retdata->buffer, sizeof(*ptime));

	return true;
}
unsigned long GetTipHeight() {
	__CallApi(GETTIPH_FUNC);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len != 4) {
		return false;
	}
	unsigned long height = false;
	memcpy(&height, retdata->buffer, sizeof(height));
	return height;
}
bool GetBlockHash(const unsigned long height,void * const pblochHash) {
	ClearParaSpace()
	;
	InsertOutData(&height, sizeof(height));
	__CallApi(GETBLOCKHASH_FUNC);
	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len > 32 || retdata->len <= 0) {
		return false;
	}

	memcpy(pblochHash, retdata->buffer, retdata->len);
	return true;
}
bool GetCurTxHash(void * const poutHash) {
	__CallApi(GetCURTXHASH_FUNC);
	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len > 32 || retdata->len <= 0) {
		return false;
	}
	memcpy(poutHash, retdata->buffer, retdata->len);
	return true;
}

bool IsAuthorited(const void* const account,const Int64* const pmoney) {
	ClearParaSpace()
	;
	InsertOutData(account, 6);
	InsertOutData(pmoney, 8);
	__CallApi(ISAUTHORIT_FUNC);
	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len != 1) {
		return false;
	}
	bool flag = false;
	memcpy(&flag, retdata->buffer, 1);
	return flag;
}
bool ReadDataDBTime(const void* const key,const unsigned char keylen, unsigned long * const ptime) {
	ClearParaSpace();
//	if(keylen <= 0)
//	{
//		return false;
//	}
	InsertOutData(key, keylen);
	__CallApi(READDBTIME_FUNC);
	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len != 4) {
		return false;
	}
	memcpy(ptime, retdata->buffer, sizeof(*ptime));
	return true;
}
bool ModifyDataDBTime(const void* const key,const unsigned char keylen, const unsigned long ptime) {
	ClearParaSpace();
//	if(keylen <= 0)
//	{
//		return false;
//	}
	InsertOutData(key, keylen);
	InsertOutData(&ptime, 4);
	__CallApi(MODIFYDBTIME_FUNC);
	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len != 1) {
		return false;
	}
	bool flag;
	memcpy(&flag, retdata->buffer, 1);
	return flag;
}

bool ModifyDataDBVavle(const void* const key,const unsigned char keylen, const void* const pvalue,const unsigned short valuelen) {
	ClearParaSpace();
//	if(keylen <= 0 || valuelen<= 0)
//	{
//		return false;
//	}
	InsertOutData(key, keylen);
	InsertOutData(pvalue, valuelen);
	__CallApi(MODIFYDBVALUE_FUNC);
	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len != 1) {
		return false;
	}
	bool flag;
	memcpy(&flag, retdata->buffer, 1);
	return flag;
}


bool IsRegID(const void* const account)
{
	char pubkey[33] = {0};
	if(GetAccountPublickey(account,pubkey,33))
	{
		return true;
	}
	return false;
}
unsigned short GetAuthUserDefine(const void* const account,void *const pout,const unsigned short maxlen)
{
	ClearParaSpace();

	InsertOutData(account, 6);
	__CallApi(GETAUTHORITDEFINE_FUNC);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len > maxlen ||  retdata->len <= 0) {
		return 0;
	}
	memcpy(pout, retdata->buffer, retdata->len);
	return retdata->len;
}
bool GetScriptData(const void* const scriptID,void* const pkey,short len,void* const pvalve,short maxlen)
{
	ClearParaSpace();
//	if(scriptID == NULL ||pkey == NULL || len <= 0)
//	{
//		return false;
//	}
	InsertOutData(scriptID, 6);
	InsertOutData(pkey, len);
	__CallApi(GETSCRIPTDATA_FUNC);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len > maxlen ||  retdata->len <= 0) {
		return 0;
	}
	memcpy(pvalve, retdata->buffer, retdata->len);
	return retdata->len;
}
bool GetCurScritpAccount(void* const account)
{
	ClearParaSpace();
	if(account == NULL)
	{
		return false;
	}
	__CallApi(GETSCRIPTID_FUNC);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len != 6) {
			return 0;
		}
	memcpy(account, retdata->buffer, retdata->len);
	return retdata->len;
	}
unsigned short GetCurTxAccount(void * const account,unsigned short maxlen)
{
		ClearParaSpace();
//		if(account == NULL || maxlen <= 0)
//		{
//			return false;
//		}
		__CallApi(GETCURTXACCOUNT_FUNC);

		FUN_RET_DATA *retdata = GetInterflowP();
		if (retdata->len < 6) {
			return 0;
		}
		memcpy(account, retdata->buffer, retdata->len);
		return retdata->len;
}
unsigned short GetCurTxContact(void * const pContact,unsigned short maxlen)
{
	ClearParaSpace();
//	if(pContact == NULL || maxlen <= 0)
//	{
//		return false;
//	}
	__CallApi(GETCURTXCONTACT_FUNC);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len <= 0) {
		return 0;
	}
	memcpy(pContact, retdata->buffer, retdata->len);
	return retdata->len;
}
//long ReadVarLong(char*buffer){
//	 long n = 0;
//	    while(true) {
//	        unsigned char chData;
//	        chData = *buffer++;
//	        n = (n << 7) | (chData & 0x7F);
//	        if (chData & 0x80)
//	            n++;
//	        else
//	            return n;
//	    }
//	    return n;
//}
//void StringAdd(char *str)
//{
//	int len;
//	len=strlen(str);
//
//
//	while(len--)
//	{
//		if(str[len]=='F' || str[len]=='f')
//		{
//			str[len]='0';
//		}
//		else
//		{
//			str[len]+=1;
//			break;
//		}
//	}
//}
//Int64 ReadVarInt64(char*buffer){
//	Int64 ret;
//	Int64 add;
//	Int64 mul;
//	Int64Inital(&add,"\x01",1);
//	Int64Inital(&mul,"\x80",1);
//	while(true) {
//		unsigned char chData;
//		chData = *buffer++;
//		//n = (n <<7) | (chData & 0x7F);
//		Int64Mul(&ret,&mul,&ret);
//		ret = ret| (chData & 0x7F);
//		if (chData & 0x80)
//			Int64Add(&ret,&add,&ret);
//		else{
//			return ret;
//		}
//
//	}
//	return ret;
//}
