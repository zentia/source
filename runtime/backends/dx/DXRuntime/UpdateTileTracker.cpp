#include "UpdateTileTracker.h"

namespace lc::dx {
void UpdateTileTracker::record(
    ID3D12Heap *heap,
    ID3D12Resource *resource,
    D3D12_TILED_RESOURCE_COORDINATE const &ResourceRegionStartCoordinate,
    D3D12_TILE_REGION_SIZE const &ResourceRegionSize,
    D3D12_TILE_RANGE_FLAGS RangeFlag,
    UINT HeapRangeStartOffset,
    UINT RangeTileCount) {
    auto iter = map.try_emplace(resource);
    auto &heap_map = iter.first->second;
    auto heap_iter = heap_map.update_tiles.try_emplace(heap);
    auto &v = heap_iter.first->second;
    v.ResourceRegionStartCoordinates.emplace_back(ResourceRegionStartCoordinate);
    v.ResourceRegionSizes.emplace_back(ResourceRegionSize);
    v.RangeFlags.emplace_back(RangeFlag);
    v.HeapRangeStartOffsets.emplace_back(HeapRangeStartOffset);
    v.RangeTileCounts.emplace_back(RangeTileCount);
}
void UpdateTileTracker::update(
    ID3D12CommandQueue *queue,
    D3D12_TILE_MAPPING_FLAGS Flags) {
    for (auto &kv : map) {
        if (!kv.second.ResourceRegionSizes.empty()) {
            if (kv.second.update_tiles.empty()) {
                queue->UpdateTileMappings(
                    kv.first,
                    kv.second.ResourceRegionStartCoordinates.size(),
                    kv.second.ResourceRegionStartCoordinates.data(),
                    kv.second.ResourceRegionSizes.data(),
                    nullptr,
                    1,
                    vstd::get_rval_ptr(D3D12_TILE_RANGE_FLAG_NULL),
                    nullptr,
                    nullptr,
                    Flags);
            } else {
                auto &first_tile = kv.second.update_tiles.begin()->second;
                vstd::push_back_all(first_tile.ResourceRegionStartCoordinates, vstd::span{kv.second.ResourceRegionStartCoordinates});
                vstd::push_back_all(first_tile.ResourceRegionSizes, vstd::span{kv.second.ResourceRegionSizes});
                vstd::push_back_all(first_tile.RangeFlags, kv.second.ResourceRegionSizes.size(), D3D12_TILE_RANGE_FLAG_NULL);
                vstd::push_back_all(first_tile.HeapRangeStartOffsets, kv.second.ResourceRegionSizes.size(), 0);
                vstd::push_back_func(first_tile.RangeTileCounts, kv.second.ResourceRegionSizes.size(), [&](size_t i) {
                    auto &size = first_tile.ResourceRegionSizes[i];
                    return size.Width * static_cast<uint>(size.Height) * static_cast<uint>(size.Depth);
                });
            }
        }
        for (auto &tile : kv.second.update_tiles) {
            queue->UpdateTileMappings(
                kv.first,
                tile.second.ResourceRegionStartCoordinates.size(),
                tile.second.ResourceRegionStartCoordinates.data(),
                tile.second.ResourceRegionSizes.data(),
                tile.first, tile.second.RangeFlags.size(),
                tile.second.RangeFlags.data(),
                tile.second.HeapRangeStartOffsets.data(),
                tile.second.RangeTileCounts.data(),
                Flags);
        }
    }
    map.clear();
}
void UpdateTileTracker::deallocate(
    ID3D12Resource *resource,
    D3D12_TILED_RESOURCE_COORDINATE const &ResourceRegionStartCoordinate,
    D3D12_TILE_REGION_SIZE const &ResourceRegionSize) {
    auto iter = map.try_emplace(resource);
    auto &v = iter.first->second;
    v.ResourceRegionStartCoordinates.emplace_back(ResourceRegionStartCoordinate);
    v.ResourceRegionSizes.emplace_back(ResourceRegionSize);
}
}// namespace lc::dx