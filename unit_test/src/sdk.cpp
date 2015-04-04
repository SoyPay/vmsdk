
#include"unit_test.h"


void test_exit()
{
	__VmExit(RUN_SCRIPT_DATA_ERR);
}


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
	char buffer[8] = {0};
	sprintf(buffer,"%d",height);
	LogPrint(buffer,5,STRING);
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

//// test db
bool testWriteDataDB()
{
	char* key =NULL;
	char *value = "hello";
	TestCheck(WriteData(key,0,value,6) == false);
	TestCheck(WriteData(key,9,value,0) == false);
	key = "key";
	value = NULL;
	TestCheck(WriteData(key,4,value,6) == true);
	key = "key";
	value = "hello";
	TestCheck(WriteData(key,4,value,6) == true);
	key = "key1";
	value = "hellow";
	TestCheck(WriteData(key,5,value,7) == true);
	return true;
}
bool testDeleteDataDB()
{
	char* key =NULL;
	TestCheck(DeleteData(key,0)== false);
	key = "key";
	TestCheck(DeleteData(key,4)== true);
	return true;
}
bool testReadDataValueDB()
{
	char* key =NULL;
	char value[7] = {0};
	TestCheck(ReadData(key,0,value,0) == 0);
	key = "key";
	TestCheck(ReadData(key,4,value,0) == 0);
	TestCheck(ReadData(key,4,value,7) == 0);
	key = "key1";
	TestCheck(ReadData(key,5,value,7)> 0);
	TestCheck(strcmp(value,"hellow") ==0);
	return true;
}
bool testModifyDataDB()
{
	char* key =NULL;
	char *value = NULL;
	TestCheck(ModifyData(key,0,value,7) == false);
	TestCheck(ModifyData(key,5,value,0) == false);
	key = "sse";
	TestCheck(ModifyData(key,4,value,0) == false);
	key = "key1";
	value= "LUO";
	TestCheck(ModifyData(key,5,value,4) == true);
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

	TestCheck(GetDBValue(0,key,(unsigned char*)&kenlen,0,value,&valen)== false);

	kenlen = 5;
	TestCheck(GetDBValue(0,key,(unsigned char*)&kenlen,0,value,&valen)== false);


	char *wkey = "bit";
	char *wvalue = "shit";
	TestCheck(WriteData(wkey,4,wvalue,5) == true);

	valen = 15;
	kenlen = sizeof(key);
	TestCheck(GetDBValue(0,key,&kenlen,15,value,&valen)== true);
	TestCheck(strcmp(key,"bit")== 0);
	TestCheck(strcmp(value,wvalue)== 0);

	valen = 4;
	TestCheck(GetDBValue(1,key,&kenlen,15,value,&valen)== true);
	TestCheck(strcmp(key,"key1")== 0);
	TestCheck(strcmp(value,"LUO")== 0);

	TestCheck(GetDBValue(1,key,&kenlen,15,value,&valen)== false);
	return true;

}
bool testModifyDataDBVavle()
{
	char *key = NULL;
	char *value = NULL;
	TestCheck(ModifyData(key,0,value,1) == false);
	TestCheck(ModifyData(key,5,value,0) == false);
	key = "123";
	value = "funk";
	TestCheck(ModifyData(key,4,value,5) == false);
	key = "key1";
	TestCheck(ModifyData(key,5,value,5) == true);
	char valen[5] ={0};
	TestCheck(ReadData(key,5,valen,5) > 0);
	TestCheck(strcmp(valen,"funk")== 0);
     return true;
}
bool testseconddb()
{
	unsigned long count = GetDBSize();
	TestCheck(count == 2);
	return true;
}
int ProcessSdk(char*pcontact)
{
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
			LogPrint("11",sizeof("11")+1,STRING);
			if(!testGetDBValue())
			{
				LogPrint("testGetDBValue error",sizeof("testGetDBValue error")+1,STRING);
				test_exit();
			}
			LogPrint("21",sizeof("21")+1,STRING);
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
			//if(testIsAuthorited1(hash))
//			{
//				LogPrint("testIsAuthorited1 error",sizeof("testIsAuthorited1 error")+1,STRING);
//				test_exit();
//			}
			LogPrint("5:test ok",sizeof("5:test ok")+1,STRING);
			break;
		}
		default:
				{
					LogPrint("ERROR",sizeof("ERROR"),STRING);
					__VmExit(RUN_SCRIPT_DATA_ERR);
					break;
				}
	}
	__VmExit(RUN_SCRIPT_OK); //break,not stop

}
