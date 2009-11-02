/*
    Copyright (c) 2007 Cyrus Daboo. All rights reserved.
    
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    
        http://www.apache.org/licenses/LICENSE-2.0
    
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

	// Use PowerPlant-specific Precompiled header

#pragma once

#include	"MSL MacHeadersMach-O.h"
//#include "MacHeadersMach-O.c"
//#include	<ctype>
//#include	<cstdio>
//#include	<cstdlib>
//using namespace std;

//	Not defined on Mac so need these here
#define htonl(x)		(x)
#define htons(x)		(x)
#define ntohl(x)		(x)
#define ntohs(x)		(x)

#define LINUX_NOTYET
//#include	"Mac2Linux.h"
#include	"os_dep.h"
