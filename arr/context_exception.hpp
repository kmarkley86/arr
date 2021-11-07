#ifndef ARR_CONTEXT_EXCEPTION_HPP
#define ARR_CONTEXT_EXCEPTION_HPP
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

#include "arr/source_context.hpp"
#include "arr/clone_macros.hpp"
#include <exception>
#include <type_traits>

namespace arr {

///
/// \ingroup exceptions
/// Exception recording a source_context
///
struct context_exception : virtual std::exception, source_context {

  context_exception()
    noexcept(std::is_nothrow_default_constructible<std::string>::value)
    : context_exception(source_context{nullptr, nullptr, 0})
  {
  }

  context_exception(const source_context& where)
    noexcept(std::is_nothrow_default_constructible<std::string>::value);

  ~context_exception() noexcept = default;
  context_exception(const context_exception& ) = default;
  context_exception(      context_exception&&)
    noexcept(std::is_nothrow_move_constructible<std::string>::value);
  context_exception& operator=(const context_exception& ) = default;
  context_exception& operator=(      context_exception&&)
    noexcept(std::is_nothrow_move_assignable<std::string>::value);
  DEFINE_CLONE

  virtual void raise() { throw *this; }

  /// Description of the error
  virtual std::string format() const;

  /// Description of the error using the std::exception interface
  virtual const char * what() const noexcept;

  protected:
  mutable std::string _string;  ///< Cached value of format()

};

}

#endif
