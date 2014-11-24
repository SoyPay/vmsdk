#include <string.h>
#include<stdlib.h>
#include <stdio.h>
#include"VmSdk.h"
#define TestCheck(a) {if(!(a)) { PrintfLine(__LINE__); return false;}}
bool testWriteDataDB()
{
	unsigned long time = 2;
	char* key = "key";
	char *value = "hello";
	TestCheck(WriteDataDB(key,4,value,6,time) == true);
	key = "key1";
	value = "hellow";
	TestCheck(WriteDataDB(key,5,value,7,time) == true);
	return true;
}
bool testModifyDataDB()
{
	char*key = "key1";
	char *value= "LUO";
	unsigned long ptime = 5;
	TestCheck(ModifyDataDB(key,5,value,4,ptime) == true);
	return true;
}
bool testDeleteDataDB()
{
	char* key = "key";
	bool flag = DeleteDataDB(key,4);
	LogPrint(&flag,1,HEX);
	TestCheck(flag == true);
	return true;
}
bool testCheckDeleteRoolbackDB()
{
	char* key = "key";
	char buffer[6] = {0};
	TestCheck(ReadDataValueDB(key,4,buffer,6) > 0);
	TestCheck(strcmp(buffer,"hello")== 0);
	return true;
}
bool testCheckModifyRoolbackDB()
{
	char* key = "key1";
	char buffer[7] = {0};
	TestCheck(ReadDataValueDB(key,5,buffer,7)> 0);
	TestCheck(strcmp(buffer,"hellow")== 0);
	return true;
}
bool testCheckWriteRoolbackDB()
{
	char* key = "key1";
	char buffer[7] = {0};
	TestCheck(ReadDataValueDB(key,5,buffer,7)== false);

	char* key1 = "key";
	char buffer1[6] = {0};
	TestCheck(ReadDataValueDB(key1,4,buffer1,6)== false);
	return true;
}

int main()
{
	__xdata static  char pcontact[2];
 	GetMemeroyData(pcontact,2);
	switch(pcontact[0])
	{
		case 0x01:
				{
					if(!testWriteDataDB())
					{
						LogPrint("testWriteDataDB error",sizeof("testWriteDataDB error"),STRING);
						__exit(RUN_SCRIPT_DATA_ERR);
					}
					break;
				}
		case 0x02:
				{
					if(!testModifyDataDB())
					{
						LogPrint("testModifyDataDB error",sizeof("testModifyDataDB error"),STRING);
						__exit(RUN_SCRIPT_DATA_ERR);
					}
					break;
				}
		case 0x03:
				{
					if(!testDeleteDataDB())
					{
						LogPrint("testDeleteDataDB error",sizeof("testDeleteDataDB error"),STRING);
						__exit(RUN_SCRIPT_DATA_ERR);
					}
					break;
				}
		case 0x04:
				{
					//Is4 = true;
					LogPrint("start",sizeof("start"),STRING);
					if(!testCheckDeleteRoolbackDB())
					{
						LogPrint("testCheckDeleteRoolbackDB error",sizeof("testCheckDeleteRoolbackDB error"),STRING);
						__exit(RUN_SCRIPT_DATA_ERR);
					}
					LogPrint("end",sizeof("end"),STRING);
					break;
				}
		case 0x05:
				{
					if(!testCheckModifyRoolbackDB())
					{
						LogPrint("testCheckModifyRoolbackDB error",sizeof("testCheckModifyRoolbackDB error"),STRING);
						__exit(RUN_SCRIPT_DATA_ERR);
					}
					break;
				}
		case 0x06:
				{
					if(!testCheckWriteRoolbackDB())
					{
						LogPrint("testCheckWriteRoolbackDB error",sizeof("testCheckWriteRoolbackDB error"),STRING);
						__exit(RUN_SCRIPT_DATA_ERR);
					}
					break;
				}
		default:
				{
					__exit(RUN_SCRIPT_DATA_ERR);
					break;
				}
	}
	__exit(RUN_SCRIPT_OK);
  return 0;
}
