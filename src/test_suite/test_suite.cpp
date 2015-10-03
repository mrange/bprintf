// ----------------------------------------------------------------------------------------------
// Copyright 2015 Mårten Rånge
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------------------------------------------------------------------------

#include "stdafx.h"

#include <cstdio>
#include <cstdlib>

#include <chrono>
#include <sstream>
#include <tuple>

#include "../bprintf/core.hpp"
#include "../bprintf/formatters.hpp"

namespace
{

  template<typename TPredicate>
  inline auto time_it (TPredicate && predicate)
  {
    auto past   = std::chrono::high_resolution_clock::now ();
    auto result = predicate ();
    auto now    = std::chrono::high_resolution_clock::now ();
    auto diff   = now - past;
    auto ms     = std::chrono::duration_cast<std::chrono::milliseconds> (diff);

    return std::make_tuple (result, ms.count ());
  }

  constexpr auto count = 1000000;

  int test_sstream ()
  {

    for (auto iter = 0; iter < count; ++iter)
    {
      std::ostringstream ss;
      ss << "Hello: " << iter;
      auto res = ss.str ();
    }

    return 0;
  }

  int test_sprintf ()
  {
    char buffer [64] {};

    for (auto iter = 0; iter < count; ++iter)
    {
      sprintf_s (buffer, "Hello: %d", iter);
    }

    return 0;
  }

  int test_bsprintf ()
  {
    using namespace better_printf;

    chars_type buffer;
    buffer.reserve (64);

    for (auto iter = 0; iter < count; ++iter)
    {
      buffer.clear ();
      bsprintf (buffer, "Hello: %0%", iter);
    }

    return 0;
  }
}

extern void test__linkage ();

int main()
{
  using namespace better_printf;

  test__linkage ();

  std::string const something = "Something";
  std::string else_           = "Else";

  char const * ptr = "Again";

  bprintf (
      "Hello: %2+4% %1-4:0x% %0% %3% %5% %4% %6:g% %7%\n"
    , std::move (else_)
    , 0xCAFE
    , "Yo yo"
    , std::string ("There")
    , something
    , ptr
    , 3.14
    );

#ifdef NDEBUG
  auto ms = [] (auto && v) { return std::get<1> (std::forward<decltype (v)> (v)); };

  printf ("Performance run\n");

  auto result_sstream   = ms (time_it (test_sstream));
  printf ("sstream  : %llu\n", result_sstream);

  auto result_sprintf   = ms (time_it (test_sprintf));
  printf ("sprintf  : %llu\n", result_sprintf);

  auto result_bsprintf  = ms (time_it (test_bsprintf));
  printf ("bsprintf : %llu\n", result_bsprintf);

#endif

  return 0;
}

