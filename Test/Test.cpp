// ----------------------------------------------------------------------------------------------
// Copyright (c) Mårten Rånge.
// ----------------------------------------------------------------------------------------------
// This source code is subject to terms and conditions of the Microsoft Public License. A 
// copy of the license can be found in the License.html file at the root of this distribution. 
// If you cannot locate the  Microsoft Public License, please send an email to 
// dlr@microsoft.com. By using this source code in any fashion, you are agreeing to be bound 
//  by the terms of the Microsoft Public License.
// ----------------------------------------------------------------------------------------------
// You must not remove this notice, or any other, from this software.
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------
#include "stdafx.h"
// ----------------------------------------------------------------------------
#include <string>
// ----------------------------------------------------------------------------
#include "../bprintf/bprintf.hpp"
// ----------------------------------------------------------------------------
int main()
{
    auto formatter  = bprintf::formatter<> ();

    auto string = formatter.format_string ("Hi there {0} {1}!\r\n{0} you are {2:000} years old\r\n", "John", std::string ("Doe"), 34);

    printf ("%s", string.c_str ());


    return 0;
}
// ----------------------------------------------------------------------------
