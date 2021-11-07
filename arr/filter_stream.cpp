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

#include "arr/filter_stream.hpp"
#include "arr/pipe.hpp"
#include "arr/child.hpp"
#include <algorithm>

namespace {

using patterns_t = std::list<std::pair<std::regex, arr::arguments>>;

const arr::arguments * search(const patterns_t& patterns, const char * path) {
  for (auto& i : patterns) {
    if (std::regex_match(path, i.first)) {
      return &i.second;
    }
  }
  return nullptr;
}

wrap::process_id filter_open_input(
    const patterns_t& patterns, wrap::file_descriptor& fd, const char * path) {
  auto filter = search(patterns, path);
  return filter ? ifilter(fd, *filter) : wrap::process_id();
}

wrap::process_id filter_open_output(
    const patterns_t& patterns, wrap::file_descriptor& fd, const char * path) {
  auto filter = search(patterns, path);
  return filter ? ofilter(fd, *filter) : wrap::process_id();
}

}

namespace arr {

wrap::process_id
ifilter(wrap::file_descriptor& fd, const arr::arguments& args) {
  arr::pipe p(SOURCE_CONTEXT);
  wrap::process_id pid =
    arr::spawn_stdio(args, ::environ, fd.get(), p.write.get());
  fd = p.read.release();
  return pid;
}

wrap::process_id
ofilter(wrap::file_descriptor& fd, const arr::arguments& args) {
  arr::pipe p(SOURCE_CONTEXT);
  wrap::process_id pid =
    arr::spawn_stdio(args, ::environ, p.read.get(), fd.get());
  fd = p.write.release();
  return pid;
}

filter_ifstream::filter_ifstream(
    arr::source_context context,
    const arr::arguments& filter,
    const char * path, int open_flags)
  : file_descriptor(wrap::open(context, path, open_flags|O_RDONLY, 0))
  , process_id(ifilter(*this, filter))
  , fd_istream(static_cast<file_descriptor&>(*this))
{
}

filter_ifstream::filter_ifstream(
    source_context context,
    const char * path, int open_flags)
  : file_descriptor(wrap::open(context, path, open_flags|O_RDONLY, 0))
  , process_id(filter_open_input(patterns, *this, path))
  , fd_istream(static_cast<file_descriptor&>(*this))
{
}

filter_ofstream::filter_ofstream(
    arr::source_context context,
    const arr::arguments& filter,
    const char * path, int open_flags, mode_t mode)
  : file_descriptor(wrap::open(context, path, open_flags|O_WRONLY, mode))
  , process_id(ofilter(*this, filter))
  , fd_ostream(static_cast<file_descriptor&>(*this))
{
}

filter_ofstream::filter_ofstream(
    source_context context,
    const char * path, int open_flags, mode_t mode)
  : file_descriptor(wrap::open(context, path, open_flags|O_WRONLY, mode))
  , process_id(filter_open_output(patterns, *this, path))
  , fd_ostream(static_cast<file_descriptor&>(*this))
{
}

std::list<std::pair<std::regex, arr::arguments>> filter_ifstream::patterns;
std::list<std::pair<std::regex, arr::arguments>> filter_ofstream::patterns;

}
