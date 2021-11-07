#ifndef ARR_TEMP_TEMPLATE_HPP
#define ARR_TEMP_TEMPLATE_HPP
//
// Copyright (c) 2013, 2021
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

#include <type_traits>
#include <cstdio>

//
// This file exists only to share constants between temp_file and temp_dir.
//

//
// Regarding stdio.h:
//
// The following macro name shall be defined as a string constant:
// P_tmpdir  Default directory prefix for tempnam().
//

namespace arr {
namespace filename_template {

constexpr const char name1[] = P_tmpdir  "XXXXXX";
constexpr const char name2[] = P_tmpdir "/XXXXXX";
constexpr auto size1 = std::extent<decltype(name1)>::value;
constexpr auto size2 = std::extent<decltype(name2)>::value;
constexpr bool trailing_slash = '/' == P_tmpdir[sizeof(P_tmpdir)-2];
constexpr auto name = trailing_slash ? name1 : name2;
constexpr auto size = trailing_slash ? size1 : size2;

}
}

#endif
