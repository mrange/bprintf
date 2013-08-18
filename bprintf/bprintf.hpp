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
#pragma once
// ----------------------------------------------------------------------------
#include <cassert>
#include <exception>
#include <string>
#include <vector>
// ----------------------------------------------------------------------------
#define BPRINTF__INLINE inline
// ----------------------------------------------------------------------------
namespace bprintf
{
    // -------------------------------------------------------------------------

    enum format_result_state
    {
        frs__success                    ,
        frs__unknown                    ,
        frs__invalid_format             ,
        frs__invalid_reference          ,
        frs__invalid_reference_format   ,
        frs__reference_out_of_bounds    ,
        frs__formatter_failed           ,
    };
    // -------------------------------------------------------------------------

    typedef std::size_t size_type;

    // -------------------------------------------------------------------------

    struct base_exception : std::exception
    {
        virtual const char* what ()  const throw ()
        {
            return "base_exception";
        }
    };

    struct programming_error_exception : base_exception
    {
        virtual const char* what ()  const throw ()
        {
            return "programming_error_exception";
        }
    };

    struct format_exception : base_exception
    {
        format_result_state result  ;

        explicit format_exception (format_result_state result)
            :   result (result)
        {

        }

        virtual const char* what ()  const throw ()
        {
            return "format_exception";
        }
    };

    // -------------------------------------------------------------------------

    namespace detail
    {
        struct char_adaptor
        {
            typedef     char                char_type   ;
            typedef     char const *        input_type  ;
            typedef     std::vector<char>   buffer_type ;
            typedef     std::string         string_type ;

            BPRINTF__INLINE static void reserve_buffer (
                    buffer_type &   buffer
                ,   size_type       size
                )
            {
                buffer.reserve (size + buffer.size ());
            }

            BPRINTF__INLINE static void append_to_buffer (
                    buffer_type &   buffer
                ,   input_type      begin
                ,   input_type      end
                )
            {
                assert (begin);
                assert (end);
                buffer.insert (buffer.end (), begin, end);
            }

            BPRINTF__INLINE static string_type to_string (
                    buffer_type &   buffer
                )
            {
                return string_type (buffer.begin (), buffer.end ());
            }

            BPRINTF__INLINE static format_result_state format (
                    buffer_type &   buffer
                ,   input_type      format_begin
                ,   input_type      format_end
                ,   input_type      value
                )
            {
                assert (format_begin);
                assert (format_end);
                assert (value);

                while (*value)
                {
                    buffer.push_back (*value);
                    ++value;
                }

                return frs__success;
            }

            BPRINTF__INLINE static format_result_state format (
                    buffer_type &       buffer
                ,   input_type          format_begin
                ,   input_type          format_end
                ,   string_type const & value
                )
            {
                assert (format_begin);
                assert (format_end);

                if (value.empty ())
                {
                    return frs__success;
                }

                auto begin  = &value.front ();
                auto end    = begin + value.size ();

                append_to_buffer (buffer, begin, end);

                return frs__success;
            }

            static void format_impl (
                    buffer_type &       buffer
                ,   long long           value
                )
            {
                assert (value >= 0);
                if (value > 0)
                {
                    auto d = value / 10;
                    auto r = static_cast<char> (value % 10);
                    format_impl (buffer, d);
                    buffer.push_back (r + '0');
                }
            }

            BPRINTF__INLINE static format_result_state format (
                    buffer_type &       buffer
                ,   input_type          format_begin
                ,   input_type          format_end
                ,   long long           value
                )
            {
                assert (format_begin);
                assert (format_end);

                if (value < 0)
                {
                    buffer.push_back ('-');
                    format_impl (buffer, -value);
                }
                else
                {
                    format_impl (buffer, value);
                }


                return frs__success;
            }

        };
    }

    template<typename TAdaptor = detail::char_adaptor>
    struct formatter 
    {
        typedef             TAdaptor                        adaptor_type    ;
        typedef typename    adaptor_type::char_type         char_type       ;
        typedef typename    adaptor_type::input_type        input_type      ;
        typedef typename    adaptor_type::buffer_type       buffer_type     ;
        typedef typename    adaptor_type::string_type       string_type     ;

        struct format_result
        {
            format_result_state state   ;
            input_type          begin   ;
            input_type          end     ;
            input_type          last    ;

            BPRINTF__INLINE static format_result create (
                    format_result_state     state
                ,   input_type              begin
                ,   input_type              end  
                ,   input_type              last
                )
            {
                format_result result    ;
                result.state    = state ;
                result.begin    = begin ;
                result.end      = end   ;
                result.last     = last  ;

                return result;
            }
        };

        BPRINTF__INLINE buffer_type get_buffer()
        {
            return buffer_type ();
        }

        template<typename ...TArgs>
        BPRINTF__INLINE string_type format_string (string_type const & format, TArgs&&... args)
        {
            if (format.empty ())
            {
                return string_type ();
            }

            auto begin  = &format.front ();
            auto end    = begin + format.size ();

            buffer_type buffer;
            auto result = format_buffer (
                    begin
                ,   end
                ,   buffer
                ,   args...
                );

            if (result.state != frs__success)
            {
                throw format_exception (result.state);
            }

            return adaptor_type::to_string (buffer);
        }

        template<typename ...TArgs>
        BPRINTF__INLINE format_result format_buffer (input_type format_begin, input_type format_end, buffer_type & buffer, TArgs&&... args)
        {
            return format_impl (
                    format_begin
                ,   format_end
                ,   buffer
                ,   args...
                );
        }

    private:
        enum parser_state
        {
            ps__string                      ,
            ps__parameter_reference_token   ,
            ps__parameter_reference         ,
            ps__parameter_format            ,
        };

        static format_result_state apply_formatter (
                buffer_type &   //buffer
            ,   size_type       //reference
            ,   input_type      //format_begin
            ,   input_type      //format_end
            )
        {
            return frs__reference_out_of_bounds; 
        }

        template<typename TValue, typename ...TArgs>
        static format_result_state apply_formatter (
                buffer_type &   buffer
            ,   size_type       reference
            ,   input_type      format_begin
            ,   input_type      format_end
            ,   TValue &&       value
            ,   TArgs &&...     args
            )
        {
            if (reference > 0)
            {
                return apply_formatter (
                        buffer
                    ,   reference - 1
                    ,   format_begin
                    ,   format_end
                    ,   std::forward<TArgs> (args)...         // TODO: std::forward<> ??
                    );
            }
            else
            {
                return adaptor_type::format (buffer, format_begin, format_end, std::forward<TValue> (value));
            }
        }

        // TODO: Shouldn't be generic as this complex function would then be duplicated which might cause code-bloat
        template<typename ...TArgs>
        static format_result format_impl (
                input_type      begin
            ,   input_type      end
            ,   buffer_type &   buffer
            ,   TArgs&&...      args
            )
        {
            if (!begin)
            {
                return format_result::create (frs__invalid_format, begin, end, begin);
            }

            if (!end)
            {
                return format_result::create (frs__invalid_format, begin, end, begin);
            }

            if (end < begin)
            {
                return format_result::create (frs__invalid_format, begin, end, begin);
            }

            auto sz         = static_cast<size_type> (end - begin);
            auto state      = ps__string;
            auto iter       = begin     ;
            auto string     = begin     ;
            auto format     = begin     ;
            auto reference  = 0U        ;

            adaptor_type::reserve_buffer (buffer, sz);

            for (; iter < end && *iter != 0; ++iter)
            {
                auto ch = *iter;
                switch (state)
                {
                case ps__string                      :
                    switch (ch)
                    {
                    case '{':
                        adaptor_type::append_to_buffer (buffer, string, iter);
                        state = ps__parameter_reference_token;
                        break;
                    default:
                        break;
                    }
                    break;
                case ps__parameter_reference_token   :
                    switch (ch)
                    {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        reference = ch - '0';
                        state = ps__parameter_reference;
                        break;
                    case '{':
                        string = iter;
                        state = ps__string;
                        break;
                    default:
                        return format_result::create (frs__invalid_reference, begin, end, iter);
                    }
                    break;
                case ps__parameter_reference         :
                    switch (ch)
                    {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        reference += 10 * reference + (ch - '0');
                        break;
                    case '}':
                        {
                            auto result = apply_formatter (buffer, reference, iter, iter, args...);
                            if (result != frs__success)
                            {
                                return format_result::create (result, begin, end, iter);
                            }
                            string  = iter + 1;
                            state   = ps__string;
                        }
                        break;
                    case ':':
                        format = iter + 1;
                        state = ps__parameter_format;
                        break;
                    default:
                        return format_result::create (frs__invalid_reference, begin, end, iter);
                    }
                    break;
                case ps__parameter_format      :
                    switch (ch)
                    {
                    case '}':
                        {
                            auto result = apply_formatter (buffer, reference, format, iter, args...);
                            if (result != frs__success)
                            {
                                return format_result::create (result, begin, end, iter);
                            }
                            string  = iter + 1;
                            state   = ps__string;
                        }
                        break;
                    default:
                        break;
                    }
                    break;
                default:
                    return format_result::create (frs__unknown, begin, end, iter);
                }
            }

            switch (state)
            {
            case ps__string                     :
                adaptor_type::append_to_buffer (buffer, string, iter);
                return format_result::create (frs__success                  , begin, end, iter);
            case ps__parameter_reference_token  :
                return format_result::create (frs__invalid_reference        , begin, end, iter);
            case ps__parameter_reference        :
                return format_result::create (frs__invalid_reference        , begin, end, iter);
            case ps__parameter_format           :
                return format_result::create (frs__invalid_reference_format , begin, end, iter);
            default:
                return format_result::create (frs__unknown                  , begin, end, iter);
            }
        }

    };
}
// ----------------------------------------------------------------------------
