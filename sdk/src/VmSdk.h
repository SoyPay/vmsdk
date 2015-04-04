#include <string.h>
#include<stdlib.h>
#include <stdio.h>

#ifndef VMSDK_H_
#define VMSDK_H_

#define MAX_LEN 21
#define MAX_PUK_LEN 65
#define MAX_ACCOUNT_LEN 6
#define check(a) {if(!(a)) { __exit(RUN_SCRIPT_DATA_ERR);}}


/**
 *LogPint format
 */
enum PRINT_FORMAT {
	STRING =0 ,//!< STRING
	HEX = 1,   //!< HEX
};


//// operate the account

enum OperType {
	ADD_FREE = 1, //!< ADD_FREE
	MINUS_FREE,   //!< MINUS_FREE
};
/*
 *the function exit
 */
enum EXIT_CODE {
	RUN_SCRIPT_DATA_ERR = 0x00,//!< RUN_SCRIPT_DATA_ERR
	RUN_SCRIPT_OK =  0x01,  //!< RUN_SCRIPT_OK
};


/***
 *
 */
enum COMP_RET {
	COMP_EQU = 0,   //!< COMP_EQU
	COMP_LESS = 1,  //!< COMP_LESS
	COMP_LARGER = 2,//!< COMP_LARGER
	COMP_ERR = 3,   //!< COMP_ERR
};
typedef struct tag_INT64 {
	unsigned char data[8];
} Int64;


static const int MAX_TAG_SIZE  = 40;
static const int MAX_UDER_ID_SIZE  = 40;
enum APP_OP_TYPE{
	ADD_FREE_OP = 1,
	SUB_FREE_OP,
	ADD_FREEZED_OP,
	SUB_FREEZED_OP
};
struct S_APP_ID
{
	unsigned char idlen;                    //!the len of the tag
	unsigned char ID[MAX_UDER_ID_SIZE];     //! the ID for the
};

struct APP_ACC_OPERATE
{
	unsigned char opeatortype;		//!OperType
	unsigned long outheight;		    //!< the transacion Timeout height
	Int64 mMoney;			        //!<The transfer amount
	S_APP_ID AppAccID;				//!< accountid
	S_APP_ID FundTag;               //!the fund tag used for find found
};


/**
 * @brief 初始化 Int64 数据 Int64 在IAR for 8051 里不支持 \n
 * @param p64
 * @param pdata 二进制数据只支持这种格式"\x03\xff\xff\xfc\x04"
 * @param len  the max len is 8
 * @return  ture ok  false error
 */
bool Int64Inital(Int64 * p64,char const *pdata, unsigned char len);

typedef struct tagACCOUNT_ID
{
	char accounid[MAX_ACCOUNT_LEN];
}ACCOUNT_ID;


typedef struct tagVMOPERATE{
	unsigned char accountid[6];      //!< account id
	unsigned char opeatortype;           //!< operate the account
	unsigned long outheight;       //!< out of the height
	Int64 money;
} VM_OPERATE;

/**
 *@brief Comparison of two digital uppercase
 *@param pM1:
 *@param pM2:
 *@return compare two digital result
 *
 */
COMP_RET Int64Compare(const Int64* const pM1, const Int64* const pM2);
/**
 *
 * @param pM1
 * @param pM2
 * @param pOutM
 * @return
 */
bool Int64Mul(const Int64* const pM1, const Int64* const pM2, Int64* const pOutM);
/**
  @brief two digital div
 *@param pM1: The divisor
 *@param pM2: Dividend
 *@param pOutM:pM1 div pM2 result put int poutM
 *@return:true run ok
 *
 */
bool Int64Div(const Int64* const pM1, const Int64* const pM2, Int64* const pOutM);
/**@brief  two digital add
 *@param pM1:
 *@param pM2:
 *@param pOutM:pM1 add pM2 result put int poutM
 *@return true run ok
 *
 */
bool Int64Add(const Int64* const pM1, const Int64* const pM2, Int64* const pOutM);
/**@brief  two digital sub
 *@param pM1:
 *@param pM2:
 *@param pOutM:pM1 sub pM2 result put int poutM
 *@return true run ok
 *
 */
bool Int64Sub(const Int64* const pM1, const Int64* const pM2, Int64* const pOutM);
/**@brief two digital sub
 *@param pfrist: Calculation of hash256 data
 *@param len:  the pfrist of length
 *@param pout: Calculation of hash256 data put in the pout
 *@return true run ok
 *
 */
bool SHA256(void const* pfrist, const unsigned short len, void * const pout);
/**@brief  Verify encrypted data
 *@param data: encrypted data
 *@param datalen: the data of length
 *@param key: Using your private exchange key to decrypt
 *@param keylen: the key of length
 *@param phash: Before encrypting data hash value
 *@param hashlen: the phashof length
 *@return true run ok
 *
 */
bool SignatureVerify(void const* data, unsigned short datalen, void const* key, unsigned short keylen,
		void const* phash, unsigned short hashlen);
/**@brief  Des or 3Des Calculation
 *@param pdata: data to encrypt or decrypt
 *@param len: the length of pdata
 *@param pkey: The secret key,8 bytes for Des,16 bytes for 3Des,others are invalid。
 *@param keylen: the length of pkey，8 bytes for Des,16 bytes for 3Des,others are invalid。
 *@param IsEn: if true encrypt false decrypt
 *@param pOut: result of encrypt or decrypt the return of length is The integral multiples of 8
 *@param poutlen: the pOut of length
 *@return true run ok
 *
 */
unsigned short Des(void const* pdata, unsigned short len, void const* pkey, unsigned short keylen, bool IsEn, void * const pOut,unsigned short poutlen);
/**@brief  exit the system
 *@param tep: RUN_SCRIPT_OK the script run ok RUN_SCRIPT_DATA_ERR
 *@return void
 */
__noreturn __root void __exit(EXIT_CODE tep);

/**@brief  print data to screen or file
 *@param pdata: the pdata Display in the screen or be writed in the file
 *@param datalen: the pdata of length
 *@param flag: PRINT_FORMAT
 *@return void
 *
 */
void LogPrint(const void *pdata, const unsigned short datalen,PRINT_FORMAT flag );
/**@brief  write the data to The exchange of data memory one by one
 *@param data: write int the memory
 *@param conter: the retpacket of count
 *@return void
 *
 */
bool WriteOutput( const VM_OPERATE* data, const unsigned short conter);
/**@brief
 *obtain current tx's  Confirmation block height
 *@return the  block's height keep int the height
 *
 */
unsigned long GetCurRunEnvHeight();

/**@brief
 *get tx contact
 * @param txhash: the tx hash
 * @param pcotact: the tx's contact
 * @param maxLen: the pcontac of max length
 * @return:return true get contact sucess
 */
bool GetTxContacts(const unsigned char * const txhash,void* const pcotact,const unsigned short maxLen);


/**@brief
 *  get tx accounts
 * @param txhash:  the tx hash
 * @param paccoutn:   the signed account every account is six char
 * @param maxlen:   the max ram cache  prevent overflow
 * @return the all of account's length
 */
unsigned short GetAccounts(const unsigned char *txhash,void* const paccoutn,unsigned short maxlen) ;


/**@brief
 *obtain some of tx's  public content
 *@param accounid: accountid and the length is six
 *@param pubkey: obtain the tx publickey keep in int the  pubkey
 *@param maxlength: maxlength<MAXPUKLEN \n
 *the  tx's pubkey Contain(flag(one char true explain the function can get the contact Correct)+data(char*))
 *@return void
 *  
 */
unsigned short GetAccountPublickey(const void* const accounid,void * const pubkey,const unsigned short maxlength);

/**@brief
 * get current script account's Balance 
 *@param account: the system's account address
 *@param pBalance: the account's balance put in Balance
 *@return void
 *
 */
bool QueryAccountBalance(const unsigned char* const account,Int64* const pBalance);
/**@brief
 *obtain the tx's  Confirmation block height
 *@return the  block's height keep int the height
 *
 */
unsigned long GetTxConFirmHeight(const void * const txhash) ;

/**@brief
 *@param key: insert key ,key must be string
 *@param keylen: the key length keylen <=8
 *@param value: insert value ,the value no need to string
 *@param valuelen:
 *@param time: save data in database time
 *@return true write success
 *
 **/
bool WriteData(const void* const key,const unsigned char keylen,const void * const value,const unsigned short valuelen);
/**@brief
 *delete data to the pScriptid table
 *@param key: delete key
 *@param keylen: the key's length
 *@return true  write success
 *
 */
bool DeleteData(const void* const key,const unsigned char keylen);
/**@brief
 *query data through key from the pScriptid table
 *@param key: query key
 *@param keylen: the key length
 *@param value: query result put in value
 *@param maxbuffer: write to value data's length must less maxbuffer
 *@return unsigned short : output value's length
 *
 */
unsigned short ReadData(const void* const key,const unsigned char keylen, void* const value,unsigned short const maxbuffer);

/**
 *brief
 *Modify data through key from the pScriptid table
 * @param key
 *  @param keylen: the key length
 * @param pvalue:
 * @param valuelen: the pvalue length
 * @return
 */
bool ModifyData(const void* const key,const unsigned char keylen, const void* const pvalue,const unsigned short valuelen);

/**@brief
 *get the database The number of keys
 *@return if the database exist data return the the number keys's count
 *
 */
unsigned long GetDBSize();
/**@brief
 *get the database data through index
 *@param index: get the database data through index,第一次获取的时候,index = 0,接着后面遍历index = 1
 *@param key: through index obtain the key save in key ,index = 0,key 值可以不用填,index = 1是，传进去的key的值为前一次获取key的值，获取到下一个key值赋值到key中去
 *@param keylen: the key's length
 *@param maxkeylen: the key recive char max length
 *@param value: The corresponding value of key save in value
 *@param maxbuffer: the write data to value data's length less maxValueLen
 *@param ptime: the time out
 *@return true obtain data success
 *
 */

bool GetDBValue(const unsigned long index,void* const key,unsigned char * const keylen,unsigned short maxkeylen,void* const value,unsigned short* const maxbuffer);

/**@brief
 *Access to the specified block height hash
 *@param height: the block heigh
 *@param pblochHash: get the block's height hash save in pblochHash
 *@return return current block height
 *
 */
bool GetBlockHash(const unsigned long height,void * const pblochHash);
/**@brief
 *Get current tx's hash
 *@param poutHash: the tx hash
 *@return return current block height
 */
bool GetCurTxHash(void * const poutHash);

bool IsRegID(const void* const account);

/**@brief
 *@param scriptID: the scriplt id ,the account of length is 6
 *@param pkey: the key value
 *@param len: the pkey of length
 *@param pvalve: the value input the pvalue
 *@param maxlen: the lenght of pvalve
 *@return return true
 *
 */
bool GetScriptData(const void* const scriptID,void* const pkey,short len,void* const pvalve,short maxlen);
/**@brief
 *@param account: the scriplt account id ,the account of length is 6
 *@param account: run the functoin the define char put into pout
 *@return return true
 *
 */
bool GetScriptID(void* const account);

bool GetCurPayAmount(Int64* const pM2);
/**@brief
 *@param account: obtain the currnet tx sign account
 *@param maxlen: the account of length
 *@return return true
 *
 */
unsigned short GetCurTxAccount(void * const account,unsigned short maxlen);
unsigned short GetCurTxContact(void * const pContact,unsigned short maxlen);
unsigned short GetDeCompressContact(void * const pinContact,unsigned short inlen,void * const poutContact,unsigned short outmaxlen);

/**@brief
 *@param pRet: the ret app account value
 *@param pUserID: the user app acount id
 *@param Idlen   :  the userID len
 *@return return true or false
 *
 */
bool GetUserAppAccFreeValue(Int64* const pRet,const S_APP_ID  * const pAppUserID);
/**@brief
 * 		get the app user freezed value  by tag
 *@param pRet: the ret app account value
 *@param pAppUserID : the user app acount id and fund tag
 *@return return true or false
 *
 */
bool GetUserAppAccFoudWithTag(Int64* const pRet,APP_ACC_OPERATE const *  pAppUserID);

/**@brief
 * 		 out put the operater for vm to  operate
 *@param  pOpertate:
 *@param conter : the conter of APP_ACC_OPERATE
 *@return return true  never false
 *
 */
bool WriteAppOperateOutput( const APP_ACC_OPERATE* pOpertate, const unsigned short conter);


void inline PrintfLine(unsigned short sort)
{
	char bffer[20]={0};
	sprintf(bffer,"line:%d ",sort);
	LogPrint(bffer,strlen(bffer),STRING);
}
void inline PrintfFileAndLine(unsigned short line, const char *pfile)
{
	char bffer[256]={0};
	sprintf(bffer,"func:%sline:%d ",pfile, line);
	LogPrint(bffer,strlen(bffer),STRING);
}
#define PrintLog(a,b,c) {if(!(a)) { PrintfLine(__LINE__),LogPrint(a,b,c);}}
//long ReadVarLong(char*buffer);
//Int64 ReadVarInt64(char*buffer);
//void Stringleft(char *str);
#endif /* VMSDK_H_ */

