#ifndef ARR_ARG_ENV_HPP
#define ARR_ARG_ENV_HPP
//
// Copyright (c) 2012, 2021, 2025
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

#include <memory>
#include <string>
#include <vector>
#include <list>
#include <map>

extern "C" char **environ; ///< User environment

namespace arr {

/// \addtogroup child_processes
/// @{

///
/// Convenience class for command-line arguments
///
/// This class provides conversion to and from a list of strings and
/// (const char * const *) for representing command line arguments.  It can be
/// constructed from \c argv.
///
struct arguments : std::list<std::string> {
  arguments(const char * const argv[] = nullptr);
  arguments(const arguments& peer) : std::list<std::string>(peer) { }
  arguments& operator=(arguments peer);
  std::unique_ptr<const char *[]> as_argv() const;
  friend void swap(arguments&, arguments&) noexcept;
};

///
/// Convenience class for the user environment
///
/// This class provides conversion to and from a map of key-value pairs and
/// (const char * const *) for representing the user environment.  It can be
/// constructed from \c envp or \c environ.
///
/// This class is a thin wrapper; it performs no data validation.
///
struct environment : std::map<std::string, std::string> {
  environment(const char * const envp[] = ::environ);
  environment(const environment& peer)
    : std::map<std::string, std::string>(peer) { }
  // Warning: this is a very unsafe interface.  This object must not be
  // modified while the pointer returned by this conversion is in use.
  operator const char * const *() const;
  mutable std::vector<std::string> collapsed;
  mutable std::unique_ptr<const char *[]> compatible;
};

/// @}

}

#endif
