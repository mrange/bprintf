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

#include "formatters.hpp"

namespace better_printf
{
  void formatter<int>::format (
      formatter_context & context
    , int                 value
    )
  {
    BPRINTF_ASSERT (context.format_begin);
    BPRINTF_ASSERT (context.format_end);

    constexpr auto buffer_size = 12U;

    auto & chars = context.chars;

    chars.reserve (buffer_size + chars.size ());

    // TODO: Don't use _itoa_s

    char_type buffer[buffer_size];
    _itoa_s (value, buffer, 10);

    details::push_cstr (context, buffer);
  }

  void formatter<double>::format (
      formatter_context & context
    , double              value
    )
  {
    BPRINTF_ASSERT (context.format_begin);
    BPRINTF_ASSERT (context.format_end);

    constexpr auto buffer_size = 32U;

    auto & chars = context.chars;

    chars.reserve (buffer_size + chars.size ());

    // TODO: Don't use sprintf_s

    char_type buffer[buffer_size];
    auto sz = sprintf_s (buffer, "%g", value);

    details::push_buffer (context, buffer, sz);
  }

  void formatter<std::string>::format (
      formatter_context & context
    , std::string const & value
    )
  {
    BPRINTF_ASSERT (context.format_begin);
    BPRINTF_ASSERT (context.format_end);

    auto & chars = context.chars;

    chars.reserve (value.size () + chars.size ());

    for (auto ch : value)
    {
      chars.push_back (ch);
    }
  }
}
