#include"unit_test.h"
void writedata()
{
	char hash[32] ={0};
	if(!GetCurTxHash(hash))
		return ;
	LogPrint(&hash,32,HEX);
	int i = 0;
	while(i < 1000){
		if(!WriteData(&i,sizeof(i),hash,32))
			{
				LogPrint("write error",sizeof("write error"),STRING);
			}
		++i;
	}
}

void readdata(char*hash,unsigned long preHeight)
{
	unsigned long size = GetDBSize();
	unsigned long Height= GetCurRunEnvHeight();
	unsigned short valen = 32;
	int key = 0;
	char phash[33] = {0};

	unsigned char keylen = sizeof(key);
	bool flag = true;

	if(!GetDBValue(0,&key,(unsigned char*)&keylen,keylen,phash,&valen))
	{
		LogPrint("GetDBValue 33",sizeof("GetDBValue 33"),STRING);
		return ;
	}

	if(!ReadData(&key,keylen,phash,valen))
	{
		LogPrint("ReadDataValueDB 39",sizeof("ReadDataValueDB 39"),STRING);
		flag = false;
	}
	char * key1 = "2_error";
	if(!flag||strcmp(phash,hash))
	{
		WriteData(key1,strlen(key1) + 1,hash,32);
		return;
	}
	size -= 1;
	while(size--)
	{
		if(!GetDBValue(1,&key,(unsigned char*)&keylen,keylen,phash,&valen))
		{
			break;
		}

		if(!ReadData(&key,keylen,phash,valen))
		{
			LogPrint("61",sizeof("61"),STRING);
			flag = false;
		}

		if(!flag || strcmp(phash,hash))
		{
			LogPrint("67",sizeof("67"),STRING);
			WriteData(key1,strlen(key1) + 1,hash,32);
			return;
		}
                
	}
}

void ModifydataDB(unsigned long poutH)
{
	int i = 400;
	char *hash = "hash";
	char *error ="3_error";
	unsigned long Height= GetCurRunEnvHeight();
	for(;i<600;i++)
	{
		if(ModifyData(&i,4,hash,5))
		{
			WriteData(error,strlen(error) + 1,hash,5);
			return;
		}
	}

	for(i = 800;i<1000;i++)
	{
		if(DeleteData(&i,4))
		{
			WriteData(error,strlen(error) + 1,hash,5);
			return;
		}
	}
}

void Lastreaddata()
{
	unsigned long size = GetDBSize();
	unsigned long Height= GetCurRunEnvHeight();
	unsigned short valen = 32;
	int key = 0;
	char phash[33] = {0};

	unsigned char keylen = 2;
	bool flag = true;
	if(!GetDBValue(0,&key,(unsigned char*)&keylen,keylen,phash,&valen))
	{
		return;
	}
	char * key1 = "4_error";
	if(!flag)
	{
		WriteData(key1,strlen(key1) + 1,"hash",5);
		return;
	}

	size -=1;
	while(size--)
	{
		if(!GetDBValue(1,&key,(unsigned char*)&keylen,keylen,phash,&valen))
		{
			break;
		}
		if(!flag )
		{
			WriteData(key1,strlen(key1) + 1,"hash",5);
			return;
		}
	}
}

bool ProcessTestDB(char*pcontact)
{
	__xdata static  char hash[33] = {0};
	switch(pcontact[0])
	{
		case 0x0F:
				{
					writedata();
					break;
				}
		case 0x10:
				{
					unsigned long heigh = 0;
					memcpy(hash,&pcontact[1],32);
					memcpy(&heigh,&pcontact[33],sizeof(heigh));
					readdata(hash,heigh);
					break;
				}
		case 0x11:
				{
					unsigned long heigh = 0;
					LogPrint(&heigh,4,HEX);
					memcpy(&heigh,&pcontact[1],sizeof(heigh));
					break;
					//ModifydataDB(heigh);
				}
		case 0x12:
		{
			Lastreaddata();
			break;
		}

	}
	__VmExit(RUN_SCRIPT_OK);
}
