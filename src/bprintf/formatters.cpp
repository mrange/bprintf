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

#include <algorithm>

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

      auto token = peek_token (context.format_begin, context.format_end);

      auto formatter = [=] (char_type token)
      {
        switch (token)
        {
        case 'x':
          return "%llx";
        case 'X':
          return "%llX";
        case 'o':
          return "%llo";
        case 'd':
        default:
          return "%lld";
        }
      } (token);

      // TODO: Don't use sprintf_s
      constexpr auto buffer_size = 24U;
      char_type buffer[buffer_size];

      auto sz = sprintf_s (buffer, formatter, value);
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

      auto token = peek_token (context.format_begin, context.format_end);

      auto formatter = [=] (char_type token)
      {
        switch (token)
        {
        case 'a':
        case 'A':
          return "%a";
        case 'e':
        case 'E':
          return "%e";
        case 'f':
        case 'F':
          return "%f";
        case 'g':
          return "%g";
        case 'G':
          return "%G";
        default:
          return "%f";
        }
      } (token);

      // TODO: Don't use sprintf_s
      constexpr auto buffer_size = 64U;
      char_type buffer[buffer_size];

      auto sz = sprintf_s (buffer, formatter, value);
      details::push_buffer (context, buffer, sz);
    }
  }

  namespace formatters
  {
    void format (
        details::formatter_context const &  context
      , cstr_type                           value
      )
    {
      BPRINTF_ASSERT (context.format_begin);
      BPRINTF_ASSERT (context.format_end);

      details::push_cstr (context, value ? value : "");
    }

    void format (
        details::formatter_context const & context
      , std::string const &                 value
      )
    {
      BPRINTF_ASSERT (context.format_begin);
      BPRINTF_ASSERT (context.format_end);

      details::push_buffer (context, value.data (), value.size ());
    }
  }
}
