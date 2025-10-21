#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT	0x0500 
#define _CRYPTCHECK_

//090925 pdy MapServer Warning Level4 ���� 
#pragma warning(disable:4100) // �������� �ʴ� �Ű�����
#pragma warning(disable:4127) // ���ǽ��� ����� ���
#pragma warning(disable:4201) // ��ǥ�� Ȯ���� ����Ѱ�� ���ο� Struct ���鶧 �̸��� ���� �ʾ���
#pragma warning(disable:4512) // ���� �����ڸ� �������� ���߽��ϴ�. 

#include <windows.h>
#include <winsock2.h>
#include <ole2.h>
#include <initguid.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <crtdbg.h>
#include <time.h>
#include <tchar.h>
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
#include <stack>
#include <conio.h>
#include <queue>
#include <cstdlib>
 
#include "..\[lib]yhlibrary\yhlibrary.h"
#include "..\[lib]yhlibrary\HashTable.h"
#include "..\[lib]yhlibrary\PtrList.h"
#include "..\[lib]yhlibrary\cLooseLinkedList.h"
#include "..\[lib]yhlibrary\cLinkedList.h"
#include "..\[lib]yhlibrary\IndexGenerator.h"
#include "..\[lib]yhlibrary\cConstLinkedList.h"
#include "..\[CC]Header\vector.h"
#include "..\[CC]Header\protocol.h"
#include "..\[CC]Header\CommonDefine.h"
#include "..\[CC]Header\CommonGameDefine.h"
#include "..\[CC]Header\ServerGameDefine.h"
#include "..\[CC]Header\CommonStruct.h"
#include "..\[CC]Header\ServerGameStruct.h"
#include "..\[CC]Header\CommonGameFunc.h"
#include "..\[CC]Header\GameResourceStruct.h"
#include "..\[CC]Header\CommonCalcFunc.h"
#include "..\[CC]ServerModule\DataBase.h"
#include "..\[CC]ServerModule\Console.h"
#include "ServerSystem.h"