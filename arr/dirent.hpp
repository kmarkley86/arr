#ifndef WRAP_DIRENT_HPP
#define WRAP_DIRENT_HPP
//
// Copyright (c) 2012, 2021
// Kyle Markley.  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. Neither the name of the author nor the names of any contributors may be
//    used to endorse or promote products derived from this software without
//    specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include "arr/source_context.hpp"
#include <sys/types.h>
#include <dirent.h>

///
/// \file
/// \ingroup system_function_wrappers
///
/// Wrappers for functions in \c <dirent.h>
///

namespace wrap {

/// \addtogroup system_function_wrappers
/// @{

///
/// Wrapper for opendir(3)
///
DIR * opendir(arr::source_context, const char * filename);

///
/// Wrapper for closedir(3)
///
void closedir(arr::source_context, DIR * dirp);

///
/// Wrapper for readdir(3)
///
struct dirent * readdir(arr::source_context, DIR * dirp);

///
/// Wrapper for rewinddir(3)
///
void rewinddir(arr::source_context, DIR * dirp);

/// @}

}

#endif
