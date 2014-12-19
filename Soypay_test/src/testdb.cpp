#include"soypay.h"
void writedata()
{
	char hash[32] ={0};
	if(!GetCurTxHash(hash))
		return ;
	LogPrint(&hash,32,HEX);
	int i = 0;
	unsigned long Height= GetCurRunEnvHeight()+5;
	while(i < 1000){
		if(!WriteDataDB(&i,sizeof(i),hash,32,Height+i/200))
			{
				LogPrint("write error",sizeof("write error"),STRING);
			}
		++i;
	}
}
bool CheckValue(int key,int Height,int outheight)
{
	return (Height+key/200 +1 == (outheight -5));
}
void readdata(char*hash,unsigned long preHeight)
{
	unsigned long size = GetDBSize();
	unsigned long Height= GetCurRunEnvHeight();
	unsigned short valen = 32;
	int key = 0;
	char phash[33] = {0};
	unsigned long poutHeight = 0;
	unsigned char keylen = sizeof(key);
	bool flag = true;

	if(!GetDBValue(0,&key,(unsigned char*)&keylen,keylen,phash,&valen, &poutHeight))
	{
		LogPrint("GetDBValue 33",sizeof("GetDBValue 33"),STRING);
		flag = false;
	}

	if(!ReadDataValueDB(&key,keylen,phash,valen))
	{
		LogPrint("ReadDataValueDB 39",sizeof("ReadDataValueDB 39"),STRING);
		flag = false;
	}
	if(!ReadDataDBTime(&key,keylen,&poutHeight))
	{
		LogPrint("ReadDataValueDB 39",sizeof("ReadDataValueDB 39"),STRING);
		flag = false;
	}
	char * key1 = "2_error";
	if(!flag || poutHeight < Height || !CheckValue(key,preHeight,poutHeight) || !strcmp(phash,hash))
	{
		LogPrint("45",sizeof("45"),STRING);
		LogPrint(&poutHeight,4,HEX);
		LogPrint(&Height,4,HEX);
		LogPrint(&preHeight,4,HEX);
		LogPrint(&phash,33,HEX);
		LogPrint(hash,33,HEX);
		WriteDataDB(key1,strlen(key1) + 1,hash,32,Height+1);
		return;
	}

	size -=1;
	while(size--)
	{
		if(!GetDBValue(1,&key,(unsigned char*)&keylen,keylen,phash,&valen, &poutHeight))
		{
			LogPrint("55",sizeof("55"),STRING);
			flag = false;
		}

		if(!ReadDataValueDB(&key,keylen,phash,valen))
		{
			LogPrint("61",sizeof("61"),STRING);
			flag = false;
		}

		if(!flag || poutHeight < Height || !CheckValue(key,preHeight,poutHeight)|| !strcmp(phash,hash))
		{
			LogPrint("67",sizeof("67"),STRING);
			WriteDataDB(key1,strlen(key1) + 1,hash,32,Height+1);
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
	for(;i<=600;i++)
	{
		if(ModifyDataDBVavle(&i,4,hash,5))
		{
			WriteDataDB(error,strlen(error) + 1,hash,5,Height+1);
			return;
		}
	}
	for(i = 601;i<=800;i++)
	{
		if(ModifyDataDBTime(&i,4,poutH))
		{
			WriteDataDB(error,strlen(error) + 1,hash,5,Height+1);
			return;
		}
	}

	for(i = 801;i<=1000;i++)
	{
		if(DeleteDataDB(&i,4))
		{
			WriteDataDB(error,strlen(error) + 1,hash,5,Height+1);
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
	unsigned long poutHeight = 0;
	unsigned char keylen = 2;
	bool flag = true;
	if(!GetDBValue(0,&key,(unsigned char*)&keylen,keylen,phash,&valen, &poutHeight))
	{
		flag = false;
	}
	char * key1 = "4_error";
	if(!flag || poutHeight < Height)
	{
		WriteDataDB(key1,strlen(key1) + 1,"hash",5,Height+1);
		return;
	}

	size -=1;
	while(size--)
	{
		if(!GetDBValue(1,&key,(unsigned char*)&keylen,keylen,phash,&valen, &poutHeight))
		{
			flag = false;
		}
		if(!flag || poutHeight < Height )
		{
			WriteDataDB(key1,strlen(key1) + 1,"hash",5,Height+1);
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
					ModifydataDB(heigh);
				}
		case 0x12:
		{
			Lastreaddata();
			break;
		}

	}
	__exit(RUN_SCRIPT_OK);
	  return true;
}
