
#include <string.h>
#include<stdlib.h>

__root __code const  char a[]@0x0008 = {0,0,0,0x80,0xFB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
__root __xdata __no_init  char exchangdata[1023]@0xFC00 ;
__root __xdata __no_init  char Result[1]@0xFBFF ;
__root  char GlobalData[1024]={0};
typedef void (*ApiFun)(void);


bool CheckSig(void const*input,unsigned short inlen,void const * PsingnedData)
{
   return true;
}

unsigned char  Decode(void const*input, void const* secretkey,void * output)
{
  
   return true;
}



__root void __exit(unsigned char  tep)
{
   Result[1] = tep;
  (( void (*)(void))(0x0008)) ();

}

typedef struct TAGVMHEADDATA
{
    unsigned char ArbitratorAccCount;
    unsigned char vAccountCount;
    unsigned char AppealTxCount;
}VmHeadData;

typedef struct TAGINITIALPACKET
{
   unsigned char fromaccountId;
   unsigned char toaccountid;
   unsigned int iniOutHeight;
   unsigned int followOutHeight;
   unsigned long  heightvalue;
   unsigned long  lowtvalue;
}  prepacket,*lpprepacket;

typedef struct TAGNEXTPACKET
{
  unsigned char fromaccountId;
  unsigned char toaccountid;
  unsigned long  heightvalue;
  unsigned long  lowtvalue;
}nextpacket,*lpnextepacket;

typedef struct TAGRETPACKET
{
   unsigned char txid;
   unsigned char opeatortype;
   unsigned char ResultCheck;
   unsigned char accountid;
   unsigned int outheight;
   unsigned long  heightvalue;
   unsigned long  lowtvalue;

}retpacket,*lpretpacket;

typedef struct TAGOPETATE
{
    retpacket muls;
    retpacket add;

}VmOperate,*lpVmOperate;

typedef struct TAGCHUCK
{
  unsigned char type;
  unsigned short txid;
  unsigned short pos;
  unsigned short length;

}chuck,*lpchuck;

inline long ByteReverse(long value)
{
    value = ((value & 0xFF00FF00) >> 8) | ((value & 0x00FF00FF) << 8);
    return (value<<16) | (value>>16);
}
inline int ByteReverse(int value)
{
    return (value<<8) | (value>>8);
}
inline unsigned long ByteReverse(unsigned long value)
{
    value = ((value & 0xFF00FF00) >> 8) | ((value & 0x00FF00FF) << 8);
    return (value<<16) | (value>>16);
}
inline unsigned int ByteReverse(unsigned int  value)
{
    return (value<<8) | (value>>8);
}

typedef struct  {
	unsigned char ArbitratorAccCount;
	unsigned char vAccountCount;
	unsigned char AppealTxCount;
}REV_HEAD,*P_REV_HEAD;


typedef struct  {
        unsigned char len;
	unsigned char Contract[1];
}FIRST_CONTRACT_DATA,P_FIRST_CONTRACT_DATA;


typedef struct  {
        unsigned char len;
	unsigned char Contract[1];
}FLOW_CONTRACT_DATA,P_FLOW_CONTRACT_DATA;


typedef struct 
{
	char version[2];
	unsigned char ArbitratorAccCount;
	unsigned char vAccountCount;
	unsigned char AppealTxCount;
	unsigned char sigConter;
	unsigned char sigVec[1];
}RX_DATA,*P_RX_DATA;

unsigned short GetVecLen(void * data)
{
   unsigned char tem[5];
   unsigned short ret = 0 ;
  memcpy(tem,data,5);
   if(tem[0] < 0xFD)
   {
     return tem[0];
   }
   else if(tem[0] == 0xFD)
   {
     memcpy(&ret,&tem[1],2);
    }
   return ret;
}

typedef struct 
{
    unsigned char Vaule[8];
    unsigned char FirstCiphertext[32];
    unsigned long FirstSum;  
    unsigned char ASig[64];
    unsigned char SecondCiphertext[32];
     unsigned long  SecondSum; 
    unsigned char BSig[64];
    unsigned char random[8];
    unsigned char ASig2[64];
}FIRST_BET_DATA,*P_FIRST_BET_DATA;


//unsigned short GetFlowContractConter(void * data)
//{
//   #pragma data_alignment = 1
//    RX_DATA* pxData = (RX_DATA*)data;
//    if(pxData->FristContractlen)
//}

bool EasyVerfyData(void * data)
{
    #pragma data_alignment = 1
    RX_DATA* pxData = (RX_DATA*)data;
    if(pxData->AppealTxCount == 0)
    {
      return false;
    }
    
     if(pxData->AppealTxCount == 0)
      {   
        if(pxData->sigConter == 1)//check the first data len is right
        {       
          return (GetVecLen(&pxData->sigVec[1]) == sizeof(FIRST_BET_DATA)) ;  
        }
        if(pxData->sigConter == 2)//if tow have signed, this is the last banlance package
        {
          return true;
        } 
      } 
    else  if(pxData->AppealTxCount == 1)
    {
      return true;
    }

      return false;
   
}

 unsigned char getTrantionConter(void * data)
 {
    #pragma data_alignment = 1
    RX_DATA* pxData = (RX_DATA*)data;
    if(pxData->AppealTxCount == 0)
    {
       if(pxData->sigConter == 2)//if tow have signed, this is the last banlance package
        {
          return 3;
        } 
      return 1;
    }
    if(pxData->AppealTxCount == 1)
    {
      return 2;
    }   
    return  0; //unreachable
 }
unsigned long CalcSum( unsigned char *pdata,unsigned char len)
{
  unsigned long ret = 0 ;
  while(len--)
  {
    ret += pdata[len];
  }
  return ret;
}


FIRST_BET_DATA const * GetFirstData(void * data)
{
  	RX_DATA* pxData = (RX_DATA*) data;
     return  (FIRST_BET_DATA const *)&pxData->sigVec[1+3];
}

#define  ERR_DATA   1
#define  B_CHEAT   2
#define  A_CHEAT    3

#define RUN_SCRIPT_OK   0
#define RUN_SCRIPT_DATA_ERR   1


unsigned char GetFirstContractDataState(FIRST_BET_DATA const * PbetData)
{
  if(CheckSig(PbetData->Vaule,sizeof(PbetData->Vaule)+sizeof(PbetData->FirstCiphertext)+sizeof(PbetData->FirstSum),PbetData->ASig))
  {
     if(CheckSig(PbetData->Vaule,8+32+4+64+32+4,PbetData->BSig))
     {
        if(CheckSig(PbetData->Vaule,8+32+4+64+32+4+64+8,PbetData->ASig2))
       {
         unsigned char tepmpdata[32];
         Decode(PbetData->FirstCiphertext,PbetData->random,tepmpdata);
         if(CalcSum(tepmpdata,sizeof(tepmpdata))== PbetData->FirstSum)
         {
            return B_CHEAT; 
         }
        
       }
     }
  }  
  return ERR_DATA;  
}
//    #pragma data_alignment = 1
//typedef struct TAGRETPACKET
//{
//   unsigned char txid;
//   unsigned char opeatortype;
//   unsigned char ResultCheck;
//   unsigned char accountid;
//   unsigned int outheight;
//   unsigned long  heightvalue;
//   unsigned long  lowtvalue;
//}retpacket,*lpretpacket;
//
//    #pragma data_alignment = 1
//typedef struct TAGOPETATE
//{
//    retpacket muls;
//    retpacket add;
//
//}VmOperate,*lpVmOperate;



enum OperType {
	ADD_FREE = 1,  //add to freedom_fund 3
	MINUS_FREE = 2, // minus freedom_fund
	ADD_SELF_FREEZD = 3,  // add self_freezd_fund 6
	ADD_INPUT_FREEZD = 4,  //4
	MINUS_FREE_TO_OUTPUT = 5,
	MINUS_OUTPUT = 6,
	MINUS_OUTPUT_OR_FREE = 7,
	MINUS_OUTPUT_OR_FREE_OR_SELF = 8,
	MINUS_INPUT = 9,
	MINUS_INPUT_OR_FREE = 10,   ///
	MINUS_INPUT_OR_FREE_OR_SELF = 11, //
	NULL_OPERTYPE,
};

void OutPutOpratorAdd(VmOperate *data , unsigned char len)
{
   unsigned short pos = 0 ;
   unsigned short curlen =exchangdata[pos];
   curlen =sizeof(*data);
   exchangdata[pos]+=len;
   while(len--)
   {
     memcpy(&exchangdata[curlen+1],data++,sizeof(*data));
     curlen += sizeof(*data);
   }

}
void OutPutOprator(VmOperate *data , unsigned char len)
{
   unsigned short pos = 0 ;
   exchangdata[pos++]=len;
   while(len--)
   {
     memcpy(&exchangdata[pos],data++,sizeof(*data));
     pos += sizeof(*data);  
   }
   
}



void ProcessFirstBet(void * data)
{
    FIRST_BET_DATA const * PbetData = GetFirstData(data);
    unsigned char state = GetFirstContractDataState(PbetData);
    if(state == ERR_DATA)
    {
      __exit(RUN_SCRIPT_DATA_ERR);
    }
    if(B_CHEAT == state)
    {
      #pragma data_alignment = 1
      VmOperate out;
      out.add.accountid =0;
      memcpy(&out.add.heightvalue,PbetData->Vaule,sizeof(PbetData->Vaule));
      out.add.ResultCheck = true;     
      out.add.outheight = 500;
      out.add.opeatortype = ADD_INPUT_FREEZD;
      
      out.muls.accountid = 1;
      memcpy(&out.muls.heightvalue,PbetData->Vaule,sizeof(PbetData->Vaule));
      out.muls.ResultCheck = true;     
      out.muls.outheight = 0;
      out.muls.opeatortype = MINUS_FREE;
      OutPutOprator(&out,1);
       __exit(RUN_SCRIPT_OK);       
    }
}





//__root  char data[] ={0x00,0x00,0x01,0x02,0x01,0x0e,0x01,0x02,0x1e,0x00,0x07,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x01,0x02,0x01,0x01,0x0a,0x00,0x01,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02};
//__root  char data[] ={0x01,0x02,0x00,0x0e,0x01,0x02,0x1e,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x02,0x01,0x02,0x00};
//__root  char data[] ={0x01,0x02,0x02,0x0e,0x01,0x02,0x1e,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x02,0x01,0x02,0x02,0x01,0x0a,0x00,0x01,0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x01,0x02,0x02,0x18,0x14,0x02,0x01,0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x01,0x02,0x01,0x00};
__root  char data[] ={0x0a,0x00,0x01,0x02,0x02,0x0e,0x01,0x02,0x1e,0x00,0x07,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x01,0x02,0x02,0x01,0x0a,0x00,0x01,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,02,0x17,0x20,0x02,0x01,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00};
int main()
{
   if(EasyVerfyData(exchangdata) == true)
   {
      unsigned char conter = getTrantionConter(exchangdata);
      switch(conter)
      {
      case 1:
        ProcessFirstBet(exchangdata);
        break;
      case 2:
        
        
      }
   }
  __exit(RUN_SCRIPT_DATA_ERR);
}
