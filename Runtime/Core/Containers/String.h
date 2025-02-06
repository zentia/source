#pragma once

#include "Runtime/Core/Containers/StringStorageDefault.h"
#include "Runtime/Core/Containers/StringOperations.h"

namespace core
{
    template<typename TChar>
    class basic_string_ref;

    template<typename TChar, typename TStorage = StringStorageDefault<TChar> >
    class basic_string : private TStorage, private basic_string_operations<typename TStorage::value_type>
    {
    public:
        typedef basic_string_operations<TChar> operations_type;
        typedef typename operations_type::value_type value_type;
        typedef typename operations_type::size_type size_type;
        typedef typename operations_type::difference_type difference_type;
        typedef typename operations_type::pointer pointer;
        typedef typename operations_type::const_pointer const_pointer;
        typedef typename operations_type::reference reference;
        typedef typename operations_type::const_reference const_reference;

        size_type length() const
        {
            return TStorage::size();
        }

        bool empty() const
        {
            return length() == 0;
        }
    };

    typedef basic_string<char> string;
    typedef basic_string<wchar_t> wstring;
}