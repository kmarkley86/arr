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

#include "arr/path_exception.hpp"
#include <utility>
#include <sstream>

namespace arr {

path_exception::path_exception(
    const source_context& where,
    std::string function_name,
    std::string path_name,
    errno_t e)
  : std::exception{}
  , syscall_exception{where, std::move(function_name), std::move(e)}
  , _path{std::move(path_name)}
{
}

path_exception::path_exception(path_exception&& peer)
  : std::exception{std::move(peer)}
  , syscall_exception{std::move(peer)}
  , _path{std::move(peer._path)}
{
}

path_exception& path_exception::operator=(path_exception&& peer) {
  syscall_exception::operator=(std::move(peer));
  _path = std::move(peer._path);
  return *this;
}

std::string path_exception::format() const {
  std::ostringstream s;
  s << function() << "() path \"" << _path << "\" ";
  s << errno_exception::format();
  return s.str();
}

}
