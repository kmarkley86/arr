#ifndef WRAP_GLOB_HPP
#define WRAP_GLOB_HPP
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
#include <glob.h>
#include <string>
#include <list>

///
/// \file
/// \ingroup system_function_wrappers
///
/// Wrappers for functions in \c <glob.h>
///

namespace wrap {

/// \addtogroup system_function_wrappers
/// @{

///
/// Wrapper for glob(3)
///
/// This is a convenience wrapper.  It does not provide the full power of
/// glob (e.g. for appending results) and has no provision for getting the
/// underlying glob return code if an error occurs.  (A syscall_exception
/// will be thrown, but that might have a zero error code if no system call
/// had failed.)  In contexts where glob must be used carefully, the real
/// system function should be called manually.
///
/// Default flags:
///   - GLOB_NOSORT : Do not sort results
///   - GLOB_BRACE  : Expand {} patterns before globbing
///   - GLOB_TILDE  : Expand ~ to the user's home directory
///
/// If there are no matches, an empty list is returned.  (This is not
/// considered an error.)
///
std::list<std::string> glob(
    arr::source_context,
    const char *pattern,
    int flags = GLOB_NOSORT|GLOB_BRACE|GLOB_TILDE,
    int (*errfunc)(const char *, int) = nullptr);

/// @}

}

#endif
