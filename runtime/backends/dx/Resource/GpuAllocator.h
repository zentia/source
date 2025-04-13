#pragma once
#include <DXRuntime/Device.h>
namespace D3D12MA {
class Allocator;
class Pool;
}// namespace D3D12MA
namespace lc::dx {
class GpuAllocator : public vstd::IOperatorNewBase {
    D3D12MA::Allocator *allocator = nullptr;
    D3D12MA::Pool *sparse_buffer_pool = nullptr;
    D3D12MA::Pool *sparse_image_pool = nullptr;
    luisa::compute::Profiler *profiler;
public:
    uint64 AllocateBufferHeap(
        Device *device,
        vstd::string_view name,
        uint64_t targetSizeInBytes,
        D3D12_HEAP_TYPE heapType,
        ID3D12Heap **heap, uint64_t *offset,
        D3D12_HEAP_FLAGS extra_flags = D3D12_HEAP_FLAG_NONE,
        bool is_sparse = false);
    uint64 AllocateTextureHeap(
        Device *device,
        vstd::string_view name,
        size_t sizeBytes,
        ID3D12Heap **heap, uint64_t *offset,
        bool isRenderTexture,
        D3D12_HEAP_FLAGS extra_flags = D3D12_HEAP_FLAG_NONE,
        bool is_sparse = false);
    void Release(uint64 handle);
    GpuAllocator(
        Device *device, luisa::compute::Profiler *profiler,
        uint64_t preferred_block_size,
        uint64_t sparse_buffer_block_size,
        uint64_t sparse_image_block_size);
    ~GpuAllocator();
};
}// namespace lc::dx
