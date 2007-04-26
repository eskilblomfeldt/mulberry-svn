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

/*
 *	svector.h          // hh 971207 Changed filename from vector to vector.h
 */

#ifndef _SVECTOR_H          // hh 971207 added standard include guards
#define _SVECTOR_H

#include "svector"
//#if __dest_os != __linux_os
#ifndef _MSL_NO_CPP_NAMESPACE            // hh 971207 Added backward compatibility
	using namespace std;
#endif
#else
	using namespace std;
#endif
//#endif
/*     Change record
// hh 971207 Changed filename from svector to svector.h
// hh 971207 added standard include guards
// hh 971207 Added backward compatibility
// hh 990120 changed name of MSIPL flags
*/
