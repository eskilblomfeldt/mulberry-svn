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

// base64.h - base64 encode/decode

#ifndef __BASE64__MULBERRY__
#define __BASE64__MULBERRY__

#include <cstddef>

using namespace std;

char* base64_encode(const unsigned char* value, size_t vlen);
unsigned char* base64_decode(const char* value, size_t& rlen);

#endif
