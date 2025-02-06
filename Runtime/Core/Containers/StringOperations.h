#pragma once

namespace core
{
    template<typename TChar>
    class basic_string_operations
    {
    public:
        typedef TChar value_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
    };
}