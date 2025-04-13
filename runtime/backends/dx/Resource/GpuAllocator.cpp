#include <Resource/D3D12MemoryAllocator/D3D12MemAlloc.h>
#include <Resource/GpuAllocator.h>
#include <Resource/Resource.h>
#include <luisa/core/logging.h>
#include <luisa/core/platform.h>

namespace lc::dx {
namespace ma_detail {
class AllocateCallback {
public:
    D3D12MA::ALLOCATION_CALLBACKS callbacks;
    AllocateCallback() {
        callbacks.pAllocate = [](size_t Size, size_t Alignment,
                                 void *pPrivateData) -> void * {
            return luisa::detail::allocator_allocate(Size, Alignment);
        };
        callbacks.pFree = [](void *pMemory, void *) -> void {
            luisa::detail::allocator_deallocate(pMemory, 0);
        };
    }
};
static AllocateCallback gAllocateCallback;
}// namespace ma_detail
GpuAllocator::~GpuAllocator() {
    if (sparse_buffer_pool) {
        sparse_buffer_pool->Release();
    }
    if (sparse_image_pool) {
        sparse_image_pool->Release();
    }
    if (allocator)
        allocator->Release();
}
uint64 GpuAllocator::AllocateTextureHeap(
    Device *device,
    vstd::string_view name,
    size_t sizeBytes,
    ID3D12Heap **heap, uint64_t *offset,
    bool isRenderTexture,
    D3D12_HEAP_FLAGS extra_flags,
    bool is_sparse) {
    using namespace D3D12MA;
    D3D12_HEAP_FLAGS heapFlag =
        isRenderTexture ? D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES : D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
    assert(!(is_sparse && isRenderTexture));// sparse can not be render texture
    ALLOCATION_DESC desc;
    desc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
    desc.Flags = ALLOCATION_FLAGS::ALLOCATION_FLAG_STRATEGY_BEST_FIT;
    desc.ExtraHeapFlags = heapFlag | extra_flags;
    desc.CustomPool = is_sparse ? sparse_image_pool : nullptr;
    D3D12_RESOURCE_ALLOCATION_INFO info;
    info.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    info.SizeInBytes = sizeBytes;
    Allocation *alloc;
    allocator->AllocateMemory(&desc, &info, &alloc);
    *heap = alloc->GetHeap();
    *offset = alloc->GetOffset();
    if (profiler) [[unlikely]] {
        auto desc = luisa::format("Texture name: \"{}\", extra heap-flags: {}", name, (uint)extra_flags);
        auto stacktrace = luisa::backtrace();
        profiler->allocate(reinterpret_cast<uint64_t>(alloc), info.Alignment, info.SizeInBytes, name, std::move(stacktrace));
    }
    return reinterpret_cast<uint64>(alloc);
}
uint64 GpuAllocator::AllocateBufferHeap(
    Device *device,
    vstd::string_view name,
    uint64_t targetSizeInBytes,
    D3D12_HEAP_TYPE heapType, ID3D12Heap **heap,
    uint64_t *offset,
    D3D12_HEAP_FLAGS extra_flags,
    bool is_sparse) {
    using namespace D3D12MA;
    ALLOCATION_DESC desc;
    desc.HeapType = heapType;
    desc.Flags = ALLOCATION_FLAGS::ALLOCATION_FLAG_STRATEGY_BEST_FIT;
    desc.ExtraHeapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS | extra_flags;
    desc.CustomPool = is_sparse ? sparse_buffer_pool : nullptr;
    D3D12_RESOURCE_ALLOCATION_INFO info;
    info.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    info.SizeInBytes = CalcPlacedOffsetAlignment(targetSizeInBytes);
    Allocation *alloc;
    allocator->AllocateMemory(&desc, &info, &alloc);
    *heap = alloc->GetHeap();
    *offset = alloc->GetOffset();
    if (profiler) [[unlikely]] {
        auto desc = luisa::format("Buffer name: \"{}\", heap type: {}, extra heap-flags: {}", name, (uint)heapType, (uint)extra_flags);
        auto stacktrace = luisa::backtrace();
        profiler->allocate(reinterpret_cast<uint64_t>(alloc), info.Alignment, info.SizeInBytes, desc, std::move(stacktrace));
    }
    return reinterpret_cast<uint64>(alloc);
}
void GpuAllocator::Release(uint64 alloc) {
    using namespace D3D12MA;
    if (alloc) {
        reinterpret_cast<Allocation *>(alloc)->Release();
        if (profiler) [[unlikely]] {
            profiler->free(alloc);
        }
    }
}
GpuAllocator::GpuAllocator(
    Device *device, luisa::compute::Profiler *profiler,
    uint64_t preferred_block_size,
    uint64_t sparse_buffer_block_size,
    uint64_t sparse_image_block_size) : profiler(profiler) {
    using namespace D3D12MA;
    ALLOCATOR_DESC desc;
    desc.Flags = ALLOCATOR_FLAGS::ALLOCATOR_FLAG_DEFAULT_POOLS_NOT_ZEROED;
    desc.pAdapter = device->adapter.Get();
    desc.pAllocationCallbacks = &ma_detail::gAllocateCallback.callbacks;
    desc.pDevice = device->device.Get();
    desc.PreferredBlockSize = preferred_block_size;
    ThrowIfFailed(D3D12MA::CreateAllocator(&desc, &allocator));
    // sparse pool
    POOL_DESC pool_desc{
        .Flags = D3D12MA::POOL_FLAG_NONE,
        .HeapProperties = D3D12_HEAP_PROPERTIES{
            .Type = D3D12_HEAP_TYPE_DEFAULT,
        },
        .HeapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS,
        .ResidencyPriority = D3D12_RESIDENCY_PRIORITY_LOW};
    pool_desc.BlockSize = sparse_buffer_block_size;
    ThrowIfFailed(allocator->CreatePool(&pool_desc, &sparse_buffer_pool));
    pool_desc.BlockSize = sparse_image_block_size;
    pool_desc.HeapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
    ThrowIfFailed(allocator->CreatePool(&pool_desc, &sparse_image_pool));
}
}// namespace lc::dx
