#include <string.h>
#include<stdlib.h>
#include <stdio.h>
#include"VmSdk.h"
#define TestCheck(a) {if(!(a)) { PrintfLine(__LINE__); return false;}}
int ProcessSdk(char*pcontact);
int ProcessRollBack(char*pcontact);
bool ProcessScript(char*pcontact);
