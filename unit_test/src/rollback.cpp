
#include"unit_test.h"
bool testWriteDataDB1()
{
	char* key = "key";
	char *value = "hello";
	TestCheck(WriteData(key,4,value,6) == true);
	key = "key1";
	value = "hellow";
	TestCheck(WriteData(key,5,value,7) == true);
	return true;
}
bool testModifyDataDB1()
{
	char*key = "key1";
	char *value= "LUO";
	unsigned long ptime = GetCurRunEnvHeight() +50;
	TestCheck(ModifyData(key,5,value,4) == true);
	return true;
}
bool testDeleteDataDB1()
{
	char* key = "key";
	bool flag = DeleteData(key,4);
	TestCheck(flag == true);
	return true;
}
bool testCheckDeleteRoolbackDB()
{
	char* key = "key";
	char buffer[6] = {0};
	TestCheck(ReadData(key,4,buffer,6) > 0);
	TestCheck(strcmp(buffer,"hello")== 0);
	return true;
}
bool testCheckModifyRoolbackDB()
{
	char* key = "key1";
	char buffer[7] = {0};
	TestCheck(ReadData(key,5,buffer,7)> 0);
	TestCheck(strcmp(buffer,"hellow")== 0);
	return true;
}
bool testCheckWriteRoolbackDB()
{
	char* key = "key1";
	char buffer[7] = {0};
	TestCheck(ReadData(key,5,buffer,7)== false);

	char* key1 = "key";
	char buffer1[6] = {0};
	TestCheck(ReadData(key1,4,buffer1,6)== false);
	return true;
}

int ProcessRollBack(char*pcontact)
{
	switch(pcontact[0])
	{
		case 0x06:
				{
					if(!testWriteDataDB1())
					{
						LogPrint("testWriteDataDB error",sizeof("testWriteDataDB error"),STRING);
						__exit(RUN_SCRIPT_DATA_ERR);
					}
					break;
				}
		case 0x07:
				{
					if(!testModifyDataDB1())
					{
						LogPrint("testModifyDataDB error",sizeof("testModifyDataDB error"),STRING);
						__exit(RUN_SCRIPT_DATA_ERR);
					}
					break;
				}
		case 0x08:
				{
					if(!testDeleteDataDB1())
					{
						LogPrint("testDeleteDataDB error",sizeof("testDeleteDataDB error"),STRING);
						__exit(RUN_SCRIPT_DATA_ERR);
					}
					break;
				}
		case 0x09:
				{
					if(!testCheckDeleteRoolbackDB())
					{
						LogPrint("testCheckDeleteRoolbackDB error",sizeof("testCheckDeleteRoolbackDB error"),STRING);
						__exit(RUN_SCRIPT_DATA_ERR);
					}
					break;
				}
		case 0x0a:
				{
					if(!testCheckModifyRoolbackDB())
					{
						LogPrint("testCheckModifyRoolbackDB error",sizeof("testCheckModifyRoolbackDB error"),STRING);
						__exit(RUN_SCRIPT_DATA_ERR);
					}
					break;
				}
		case 0x0b:
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
