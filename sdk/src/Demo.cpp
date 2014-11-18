#include <string.h>
#include<stdlib.h>
#include <stdio.h>
#include"VmSdk.h"
extern unsigned char *GetMemeryData();
/*! \mainpage Developer documentation
 *
 * \section intro_sec Introduction
 *
 * This is the developer documentation of the reference client for an experimental new digital currency called Bitcoin (http://www.bitcoin.org/),
 * which enables instant payments to anyone, anywhere in the world. Bitcoin uses peer-to-peer technology to operate
 * with no central authority: managing transactions and issuing money are carried out collectively by the network.
 *
 * The software is a community-driven open source project, released under the MIT license.
 *
 Testing automatic link generation.

 * \section FuntionList
 *  bool Int64Mul(const Int64* const pM1, const Int64* const pM2, Int64* const pOutM);\n
 *  bool SHA256(void const* pfrist, unsigned short len, void * const pout);\n
 *  bool Int64Inital(Int64 * p64,char const *pdata, unsigned char len);\n
 *  COMP_RET Compare(const Int64* pM1, const Int64* pM2);\n
 *  bool Int64Div(const Int64* const pM1, const Int64* const pM2, Int64* const pOutM);\n
 *  bool IntAdd(const Int64* const pM1, const Int64* const pM2, Int64* const pOutM)\n
 *  bool Int64Sub(const Int64* const pM1, const Int64* const pM2, Int64* const pOutM);\n
 *  bool SignatureVerify(void const* data, unsigned short datalen, void const* key, unsigned short keylen,void const* phash, unsigned short hashlen);\n
 *  bool Des(void const* pdata, unsigned short len, void const* pkey, unsigned short keylen, bool IsEn, void * const pOut);\n
 *	void __exit(unsigned char tep);\n
 *	void InsertCheckData(void const* pfrist, unsigned short len);\n
 *	void LogPrint(const void *pdata, const unsigned short datalen,PRINT_FORMAT flag );\n
 *	bool WriteOutput( const VM_OPERATE* data, const unsigned short conter);\n
 *	unsigned long GetCurRunEnvHeight();\n
 *  bool GetTxSignedAccounts(const unsigned char *txhash,void* const paccoutn,unsigned short* const  maxLen);\n
 *  bool GetTxContacts(const unsigned char * const txhash,void* const pcotact,const unsigned short maxLen);\n
 *	bool GetAccountPublickey(const unsigned char* const accounid,void * const pubkey,const unsigned short maxlength);\n
 *	bool QueryAccountBalance(const unsigned char* const account,Int64* const pBalance);\n
 *	unsigned long GetTxConFirmHeight(const unsigned char * const txhash);\n
 *	bool WriteDataDB(const unsigned char* const key,const unsigned char keylen,const void * const value,const unsigned short valuelen,const unsigned long time);\n
 *	bool DeleteDataDB(const unsigned char* const key,const unsigned char keylen);\n
 *	unsigned short ReadDataValueDB(const unsigned char* const key,const unsigned char keylen, unsigned char* const value,unsigned short const maxbuffer);\n
 *	bool ReadDataDBTime(const unsigned char* const key,const unsigned char keylen, unsigned long * const ptime);\n
 *	bool ModifyDataDB(const unsigned char* const key,const unsigned char keylen, const void* const pvalue,const unsigned short valuelen,const unsigned long ptime);\n
 *	bool ModifyDataDBVavle(const unsigned char* const key,const unsigned char keylen, const void* const pvalue,const unsigned short valuelen);\n
 *	bool ModifyDataDBTime(const unsigned char* const key,const unsigned char keylen, const unsigned long ptime);\n
 *	unsigned long GetDBSize();\n
 *	unsigned long GetTipHeight();\n
 *	bool GetBlockHash(const unsigned long height,unsigned char * const pblochHash,unsigned short maxlen);\n
 *	void InsertOutData(void const* pfrist, unsigned short len);\n
 *  void ClearParaSpace(void);\n
 *  bool GetCurTxHash(unsigned char * const poutHash,unsigned short maxlen);\n
 * bool IsAuthorited(const unsigned char* const account,const Int64* const pmoney);\n
 *  bool GetMemeroyData(void const* pfrist,const unsigned long* len);\n




 \section Navigation
 * Use the buttons <code>Namespaces</code>, <code>Classes</code> or <code>Files</code> at the top of the page to start navigating the code.
 */

/**
 * @brief this the main function
 */

// class script funciton
//int main()
//{
//   /// GetCurTxConfirmHeight()
//	unsigned long height = GetCurRunEnvHeight();
//	char buffer[100] = {0};
//	sprintf(buffer,"GetCurRunEnvHeight:%d",height);
//	LogPrint(NULL,buffer, 100);
//
//	char* errormsg = NULL;
//	unsigned char poutHash[32] = {0};
//	bool flag =  GetCurTxHash((unsigned char *)poutHash,32);
//	if(!flag)
//	{
//		errormsg = "GetCurTxHash error";
//		LogPrint(NULL, errormsg, strlen(errormsg)+1);
//		__exit(RUN_SCRIPT_DATA_ERR); //break,not stop
//	}
//	else
//	{
//		LogPrint(NULL, poutHash,32);
//	}
//	const unsigned char* key = "key";
//	const void *value = "hellow";
//	 unsigned long time = 500;
//	 unsigned char kenlen = 4;
//	flag = WriteDataDB(key,kenlen,value,7,time);
//
//	if(!flag)
//	{
//		errormsg = "WriteDataDB error";
//		LogPrint(NULL, errormsg, strlen(errormsg)+1);
//		__exit(RUN_SCRIPT_DATA_ERR); //break,not stop
//	}
//
//
//	char pout[100] = {0};
//	unsigned short pvalen = 100;
//	flag = ReadDataValueDB(key,kenlen,(unsigned char*)pout,&pvalen);
//	if(!flag)
//	{
//		errormsg = "ReadDataDB error";
//		LogPrint(NULL, errormsg, strlen(errormsg)+1);
//		__exit(RUN_SCRIPT_DATA_ERR); //break,not stop
//	}
//	else
//	{
//		LogPrint(NULL, pout, 100);
//	}
//	unsigned long count =GetDBSize();
//	sprintf(buffer,"GetDBSize:%d",count);
//	LogPrint(NULL,buffer, 100);
//	unsigned long nptime = 0;
//	unsigned char nkey[50] = {0};
//	unsigned char nvalue[50] = {0};
//
//	kenlen = 50;
//	pvalen = 100;
//	flag  = GetDBValue(0,nkey,&kenlen,nvalue,&pvalen,&nptime);
//	if(!flag)
//	{
//		errormsg = "GetDBValue error";
//		LogPrint(NULL, errormsg, strlen(errormsg)+1);
//		__exit(RUN_SCRIPT_DATA_ERR); //break,not stop
//	}
//	else
//	{
//		char buffer[200] = {0};
//		sprintf(buffer,"GetDBValue key:%s,vale:%s,height:%d",nkey,nvalue,nptime);
//		LogPrint(NULL, buffer, 100);
//	}
//	 unsigned long ptime;
//        unsigned char  len =strlen((char*)key)+ 1;
//	flag = ReadDataDBTime(key,len,(unsigned long *)&ptime);
//	if(!flag)
//	{
//		errormsg = "ReadDataDBTime error";
//		LogPrint(NULL, errormsg, strlen(errormsg)+1);
//		__exit(RUN_SCRIPT_DATA_ERR); //break,not stop
//	}
//	else
//	{
//		sprintf(buffer,"ReadDataDB:%d",ptime);
//		LogPrint(NULL, buffer, 100);
//	}
//	value = "luojinlian";
//	time = 600;
//	flag = ModifyDataDB(key,kenlen,value,11,time);
//
//	if(!flag)
//	{
//		errormsg = "ModifyDataDB error";
//		LogPrint(NULL, errormsg, strlen(errormsg)+1);
//		__exit(RUN_SCRIPT_DATA_ERR); //break,not stop
//	}
//
//	value = "1213";
//	flag = ModifyDataDBVavle(key,kenlen,value,10);
//
//	time = 700;
//	flag = ModifyDataDBTime(key,kenlen, time);
//	if(!flag)
//	{
//		errormsg = "ModifyDataDBTime error";
//		LogPrint(NULL, errormsg, strlen(errormsg)+1);
//		__exit(RUN_SCRIPT_DATA_ERR); //break,not stop
//	}else
//	{
//		LogPrint(NULL, "ModifyDataDBTime success", 100);
//	}
//	flag = DeleteDataDB(key,kenlen);
//
//	if(!flag)
//	{
//		errormsg = "DeleteDataDB error";
//		LogPrint(NULL, errormsg, strlen(errormsg)+1);
//		__exit(RUN_SCRIPT_DATA_ERR); //break,not stop
//	}
//	else
//	{
//		LogPrint(NULL, "DeleteDataDB success", 100);
//	}
//
//	__exit(1);
//}
void test_exit()
{
	__exit(RUN_SCRIPT_DATA_ERR);
}

#define TestCheck(a) {if(!(a)) { PrintfLine(__LINE__); return false;}}

/**
 * @brief define the variable and call the api
 * @return
 */
bool testInt64Inital()
{
	Int64 m1;
	TestCheck(Int64Inital(&m1,"\xff",1) == true);
	TestCheck(Int64Inital(&m1,"\xff",20) == false);
	TestCheck(Int64Inital(&m1,"\xff\xff\xff\xff\xff\xff\xff\xff",8) == true);
	return true;
}

bool testCompare()
{
	Int64 m1,m2;
	Int64Inital(&m1,"\x01",1);
	Int64Inital(&m2,"\x01",1);
	TestCheck(Int64Compare(&m1,&m2) == COMP_EQU);
	Int64Inital(&m1,"\x02",1);
	Int64Inital(&m2,"\x01",1);
	TestCheck(Int64Compare(&m1,&m2) == COMP_LARGER);
	Int64Inital(&m1,"\x01",1);
	Int64Inital(&m2,"\x02",1);
	TestCheck(Int64Compare(&m1,&m2) == COMP_LESS);
	return true;
}
bool testadd()
{
	Int64 m1,m2,m3,m4,result;//0
	/// m1 边界值加上不是边界值 看是否正确
	Int64Inital(&m1,"\xff\xff\xff\xff\xff\xff\xff\xff",8);
	Int64Inital(&m2,"\x01", 1);
	Int64Inital(&result,"\x00", 1);
	Int64Add(&m1, &m2, &m3);

	unsigned char kk = Int64Compare(&m3, &result);
	TestCheck(COMP_EQU == kk);

	/// 正常值相加验证正确与否
	Int64Inital(&m1,"\x01",1);
	Int64Inital(&m2,"\x01", 1);
	Int64Inital(&m4,"\x02", 1);
	Int64Add(&m1, &m2, &m3);
	 kk = Int64Compare(&m3, &m4);
	 TestCheck(COMP_EQU == kk);

	 // 两个值相加正好等于边界值
	Int64Inital(&m1,"\xff\xff\xff\xff\xff\xff\xff\xfe",8);
	Int64Inital(&m2,"\x01", 1);
	Int64Inital(&m4,"\xff\xff\xff\xff\xff\xff\xff\xff", 8);
	Int64Add(&m1, &m2, &m3);
	 kk = Int64Compare(&m3, &m4);
	 TestCheck(COMP_EQU == kk);
	 return true;
}
bool testmul()
{
	Int64 m1,m2,m3,m4,result;//0
	///用例是两个边界值相乘
	Int64Inital(&m1,"\xff\xff\xff\xff\xff\xff\xff\xff",8);
	Int64Inital(&m2,"\xff\xff\xff\xff\xff\xff\xff\xff", 8);
	Int64Inital(&result,"\x01", 1);
	Int64Mul(&m1, &m2, &m3);

	unsigned char kk = Int64Compare(&m3, &result);
	TestCheck(COMP_EQU == kk);
	///用例是两个值相乘超过边界值
	Int64Inital(&m1,"\xff\xff\xff\xff\xff\xff\xff\x01",8);
	Int64Inital(&m2,"\x04", 1);
	Int64Inital(&m4,"\xff\xff\xff\xff\xff\xff\xfc\x04", 8);
	Int64Mul(&m1, &m2, &m3);
	kk = Int64Compare(&m3, &m4);
	TestCheck(COMP_EQU == kk);

	///用例是两个值相乘不超过边界值
	Int64Inital(&m1,"\xff\xff\xff\x01",4);
	Int64Inital(&m2,"\x04", 1);
	Int64Inital(&m4,"\x03\xff\xff\xfc\x04", 5);
	Int64Mul(&m1, &m2, &m3);
	 kk = Int64Compare(&m3, &m4);
	 TestCheck(COMP_EQU == kk);
	 return true;

}
bool testsub()
{
	Int64 m1,m2,m3,m4;//0
	///case:两个边界值相减
	Int64Inital(&m1,"\xff\xff\xff\xff\xff\xff\xff\xff",8);
	Int64Inital(&m2,"\xff\xff\xff\xff\xff\xff\xff\xff", 8);
	Int64Inital(&m4,"\x00", 1);
	Int64Sub(&m1, &m2, &m3);

	unsigned char kk = Int64Compare(&m3, &m4);
	TestCheck(COMP_EQU == kk);
	///case:边界值减去非边界值
	Int64Inital(&m2,"\x01", 1);
	Int64Inital(&m4,"\xff\xff\xff\xff\xff\xff\xff\xfe", 8);
	Int64Sub(&m1, &m2, &m3);
	 kk = Int64Compare(&m3, &m4);
	 TestCheck(COMP_EQU == kk);

	 // case:非边界值减去边界值
	Int64Inital(&m2,"\x01", 1);
	Int64Inital(&m4,"\x02", 1);
	Int64Sub(&m2, &m1, &m3);
	 kk = Int64Compare(&m3, &m4);
	 TestCheck(COMP_EQU == kk);

	 // case:非边界值减去边界值
	Int64Inital(&m1,"\x01",1);
	Int64Inital(&m2,"\x02", 1);
	Int64Inital(&m4,"\xff\xff\xff\xff\xff\xff\xff\xff", 8);
	Int64Sub(&m1, &m2, &m3);

	 kk = Int64Compare(&m3, &m4);
	 TestCheck(COMP_EQU == kk);
	 return true;

}
bool testdiv()
{
	Int64 m1,m2,m3,m4;//0
	 // case:非边界值除以非界值
	Int64Inital(&m1,"\xff\xff\xff\xff\xff\xff\xff\xff",8);
	Int64Inital(&m2,"\x01", 1);
	Int64Inital(&m4,"\xff\xff\xff\xff\xff\xff\xff\xff", 8);
	Int64Div(&m1, &m2, &m3);

	unsigned char kk = Int64Compare(&m3, &m4);
	TestCheck(COMP_EQU == kk);

	 // case:边界值除以边界值
	Int64Div(&m2, &m1, &m3);

	 kk = Int64Compare(&m3, &m4);
	 TestCheck(COMP_EQU == kk);

	 /// 会跟系统一样
//	Int64Inital(&m2,"\x00", 1);
//	bool flag = Int64Div(&m1, &m2, &m3);
//	TestCheck(flag == false);
	return true;
}
bool testSHA256()
{
	char* phashdata = NULL;
	unsigned short len= 0;
	char hash[32] = {0};
	TestCheck(SHA256(phashdata,len,hash) == false);
	char xx[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
	TestCheck(SHA256(xx,8,hash) == true);
    return true;
}
bool testSignatureVerify()
{
	char sig[] ={0};
	char key[] = {0};
	char hash[] = {0};
	TestCheck(SignatureVerify(sig, sizeof(sig), key, sizeof(key), hash, sizeof(hash)) == false);
	char sig1[] = { 0x30, 0x44, 0x02, 0x20, 0x0b, 0xe0, 0x2f, 0x1c, 0x72, 0xe0, 0x79, 0xec, 0xe1, 0x25, 0x49, 0x5b,
			0x29, 0x3a, 0x2d, 0x99, 0x51, 0x66, 0xc7, 0x0d, 0xfb, 0x09, 0x6f, 0x52, 0x56, 0x99, 0x0b, 0x44, 0x50,
			0xa0, 0x8d, 0xd9, 0x02, 0x20, 0x0a, 0x95, 0xc1, 0xf2, 0xb4, 0x18, 0xc0, 0xcc, 0x87, 0x55, 0x68, 0x65,
			0x43, 0x37, 0x60, 0x12, 0xb7, 0x58, 0xfe, 0x16, 0xac, 0x11, 0x16, 0x86, 0x6f, 0xdd, 0xf0, 0x04, 0xdd,
			0x43, 0x93, 0xb1 };
	char key1[] = { 0x03, 0x3e, 0x85, 0xac, 0xa4, 0xd7, 0x96, 0x5d, 0x8b, 0x46, 0x7b, 0x98, 0x29, 0xd5, 0x77, 0x4a,
			0xbd, 0xd9, 0xc6, 0x6a, 0x60, 0x18, 0xde, 0xe8, 0x5f, 0x82, 0xf1, 0xe2, 0x6b, 0xa2, 0x8b, 0x6d, 0xf2 };
	char hash1[] = { 0x01, 0x00, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x09, 0x08, 0x07, 0x06,
			0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00 };
	TestCheck(SignatureVerify(sig1, sizeof(sig1), key1, sizeof(key1), hash1, sizeof(hash1)) == true);
	/// change sig2 error
	char sig2[] = { 0x30, 0x44, 0x02, 0x20, 0x0b, 0xe0, 0x2f, 0x1c, 0x72, 0xe0, 0x79, 0xec, 0xe1, 0x25, 0x49, 0x5b,
			0x29, 0x3a, 0x2d, 0x99, 0x51, 0x66, 0xc7, 0x0d, 0xfb, 0x09, 0x6f, 0x52, 0x56, 0x99, 0x0b, 0x44, 0x50,
			0xa0, 0x8d, 0xd9, 0x02, 0x20, 0x0a, 0x95, 0xc1, 0xf2, 0xb4, 0x18, 0xc0, 0xcc, 0x87, 0x55, 0x68, 0x65,
			0x43, 0x37, 0x60, 0x12, 0xb7, 0x58, 0xfe, 0x16, 0xac, 0x11, 0x16, 0x86, 0x6f, 0xdf, 0xf0, 0x04, 0xdd,
			0x43, 0x93, 0xb1 };
	char key2[] = { 0x03, 0x3e, 0x85, 0xac, 0xa4, 0xd7, 0x96, 0x5d, 0x8b, 0x46, 0x7b, 0x98, 0x29, 0xd5, 0x77, 0x4a,
			0xbd, 0xd9, 0xc6, 0x6a, 0x60, 0x18, 0xde, 0xe8, 0x5f, 0x82, 0xf1, 0xe2, 0x6b, 0xa2, 0x8b, 0x6d, 0xf2 };
	char hash2[] = { 0x01, 0x00, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x09, 0x08, 0x07, 0x06,
			0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00 };
	TestCheck(SignatureVerify(sig2, sizeof(sig2), key2, sizeof(key2), hash2, sizeof(hash2)) == false);
	//// change key error
	char sig3[] = { 0x30, 0x44, 0x02, 0x20, 0x0b, 0xe0, 0x2f, 0x1c, 0x72, 0xe0, 0x79, 0xec, 0xe1, 0x25, 0x49, 0x5b,
			0x29, 0x3a, 0x2d, 0x99, 0x51, 0x66, 0xc7, 0x0d, 0xfb, 0x09, 0x6f, 0x52, 0x56, 0x99, 0x0b, 0x44, 0x50,
			0xa0, 0x8d, 0xd9, 0x02, 0x20, 0x0a, 0x95, 0xc1, 0xf2, 0xb4, 0x18, 0xc0, 0xcc, 0x87, 0x55, 0x68, 0x65,
			0x43, 0x37, 0x60, 0x12, 0xb7, 0x58, 0xfe, 0x16, 0xac, 0x11, 0x16, 0x86, 0x6f, 0xdd, 0xf0, 0x04, 0xdd,
			0x43, 0x93, 0xb1 };
	char key3[] = { 0x03, 0x3e, 0x85, 0xac, 0xa4, 0xd7, 0x96, 0x5d, 0x8b, 0x46, 0x7b, 0x98, 0x29, 0xd5, 0x77, 0x4a,
			0xbd, 0xd9, 0xc6, 0x6a, 0x60, 0x19, 0xde, 0xe8, 0x5f, 0x82, 0xf1, 0xe2, 0x6b, 0xa2, 0x8b, 0x6d, 0xf2 };
	char hash3[] = { 0x01, 0x00, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x09, 0x08, 0x07, 0x06,
			0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00 };
	TestCheck(SignatureVerify(sig3, sizeof(sig3), key3, sizeof(key3), hash3, sizeof(hash3)) == false);

	//// change hash error
	char sig4[] = { 0x30, 0x44, 0x02, 0x20, 0x0b, 0xe0, 0x2f, 0x1c, 0x72, 0xe0, 0x79, 0xec, 0xe1, 0x25, 0x49, 0x5b,
			0x29, 0x3a, 0x2d, 0x99, 0x51, 0x66, 0xc7, 0x0d, 0xfb, 0x09, 0x6f, 0x52, 0x56, 0x99, 0x0b, 0x44, 0x50,
			0xa0, 0x8d, 0xd9, 0x02, 0x20, 0x0a, 0x95, 0xc1, 0xf2, 0xb4, 0x18, 0xc0, 0xcc, 0x87, 0x55, 0x68, 0x65,
			0x43, 0x37, 0x60, 0x12, 0xb7, 0x58, 0xfe, 0x16, 0xac, 0x11, 0x16, 0x86, 0x6f, 0xdd, 0xf0, 0x04, 0xdd,
			0x43, 0x93, 0xb1 };
	char key4[] = { 0x03, 0x3e, 0x85, 0xac, 0xa4, 0xd7, 0x96, 0x5d, 0x8b, 0x46, 0x7b, 0x98, 0x29, 0xd5, 0x77, 0x4a,
			0xbd, 0xd9, 0xc6, 0x6a, 0x60, 0x19, 0xde, 0xe8, 0x5f, 0x82, 0xf1, 0xe2, 0x6b, 0xa2, 0x8b, 0x6d, 0xf2 };
	char hash4[] = { 0x01, 0x00, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x09, 0x08, 0x07, 0x06,
			0x07, 0x04, 0x03, 0x02, 0x01, 0x00, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00 };
	TestCheck(SignatureVerify(sig4, sizeof(sig4), key4, sizeof(key4), hash4, sizeof(hash4)) == false);
   return true;
}

bool testDes()
{
	unsigned char mm[50] = { 0 };
	TestCheck(Des("", 0, "", 0, true,mm,50) == 0);
	TestCheck(Des("", 0, "", 0, false,mm,50) == 0);
	// 加密数据
	unsigned short len = Des("\x01\x02\x03\x04\x05\xa7\x48\x89\x20\xb3\x8b\xd8\xe4", 13, "\x01\x02\x03\x04\x05\x06\x07\x08", 8, true,mm,50);
	TestCheck(len > 0);
	unsigned char des[16] = { 0 };
	TestCheck(Des(mm, len, "\x01\x02\x03\x04\x05\x06\x07\x08",8, false, des,16) == 16);
	//TestCheck(strcmp((char*)des,"\x01\x02\x03\x04\x05\xa7\x48\x89\x20\xb3\x8b\xd8\xe4") != 0);
	mm[len-5] ='\x09';
	mm[len-1] ='\x05';
	TestCheck(Des(mm, len, "\x01\x02\x03\x04\x05\x06\x07\x08",8, false, des,16) >0);
	//TestCheck(strcmp((char*)des,"\x01\x02\x03\x04\x05\xa7\x48\x89\x20\xb3\x8b\xd8\xe4") == 0);

	return true;
}
bool testLogPrint()
{
	LogPrint("testLogPrint TEST",sizeof("testLogPrint TEST")+1,STRING);
	LogPrint("testLogPrint TEST",sizeof("testLogPrint TEST")+1,HEX);
	return true;
}
bool testWriteOutput()
{
	VM_OPERATE data;
	WriteOutput( &data, 1);
	return true;
}
bool testGetCurRunEnvHeight()
{
	unsigned long height = GetCurRunEnvHeight();
	char buffer[50] = {0};
	sprintf(buffer,"testGetCurRunEnvHeight:%d",height);
	LogPrint(buffer,52,STRING);
	return true;
}
bool testGetTxContacts(char *phash)
{
	LogPrint(phash,32,HEX);
	char pcontact[50] = {0};
	GetTxContacts((unsigned char*)phash,pcontact,50);
	LogPrint("testGetTxContacts:",sizeof("testGetTxContacts:"),STRING);
	LogPrint(pcontact,50,HEX);
	return true;
}
bool testGetAccounts(char *phash)
{
	char pcontact[6] = {0};
	char * hash1= NULL;
	unsigned short length = GetAccounts((unsigned char*)hash1,pcontact,6);
	TestCheck(length == 0);

	length =GetAccounts((unsigned char*)phash,pcontact,6);
	TestCheck(length == 6);
	LogPrint("testGetAccounts:",sizeof("testGetAccounts:"),STRING);
	LogPrint(pcontact,6,HEX);
	return true;
}
bool testGetAccountPublickey(char *phash)
{
	char pcontact[6] = {0};
	char pubkey[33] = {0};
	char*temphash = NULL;
	unsigned  short length = GetAccountPublickey(temphash,pubkey,33);
	TestCheck(length == 33);
	GetAccounts((unsigned char*)phash,pcontact,6);

	length = GetAccountPublickey(pcontact,pubkey,33);
	TestCheck(length == 33);
	LogPrint("testGetAccountPublickey:",sizeof("testGetAccountPublickey:"),STRING);
	LogPrint(pubkey,33,HEX);
	return true;
}
bool testQueryAccountBalance(char *phash)
{
	char paccount[6] = {0};
	Int64 pBalance;
	TestCheck(QueryAccountBalance((unsigned char*)paccount,ACOUNT_ID,&pBalance) == true);
	LogPrint(&pBalance,8,HEX);
	GetAccounts((unsigned char*)phash,paccount,6);
	TestCheck(QueryAccountBalance((unsigned char*)paccount,ACOUNT_ID,&pBalance) == true);
	LogPrint(&pBalance,8,HEX);
	return true;
}
/// the api not uses
bool testGetTxConFirmHeight(char *txhash)
{
	unsigned long height = GetTxConFirmHeight(txhash);
	char buffer[50] = {0};
	sprintf(buffer,"testGetTxConFirmHeight:%d",height);
	LogPrint(buffer,50,STRING);
	return true;
}
bool testGetTipHeight()
{
	unsigned long height = GetTipHeight();
	char buffer[50] = {0};
	sprintf(buffer,"testGetTipHeight:%d",height);
	LogPrint(buffer,50,STRING);
	return true;
}
bool testGetBlockHash()
{
	char bhash[32] = {0};
	GetBlockHash(1,bhash);
	LogPrint("testGetBlockHash:",sizeof("testGetBlockHash:"),STRING);
	LogPrint(bhash,32,HEX);
	return true;
}
bool testGetCurTxHash()
{
	char txhash[32] = {0};
	GetCurTxHash(txhash);
	LogPrint("testGetCurTxHash:",sizeof("testGetCurTxHash:"),STRING);
	LogPrint(txhash,32,HEX);
	return true;
}
/// 没有授权
bool testIsAuthorited(char *phash)
{
	char paccount[6] = {0};
	Int64 pmoney;
	TestCheck(IsAuthorited(paccount,&pmoney) == false);
	GetAccounts((unsigned char*)phash,paccount,6);
	Int64Inital(&pmoney,"\x01", 1);
	TestCheck(IsAuthorited(paccount,&pmoney) == false);
	return true;
}
///授权的账户
bool testIsAuthorited1(char *phash)
{
	char paccount[6] = {0};
	Int64 pmoney;
	GetAccounts((unsigned char*)phash,paccount,6);
	Int64Inital(&pmoney,"\x01", 1);
	TestCheck(IsAuthorited(paccount,&pmoney) == true);
	Int64Inital(&pmoney,"\xff\xff\xff\xff\xff\xff\xff\xff", 8);
	TestCheck(IsAuthorited(paccount,&pmoney) == false);
	return true;
}

//// test db
bool testWriteDataDB()
{
	char* key =NULL;
	char *value = "hello";
	unsigned long time = 2;
	TestCheck(WriteDataDB(key,0,value,6,time) == false);
	TestCheck(WriteDataDB(key,9,value,0,time) == false);
	key = "key";
	value = NULL;
	TestCheck(WriteDataDB(key,4,value,6,time) == true);
	key = "key";
	value = "hello";
	TestCheck(WriteDataDB(key,4,value,6,time) == true);
	key = "key1";
	value = "hellow";
	TestCheck(WriteDataDB(key,5,value,7,time) == true);
	return true;
}
bool testDeleteDataDB()
{
	char* key =NULL;
	TestCheck(DeleteDataDB(key,0)== false);
	key = "key";
	TestCheck(DeleteDataDB(key,4)== true);
	return true;
}
bool testReadDataValueDB()
{
	char* key =NULL;
	char value[7] = {0};
	TestCheck(ReadDataValueDB(key,0,value,0) == 0);
	key = "key";
	TestCheck(ReadDataValueDB(key,4,value,0) == 0);
	TestCheck(ReadDataValueDB(key,4,value,7) == 0);
	key = "key1";
	TestCheck(ReadDataValueDB(key,5,value,7)> 0);
	TestCheck(strcmp(value,"hellow") ==0);
	return true;
}
bool testModifyDataDB()
{
	char* key =NULL;
	char *value = NULL;
	unsigned long ptime = 0;
	TestCheck(ModifyDataDB(key,0,value,7,ptime) == false);
	TestCheck(ModifyDataDB(key,5,value,0,ptime) == false);
	key = "sse";
	TestCheck(ModifyDataDB(key,4,value,0,ptime) == false);
	key = "key1";
	value= "LUO";
	ptime = 5;
	TestCheck(ModifyDataDB(key,5,value,4,ptime) == true);
	return true;
}
bool testGetDBSize()
{
	int b = GetDBSize();
	char buffer[10] = {0};
	sprintf(buffer,"dbsize:%d",b);
	LogPrint(buffer,sizeof(buffer),STRING);
	TestCheck(GetDBSize() == 1);

	return true;
}
bool testGetDBValue()
{
	char key[15] ={0};
	char value[15] = {0};
	unsigned char kenlen =0;
	unsigned short valen = 0;
	unsigned long ptime = 0;
	TestCheck(GetDBValue(0,key,(unsigned char*)&kenlen,0,value,&valen,&ptime)== false);
	kenlen = 5;
	TestCheck(GetDBValue(0,key,(unsigned char*)&kenlen,0,value,&valen,&ptime)== false);

	char *wkey = "bit";
	char *wvalue = "shit";
	ptime = 7;
	TestCheck(WriteDataDB(wkey,4,wvalue,5,ptime) == true);

	valen = 15;
	ptime = 0;
	kenlen = sizeof(key);
	TestCheck(GetDBValue(0,key,&kenlen,15,value,&valen,&ptime)== true);
	TestCheck(strcmp(key,"bit")== 0);
	TestCheck(strcmp(value,wvalue)== 0);
	TestCheck(ptime == 7);

	valen = 4;
	ptime = 0;
	TestCheck(GetDBValue(1,key,&kenlen,15,value,&valen,&ptime)== true);
	TestCheck(strcmp(key,"key1")== 0);
	TestCheck(strcmp(value,"LUO")== 0);
	TestCheck(ptime == 5);

	TestCheck(GetDBValue(1,key,&kenlen,15,value,&valen,&ptime)== false);
	return true;

}
bool testReadDataDBTime()
{
	char *key = NULL;
	unsigned long ptime = 0;
	TestCheck(ReadDataDBTime(key,0,&ptime) == false);
	key = "serf";
	TestCheck(ReadDataDBTime(key,5,&ptime) == false);
	key = "key1";
	TestCheck(ReadDataDBTime(key,5,&ptime) == true);
	TestCheck(ptime == 5);
	return true;
}
bool testModifyDataDBTime()
{
	char *key = NULL;
	unsigned long ptime = 0;
	TestCheck(ModifyDataDBTime(key,0,ptime) == false);
	key = "serf";
	TestCheck(ModifyDataDBTime(key,0,ptime) == false);
	key = "key1";
	ptime = 7;
	TestCheck(ModifyDataDBTime(key,5,ptime) == true);
	return true;
}
bool testModifyDataDBVavle()
{
	char *key = NULL;
	char *value = NULL;
	TestCheck(ModifyDataDBVavle(key,0,value,1) == false);
	TestCheck(ModifyDataDBVavle(key,5,value,0) == false);
	key = "123";
	value = "funk";
	TestCheck(ModifyDataDBVavle(key,4,value,5) == false);
	key = "key1";
	TestCheck(ModifyDataDBVavle(key,5,value,5) == true);
	unsigned long ptime = 0;
	char valen[5] ={0};
	TestCheck(ReadDataValueDB(key,5,valen,5) > 0);
	TestCheck(strcmp(valen,"funk")== 0);
	TestCheck(ReadDataDBTime(key,5,&ptime) == true);
	TestCheck(ptime == 7);
        return true;
}
bool testseconddb()
{
	unsigned long count = GetDBSize();
	TestCheck(count == 2);
	return true;
}
int main()
{
	__xdata static  char pcontact[512];

	unsigned long len = 512;
 	GetMemeroyData(pcontact,len);
	switch(pcontact[0])
	{
		case 0x01:
		{
			if(testInt64Inital() == false)
			{
				LogPrint("testInt64Inital",sizeof("testInt64Inital")+1,STRING);
				test_exit();
			}
			if(!testCompare())
			{
				LogPrint("testCompare",sizeof("testCompare")+1,STRING);
				test_exit();
			}
			if(!testadd())
			{
				LogPrint("testadd",sizeof("testadd")+1,STRING);
				test_exit();
			}
			if(!testmul())
			{
				LogPrint("testmul",sizeof("testmul")+1,STRING);
				test_exit();
			}
			if(!testsub())
			{
				LogPrint("testsub",sizeof("testsub")+1,STRING);
				test_exit();
			}
			if(!testdiv())
			{
				LogPrint("testdiv",sizeof("testdiv")+1,STRING);
				test_exit();
			}
			if(!testSHA256())
			{
				LogPrint("testSHA256",sizeof("testSHA256")+1,STRING);
				test_exit();
			}
			if(!testSignatureVerify())
			{
				LogPrint("testSignatureVerify",sizeof("testSignatureVerify")+1,STRING);
				test_exit();
			}
			if(!testDes())
			{
				LogPrint("testDes",sizeof("testDes")+1,STRING);
				test_exit();
			}
			if(!testLogPrint())
			{
				LogPrint("testLogPrint",sizeof("testLogPrint")+1,STRING);
				test_exit();
			}
			if(!testGetCurRunEnvHeight())
			{
				LogPrint("testGetCurRunEnvHeight",sizeof("testGetCurRunEnvHeight")+1,STRING);
				test_exit();
			}
			if(!testGetTipHeight())
			{
				LogPrint("testGetTipHeight",sizeof("testGetTipHeight")+1,STRING);
				test_exit();
			}
			if(!testGetBlockHash())
			{
				LogPrint("testGetBlockHash",sizeof("testGetBlockHash")+1,STRING);
				test_exit();
			}
			if(!testGetCurTxHash())
			{
				LogPrint("testGetCurTxHash",sizeof("testGetCurTxHash")+1,STRING);
				test_exit();
			}
			LogPrint("1:test ok",sizeof("1:test ok")+1,STRING);
			break;
		}
		case 0x02:
		{
			char hash[32];
			memcpy(&hash,&pcontact[1],32);
			if(!testGetTxContacts(hash))
			{
				LogPrint("testGetTxContacts",sizeof("testGetTxContacts")+1,STRING);
				test_exit();
			}
			if(!testGetAccounts(hash))
			{
				LogPrint("testGetAccounts",sizeof("testGetAccounts")+1,STRING);
				test_exit();
			}
			if(!testGetAccountPublickey(hash))
			{
				LogPrint("testGetAccountPublickey",sizeof("testGetAccountPublickey")+1,STRING);
				test_exit();
			}
			if(!testQueryAccountBalance(hash))
			{
				LogPrint("testQueryAccountBalance",sizeof("testQueryAccountBalance")+1,STRING);
				test_exit();
			}
			if(!testGetTxConFirmHeight(hash))
			{
				LogPrint("testGetTxConFirmHeight",sizeof("testGetTxConFirmHeight")+1,STRING);
				test_exit();
			}
			if(!testIsAuthorited(hash))
			{
				LogPrint("testIsAuthorited",sizeof("testIsAuthorited")+1,STRING);
				test_exit();
			}
			LogPrint("2:test ok",sizeof("2:test ok")+1,STRING);
			break;
		}
		case 0x03:
		{
			if(!testWriteDataDB())
			{
				LogPrint("testWriteDataDB",sizeof("testWriteDataDB")+1,STRING);
				test_exit();
			}
			if(!testDeleteDataDB())
			{
				LogPrint("testDeleteDataDB",sizeof("testDeleteDataDB")+1,STRING);
				test_exit();
			}
			if(!testReadDataValueDB())
			{
				LogPrint("testReadDataValueDB",sizeof("testReadDataValueDB")+1,STRING);
				test_exit();
			}

			if(!testModifyDataDB())
			{
				LogPrint("testModifyDataDB",sizeof("testModifyDataDB")+1,STRING);
				test_exit();
			}

			if(!testGetDBSize())
			{
				LogPrint("testGetDBSize",sizeof("testGetDBSize")+1,STRING);
				test_exit();
			}

			if(!testGetDBValue())
			{
				LogPrint("testGetDBValue",sizeof("testGetDBValue")+1,STRING);
				test_exit();
			}

			if(!testReadDataDBTime())
			{
				LogPrint("testReadDataDBTime",sizeof("testReadDataDBTime")+1,STRING);
				test_exit();
			}

			if(!testModifyDataDBTime())
			{
				LogPrint("testModifyDataDBTime",sizeof("testModifyDataDBTime")+1,STRING);
				test_exit();
			}
			if(!testModifyDataDBVavle())
			{
				LogPrint("testModifyDataDBVavle",sizeof("testModifyDataDBVavle")+1,STRING);
				test_exit();
			}
			LogPrint("3:test ok",sizeof("3:test ok")+1,STRING);
			break;
		}
		case 0x04:
		{
			if(!testseconddb())
			{
				LogPrint("testseconddb",sizeof("testseconddb")+1,STRING);
				test_exit();
			}
			LogPrint("4:test ok",sizeof("4:test ok")+1,STRING);
			break;
		}

		case 0x05:
		{
			char hash[32];
			memcpy(&hash,&pcontact[1],32);
			if(testIsAuthorited1(hash))
			{
				LogPrint("testIsAuthorited1",sizeof("testIsAuthorited1")+1,STRING);
				test_exit();
			}
			LogPrint("5:test ok",sizeof("5:test ok")+1,STRING);
			break;
		}
		default:
				{
					__exit(RUN_SCRIPT_DATA_ERR);
					break;
				}
	}
	__exit(RUN_SCRIPT_OK); //break,not stop
   return 0;
}
//int main() {
//	char * errormsg = "";
//	{
//		char sig[] = { 0x30, 0x44, 0x02, 0x20, 0x0b, 0xe0, 0x2f, 0x1c, 0x72, 0xe0, 0x79, 0xec, 0xe1, 0x25, 0x49, 0x5b,
//				0x29, 0x3a, 0x2d, 0x99, 0x51, 0x66, 0xc7, 0x0d, 0xfb, 0x09, 0x6f, 0x52, 0x56, 0x99, 0x0b, 0x44, 0x50,
//				0xa0, 0x8d, 0xd9, 0x02, 0x20, 0x0a, 0x95, 0xc1, 0xf2, 0xb4, 0x18, 0xc0, 0xcc, 0x87, 0x55, 0x68, 0x65,
//				0x43, 0x37, 0x60, 0x12, 0xb7, 0x58, 0xfe, 0x16, 0xac, 0x11, 0x16, 0x86, 0x6f, 0xdd, 0xf0, 0x04, 0xdd,
//				0x43, 0x93, 0xb1 };
//		char key[] = { 0x03, 0x3e, 0x85, 0xac, 0xa4, 0xd7, 0x96, 0x5d, 0x8b, 0x46, 0x7b, 0x98, 0x29, 0xd5, 0x77, 0x4a,
//				0xbd, 0xd9, 0xc6, 0x6a, 0x60, 0x18, 0xde, 0xe8, 0x5f, 0x82, 0xf1, 0xe2, 0x6b, 0xa2, 0x8b, 0x6d, 0xf2 };
//		char hash[] = { 0x01, 0x00, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x09, 0x08, 0x07, 0x06,
//				0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00 };
//		bool rslt = SignatureVerify(sig, sizeof(sig), key, sizeof(key), hash, sizeof(hash));
//
//		if(rslt == false)
//		{
//			errormsg = "SignatureVerify function error";
//			LogPrint(errormsg, strlen(errormsg)+1,STRING);
//			__exit(RUN_SCRIPT_DATA_ERR); //break,not stop
//		}
//
//	}
//
//	{
//
//		Int64 m1,m3;//4328719365
//		Int64Inital(&m1,"\x01\x02\x03\x04\x05", 5);
//		Int64 m2 ;//66051
//		Int64Inital(&m2,"\x00\x01\x02\x03", 4);
//
//		IntAdd(&m1, &m2, &m3);
//		Int64 m4;
//		Int64Inital(&m4,"\x01\x02\x04\x06\x08",5);
//		unsigned char kk = Compare(&m3, &m4);
//        if(kk != COMP_EQU)
//        {
//			errormsg = "MoneyAdd function error";
//			LogPrint(errormsg, strlen(errormsg)+1,STRING);
//			__exit(RUN_SCRIPT_DATA_ERR); //break,not stop
//        }
//
//		Int64Mul(&m1, &m2, &m3);
//		Int64 m5;
//		Int64Inital(&m5,"\x01\x04\x0A\x10\x16\x16\x0F",7);
//		kk = Compare(&m3, &m5);
//        if(kk != COMP_EQU)
//        {
//			errormsg = "MoneyMul function error";
//			LogPrint(errormsg, strlen(errormsg)+1,STRING);
//			__exit(RUN_SCRIPT_DATA_ERR); //break,not stop
//        }
//		Int64Sub(&m1, &m2, &m3);
//		Int64 m6;
//		Int64Inital(&m6,"\x01\x02\x02\x02\x02",5);
//		kk = Compare(&m3, &m6);
//        if(kk != COMP_EQU)
//        {
//			errormsg = "MoneySub function error";
//			LogPrint(errormsg, strlen(errormsg)+1,STRING);
//			__exit(RUN_SCRIPT_DATA_ERR); //break,not stop
//        }
//		Int64Div(&m1, &m2, &m3);
//		Int64 m7;
//		Int64Inital(&m7,"\x01\x00\x00",3);
//		kk = Compare(&m3, &m7);
//        if(kk != COMP_EQU)
//        {
//			errormsg = "MoneyDiv function error";
//			LogPrint(errormsg, strlen(errormsg)+1,STRING);
//			__exit(RUN_SCRIPT_DATA_ERR); //break,not stop
//        }
//
//	 kk = Compare(&m1, &m2);
//        if(kk != COMP_LARGER)
//        {
//			errormsg = "Compare function error";
//			LogPrint(errormsg, strlen(errormsg)+1,STRING);
//			__exit(RUN_SCRIPT_DATA_ERR); //break,not stop
//        }
//	}
//
//	{
//		unsigned char mm[50] = { 0 };
//		Des("\x01\x02\x03\x04\x05\xa7\x48\x89\x20\xb3\x8b\xd8\xe4", 13, "\x01\x02\x03\x04\x05\x06\x07\x08", 8, true,
//				mm);
//
//		Des("\x17\x26\xc7\x5f\x28\x16\x71\x5f\xde\x89\x62\x08\x43\x34\x39\xa7", 16, "\x01\x02\x03\x04\x05\x06\x07\x08",
//				8, false, mm);
//	}
//
//	{
//		char xx[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
//		char yy[32] = { 0 };
//		SHA256(xx, 8, yy);
//		LogPrint(yy, 32,STRING);
//	}
//
//	char sig[] = { 0x30, 0x44, 0x02, 0x20, 0x0b, 0xe0, 0x2f, 0x1c, 0x72, 0xe0, 0x79, 0xec, 0xe1, 0x25, 0x49, 0x5b,
//					0x29, 0x3a, 0x2d, 0x99, 0x51, 0x66, 0xc7, 0x0d, 0xfb, 0x09, 0x6f, 0x52, 0x56, 0x99, 0x0b, 0x44, 0x50,
//					0xa0, 0x8d, 0xd9, 0x02, 0x20, 0x0a, 0x95, 0xc1, 0xf2, 0xb4, 0x18, 0xc0, 0xcc, 0x87, 0x55, 0x68, 0x65,
//					0x43, 0x37, 0x60, 0x12, 0xb7, 0x58, 0xfe, 0x16, 0xac, 0x11, 0x16, 0x86, 0x6f, 0xdd, 0xf0, 0x04, 0xdd,
//					0x43, 0x93, 0xb1 };
//
//	LogPrint(sig, sizeof(sig),STRING);
//
//	LogPrint("test deamon",20,STRING);
//
//	LogPrint("123456789", sizeof("123456789"),STRING);
//
//	int tlen = 1011;
//
//	LogPrint("test len = ",sizeof(tlen),STRING);
//     __exit(1); //break,not stop
//   return 0;
//}

//int main() {
//    char blockhash[32] = {0};
//    unsigned char txhash[32] = {0};
//    GetCurTxHash((unsigned char *)txhash,32);
//    LogPrint(txhash, 32,HEX);
//     GetBlockHash(1,(unsigned char*)blockhash,32);
//     LogPrint(blockhash, sizeof(blockhash),HEX);
//
//     char pcotact[512];
//     GetTxContacts(txhash,pcotact,sizeof(pcotact));
//     LogPrint(pcotact, sizeof(pcotact),HEX);
//     GetTxSignedAccounts(txhash,pcotact,sizeof(pcotact));
//     LogPrint(pcotact, sizeof(pcotact),HEX);
//
//     char*accounid = "5zQPcC1YpFMtwxiH787pSXanUECoGsxUq3KZieJxVG";
//
//     GetAccountPublickey((unsigned char *)accounid,pcotact,65);
//     LogPrint(pcotact, sizeof(pcotact),HEX);
//
//     Int64 pBalance;
//     QueryAccountBalance(( unsigned char*)accounid,&pBalance);
//     LogPrint(&pBalance, 8,HEX);
//     //bool IsAuthorited(const unsigned char*account,Int64* pmoney);
//     __exit(RUN_SCRIPT_OK); //break,not stop
//}
//int main() {
//  #pragma data_alignment = 1
//	VMOPERATE data;
//	data.TYPE =ACCOUNTID;
//	data.opeatortype = ADD_FREE;
//	data.outheight = 50;
//        int size = sizeof(VMOPERATE);
//	WriteOutput(&data,1);
//	__exit(1);
//}

