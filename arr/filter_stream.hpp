#ifndef ARR_FILTER_STREAM_HPP
#define ARR_FILTER_STREAM_HPP
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

#include "arr/fcntl.hpp"
#include "arr/file_descriptor.hpp"
#include "arr/process_id.hpp"
#include "arr/fd_stream.hpp"
#include "arr/arg_env.hpp"
#include <utility>
#include <regex>

namespace arr {

/// \addtogroup child_processes
/// @{

///
/// Send the input from a file descriptor through a filter program
///
/// \return             The \c wrap::process_id of the filter program
/// \param[in,out] fd   The file descriptor providing data
/// \param         args Arguments for the filter program
///
wrap::process_id ifilter(wrap::file_descriptor& fd, const arr::arguments& args);

///
/// Send the output to a file descriptor through a filter program
///
/// \return             The \c wrap::process_id of the filter program
/// \param[in,out] fd   The file descriptor receiving data
/// \param         args Arguments for the filter program
///
wrap::process_id ofilter(wrap::file_descriptor& fd, const arr::arguments& args);

///
/// Input stream from a filter program from a file
///
struct filter_ifstream
  : private wrap::file_descriptor
  , public  wrap::process_id
  , public  arr::fd_istream
{

  ///
  /// Create a stream using the given filter to read the given file
  ///
  filter_ifstream(
      arr::source_context,
      const arr::arguments& filter,
      const char * path, int open_flags);
  ///
  /// Create a stream using the given filter to read the given file
  ///
  filter_ifstream(
      const arr::arguments& filter,
      const char * path, int open_flags)
    : filter_ifstream(SOURCE_CONTEXT, filter, path, open_flags) { }

  ///
  /// Create a stream, looking up the filter, to read the given file
  ///
  filter_ifstream(
      arr::source_context,
      const char * path, int open_flags);
  ///
  /// Create a stream, looking up the filter, to read the given file
  ///
  filter_ifstream(
      const char * path, int open_flags)
    : filter_ifstream(SOURCE_CONTEXT, path, open_flags) { }

  ///
  /// Registered filter patterns
  ///
  static std::list<std::pair<std::regex, arr::arguments>> patterns;

  ///
  /// Add a filter pattern
  ///
  static void add_filter(std::regex match, arr::arguments filter) {
    patterns.emplace_back(std::move(match), std::move(filter));
  }

};

///
/// Output stream to a filter program to a file
///
struct filter_ofstream
  : private wrap::file_descriptor
  , public  wrap::process_id
  , public  arr::fd_ostream
{

  ///
  /// Create a stream using the given filter to write the given file
  ///
  filter_ofstream(
      arr::source_context,
      const arr::arguments& filter,
      const char * path, int open_flags, mode_t mode);
  ///
  /// Create a stream using the given filter to write the given file
  ///
  filter_ofstream(
      const arr::arguments& filter,
      const char * path, int open_flags, mode_t mode)
    : filter_ofstream(SOURCE_CONTEXT, filter, path, open_flags, mode) { }

  ///
  /// Create a stream, looking up the filter, to write the given file
  ///
  filter_ofstream(
      arr::source_context,
      const char * path, int open_flags, mode_t mode);
  ///
  /// Create a stream, looking up the filter, to write the given file
  ///
  filter_ofstream(
      const char * path, int open_flags, mode_t mode)
    : filter_ofstream(SOURCE_CONTEXT, path, open_flags, mode) { }

  ///
  /// Registered filter patterns
  ///
  static std::list<std::pair<std::regex, arr::arguments>> patterns;

  ///
  /// Add a filter pattern
  ///
  static void add_filter(std::regex match, arr::arguments filter) {
    patterns.emplace_back(std::move(match), std::move(filter));
  }

};

/// @}

}

#endif
