#ifndef ARR_PATH_EXCEPTION_HPP
#define ARR_PATH_EXCEPTION_HPP
//
// Copyright (c) 2012, 2015, 2021
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

#include "arr/syscall_exception.hpp"

namespace arr {

///
/// \ingroup exceptions
/// Exception referring to a failed system call involving a path
///
struct path_exception : syscall_exception {
  using errno_exception::errno_t;

  ///
  /// Construct a path_exception
  ///
  /// @param function_name Function name
  /// @param path_name     Path resulting in an error
  /// @param e             Error number (defaults to current value of errno)
  ///
  path_exception(
      std::string function_name,
      std::string path_name,
      errno_t e = errno)
    : path_exception{source_context{nullptr, nullptr, 0},
      function_name, path_name, e}
  {
  }

  ///
  /// Construct a path_exception
  ///
  /// @param where         Context of the exception
  /// @param function_name Function name
  /// @param path_name     Path resulting in an error
  /// @param e             Error number (defaults to current value of errno)
  ///
  path_exception(
      const source_context& where,
      std::string function_name,
      std::string path_name,
      errno_t e = errno);

  ~path_exception() noexcept = default;
  path_exception(const path_exception& ) = default;
  path_exception(      path_exception&&);
  path_exception& operator=(const path_exception& ) = default;
  path_exception& operator=(      path_exception&&);
  DEFINE_CLONE

  virtual void raise() { throw *this; }

  /// Description of the error
  virtual std::string format() const;

  /// Path resulting in an error
  const std::string& path() const { return _path; }

private:
  std::string _path;    ///< Path resulting in an error
};

}

#endif
