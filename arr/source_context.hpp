#ifndef ARR_SOURCE_CONTEXT_HPP
#define ARR_SOURCE_CONTEXT_HPP
//
// Copyright (c) 2012, 2013, 2021
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

#include <string>

namespace arr {

/// \addtogroup miscellaneous
/// @{

///
/// Source code context
///
struct source_context {

  ///
  /// @details
  ///
  /// This constructor is not intended to be called directly.  You should use
  /// the \c SOURCE_CONTEXT macro as a drop-in replacement wherever you would
  /// call the constructor.
  ///
  constexpr source_context(
      const char * function_name,
      const char * source_file_name,
      unsigned source_line_number) noexcept
    : func(function_name)
    , file(source_file_name)
    , line(source_line_number)
  {
  }

  /// Description of the source code context
  std::string context() const;

  const char * func;    ///< Function name
  const char * file;    ///< Source file name
  unsigned     line;    ///< Line number in source file
};

///
/// Macro representing the current source context
///
#if defined(__clang__) || defined(__GNUC__)
#define SOURCE_CONTEXT arr::source_context{__PRETTY_FUNCTION__, __FILE__, __LINE__}
#else
#define SOURCE_CONTEXT arr::source_context{__func__, __FILE__, __LINE__}
#endif

/// @}

}

#endif
