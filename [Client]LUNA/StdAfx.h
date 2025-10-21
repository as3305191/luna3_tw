#pragma once

#define _WIN32_WINNT	0x0500 
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _FOR_MUKHYANG
#define _CLIENT_
#define _CRYPTCHECK_

#ifdef _GMTOOL_
#define _CHEATENABLE_
#endif
#define _ASSERTENABLE_

#pragma warning(disable:4100) // �������� �ʴ� �Ű�����
#pragma warning(disable:4127) // ���ǽ��� ����� ���
#pragma warning(disable:4201) // ��ǥ�� Ȯ���� ����Ѱ�� ���ο� Struct ���鶧 �̸��� ���� �ʾ���.
#pragma warning(disable:4512) // ���� �����ڸ� �������� ���߽��ϴ�. 

#include <windows.h>
#include <wingdi.h>
#include <ole2.h>
#include <initguid.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <crtdbg.h>
#include <vfw.h>
#include <dbghelp.h>
#include <time.h>
#include <shellapi.h.>
#include <direct.h>
#include <conio.h>
#include <map>
#include <set>
#include <hash_map>
#include <hash_set>
#include <list>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include <functional>
#include <queue>
#include "resource.h"

#include "..\[lib]yhlibrary\yhlibrary.h"
#include "..\[lib]yhlibrary\cLinkedList.h"
#include "..\[lib]yhlibrary\PtrList.h"
#include "..\[lib]yhlibrary\IndexGenerator.h"
#include "..\4DyuchiGXGFunc\global.h"
#include "..\[CC]Header\protocol.h"
#include "..\[CC]Header\CommonDefine.h"
#include "..\[CC]Header\CommonGameDefine.h"
#include "..\[CC]Header\ClientGameDefine.h"
#include "..\[CC]Header\CommonStruct.h"
#include "..\[CC]Header\CommonGameFunc.h"

#include "Motion.h"
#include "MHNetwork.h"
#include "ErrorMsg.h"
#include "DirectoryManager.h"

#ifdef _TL_LOCAL_
#include "ThaiBreaker.h"
#endif

extern BOOL g_bUsingEnginePack;