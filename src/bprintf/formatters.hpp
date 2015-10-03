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

#include "core.hpp"

#include <string>

namespace better_printf
{
  namespace details
  {
    void format__long_long (
        formatter_context const & context
      , long long                 value
      );

    void format__double (
        formatter_context const & context
      , double                    value
      );
  }

  namespace formatters
  {

    template<typename TIntegral>
    constexpr std::enable_if_t<std::is_integral<TIntegral>::value> format (
        formatter_context const & context
      , TIntegral                 value
      )
    {
      details::format__long_long (context, value);
    }

    template<typename TFloat>
    constexpr std::enable_if_t<std::is_floating_point<TFloat>::value> format (
        formatter_context const & context
      , TFloat                    value
      )
    {
      details::format__double (context, value);
    }

    void format (
        formatter_context const & context
      , char_type const *         value
      );

    void format (
        formatter_context const & context
      , std::string const &       value
      );
  }
}
