
#include <string.h>
#include<stdlib.h>
#include <stdio.h>
#include"VmSdk.h"

#define add_moth      1440l*30l

//#define add_moth      2
enum GETDAWEL{
	TX_REGID = 0x01,
	TX_BASE58 = 0x02,
};
typedef struct {
	unsigned char address[35]; //  ȫ���ǵ�ַ
	Int64 money;               //  �ܽ�� >= freemoney + freeMothmoney * 12 * 3
	Int64 freemoney;           //  ���ɽ��
	Int64 freeMothmoney;       //  ÿ��������Ľ��(�ɶ����תΪ���ɽ��)
}IPO_USER;

//const static char SendAccount[6]="\x00\x00\x00\x00\x14\x00";
const static char SendAccount[6]="\x00\x00\x00\x00\x02\x00";  //"50717-3"

bool WriteWithDrawal(const char *account,Int64 money){
	VM_OPERATE writeCode[2];
	VM_OPERATE ret;
	ret.type = REG_ID;
	memcpy(ret.accountid,account,sizeof(ret.accountid));
	memcpy(&ret.money,&money,sizeof(Int64));
	ret.opeatortype = ADD_FREE;
	writeCode[0]=ret;

	char accountid[6] = {0};     //ȡĿ���˻�ID
	if(!GetScriptID(&accountid)){
		return false;
	}

	ret.opeatortype = MINUS_FREE;
	memcpy(ret.accountid,&accountid,sizeof(ret.accountid));
	writeCode[1]=ret;
	WriteOutput((VM_OPERATE*)writeCode,2);
	return true;
}
/*
* @brief 	step1:��鴫���ĵ�ַ�����Ƿ�Ϸ�\n
* 			step2:����˻����ܹ����ֵ����ɽ���Ϊ��\n
* 			step3:�����ɽ����˻���,ͬʱ�ű��˻���Ҫ��ȥ��Ӧ�Ľ��\n
*/
bool WithDrawal(char type){

	if(type != TX_REGID&&type != TX_BASE58)
		return false;
	char account[6];
	if(GetCurTxAccount(account,sizeof(account)) <= 0){
		return false;
	}
	S_APP_ID id;
	if(type == TX_REGID){
		id.idlen = sizeof(account);
		memcpy(id.ID,account,sizeof(account));
	}else{
		char dacrsaddr[35] ={0};
		if(!GetDacrsAddress(account,sizeof(account),dacrsaddr,sizeof(dacrsaddr)))
			{
				LogPrint("��ȡ�˻�",sizeof("��ȡ�˻�")+1,STRING);
				return false;
			}
		id.idlen = strlen((char*)dacrsaddr);
		memcpy(id.ID,dacrsaddr,sizeof(dacrsaddr));
	}

	Int64 balance;
	if(!GetUserAppAccFreeValue(&balance,&id)){
		LogPrint("�˻����ʧ��",sizeof("�˻����ʧ��")+1,STRING);
		return false;
	}
	Int64 compare;
	Int64Inital(&compare,"\x00",1);
	if(Int64Compare(&balance, &compare) == COMP_EQU){
		LogPrint("��ȡ�˻����Ϊ��",sizeof("��ȡ�˻����Ϊ��")+1,STRING);
		   return false;
	}

	APP_ACC_OPERATE appID;
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	memcpy(&appID.mMoney,&balance,sizeof(Int64));
	appID.FundTag.idlen = 0 ;
	appID.opeatortype = SUB_FREE_OP;

	if(!WriteAppOperateOutput(&appID, 1)){
				return false;
			}

  if(!WriteWithDrawal(account,balance)){
	  return false;
  }
   return true;
}
/*
* @brief 	step1:ת���˵��˻��Ƿ����\n
* 			step2:���ת�����Ƿ���ϵͳָ����ת���˻�\n
* 			step3:�˻�ת�˵��ű��еĽ���Ƿ��Ͷ�ʵĽ�����\n
* 			step4:�����н����˻���\n
* 			step5:д�˻��Ķ�����\n
* 			step6:�����ׯ�������ɽ��ӽű��м���\n
*/
//// ��ֵ
bool Recharge(IPO_USER *ipouser,unsigned char type)
{
	char account[6];
	if(!GetCurTxAccount(account,sizeof(account))){
		return false;
	}
     // ���ҵ�ַУ��
	if(memcmp(account,SendAccount,sizeof(account)) != 0)
	{
		LogPrint("�˻�����ȷ",sizeof("�˻�����ȷ")+1,STRING);
		return false;
	}
     // �ܽ�����ֵ���Ҫ���
	Int64 paymoey;
	Int64Inital(&paymoey,"\x00",1);
	 if(!GetCurTxPayAmount(&paymoey)){
		 return false;
	 }
	 if(Int64Compare(&ipouser->money, &paymoey) != COMP_EQU){
		 LogPrint("��ֵ����ȷ1",sizeof("��ֵ����ȷ1")+1,STRING);
	 	  return false;
	 }

	 //�������,����ֵ���
	 Int64 freezedMoney,maxMoney;
	 Int64Inital(&freezedMoney,"\x00",1); //���ɽ��¶������С��0
	 if((Int64Compare(&ipouser->freemoney, &freezedMoney) == COMP_LESS) ||
		   (Int64Compare(&ipouser->freeMothmoney, &freezedMoney) == COMP_LESS)){
		 LogPrint("��ֵ����ȷ2",sizeof("��ֵ����ȷ2")+1,STRING);
		  return false;
	 }
	 Int64Inital(&freezedMoney,"\x01",1); // �ܽ���С��1,����С�����ɽ����¶�����
	 if((Int64Compare(&ipouser->money, &freezedMoney) == COMP_LESS) ||
		  (Int64Compare(&ipouser->money, &ipouser->freemoney) == COMP_LESS) ||
		  (Int64Compare(&ipouser->money, &ipouser->freeMothmoney) == COMP_LESS)){
		 LogPrint("��ֵ����ȷ3",sizeof("��ֵ����ȷ3")+1,STRING);
		  return false;
	 }
	 //����������ֵ���
	 Int64Inital(&maxMoney,"\x01\x63\x45\x78\x5D\x8A\x00\x00",8);     //�ܽ��, 10 00000000 00000000
	 Int64Inital(&paymoey,"\x00\x03\x8D\x7E\xA4\xC6\x80\x00",8); //���ɽ�                 10000000 00000000
	 if((Int64Compare(&ipouser->money, &maxMoney) != COMP_LESS) ||
		  (Int64Compare(&ipouser->freemoney,&paymoey) != COMP_LESS) ||
		  (Int64Compare(&ipouser->freeMothmoney,&paymoey) != COMP_LESS)){
		 LogPrint("��ֵ����ȷ4",sizeof("��ֵ����ȷ4")+1,STRING);
		  return false;
	 }

	 /// ����ֵ�ܽ������ɽ����϶������Ƿ����
	 Int64 mul;
	  char freezeNum = 10;  //11 ��Ϊ����10��
     if(type == 1)
     {
	 Int64Inital(&mul,"\x01",1); // ����1��
     }else{
//	 Int64Inital(&mul,"\x0b",1); // ����11��
	 Int64Inital(&mul,&freezeNum,1);
     }
	 if(Int64Mul(&ipouser->freeMothmoney,&mul,&freezedMoney)) //�ܽ���С�� �ܵ��¶�����
	 {
		 if((Int64Compare(&freezedMoney,&mul) == COMP_LESS) ||
			   (Int64Compare(&freezedMoney,&ipouser->freeMothmoney) == COMP_LESS) ||
			   (Int64Compare(&ipouser->money,&freezedMoney) == COMP_LESS)){
			 LogPrint("Int64Mul err0",sizeof("Int64Mul err0")+1,STRING);
			  return false;
		 }
	 }
	 else{
		 LogPrint("Int64Mul err1",sizeof("Int64Mul err1")+1,STRING);
	 	 return false;
	 }
	 if(Int64Add(&freezedMoney,&ipouser->freemoney,&freezedMoney))
	 {
		 if(Int64Compare(&ipouser->money, &freezedMoney) != COMP_EQU){
			 LogPrint("������ͳ�ֵ����ȷ5",sizeof("������ͳ�ֵ����ȷ5")+1,STRING);
			  return false;
		 }
	 }else{
		 LogPrint("Int64Add err",sizeof("Int64Add err")+1,STRING);
	 	 return false;
	 }

	 VM_OPERATE ret;
	 ret.type = BASE_58_ADDR;
	 ret.opeatortype = ADD_FREE;

	 //// ��Ǯ�����ɽ��
	 memcpy(ret.accountid,ipouser->address,sizeof(ret.accountid));
	 memcpy(&ret.money,&ipouser->freemoney,sizeof(Int64));
	 WriteOutput(&ret,1);

	 unsigned long curheight = 93926;//GetCurRunEnvHeight();  50726

	S_APP_ID id;
	APP_ACC_OPERATE appID;
	id.idlen = strlen((char*)ipouser->address);
	memcpy(id.ID,ipouser->address,sizeof(ipouser->address));
	memcpy(&appID.AppAccID,&id,sizeof(S_APP_ID));
	memcpy(&appID.mMoney,&ipouser->freeMothmoney,sizeof(Int64));
	appID.FundTag.idlen = 0 ;
	appID.opeatortype = ADD_FREEZED_OP;
    if(type == 1)
    { // ����1��
		 appID.outheight = 525926;
		if(!WriteAppOperateOutput(&appID, 1)){
					return false;
				}
    }else
    { // ����11��  ��Ϊ����10��
	 for(char month = 0;month < freezeNum;month++)   //11
	 {
		 appID.outheight = curheight  + add_moth*(month+1);
		if(!WriteAppOperateOutput(&appID, 1)){
					return false;
				}
	 }
    }
	char accountid[6] = {0};
	if(!GetScriptID(&accountid)){
		return false;
	}

	ret.type = REG_ID;
	ret.opeatortype = MINUS_FREE;
	memcpy(&ret.money,&ipouser->freemoney,sizeof(Int64));
	memcpy(ret.accountid,&accountid,sizeof(accountid));
	WriteOutput(&ret,1);
	return true;
}
 //__xdata static  char pcontact[100] ="\xff\x01\x02";//\x75\x4b\x66\x4e\x79\x71\x36\x7a\x73\x75\x79\x32\x43\x4d\x47\x4d\x46\x56\x72\x51\x4c\x75\x47\x69\x39\x35\x55\x43\x37\x77\x36\x44\x56\x10\x27\x00\x00\x00\x00\x00\x00\xc8\x00\x00\x00\x00\x00\x00\x00\x16\x00\x00\x00\x00\x00\x00\x00\x00";
int main( void )
{

	__xdata static  char pcontact[100] ;
	 unsigned long len = 100;
	GetCurTxContact(pcontact,len);
	switch(pcontact[0])
	{
		case  0xff:
		{
			if(pcontact[1] != 0x01 && pcontact[1] != 0x02){
				LogPrint("data error",sizeof("data error"),STRING);
				__VmExit(RUN_SCRIPT_DATA_ERR);
			}
			if(pcontact[1] == 0x01)
			{
				LogPrint("WithDrawal",sizeof("WithDrawal"),STRING);
				if(!WithDrawal(pcontact[2]))
				{
					LogPrint("WithDrawal error",sizeof("WithDrawal error"),STRING);
					__VmExit(RUN_SCRIPT_DATA_ERR);
				}
			}
			else if(pcontact[1] == 0x02)
			{
				LogPrint("Recharge",sizeof("Recharge"),STRING);
				IPO_USER user;
				memcpy(&user,&pcontact[2],sizeof(IPO_USER));
				if(!Recharge((IPO_USER*)&user,0))
//				if(!Recharge((IPO_USER*)&user,1))
				{
					LogPrint("Recharge error",sizeof("Recharge error"),STRING);
					__VmExit(RUN_SCRIPT_DATA_ERR);
				}
			}
			break;
		}
		default:
		{
			LogPrint("tx format error",sizeof("tx format error"),STRING);
			__VmExit(RUN_SCRIPT_DATA_ERR);
			break;
		}
	}
	__VmExit(RUN_SCRIPT_OK);
	return 0;
}
