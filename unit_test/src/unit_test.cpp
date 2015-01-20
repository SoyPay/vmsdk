
#include "unit_test.h"
int main()
{
	__xdata static  char pcontact[512];
	GetCurTxContact(pcontact,512);
	switch(pcontact[0])
	{
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
		case 0x05:
				{
					ProcessSdk((char*)pcontact);
					break;
				}
		case 0x06:
		case 0x07:
		case 0x08:
		case 0x09:
		case 0x0a:
		case 0x0b:
				{
					ProcessRollBack((char*)pcontact);
					break;
				}
		case 0x0d:
		case 0x0e:
				{
					ProcessScript((char*)pcontact);
					break;
				}
		case 0x0F:
		case 0x10:
		case 0x11:
		case 0x12:
				{
					ProcessTestDB((char*)pcontact);
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
