#include <string.h>
#include<stdlib.h>
#include <stdio.h>
#include"VmSdk.h"

///���ڱ� ��������
/*
 *��һ������
 *��Լ���ݣ����m ��ַ ��� ��ַ ��� ....
 *ִ�д˽ű�:���m����Լ�˻��������txhash�����ݿ�
 *��������ݿ�����������¼���Ϳ���ִ�д�� ����ִ�У�ֱ����¼>2,ִ�д��������,��������ɾ��
 */

///�ڱҵ�������
/**
 * ����
 * @return
 */
/*! \mainpage Developer documentation
 *
 * \section ��Ҫת�˵��˷��ͽ��װ�
 *�����������Ҫת�Ľ��Ѿ���ַ\n
 *������bool ProcessFirstContract(const FIRST_CONTRACT* pContract);\n
 */
typedef struct  {
	ACCOUNT_ID 	Sender;						//!<ת����ID������6�ֽڵ��˻�ID��
	int nHeight;							//!<��ʱ���Ը߶�
	Int64 nPayMoney;						//!<ת�˵���֧���Ľ��
	unsigned short len;                     //!<����Ǯ�˻���Ϣ����
	char buffer[1];         				//!<����Ǯ�˻���Ϣ
}CONTRACT;

typedef struct  {
	ACCOUNT_ID 	account;						//!<����Ǯ��ID������6�ֽڵ��˻�ID��
	Int64 nReciMoney;						    //!<	�յ�Ǯ�Ľ��
} ACCOUNT_INFO;
/*
* @brief 	step1:ת���˵��˻��Ƿ����\n
* 			step2:���ת�����Ƿ���Ȩ��\n
* 			step3:������Ǯ���˻��Ƿ����\n
* 			step4:���תǮ����ܵ�Ǯ�Ƿ�ƽ��\n
*/
bool CheckContact(const CONTRACT* const pContract)
{
	return true;
}
void WriteOperate()
{

}
/*
* @brief 	step1:��鵱ǰ��Լ�����Ƿ�Ϸ�\n
* 			step2:�������ݿ������з��д���2�������ݣ�������������Ч�ģ����ָ����
* 			step3:д�뵱ǰtx��hash�����ݿ�\n
*/
bool ProcessContract(const CONTRACT* const pContract)
{
	if(!CheckContact(pContract))
	{
		return false;
	}
	WriteOperate();
}
int main()
{
	__xdata static  char pcontact[100];
	unsigned long len = 100;
 	GetMemeroyData(pcontact,len);
 	if(!ProcessContract((CONTRACT*)pcontact))
 	{
 		__exit(RUN_SCRIPT_DATA_ERR);
 	}
 	__exit(RUN_SCRIPT_OK);
  return 0;
}
