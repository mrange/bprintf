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

#ifndef BPRINTF_BPRINTF__HPP
#define BPRINTF_BPRINTF__HPP

#include "core.hpp"
#include "formatters.hpp"

namespace better_printf
{
  namespace details
  {
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
        formatters::format (context, head);
      }
    }
  }

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

#endif // BPRINTF_BPRINTF__HPP
