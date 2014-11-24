#include <string.h>
#include<stdlib.h>
#include <stdio.h>
#include"VmSdk.h"
extern unsigned char *GetMemeryData();
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

	TestCheck(memcmp(hash, "\xea\x96\xdb\xb9\xbb\x08\xcf\xb3\x8a\x22\xfd\x3e\x7f\x40\xf8\xa3\x56\xeb\x1f\x61\xbe\x58\x3f\x23\xab\x21\xfb\x5a\x1d\x87\x9d\xac", sizeof(hash)) == 0);

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
bool testWriteOutput()
{
	VM_OPERATE data;
	WriteOutput(&data, 1);
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
	char buffer[10] = {0};
	sprintf(buffer,"bb:%d",count);
	LogPrint(buffer,sizeof(buffer),STRING);
	TestCheck(count == 2);
	return true;
}
int main()
{
		if(!testInt64Inital())
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
		if(!testseconddb())
		{
			LogPrint("testseconddb",sizeof("testseconddb")+1,STRING);
			test_exit();
		}
__exit(RUN_SCRIPT_OK); //break,not stop
   return 0;
}
