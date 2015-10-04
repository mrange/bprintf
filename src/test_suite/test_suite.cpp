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

#include "../bprintf/bprintf.hpp"

struct TestClass
{
};

namespace better_printf
{
  namespace formatters
  {
    void format (
        details::formatter_context const & context
      , TestClass const &
      )
    {
      BPRINTF_ASSERT (context.format_begin);
      BPRINTF_ASSERT (context.format_end);

      details::push_cstr (context, "{TestClass}");
    }
  }
}
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

  TestClass   testClass;

  char const * ptr = "Again";

  bprintf (
      "Hello: %2+10% %0% 0x%1:X% %3-20% %5% %4% %6+20:g% %7% %8%\n"
    , std::move (else_)
    , static_cast<std::int16_t> (0xCAFE)
    , "Yo yo"
    , std::string ("There")
    , something
    , ptr
    , 3.14
    , testClass
    );

#ifdef NDEBUG
  auto measure = [] (char const * name, auto && v)
    {
      auto result = time_it (std::forward<decltype(v)> (v));
      auto ms     = std::get<1> (result);
      bprintf ("%0-20%: %1%\n", name, ms);
    };

  bprintf ("Performance run\n");

  measure ("sstream"  , test_sstream);
  measure ("sprintf"  , test_sprintf);
  measure ("bsprintf" , test_bsprintf);
#endif

  return 0;
}

