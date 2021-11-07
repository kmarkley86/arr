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

#include "arr/context_exception.hpp"
#include <utility>
#include <typeinfo>

namespace arr {

context_exception::context_exception(const source_context& where)
  noexcept(std::is_nothrow_default_constructible<std::string>::value)
  : std::exception{}
  , source_context{where}
  , _string{}
{
}

// Cannot default because std::exception does not have a move constructor
context_exception::context_exception(context_exception&& peer)
  noexcept(std::is_nothrow_move_constructible<std::string>::value)
  : std::exception{std::move(peer)}
  , source_context{std::move(peer)}
  , _string{std::move(peer._string)}
{
}

// Cannot default due to virtual base class
context_exception&
context_exception::operator=(context_exception&& peer)
  noexcept(std::is_nothrow_move_assignable<std::string>::value)
{
  std::exception::operator=(std::move(peer));
  source_context::operator=(std::move(peer));
  _string = std::move(peer._string);
  return *this;
}

std::string context_exception::format() const {
  return context();
}

const char * context_exception::what() const noexcept {
  try {
    _string = format();
    return _string.c_str();
  } catch (...) {
    return typeid(*this).name();
  }
}

}
