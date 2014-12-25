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
/***
 *操作账户支持的操作
 */
enum OperType {
	ADD_FREE = 1, //!< ADD_FREE
	MINUS_FREE,   //!< MINUS_FREE
	ADD_SELF,     //!< ADD_SELF
	MINUS_SELF,   //!< MINUS_SELF
	ADD_FREEZD,   //!< ADD_FREEZD
	MINUS_FREEZD, //!< MINUS_FREEZD
	NULL_OPERTYPE,//!< NULL_OPERTYPE
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
	OperType opeatortype;           //!< operate the account
	unsigned long outheight;       //!< out of the height
	Int64 money;
} VM_OPERATE;

/**
 *ACCOUNT_RET struct
 *
 */
typedef struct tagACCOUNT
{
	ACCOUNT_TYPE type;      //!< the len of account  20 indicate that key Id less than 9 that accountID
	unsigned char Data[20]; //!< ID
}ACCOUNT_RET;
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
/**@brief  insert data to The exchange of data memory
 *@param pfrist:
 *@param len: the pfrist of length
 *@return void
 *
 */
void InsertCheckData(const void * pfrist, const unsigned short len);

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
bool WriteDataDB(const void* const key,const unsigned char keylen,const void * const value,const unsigned short valuelen,const unsigned long time);
/**@brief
 *delete data to the pScriptid table
 *@param key: delete key
 *@param keylen: the key's length
 *@return true  write success
 *
 */
bool DeleteDataDB(const void* const key,const unsigned char keylen);
/**@brief
 *query data through key from the pScriptid table
 *@param key: query key
 *@param keylen: the key length
 *@param value: query result put in value
 *@param maxbuffer: write to value data's length must less maxbuffer
 *@return unsigned short : output value's length
 *
 */
unsigned short ReadDataValueDB(const void* const key,const unsigned char keylen, void* const value,unsigned short const maxbuffer);
/**
 * @brief get outtime
 * @param key:
 * @param keylen: the key length
 * @param ptime:
 * @return
 */
bool ReadDataDBTime(const void* const key,const unsigned char keylen, unsigned long * const ptime);
/**@brief
 *Modify data through key from the pScriptid table
 *@param key: throng find key to modify value and time
 *@param keylen: the key length
 *@param pvalue: modify value to pvalue
 *@param valuelen: the pvalue length
 *@param ptime: modify the database time to ptime
 *@return true modify success
 *
 */

bool ModifyDataDB(const void* const key,const unsigned char keylen, const void* const pvalue,const unsigned short valuelen,const unsigned long ptime);

/**
 *brief
 *Modify data through key from the pScriptid table
 * @param key
 *  @param keylen: the key length
 * @param pvalue:
 * @param valuelen: the pvalue length
 * @return
 */
bool ModifyDataDBVavle(const void* const key,const unsigned char keylen, const void* const pvalue,const unsigned short valuelen);
/**
 * @brief modify outtime through key
 * @param key: key's word
 * @param keylen: the key length
 * @param ptime
 * @return
 */
bool ModifyDataDBTime(const void* const key,const unsigned char keylen, const unsigned long ptime);
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

bool GetDBValue(const unsigned long index,void* const key,unsigned char * const keylen,unsigned short maxkeylen,void* const value,unsigned short* const maxbuffer, unsigned long* const ptime);
/**@brief
 *get current tip block height
 *@return return tip block height
 *
 */
unsigned long GetTipHeight();
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
/**
 * @brief
 * the account the Authorited
 * @param account: the account's address
 * @param pmoney: the money
 * @return: true Expressed permission false No authorization
 */
bool IsAuthorited(const void* const account,const Int64* const pmoney);

unsigned long GetMemeroyData(void * const pfrist, unsigned long const len);
bool IsRegID(const void* const account);
/**@brief
 *@param account: the account id ,the account of length is 6
 *@param pout: run the functoin the define char put into pout
 *@param maxlen: the pout of length
 *@return return the pout of length
 *
 */
unsigned short GetAuthUserDefine(const void* const account,void *const pout,const unsigned short maxlen);
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
bool GetCurScritpAccount(void* const account);
/**@brief
 *@param account: obtain the currnet tx sign account
 *@param maxlen: the account of length
 *@return return true
 *
 */
unsigned short GetCurTxAccount(void * const account,unsigned short maxlen);
unsigned short GetCurTxContact(void * const pContact,unsigned short maxlen);
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

#endif /* VMSDK_H_ */

