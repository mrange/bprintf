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

#ifndef BPRINTF_FORMATTERS__HPP
#define BPRINTF_FORMATTERS__HPP

#include "core.hpp"

#include <string>

namespace better_printf
{
  namespace details
  {
    template<typename TIntegral>
    using enable_if_signed_integral_t   = std::enable_if_t<std::is_integral<TIntegral>::value && std::is_signed<TIntegral>::value>;

    template<typename TIntegral>
    using enable_if_unsigned_integral_t = std::enable_if_t<std::is_integral<TIntegral>::value && std::is_unsigned<TIntegral>::value>;

    template<typename TFloat>
    using enable_if_floating_point_t    = std::enable_if_t<std::is_floating_point<TFloat>::value>;

    constexpr char_type test_token (char_type)
    {
      return null_char;
    }

    template<typename ...TTail>
    constexpr char_type test_token (
        char_type   test
      , char_type   head
      , TTail &&    ...tail
      ) noexcept
    {
      return test == head
        ? test
        : test_token (test, std::forward<TTail> (tail)...)
        ;
    }

    template<typename ...TTokens>
    constexpr char_type peek_token (
        cstr_type   begin
      , cstr_type   end
      , TTokens &&  ...tokens
      ) noexcept
    {
      return begin < end
        ? test_token (*begin, std::forward<TTokens> (tokens)...)
        : null_char
        ;
    }

    template<typename ...TTokens>
    constexpr char_type peek_token (
        cstr_type   begin
      , cstr_type   end
      ) noexcept
    {
      return begin < end
        ? *begin
        : null_char
        ;
    }

    inline void push_buffer (
        formatter_context const & context
      , cstr_type                 buffer
      , std::size_t               size
      )
    {
      BPRINTF_ASSERT (buffer);

      auto & chars  = context.chars ;
      auto width    = context.width ;
      auto fill     = context.fill  ;
      auto fsz      = width - size  ;

      if (width <= size)
      {
        chars.insert (chars.end (), buffer, buffer + size);
      }
      else if (context.right_align)
      {
        chars.insert (chars.end (), fsz, fill);
        chars.insert (chars.end (), buffer, buffer + size);
      }
      else
      {
        chars.insert (chars.end (), buffer, buffer + size);
        chars.insert (chars.end (), fsz, fill);
      }
    }

    inline void push_cstr (
        formatter_context const & context
      , cstr_type                 cstr
      )
    {
      BPRINTF_ASSERT (cstr);

      auto size = strlen (cstr);

      push_buffer (context, cstr, size);
    }

    void format__int64 (
        formatter_context const & context
      , std::int64_t              value
      );

    void format__uint64 (
        formatter_context const & context
      , std::uint64_t             value
      );

    void format__double (
        formatter_context const & context
      , double                    value
      );
  }

  namespace formatters
  {
    template<typename TIntegral>
    constexpr details::enable_if_signed_integral_t<TIntegral> format (
        details::formatter_context const &  context
      , TIntegral                           value
      )
    {
      details::format__int64 (context, value);
    }

    template<typename TIntegral>
    constexpr details::enable_if_unsigned_integral_t<TIntegral> format (
        details::formatter_context const &  context
      , TIntegral                           value
      )
    {
      details::format__uint64 (context, value);
    }

    template<typename TFloat>
    constexpr details::enable_if_floating_point_t<TFloat> format (
        details::formatter_context const &  context
      , TFloat                              value
      )
    {
      details::format__double (context, value);
    }

    void format (
        details::formatter_context const &  context
      , cstr_type                           value
      );

    void format (
        details::formatter_context const &  context
      , std::string const &                 value
      );
  }
}

#endif // BPRINTF_FORMATTERS__HPP
