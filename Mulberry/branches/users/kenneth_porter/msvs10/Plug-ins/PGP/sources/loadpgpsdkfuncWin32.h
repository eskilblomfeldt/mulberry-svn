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

#define IMPORT_FUNCTION_PTR(ret, name, args, vars) \
extern "C" { \
typedef ret (*FP_##name) args; \
extern FP_##name p##name; \
}

#define PGP_WIN32		1
#define PGP_DEBUG		0

#include "pgpFeatures.h"
#include "pgpEncode.h"
#include "pgpRandomPool.h"
#include "pgpKeys.h"
#include "pgpErrors.h"
#include "pgpUtilities.h"

#include <stdarg.h>

void PGPSDK_LoadFuncPtrs();

#pragma mark ______________________PGPSDK

//PGPError PGPAddJobOptions(PGPJobRef job, PGPOptionListRef firstOption, ...);
IMPORT_FUNCTION_PTR(PGPError, PGPAddJobOptions, (PGPJobRef job, PGPOptionListRef firstOption, ...), (job, firstOption, __va_start(firstOption)))

//PGPError PGPEncode(PGPContextRef context, PGPOptionListRef firstOption, ...);
IMPORT_FUNCTION_PTR(PGPError, PGPEncode, (PGPContextRef context, PGPOptionListRef firstOption, ...), (context, firstOption, __va_start(firstOption)))

//PGPError PGPDecode(PGPContextRef context, PGPOptionListRef firstOption, ...);
IMPORT_FUNCTION_PTR(PGPError, PGPDecode, (PGPContextRef context, PGPOptionListRef firstOption, ...), (context, firstOption, __va_start(firstOption)))

//PGPOptionListRef PGPODetachedSig(PGPContextRef context, PGPOptionListRef firstOption, ...);
IMPORT_FUNCTION_PTR(PGPOptionListRef, PGPODetachedSig, (PGPContextRef context, PGPOptionListRef firstOption, ...), (context, firstOption, __va_start(firstOption)))

//PGPOptionListRef PGPOSignWithKey(PGPContextRef context, PGPKeyDBObjRef keyDBObjRef, PGPOptionListRef firstOption, ...);
IMPORT_FUNCTION_PTR(PGPOptionListRef, PGPOSignWithKey, (PGPContextRef context, PGPKeyDBObjRef keyDBObjRef, PGPOptionListRef firstOption, ...), (context, keyDBObjRef, firstOption, __va_start(firstOption)))
