#pragma once
#include <cstdint>

namespace core
{
    enum class StringRepresentation : uint8_t
    {
	    EMBEDDED,
        HEAP,
        EXTERNAL,
    };
    template<typename TChar>
    class StringStorageDefault
    {
    public:
        typedef TChar value_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef value_type* iterator;
        typedef const value_type* const_iterator;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;

        static const size_type kInternalBufferByteSize = sizeof(pointer) + (sizeof(size_type) * 3);
        static constexpr size_type kInternalBufferCapacity = (kInternalBufferByteSize / sizeof(value_type)) - 1;
        static constexpr uint8_t kRepresentationByteOffset = kInternalBufferByteSize - 1;
        static const uint8_t representation_bit_mask = 0xC0;
        static const uint8_t representation_bit_shift = 6;
        static constexpr uint8_t representation_clear_mask = (uint8_t)~representation_bit_mask;

        StringStorageDefault get_data_representation() const noexcept
        {
            const uint8_t* union_base_ptr = reinterpret_cast<const uint8_t*>(&m_embedded);
            uint8_t representation_byte = *(union_base_ptr + kRepresentationByteOffset);
            return static_cast<StringRepresentation>((representation_byte & representation_bit_mask) >> representation_bit_shift);
        }

        size_type size() const
        {
            return (get_data_representation() == StringRepresentation::EMBEDDED) ? kInternalBufferCapacity - m_embedded.free_count : m_heap.size;
        }

        struct StackAllocatedRepresentation
        {
            value_type data[kInternalBufferCapacity];
            value_type free_count;
        };

        struct HeapAllocatedRepresentation
        {
            pointer data;
            size_type capacity;
            size_type size;
        };

        union 
        {
            StackAllocatedRepresentation m_embedded;
            HeapAllocatedRepresentation m_heap;
        };
    };
}
