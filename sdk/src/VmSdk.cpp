#include <string.h>
#include<stdlib.h>
#include <stdio.h>
#include"VmSdk.h"

/***
 *  @brief 虚拟机调用系统功能的函数指令
 */
typedef enum tagCALL_API_FUN {
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
	GETBLOCKHASH_FUNC,        //!< GETBLOCKHASH_FUNC


	//// tx api
	GETCTXCONFIRMH_FUNC,//!< GETCTXCONFIRMH_FUNC
	WRITEDB_FUNC,       //!< WRITEDB_FUNC
	DELETEDB_FUNC,      //!< DELETEDB_FUNC
	READDB_FUNC,        //!< READDB_FUNC
	GETDBSIZE_FUNC,     //!< GETDBSIZE_FUNC
	GETDBVALUE_FUNC,    //!< GETDBVALUE_FUNC
	GetCURTXHASH_FUNC,  //!< GetCURTXHASH_FUNC
	MODIFYDBVALUE_FUNC ,  //!< MODIFYDBVALUE_FUNC
	WRITEOUTPUT_FUNC,     //!<WRITEOUTPUT_FUNC
	GETSCRIPTDATA_FUNC,		  //!<GETSCRIPTDATA_FUNC
	GETSCRIPTID_FUNC,		  //!<GETSCRIPTID_FUNC
	GETCURTXACCOUNT_FUNC,		  //!<GETSCRIPTID_FUNC
	GETCURTXCONTACT_FUNC,		 //!<GETCURTXCONTACT_FUNC
	GETCURDECOMPRESSCONTACR_FUNC,   //!<GETCURDECOMPRESSCONTACR_FUNC
	GETDECOMPRESSCONTACR_FUNC,   	//!<GETDECOMPRESSCONTACR_FUNC
	GETCURPAYMONEY_FUN,             //!<GETCURPAYMONEY_FUN

	//add by ranger
	GET_APP_USER_ACC_VALUE_FUN,             //!<GET_APP_USER_ACC_FUN
	GET_APP_USER_ACC_FUND_WITH_TAG_FUN,             //!<GET_APP_USER_ACC_FUND_WITH_TAG_FUN
	GET_WIRITE_OUT_APP_OPERATE_FUN,             //!<GET_WIRITE_OUT_APP_OPERATE_FUN
}CALL_API_FUN;
typedef enum tagCOMPRESS_TYPE {
	U16_TYPE = 0,					// U16_TYPE
	I16_TYPE = 1,					// I16_TYPE
	U32_TYPE = 2,					// U32_TYPE
	I32_TYPE = 3,					// I32_TYPE
	U64_TYPE = 4,					// U64_TYPE
	I64_TYPE = 5,					// I64_TYPE
	NO_TYPE = 6,                   // NO_TYPE +n (tip char)
}COMPRESS_TYPE;
__root __code static const char version[]@0x0004 = {0x00,0x01,0x01};
__root __code static const char exitcall[]@0x0008 = {0x22,0x22};
__root __code static const char apicall[]@0x0012 = {0x22,0x22};
__root __xdata __no_init static  unsigned char Communicate[4*1024]@0xEFFF;
__root __xdata __no_init static unsigned char Result[2]@0xEFFD;

__noreturn  void __VmExit(EXIT_CODE tep) {
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
		LogPrint("IsOverShortFlow error",sizeof("IsOverShortFlow error")+1,STRING);
		__VmExit(RUN_SCRIPT_DATA_ERR);
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
unsigned long GetCurRunEnvHeight() {
	ClearParaSpace()
	;
	__CallApi(GETCTXCONFIRMH_FUNC);
	#pragma data_alignment = 1
	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len != 4) {
		return 0;
	}
	unsigned long height = 0;
	memcpy(&height, retdata->buffer, sizeof(long));
	return height;
}


bool GetTxContacts(const unsigned char * const txhash,void* const pcotact,const unsigned short maxLen) {
	ClearParaSpace()
	;
	InsertOutData(txhash, 32);
	__CallApi(GETTX_CONTRACT_FUNC);
	#pragma data_alignment = 1
	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len > maxLen || retdata->len == 0) {
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
	if (retdata->len != 33) {
		return 0;
	}
	memcpy(pubkey, retdata->buffer, retdata->len);
	return 33;
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
bool WriteData(const void* const key,const unsigned char keylen,const void * const value,const unsigned short valuelen) {
	ClearParaSpace();
//	if(keylen <=0 || valuelen<=0)
//	{
//		return false;
//	}
	InsertOutData(key, keylen);
	InsertOutData(value, valuelen);
	__CallApi(WRITEDB_FUNC);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len != 1) {
		return false;
	}
//	bool ret = false;
//	memcpy(&ret, retdata->buffer, 1);
//	return ret;
	return retdata->buffer[0];
}
bool DeleteData(const void* const key,const unsigned char keylen) {
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

unsigned short ReadData(const void* const key,const unsigned char keylen, void* const value,unsigned short const maxbuffer) {
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
	return retdata->len;

}
bool ModifyData(const void* const key,const unsigned char keylen, const void* const pvalue,const unsigned short valuelen) {
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
//	bool flag;
//	memcpy(&flag, retdata->buffer, 1);
	return retdata->buffer[0];
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
bool GetDBValue(const unsigned long index,void* const key,unsigned char * const keylen,unsigned short maxkeylen,void* const value,unsigned short* const maxbuffer)
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
	retdata = (FUN_RET_DATA *) ((unsigned char*) retdata->buffer + retdata->len);
	if (retdata->len > *maxbuffer) {
		return false;
	}
	memcpy(value, retdata->buffer, retdata->len);
	*maxbuffer = retdata->len;

	return true;
}

bool GetBlockHash(const unsigned long height,void * const pblochHash) {
	ClearParaSpace()
	;
	InsertOutData(&height, sizeof(height));
	__CallApi(GETBLOCKHASH_FUNC);
	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len != 32) {
		return false;
	}
	memcpy(pblochHash, retdata->buffer,32);
	return true;
}
bool GetCurTxHash(void * const poutHash) {
	__CallApi(GetCURTXHASH_FUNC);
	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len != 32) {
		return false;
	}
	memcpy(poutHash, retdata->buffer, retdata->len);
	return true;
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
	if (retdata->len > maxlen) {
		return 0;
	}
	memcpy(pvalve, retdata->buffer, retdata->len);
	return retdata->len;
}
bool GetScriptID(void* const account)
{
	ClearParaSpace();
//	if(account == NULL)
//	{
//		return false;
//	}
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
	memcpy(pContact, retdata->buffer, retdata->len);
	return retdata->len;
}
unsigned short CurDeCompressContact(unsigned char *pformat,unsigned short formlen,void * const poutContact, unsigned short outmaxlen) {

	ClearParaSpace()
	;
	InsertOutData(pformat, formlen);
	__CallApi(GETCURDECOMPRESSCONTACR_FUNC);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len > outmaxlen || retdata->len == 0)
		return retdata->len;
	memcpy(poutContact, retdata->buffer, retdata->len);
	return retdata->len;
}

unsigned short DeCompressContact(unsigned char *pformat,unsigned short formlen,const unsigned char * const txhash, void * const poutContact,
		unsigned short outmaxlen) {

	ClearParaSpace()
	;
	InsertOutData(pformat, formlen);
	InsertOutData(txhash, 32);
	__CallApi(GETDECOMPRESSCONTACR_FUNC);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len > outmaxlen || retdata->len == 0)
		return retdata->len;
	memcpy(poutContact, retdata->buffer, retdata->len);
	return retdata->len;
}
bool GetCurPayAmount(Int64* const pM2){
	ClearParaSpace();
	__CallApi(GETCURPAYMONEY_FUN);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len != sizeof(Int64)) {
			return false;
		}
	memcpy(pM2, retdata->buffer, sizeof(Int64));
	return true;
}

bool GetUserAppAccFreeValue( Int64* const pRet,const S_APP_ID *  pAppID)
{
	ClearParaSpace()
	;
	InsertOutData(pAppID, sizeof(*pAppID));
	__CallApi(GET_APP_USER_ACC_VALUE_FUN);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len != sizeof(Int64))
		return false;
	memcpy(pRet, retdata->buffer, sizeof(Int64));
	return true;
}

bool GetUserAppAccFoudWithTag(Int64* const pRet,APP_ACC_OPERATE const *  pAppID)
{
	ClearParaSpace()
	;
	InsertOutData(pAppID, sizeof(*pAppID));
	__CallApi(GET_APP_USER_ACC_FUND_WITH_TAG_FUN);

	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len != sizeof(Int64))
		return false;
	memcpy(pRet, retdata->buffer, sizeof(Int64));
	return true;
}




bool WriteAppOperateOutput( const APP_ACC_OPERATE* pOpertate, const unsigned short conter) {
	ClearParaSpace()
	;
	InsertOutData(pOpertate, conter * sizeof(*pOpertate));
	__CallApi(GET_WIRITE_OUT_APP_OPERATE_FUN);
	return true;
}

void PrintfLine(unsigned short sort)
{
	char bffer[20];
	sprintf(bffer,"line:%d ",sort);
	LogPrint(bffer,strlen(bffer),STRING);
}

void  PrintfFileAndLine(unsigned short line, const char *pfile)
{
	char bffer[256];
	sprintf(bffer,"func:%sline:%d ",pfile, line);
	LogPrint(bffer,strlen(bffer),STRING);
}

