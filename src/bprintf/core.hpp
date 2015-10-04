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

#ifndef BPRINTF_CORE__HPP
#define BPRINTF_CORE__HPP

#include <cassert>
#include <cstdint>

#include <vector>
#include <type_traits>

#if NDEBUG
# define BPRINTF_ASSERT(expr) (void) (expr)
#else
# define BPRINTF_ASSERT assert
#endif

namespace better_printf
{
  using char_type                             = char                    ;
  using chars_type                            = std::vector<char_type>  ;
  using cstr_type                             = char const *            ;

  namespace details
  {
    constexpr std::size_t const initial_buffer  = 1024                  ;

    constexpr char_type const   null_char       = '\0'                  ;

    constexpr char_type const   zero_char       = '0'                   ;
    constexpr char_type const   nine_char       = '9'                   ;
    constexpr char_type const   hex_a_char      = 'A'                   ;

    constexpr char_type const   plus_char       = '+'                   ;
    constexpr char_type const   minus_char      = '-'                   ;
    constexpr char_type const   colon_char      = ':'                   ;
    constexpr char_type const   space_char      = ' '                   ;

    constexpr char_type const   format_prelude  = '%'                   ;
    constexpr char_type const   format_epilogue = '%'                   ;

    struct formatter_context
    {
      formatter_context (
          chars_type &  chars
        , cstr_type     format
        ) noexcept;

      formatter_context (formatter_context const &)             = delete;
      formatter_context (formatter_context &&)                  = delete;

      formatter_context & operator= (formatter_context const &) = delete;
      formatter_context & operator= (formatter_context &&)      = delete;

      chars_type &  chars         ;

      std::size_t   index         ;
      bool          right_align   ;
      std::size_t   width         ;
      char_type     fill          ;

      cstr_type     current       ;
      cstr_type     format_begin  ;
      cstr_type     format_end    ;
    };

    bool scan (formatter_context & context);

    chars_type & get_thread_local_chars ();

    void write_to_cout (chars_type const & chars);

    void apply_formatter (
        formatter_context & context
      );

    template<typename THead, typename ...TTail>
    void apply_formatter (
        formatter_context &    context
      , THead const &          head
      , TTail const &       ...tail
      )
    {
      if (context.index != 0)
      {
        --context.index;
        apply_formatter (context, tail...);
      }
      else
      {
        using value_t = std::decay_t<THead const &>;
        formatters::format (context, head);
      }
    }
  }

  template<typename T>
  struct formatter;

  template<typename ...TArgs>
  void bsprintf (
      chars_type &  chars
    , cstr_type     format
    , TArgs &&      ...args
    )
  {
    details::formatter_context context (chars, format);

    while (details::scan (context))
    {
      details::apply_formatter (context, args...);
    }
  }

  template<typename ...TArgs>
  void bprintf (
      cstr_type     format
    , TArgs &&      ...args
    )
  {
    auto & chars = details::get_thread_local_chars ();

    bsprintf (chars, format, std::forward<TArgs> (args)...);

    details::write_to_cout (chars);
  }
}

#endif // BPRINTF_CORE__HPP
