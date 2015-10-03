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
  namespace details
  {
    void format__long_long (
        formatter_context const & context
      , long long                 value
      )
    {
      BPRINTF_ASSERT (context.format_begin);
      BPRINTF_ASSERT (context.format_end);

      constexpr auto buffer_size = 24U;

      auto & chars = context.chars;

      chars.reserve (buffer_size + chars.size ());

      // TODO: Don't use sprintf_s

      char_type buffer[buffer_size];
      auto sz = sprintf_s (buffer, "%lld", value);
      details::push_buffer (context, buffer, sz);

      //_itoa_s (value, buffer, 10);
      //details::push_cstr (context, buffer);
    }

    void format__double (
        formatter_context const & context
      , double                    value
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
  }

  namespace formatters
  {
    void format (
        formatter_context const & context
      , char_type const *         value
      )
    {
      BPRINTF_ASSERT (context.format_begin);
      BPRINTF_ASSERT (context.format_end);

      details::push_cstr (context, value ? value : "");
    }

    void format (
        formatter_context const & context
      , std::string const &       value
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
}
