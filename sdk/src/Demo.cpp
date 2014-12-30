#include <string.h>
#include<stdlib.h>
#include <stdio.h>
#include"VmSdk.h"


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
	char hash[32] = {0};
	char hashdata[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
//error paras test
	TestCheck(SHA256(hashdata, 0, hash) == false);
	//NULL input need to test or not

//normal paras test
	TestCheck(SHA256(hashdata, sizeof(hashdata), hash) == true);

//	LogPrint(hash, sizeof(hash), HEX);
	TestCheck(memcmp(hash, "\xac\x9d\x87\x1d\x5a\xfb\x21\xab\x23\x3f\x58\xbe\x61\x1f\xeb\x56\xa3\xf8\x40\x7f\x3e\xfd\x22\x8a\xb3\xcf\x08\xbb\xb9\xdb\x96\xea", sizeof(hash)) == 0);

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
	unsigned char input[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0xa7, 0x48, 0x89, 0x20, 0xb3, 0x8b, 0xd8, 0xe4};
	unsigned char result[50] = { 0 };
	unsigned short rltlen = 0;
	unsigned char key1[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
	unsigned char key3[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18};
//error paras test
	TestCheck(Des(input, 0, "", 0, true, result, sizeof(result)) == 0);
	TestCheck(Des(input, 0, "", 0, false, result, sizeof(result)) == 0);
	TestCheck(Des(input, sizeof(input), key1, 4, true, result, sizeof(result)) == 0);
	TestCheck(Des(input, sizeof(input), key1, 5, false, result, sizeof(result)) == 0);
	TestCheck(Des(input, sizeof(input), key3, 13, false, result, sizeof(result)) == 0);
	TestCheck(Des(input, sizeof(input), key3, 14, false, result, sizeof(result)) == 0);

//normal paras test-des
	rltlen = Des(input, sizeof(input), key1, sizeof(key1), true, result, sizeof(result));
	TestCheck(rltlen > 0);
	TestCheck(memcmp(result, "\x17\x26\xc7\x5f\x28\x16\x71\x5f\xde\x89\x62\x08\x43\x34\x39\xa7", rltlen) == 0);
	{
		unsigned char rtmp[50] = { 0 };
		unsigned short rtmplen = Des(result, rltlen, key1, sizeof(key1), false, rtmp, sizeof(rtmp));
		TestCheck(rtmplen >= sizeof(input));
		TestCheck(memcmp(input, rtmp, sizeof(input)) == 0);
	}
	//clear data
	memset(result, 0, sizeof(result));
	rltlen = 0;

//normal paras test-3des
	rltlen = Des(input, sizeof(input), key3, sizeof(key3), true, result, sizeof(result));
	TestCheck(rltlen > 0);
	TestCheck(memcmp(result, "\x74\x9b\x65\x42\xf9\x74\x1d\x39\x2d\xab\xae\x84\x7e\x50\x82\x70", rltlen) == 0);
	{
		unsigned char rtmp[50] = { 0 };
		unsigned short rtmplen = Des(result, rltlen, key3, sizeof(key3), false, rtmp, sizeof(rtmp));
		TestCheck(rtmplen >= sizeof(input));
		TestCheck(memcmp(input, rtmp, sizeof(input)) == 0);
	}

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
	TestCheck(height == 2);
	return true;
}
bool testGetTxContacts(char *phash)
{
	char pcontact[50] = {0};
	GetTxContacts((unsigned char*)phash,pcontact,50);
	TestCheck(pcontact[0] == 0x01);
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
	char compare[6] = {0x00,0x00,0x00,0x00,0x08,0x00};
	TestCheck(strcmp(pcontact,compare) == 0);
	return true;
}
bool testGetAccountPublickey(char *phash)
{
	char pcontact[6] = {0};
	char pubkey[34] = {0};
	char*temphash = NULL;
	unsigned  short length = GetAccountPublickey(temphash,pubkey,33);
	TestCheck(length == 33);
	GetAccounts((unsigned char*)phash,pcontact,6);

	length = GetAccountPublickey(pcontact,pubkey,33);
	TestCheck(length == 33);
	char compare[34]={0x03,0x11,0xca,0x27,0x7a,0x0f,0x38,0x80,
	0xee,0xfe,0xd3,0x49,0x72,0x7c,0xc9,0x54,
	0x35,0x4c,0x4e,0x53,0x9b,0x81,0x28,0xc7,
	0x9f,0x18,0xa8,0x7c,0x6f,0x2b,0x97,0x91,
	0x86,0x00};
	TestCheck(strcmp(pubkey,compare) == 0);
	return true;
}
bool testQueryAccountBalance(char *phash)
{
	char paccount[6] = {0};
	Int64 pBalance;
	Int64Inital(&pBalance,"\x00", 1);
	TestCheck(QueryAccountBalance((unsigned char*)paccount,&pBalance) == true);
	LogPrint(&pBalance,8,HEX);
	GetAccounts((unsigned char*)phash,paccount,6);
	TestCheck(QueryAccountBalance((unsigned char*)paccount,&pBalance) == true);
	LogPrint(&pBalance,8,HEX);
	return true;
}
/// the api not uses
bool testGetTxConFirmHeight(char *txhash)
{
	unsigned long height = GetTxConFirmHeight(txhash);
	TestCheck(height ==2);
	return true;
}
bool testGetTipHeight()
{
	unsigned long height = GetTipHeight();
	TestCheck(height ==1);
	return true;
}
bool testGetBlockHash()
{
	char bhash[32] = {0};
	TestCheck(GetBlockHash(1,bhash) == true);
	return true;
}
bool testGetCurTxHash()
{
	char txhash[32] = {0};
	TestCheck(GetCurTxHash(txhash) == true);
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
	unsigned long time = 20;
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
	ptime = 40;
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
	ptime = 70;
	TestCheck(WriteDataDB(wkey,4,wvalue,5,ptime) == true);

	valen = 15;
	ptime = 0;
	kenlen = sizeof(key);
	TestCheck(GetDBValue(0,key,&kenlen,15,value,&valen,&ptime)== true);
	TestCheck(strcmp(key,"bit")== 0);
	TestCheck(strcmp(value,wvalue)== 0);
	TestCheck(ptime == 70);

	valen = 4;
	ptime = 0;
	TestCheck(GetDBValue(1,key,&kenlen,15,value,&valen,&ptime)== true);
	TestCheck(strcmp(key,"key1")== 0);
	TestCheck(strcmp(value,"LUO")== 0);
	TestCheck(ptime == 40);

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
	TestCheck(ptime == 40);
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
	ptime = 60;
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
	TestCheck(ptime == 60);
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
	GetCurTxContact(pcontact,len);
	switch(pcontact[0])
	{
		case 0x01:
		{
			if(testInt64Inital() == false)
			{
				LogPrint("testInt64Inital error",sizeof("testInt64Inital error")+1,STRING);
				test_exit();
			}
			if(!testCompare())
			{
				LogPrint("testCompare error",sizeof("testCompare error")+1,STRING);
				test_exit();
			}
			if(!testadd())
			{
				LogPrint("testadd error",sizeof("testadd error")+1,STRING);
				test_exit();
			}
			if(!testmul())
			{
				LogPrint("testmul error",sizeof("testmul error")+1,STRING);
				test_exit();
			}
			if(!testsub())
			{
				LogPrint("testsub error",sizeof("testsub error")+1,STRING);
				test_exit();
			}
			if(!testdiv())
			{
				LogPrint("testdiv error",sizeof("testdiv error")+1,STRING);
				test_exit();
			}
			if(!testSHA256())
			{
				LogPrint("testSHA256 error",sizeof("testSHA256 error")+1,STRING);
				test_exit();
			}
			if(!testSignatureVerify())
			{
				LogPrint("testSignatureVerify error",sizeof("testSignatureVerify error")+1,STRING);
				test_exit();
			}
			if(!testDes())
			{
				LogPrint("testDes error",sizeof("testDes error")+1,STRING);
				test_exit();
			}
			if(!testLogPrint())
			{
				LogPrint("testLogPrint error",sizeof("testLogPrint error")+1,STRING);
				test_exit();
			}
			if(!testGetCurRunEnvHeight())
			{
				LogPrint("testGetCurRunEnvHeight error",sizeof("testGetCurRunEnvHeight error")+1,STRING);
				test_exit();
			}
			if(!testGetTipHeight())
			{
				LogPrint("testGetTipHeight error",sizeof("testGetTipHeight error")+1,STRING);
				test_exit();
			}
			if(!testGetBlockHash())
			{
				LogPrint("testGetBlockHash error",sizeof("testGetBlockHash error")+1,STRING);
				test_exit();
			}
			if(!testGetCurTxHash())
			{
				LogPrint("testGetCurTxHash error",sizeof("testGetCurTxHash error")+1,STRING);
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
				LogPrint("testGetTxContacts error",sizeof("testGetTxContacts error")+1,STRING);
				test_exit();
			}
			if(!testGetAccounts(hash))
			{
				LogPrint("testGetAccounts error",sizeof("testGetAccounts error")+1,STRING);
				test_exit();
			}
			if(!testGetAccountPublickey(hash))
			{
				LogPrint("testGetAccountPublickey error",sizeof("testGetAccountPublickey error")+1,STRING);
				test_exit();
			}
			if(!testQueryAccountBalance(hash))
			{
				LogPrint("testQueryAccountBalance error",sizeof("testQueryAccountBalance error")+1,STRING);
				test_exit();
			}
			if(!testGetTxConFirmHeight(hash))
			{
				LogPrint("testGetTxConFirmHeight error",sizeof("testGetTxConFirmHeight error")+1,STRING);
				test_exit();
			}
//			if(!testIsAuthorited(hash))
//			{
//				LogPrint("testIsAuthorited",sizeof("testIsAuthorited")+1,STRING);
//				test_exit();
//			}
			LogPrint("2:test ok",sizeof("2:test ok")+1,STRING);
			break;
		}
		case 0x03:
		{
			if(!testWriteDataDB())
			{
				LogPrint("testWriteDataDB error",sizeof("testWriteDataDB error")+1,STRING);
				test_exit();
			}
			if(!testDeleteDataDB())
			{
				LogPrint("testDeleteDataDB error",sizeof("testDeleteDataDB error")+1,STRING);
				test_exit();
			}
			if(!testReadDataValueDB())
			{
				LogPrint("testReadDataValueDB error",sizeof("testReadDataValueDB error")+1,STRING);
				test_exit();
			}

			if(!testModifyDataDB())
			{
				LogPrint("testModifyDataDB error",sizeof("testModifyDataDB error")+1,STRING);
				test_exit();
			}

			if(!testGetDBSize())
			{
				LogPrint("testGetDBSize error",sizeof("testGetDBSize error")+1,STRING);
				test_exit();
			}

			if(!testGetDBValue())
			{
				LogPrint("testGetDBValue error",sizeof("testGetDBValue error")+1,STRING);
				test_exit();
			}

			if(!testReadDataDBTime())
			{
				LogPrint("testReadDataDBTime error",sizeof("testReadDataDBTime error")+1,STRING);
				test_exit();
			}

			if(!testModifyDataDBTime())
			{
				LogPrint("testModifyDataDBTime error",sizeof("testModifyDataDBTime error")+1,STRING);
				test_exit();
			}
			if(!testModifyDataDBVavle())
			{
				LogPrint("testModifyDataDBVavle error",sizeof("testModifyDataDBVavle error")+1,STRING);
				test_exit();
			}
			LogPrint("3:test ok",sizeof("3:test ok")+1,STRING);
			break;
		}
		case 0x04:
		{
			if(!testseconddb())
			{
				LogPrint("testseconddb error",sizeof("testseconddb error")+1,STRING);
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
				LogPrint("testIsAuthorited1 error",sizeof("testIsAuthorited1 error")+1,STRING);
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

