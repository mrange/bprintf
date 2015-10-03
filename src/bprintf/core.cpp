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

#include "core.hpp"
#include "formatters.hpp"

#include <cstdio>

namespace better_printf
{
  formatter_context::formatter_context (
      chars_type &      chars
    , char_type const * format
    ) noexcept
    : chars         (chars)
    , index         (0)
    , right_align   (false)
    , width         (0)
    , current       (format ? format : "")
    , format_begin  (current)
    , format_end    (current)
  {
  }

  namespace details
  {
    void format (
        formatter_context & context
      )
    {
      formatters::format (context, "BPRINTF_OUT_OF_BOUNDS");
    }

    bool scan (formatter_context & context)
    {
      auto current  = context.current;

      BPRINTF_ASSERT (current);

      auto & chars = context.chars;

      while (*current != null_char)
      {
        // Not EOS

        if (*current != format_prelude)
        {
          // Current char isn't prelude, copy it to buffer
          ++current;
          continue;
        }

        chars.insert (chars.end (), context.current, current);

        // Prelude found
        ++current;

        if (*current == format_prelude)
        {
          // Next char is prelude char
          // Double prelude char is an escape sequence, copy prelude char to buffer
          chars.push_back (format_prelude);
          continue;
        }

        auto format_begin = current;

        // Scan for epilogue char
        while (*current != null_char && *current != format_epilogue)
        {
          ++current;
        }

        if (*current == null_char)
        {
          // Found EOS, copy the incomplete format string to buffer
          chars.insert (chars.end (), format_begin, current);
          continue;
        }

        // Found epilogue char
        auto format_end = current;

        // Parse parameter index
        auto index = 0U;

        for (; format_begin < format_end && *format_begin >= zero_char && *format_begin <= nine_char; ++format_begin)
        {
          index = index * 10 + (*format_begin - zero_char);
        }

        ++current;

        context.current       = current       ;
        context.index         = index         ;
        context.format_begin  = format_begin  ;
        context.format_end    = format_end    ;

        return true;
      }

      chars.insert (chars.end (), context.current, current);

      context.current = current;

      return false;
    }

    namespace
    {
      chars_type create_chars ()
      {
        chars_type chars;

        chars.reserve (initial_buffer);

        return chars;
      }

      thread_local chars_type thread_local_chars = create_chars ();
    }

    chars_type & get_thread_local_chars ()
    {
      thread_local_chars.clear ();

      return thread_local_chars;
    }

    void write_to_cout (chars_type const & chars)
    {
      auto sz = chars.size ();
      if (sz > 0)
      {
        auto written_objects = std::fwrite (&chars.front (), sizeof (char_type), sz, stdout);
        BPRINTF_ASSERT (written_objects == sz);

        auto fflush_result = std::fflush (stdout);
        BPRINTF_ASSERT (fflush_result == 0);
      }
    }

  }
}
