#ifndef ARR_ERRNO_EXCEPTION_HPP
#define ARR_ERRNO_EXCEPTION_HPP
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

#include "arr/context_exception.hpp"
#include <type_traits>
#include <cerrno>
#include <system_error>

namespace arr {

///
/// \ingroup exceptions
/// Exception holding an errno value
///
struct errno_exception : context_exception {
  typedef std::remove_reference<decltype(errno)>::type errno_t;

  ///
  /// Construct an errno_exception
  ///
  /// @param e Error number (defaults to current value of errno)
  ///
  errno_exception(errno_t e = errno)
    : errno_exception{source_context{nullptr, nullptr, 0}, e}
  {
  }

  ///
  /// Construct an errno_exception
  ///
  /// @param where Context of the exception
  /// @param e     Error number (defaults to current value of errno)
  ///
  errno_exception(const source_context& where, errno_t e = errno);

  ~errno_exception() noexcept = default;
  errno_exception(const errno_exception& ) = default;
  errno_exception(      errno_exception&&);
  errno_exception& operator=(const errno_exception& ) = default;
  errno_exception& operator=(      errno_exception&&);
  DEFINE_CLONE

  virtual void raise() { throw *this; }

  /// Description of the error
  virtual std::string format() const;

  /// \c error_code object representing the error
  const std::error_code& code() const { return _code; }

  /// Raw error number
  int value() const { return _code.value(); }

  /// Raw error string
  std::string message() const { return _code.message(); }

private:
  std::error_code _code;        ///< \c error_code representing the error
};

}

#endif
