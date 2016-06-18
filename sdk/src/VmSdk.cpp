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
	GET_DACRS_ADDRESS_FUN,                      //!<GET_DACRS_ADDRESS_FUN
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
__root __code static const char version[]@0x0004 = {0x00,0x02,0x02};  // {0x00,0x01,0x01}
//__root __code static const char version[]@0x0004 = {0x00,0x01,0x01};
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

unsigned short GetVecLen(unsigned char const * data) {
	return *((unsigned short*)data);
}

void InsertOutData(void const* pfrist, unsigned short len) {
	unsigned char * pbuffer = (unsigned char *) GetInterflowP();
	unsigned short len2 = GetVecLen(pbuffer);
	unsigned short maxlen = sizeof(Communicate)-2-len2;

	if(len >= maxlen )
	{
		LogPrint("IsOverShortFlow error",sizeof("IsOverShortFlow error")+1,STRING);
		__VmExit(RUN_SCRIPT_DATA_ERR);
	}
	unsigned short len3 = len2 + (len + sizeof(len));

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
	memcpy(&height, retdata->buffer, sizeof(height));  //long
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
	if (retdata->len != sizeof(unsigned long)) {
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
	if (retdata->len != sizeof(unsigned long)) {
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
    if(retdata->len > 0)
    {
    	if(retdata->len <= maxlen)
    	{
    		memcpy(pContact, retdata->buffer, retdata->len);
    	}else{
    		memcpy(pContact, retdata->buffer, maxlen);
    	}
    }
//    memcpy(pContact, retdata->buffer, retdata->len);
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
bool GetCurTxPayAmount(Int64* const pM2){
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
bool GetDacrsAddress(void * const account,unsigned short len,void* const address,unsigned short retmaxlen){
	ClearParaSpace()
	;
	InsertOutData(account, len);
	__CallApi(GET_DACRS_ADDRESS_FUN);
	FUN_RET_DATA *retdata = GetInterflowP();
	if (retdata->len > retmaxlen)
		return false;
	memcpy(address, retdata->buffer,retdata->len);
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


//////////////////////
////下述是给应用 的公共 充值提现接口

enum GETDAWEL{
	TX_REGID = 0x01,
	TX_BASE58 = 0x02,
};
typedef struct {
	unsigned char type;
	Int64 money;
}DAWEL_DATA;

static bool WriteWithDrawal(const char *account,Int64 money){
	VM_OPERATE writeCode[2];
	VM_OPERATE ret;
	ret.type = REG_ID;
	memcpy(ret.accountid,account,sizeof(ret.accountid));
	memcpy(&ret.money,&money,sizeof(Int64));
	ret.opeatortype = ADD_FREE;
	writeCode[0]=ret;

	char accountid[6] = {0};
	if(!GetScriptID(&accountid)){
		return false;
	}

	ret.opeatortype = MINUS_FREE;
	memcpy(ret.accountid,&accountid,sizeof(ret.accountid));
	writeCode[1]=ret;
	WriteOutput((VM_OPERATE*)&writeCode,2);
	return true;
}
/*
* @brief 	step1:检查传来的地址的合法性\n
* 			step2:检查账户中能够体现的自由金额不能为零\n
* 			step3:将自由金额到了账户中,同时脚本账户中要减去相应的金额\n
*/
static bool WithDrawal(char type){

	if(type != TX_REGID&&type != TX_BASE58)
		return false;
	char account[6];
	if(!GetCurTxAccount(account,sizeof(account))){
		return false;
	}
	S_APP_ID id;
	if(type == TX_REGID){
		id.idlen = sizeof(account);
		memcpy(&id.ID[0],&account[0],sizeof(account));
	}else if(type == TX_BASE58){
		char dacrsaddr[35] ={0};
		if(!GetDacrsAddress(account,sizeof(account),dacrsaddr,sizeof(dacrsaddr)))
			{
				LogPrint("get base58addr error",sizeof("get base58addr error")+1,STRING);
				return false;
			}
		id.idlen = strlen((char*)dacrsaddr);
		memcpy(&id.ID[0],&dacrsaddr[0],sizeof(dacrsaddr));
	}

	Int64 ret;
	if(!GetUserAppAccFreeValue(&ret,&id)){
		LogPrint("get accountfree value error ",sizeof("get accountfree value error")+1,STRING);
		return false;
	}
	Int64 compare;
	Int64Inital(&compare,"\x00",1);
	if(Int64Compare(&ret, &compare) == COMP_EQU){
		LogPrint("the account money is zero",sizeof("the account money is zero")+1,STRING);
		   return false;
	}

	APP_ACC_OPERATE pAppID;
	memcpy(&pAppID.AppAccID,&id,sizeof(S_APP_ID));
	memcpy(&pAppID.mMoney,&ret,sizeof(Int64));
	pAppID.FundTag.idlen = 0 ;
	pAppID.opeatortype = SUB_FREE_OP;

	if(!WriteAppOperateOutput(&pAppID, 1)){
				return false;
			}

  if(!WriteWithDrawal(account,ret)){
	  return false;
  }
   return true;
}
//// 提现一定的金额
static bool WithDrawalSomeMoney(const DAWEL_DATA* const pContract){

	if(pContract->type != TX_REGID&&pContract->type != TX_BASE58)
		return false;
	char account[6];
	if(!GetCurTxAccount(account,sizeof(account))){
		return false;
	}
	S_APP_ID id;
	if(pContract->type == TX_REGID){
		id.idlen = sizeof(account);
		memcpy(&id.ID[0],&account[0],sizeof(account));
	}else if(pContract->type == TX_BASE58){
		char dacrsaddr[35] ={0};
		if(!GetDacrsAddress(account,sizeof(account),dacrsaddr,sizeof(dacrsaddr)))
			{
				LogPrint("get base58addr error",sizeof("get base58addr error")+1,STRING);
				return false;
			}
		id.idlen = strlen((char*)dacrsaddr);
		memcpy(&id.ID[0],&dacrsaddr[0],sizeof(dacrsaddr));
	}

	Int64 ret;
	if(!GetUserAppAccFreeValue(&ret,&id)){
		LogPrint("get accountfree value error ",sizeof("get accountfree value error")+1,STRING);
		return false;
	}

	if(Int64Compare(&ret, &pContract->money) == COMP_LESS){
		LogPrint("the account money not enough money",sizeof("the account money not enough money")+1,STRING);
		   return false;
	}

	APP_ACC_OPERATE pAppID;
	memcpy(&pAppID.AppAccID,&id,sizeof(S_APP_ID));
	memcpy(&pAppID.mMoney,&pContract->money,sizeof(Int64));
	pAppID.FundTag.idlen = 0 ;
	pAppID.opeatortype = SUB_FREE_OP;

	if(!WriteAppOperateOutput(&pAppID, 1)){
				return false;
			}

  if(!WriteWithDrawal(account,pContract->money)){
	  return false;
  }
   return true;
}

//// 充值
static bool Recharge(void)
{

	char account[6];
	if(!GetCurTxAccount(account,sizeof(account))){
		return false;
	}
	Int64 paymoey,cmpmoney;
	Int64Inital(&paymoey,"\x00",1);
	Int64Inital(&cmpmoney,"\x00",1);
	 if((!GetCurTxPayAmount(&paymoey)) || (Int64Compare(&paymoey, &cmpmoney) == COMP_EQU)){
		  LogPrint("充值金额不正确或取金额错",sizeof("充值金额不正确或取金额错")+1,STRING);
		 return false;
	 }

	S_APP_ID id;
	APP_ACC_OPERATE pAppID;
	id.idlen = sizeof(account);
	memcpy(id.ID,account,sizeof(account));
	memcpy(&pAppID.AppAccID,&id,sizeof(S_APP_ID));
	memcpy(&pAppID.mMoney,&paymoey,sizeof(Int64));

	// 往应用账户自由金额打钱
	pAppID.opeatortype = ADD_FREE_OP;

	if(!WriteAppOperateOutput((APP_ACC_OPERATE*)&pAppID, 1)){
		return false;
	}

	return true;
}


/**充值，提现公共接口 */
bool RechargeWithdraw(unsigned char *pcontact) {
	if(pcontact[0] != 0xFF)
	{
		return false;
	}

//	LogPrint("WithDrawal", sizeof("WithDrawal"), STRING);
	if (pcontact[1] != 0x01 && pcontact[1] != 0x02 && pcontact[1] != 0x03) {
		__VmExit(RUN_SCRIPT_DATA_ERR);
	}
	if (pcontact[1] == 0x01) {
		LogPrint("WithDrawal", sizeof("WithDrawal"), STRING);
		if (!WithDrawal(pcontact[2])) {
			LogPrint("WithDrawal error", sizeof("WithDrawal error"), STRING);
			__VmExit(RUN_SCRIPT_DATA_ERR);
		}
	} else if (pcontact[1] == 0x02) {
		LogPrint("Recharge", sizeof("Recharge"), STRING);
		if (!Recharge()) {
			LogPrint("Recharge error", sizeof("Recharge error"), STRING);
			__VmExit(RUN_SCRIPT_DATA_ERR);
		}
	} else if (pcontact[1] == 0x03) {
		LogPrint("WithDrawalSomeMoney", sizeof("WithDrawalSomeMoney"), STRING);
		char buffer[12];
		memcpy(&buffer[0], &pcontact[2], 12);
		if (!WithDrawalSomeMoney((DAWEL_DATA*) (buffer))) {
			LogPrint("WithDrawalSomeMoney error", sizeof("WithDrawalSomeMoney error"), STRING);
			__VmExit(RUN_SCRIPT_DATA_ERR);
		}
	}
	return true;
}

