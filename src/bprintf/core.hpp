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

#pragma once

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

  constexpr std::size_t const initial_buffer  = 1024                    ;

  constexpr char_type const   null_char       = '\0'                    ;

  constexpr char_type const   zero_char       = '0'                     ;
  constexpr char_type const   nine_char       = '9'                     ;

  constexpr char_type const   format_prelude  = '%'                     ;
  constexpr char_type const   format_epilogue = '%'                     ;

  struct formatter_context
  {
    formatter_context (
        chars_type &      chars
      , char_type const * format
      ) noexcept;

    formatter_context (formatter_context const &)             = delete;
    formatter_context (formatter_context &&)                  = delete;

    formatter_context & operator= (formatter_context const &) = delete;
    formatter_context & operator= (formatter_context &&)      = delete;

    chars_type &        chars         ;

    std::size_t         index         ;
    bool                right_align   ;
    std::size_t         width         ;
    char_type           fill          ;

    char_type const *   current       ;
    char_type const *   format_begin  ;
    char_type const *   format_end    ;
  };

  namespace details
  {
    inline void push_buffer (
        formatter_context const & context
      , char_type const * buffer
      , std::size_t       size
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
      , char_type const * cstr
      )
    {
      BPRINTF_ASSERT (cstr);

      auto size = strlen (cstr);

      push_buffer (context, cstr, size);
    }

    void format (
        formatter_context & context
      );

    template<typename THead, typename ...TTail>
    void format (
        formatter_context &    context
      , THead const &          head
      , TTail const &       ...tail
      )
    {
      if (context.index != 0)
      {
        --context.index;
        format (context, tail...);
      }
      else
      {
        using value_t = std::decay_t<THead const &>;
        formatter<value_t>::format (context, head);
      }
    }

    bool scan (formatter_context & context);

    chars_type & get_thread_local_chars ();

    void write_to_cout (chars_type const & chars);
  }

  template<typename T>
  struct formatter;

  template<>
  struct formatter<char_type const *>
  {
    static void format (
        formatter_context const & context
      , char_type const *         value
      );
  };

  template<typename ...TArgs>
  void bsprintf (
      chars_type &      chars
    , char_type const * format
    , TArgs &&          ...args
    )
  {
    formatter_context context (chars, format);

    while (details::scan (context))
    {
      details::format (context, args...);
    }
  }

  template<typename ...TArgs>
  void bprintf (
      char_type const *   format
    , TArgs &&            ...args
    )
  {
    auto && chars = details::get_thread_local_chars ();

    bsprintf (chars, format, std::forward<TArgs> (args)...);

    details::write_to_cout (chars);
  }
}
