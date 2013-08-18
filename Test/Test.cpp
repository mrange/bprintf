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
#include <chrono>
#include <string>
// ----------------------------------------------------------------------------
#include "../bprintf/bprintf.hpp"
// ----------------------------------------------------------------------------
#define TEST_PRELUDE()                  test_prelude(__FILE__, __LINE__, __FUNCTION__)
#define TEST_ASSERT(expected, found)    test_assert(__FILE__, __LINE__, expected, #expected, found, #found, (expected == found))
// ----------------------------------------------------------------------------
namespace
{
    std::size_t             errors          = 0;

    void test_prelude (
            char const *    file
        ,   int             line_no
        ,   char const *    test
        )
    {
        printf (
                "%s(%d): RUNNING: %s\r\n"
            ,   file
            ,   line_no
            ,   test
            );
    }

    bool test_assert (
            char const *    file
        ,   int             line_no
        ,   bool            expected
        ,   char const *    expected_name
        ,   bool            found
        ,   char const *    found_name
        ,   bool            result
        )
    {
        if (!result)
        {
            ++errors;
            printf (
                    "%s(%d): ERROR_EXPECTED: %s(%s), FOUND: %s(%s)\r\n"
                ,   file
                ,   line_no
                ,   expected    ? "true" : "false"
                ,   expected_name
                ,   found       ? "true" : "false"
                ,   found_name
                );
        }

        return result;
    }

    bool test_assert (
            char const *    file
        ,   int             line_no
        ,   std::string     expected
        ,   char const *    expected_name
        ,   std::string     found
        ,   char const *    found_name
        ,   bool            result
        )
    {
        if (!result)
        {
            ++errors;
            printf (
                    "%s(%d): ERROR_EXPECTED: %s(%s), FOUND: %s(%s)\r\n"
                ,   file
                ,   line_no
                ,   expected.c_str ()
                ,   expected_name
                ,   found.c_str ()
                ,   found_name
                );
        }

        return result;
    }

    bool test_assert (
            char const *    file
        ,   int             line_no
        ,   int             expected
        ,   char const *    expected_name
        ,   int             found
        ,   char const *    found_name
        ,   bool            result
        )
    {
        if (!result)
        {
            ++errors;
            printf (
                    "%s(%d): ERROR_EXPECTED: %d(%s), FOUND: %d(%s)\r\n"
                ,   file
                ,   line_no
                ,   expected
                ,   expected_name
                ,   found
                ,   found_name
                );
        }

        return result;
    }

    bool test_assert (
            char const *    file
        ,   int             line_no
        ,   std::size_t     expected
        ,   char const *    expected_name
        ,   std::size_t     found
        ,   char const *    found_name
        ,   bool            result
        )
    {
        if (!result)
        {
            ++errors;
            printf (
                    "%s(%d): ERROR_EXPECTED: %d(%s), FOUND: %d(%s)\r\n"
                ,   file
                ,   line_no
                ,   expected
                ,   expected_name
                ,   found
                ,   found_name
                );
        }

        return result;
    }

    template<typename TPredicate>
    long long execute_testruns (
            std::size_t test_runs
        ,   TPredicate predicate
        )
    {
        auto then = std::chrono::high_resolution_clock::now ();

        for (auto test_run = 0U; test_run < test_runs; ++test_run)
        {
            predicate ();
        }

        auto now = std::chrono::high_resolution_clock::now ();

        auto diff = now - then;

        auto diff_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count ();

        return diff_in_ms;
    }

    void test_performance ()
    {
        TEST_PRELUDE ();

        auto formatter  = bprintf::formatter<> ();

        int         const test_repeat   = 10000   ;
        int         const test_size     = 100     ;

        auto first_name = "John";
        std::string last_name ("Doe");

        char ebuffer [1024];

        auto expected = execute_testruns (
                test_repeat
            ,   [&] ()
                {
                    for (auto iter = 0; iter < test_size; ++iter)
                    {
                        sprintf(ebuffer, "Hi there %s %s!\r\n%s you are %f years old\r\n", first_name, last_name.c_str (), first_name, 34.5);
                    }
                }
            );

        auto rbuffer = formatter.get_buffer ();

        char const  format_begin [] = "Hi there {0} {1}!\r\n{0} you are {2:000} years old\r\n";
        char const* format_end      = format_begin + sizeof (format_begin) / sizeof (format_begin [0]);


        auto result = execute_testruns (
                test_repeat
            ,   [&] ()
                {
                    for (auto iter = 0; iter < test_size; ++iter)
                    {
                        rbuffer.clear ();
                        formatter.format_buffer (format_begin, format_end, rbuffer, first_name, last_name, 34.5);
                    }
                }                 
            );

        auto ratio_limit    = 3.0; 
        auto ratio          = ((double)expected)/result;
        TEST_ASSERT (true, (ratio > 1/ratio_limit && ratio < ratio_limit));
        printf (
                "Performance numbers for bprintf, expected:%d, result:%d, ratio_limit:%f, ratio:%f\r\n"
            ,   (int)expected
            ,   (int)result
            ,   ratio_limit
            ,   ratio
            );
    }

    bool run_all_tests (bool run_perfomance_tests)
    {
        // -------------------------------------------------------------------------
        if (run_perfomance_tests)
        {
            test_performance ();
        }
        // -------------------------------------------------------------------------
        if (errors == 0)
        {
            printf ("PASS\r\n");
        }
        else
        {
            printf ("FAIL\r\n");
        }
        // -------------------------------------------------------------------------
        return errors == 0;
        // -------------------------------------------------------------------------
    }

}
// ----------------------------------------------------------------------------
int main()
{
    auto formatter  = bprintf::formatter<> ();

    auto string = formatter.format_string ("Hi there {0} {1}!\r\n{0} you are {2:000} years old\r\n", "John", std::string ("Doe"), 34.5);

    printf("%s", string.c_str ());

    auto r = run_all_tests (
#if _DEBUG
            false
#else
            true
#endif
        );

    return r ? 0 : 101;
}
// ----------------------------------------------------------------------------
