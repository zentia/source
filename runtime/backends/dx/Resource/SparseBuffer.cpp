#include "SparseBuffer.h"
#include <Resource/GpuAllocator.h>
#include <luisa/core/logging.h>
#include "SparseHeap.h"
#include <DXRuntime/UpdateTileTracker.h>
namespace lc::dx {
SparseBuffer::SparseBuffer(
    Device *device,
    uint64 byteSize,
    D3D12_RESOURCE_STATES initState)
    : Buffer(device),
      byteSize(byteSize),
      initState(initState) {
    auto buffer = CD3DX12_RESOURCE_DESC::Buffer(byteSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    ThrowIfFailed(device->device->CreateReservedResource(
        &buffer,
        GetInitState(),
        nullptr,
        IID_PPV_ARGS(resource.GetAddressOf())));
}
vstd::optional<D3D12_SHADER_RESOURCE_VIEW_DESC> SparseBuffer::GetColorSrvDesc(bool isRaw) const {
    return GetColorSrvDesc(0, byteSize, isRaw);
}
vstd::optional<D3D12_UNORDERED_ACCESS_VIEW_DESC> SparseBuffer::GetColorUavDesc(bool isRaw) const {
    return GetColorUavDesc(0, byteSize, isRaw);
}
vstd::optional<D3D12_SHADER_RESOURCE_VIEW_DESC> SparseBuffer::GetColorSrvDesc(uint64 offset, uint64 byteSize, bool isRaw) const {
    return GetColorSrvDescBase(offset, byteSize, isRaw);
}
vstd::optional<D3D12_UNORDERED_ACCESS_VIEW_DESC> SparseBuffer::GetColorUavDesc(uint64 offset, uint64 byteSize, bool isRaw) const {
    return GetColorUavDescBase(offset, byteSize, isRaw);
}
SparseBuffer::~SparseBuffer() {
}
void SparseBuffer::DeAllocateTile(uint coord, uint size, UpdateTileTracker *tile_tracker) const {
    D3D12_TILED_RESOURCE_COORDINATE tileCoord{
        .X = coord,
        .Subresource = 0};
    D3D12_TILE_REGION_SIZE tileSize{
        .NumTiles = static_cast<uint>(size),
        .UseBox = true,
        .Width = static_cast<uint>(size),
        .Height = 1,
        .Depth = 1};
    tile_tracker->deallocate(resource.Get(), tileCoord, tileSize);
}
void SparseBuffer::AllocateTile(uint coord, uint size, uint64 alloc, UpdateTileTracker *tile_tracker) const {
    auto heap = reinterpret_cast<SparseHeap const *>(alloc);
    D3D12_TILED_RESOURCE_COORDINATE tileCoord{
        .X = coord,
        .Subresource = 0};
    if (heap->size_bytes < size * D3D12_TILED_RESOURCE_TILE_SIZE_IN_BYTES) [[unlikely]] {
        LUISA_ERROR("Map size out of range. Required size: {}, heap size: {}", size * D3D12_TILED_RESOURCE_TILE_SIZE_IN_BYTES, heap->size_bytes);
    }
    D3D12_TILE_REGION_SIZE tileSize{
        .NumTiles = size};
    uint tileOffset = heap->offset / D3D12_TILED_RESOURCE_TILE_SIZE_IN_BYTES;
    tile_tracker->record(heap->heap, resource.Get(), tileCoord, tileSize, D3D12_TILE_RANGE_FLAG_NONE, tileOffset, size);
}
}// namespace lc::dx
