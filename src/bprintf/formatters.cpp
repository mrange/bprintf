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
    namespace
    {
      char_type table__i_to_char []
      {
        '0' ,
        '1' ,
        '2' ,
        '3' ,
        '4' ,
        '5' ,
        '6' ,
        '7' ,
        '8' ,
        '9' ,
        'A' ,
        'B' ,
        'C' ,
        'D' ,
        'E' ,
        'F' ,
      };

      template<std::uint64_t divisor>
      inline void format__integral_impl (
          formatter_context const & context
        , char_type                 prefix
        , std::uint64_t             value
        )
      {
        static_assert (divisor >= 8U  , "divisor must be greater or equalt to 8");
        static_assert (divisor <= 16U , "divisor must be lesser or equal to 16");

        BPRINTF_ASSERT (context.format_begin);
        BPRINTF_ASSERT (context.format_end);

        // 23 is enough for octal + prefix
        //  Rationale: 64 bits / 3 bits/char = 21 + 1/3 chars < 22 chars, +1 for prefix => 23 chars
        constexpr auto buffer_size = 23U;
        char_type buffer[buffer_size];

        auto begin = buffer_size;

        do
        {
          buffer[--begin] = table__i_to_char[value % divisor];
          value           = value / divisor;
        } while (value != 0);

        if (prefix != null_char)
        {
          buffer[--begin] = prefix;
        }

        details::push_buffer (context, buffer + begin, buffer_size - begin);
      }

      inline void format__integral (
          formatter_context const & context
        , char_type                 prefix
        , std::uint64_t             value
        )
      {
        BPRINTF_ASSERT (context.format_begin);
        BPRINTF_ASSERT (context.format_end);

        auto token  = peek_token (context.format_begin, context.format_end);

        switch (token)
        {
        case 'x':
        case 'X':
          format__integral_impl<16U>  (context, prefix, value);
          break;
        case 'o':
          format__integral_impl<8U>   (context, prefix, value);
          break;
        case 'd':
        default:
          format__integral_impl<10U>  (context, prefix, value);
          break;
        }
      }

    }

    void format__uint64 (
        formatter_context const & context
      , std::uint64_t             value
      )
    {
      BPRINTF_ASSERT (context.format_begin);
      BPRINTF_ASSERT (context.format_end);

      details::format__integral (context, null_char, value);
    }

    void format__int64 (
        formatter_context const & context
      , std::int64_t              value
      )
    {
      BPRINTF_ASSERT (context.format_begin);
      BPRINTF_ASSERT (context.format_end);

      details::format__integral (
          context
        , value < 0 ? minus_char : null_char
        , value < 0 ? -value     : value
        );
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

      // TODO: Don't use sprintf
      constexpr auto buffer_size = 64U;
      char_type buffer[buffer_size];

      auto sz = sprintf (buffer, formatter, value);
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
