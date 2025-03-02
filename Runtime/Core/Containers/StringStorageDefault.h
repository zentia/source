#pragma once
#include <assert.h>
#include <cstdint>

#include "Runtime/Misc/AllocatorLabels.h"
#include "Runtime/Utitlities/Annotations.h"

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

        void set_data_representation(StringRepresentation representation) noexcept
        {
            uint8_t* union_base_str = reinterpret_cast<uint8_t*>(&m_embedded);
            uint8_t* representation_byte = (union_base_str + kRepresentationByteOffset);

            *representation_byte &= representation_clear_mask;

            *representation_byte |= (static_cast<uint8_t>(representation) << representation_bit_shift);
        }

        StringStorageDefault get_data_representation() const noexcept
        {
            const uint8_t* union_base_ptr = reinterpret_cast<const uint8_t*>(&m_embedded);
            uint8_t representation_byte = *(union_base_ptr + kRepresentationByteOffset);
            return static_cast<StringRepresentation>((representation_byte & representation_bit_mask) >> representation_bit_shift);
        }

        void assign_external(pointer str, size_type strLen)
        {
	        if (strLen == 0)
	        {
                clear();
	        }
            else
            {
                assert(str != nullptr);
                
            }
        }

        pointer get_data()
        {
            return (get_data_representation() == StringRepresentation::EMBEDDED) ? m_embedded.data : m_heap.data;
        }

        const_pointer get_data()
        {
            return (get_data_representation() == StringRepresentation::EMBEDDED) ? m_embedded.data : m_heap.data;
        }

        size_type size() const
        {
            return (get_data_representation() == StringRepresentation::EMBEDDED) ? kInternalBufferCapacity - m_embedded.free_count : m_heap.size;
        }

        void clear()
        {
	        switch (get_data_representation())
	        {
	        case StringRepresentation::EXTERNAL:
	        case StringRepresentation::EMBEDDED:
                init();
                break;
	        case StringRepresentation::HEAP:
                m_heap.data[0] = static_cast<value_type>(0);
                m_heap.size = static_cast<size_type>(0);
                break;
	        }
        }

        void deallocate()
        {
	        if (get_data_representation() == StringRepresentation::HEAP)
	        {
		        
	        }
        }
    private:
        size_type capacity_internal() const
        {
            return (get_data_representation() == StringRepresentation::EMBEDDED) ? kInternalBufferCapacity : m_heap.capacity;
        }

        void init() noexcept
        {
            set_data_representation(StringRepresentation::EMBEDDED);
            m_embedded.data[0] = static_cast<value_type>(0);
            m_embedded.free_count = kInternalBufferCapacity;
        }

        SOURCE_NOINLINE pointer grow(size_type newCapacity)
        {
            if (newCapacity <= capacity_internal())
                return get_data();

            StringRepresentation repr = get_data_representation();
            if (OPTIMIZER_UNLIKELY(repr == StringRepresentation::HEAP))
            {
	            m_heap.data = static_cast<value_type*>()
            }
        }
    public:
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

        bool is_ptr_within_range(const value_type* replacementStr) const
        {
            const value_type* data_ptr = get_data();
            return (data_ptr <= replacementStr) && (replacementStr < data_ptr + size());
        }

        union 
        {
            StackAllocatedRepresentation m_embedded;
            HeapAllocatedRepresentation m_heap;
        };

        MemLabelId m_label;
    };
}
