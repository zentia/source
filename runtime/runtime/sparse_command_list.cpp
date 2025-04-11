#include <luisa/runtime/sparse_command_list.h>
#include <luisa/core/logging.h>
#include <luisa/runtime/rhi/device_interface.h>

namespace luisa::compute {

SparseCommandList::SparseCommandList() noexcept = default;

SparseCommandList::~SparseCommandList() noexcept {
    LUISA_ASSERT(_update_cmd.empty(),
                 "Destructing non-empty command list. "
                 "Did you forget to commit?");
}

SparseCommandList &SparseCommandList::operator<<(SparseUpdateTile &&tile) noexcept {
    _update_cmd.emplace_back(std::move(tile));
    return *this;
}

SparseCommandList &SparseCommandList::add_range(SparseCommandList &&cmdlist) noexcept {
    if (cmdlist.empty()) [[unlikely]]
        return *this;
    auto size = _update_cmd.size();
    luisa::enlarge_by(_update_cmd, cmdlist._update_cmd.size());
    std::memcpy(_update_cmd.data() + size, cmdlist._update_cmd.data(), luisa::size_bytes(cmdlist._update_cmd));
    cmdlist._update_cmd.clear();
    return *this;
}

SparseCommandListCommit SparseCommandList::commit() noexcept {
    return {std::move(*this)};
}

void SparseCommandList::clear() noexcept {
    _update_cmd.clear();
}

void SparseCommandListCommit::operator()(DeviceInterface *device, uint64_t stream_handle) noexcept {
    device->update_sparse_resources(stream_handle, std::move(cmd_list._update_cmd));
    cmd_list._update_cmd.clear();
}

}// namespace luisa::compute
