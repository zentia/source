// Copyright 2019 The Marl Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "marl/memory.h"

#include "marl/debug.h"
#include "marl/sanitizers.h"

#ifndef MARL_USE_SYSTEM_STL
#include <EASTL/allocator.h>
#endif
#include <cstring>


#if defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__) || \
    defined(__EMSCRIPTEN__)
#include <sys/mman.h>
#include <unistd.h>
namespace marl::detail {
// This was a static in pageSize(), but due to the following TSAN false-positive
// bug, this has been moved out to a global.
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=68338
const size_t kPageSize = sysconf(_SC_PAGESIZE);
inline size_t pageSize() {
  return kPageSize;
}
inline void* allocatePages(size_t count) {
  auto mapping = mmap(nullptr, count * pageSize(), PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  MARL_ASSERT(mapping != MAP_FAILED, "Failed to allocate %d pages", int(count));
  if (mapping == MAP_FAILED) {
    mapping = nullptr;
  }
  return mapping;
}
inline void freePages(void* ptr, size_t count) {
  auto res = munmap(ptr, count * pageSize());
  (void)res;
  MARL_ASSERT(res == 0, "Failed to free %d pages at %p", int(count), ptr);
}
inline void protectPage(void* addr) {
  auto res = mprotect(addr, pageSize(), PROT_NONE);
  (void)res;
  MARL_ASSERT(res == 0, "Failed to protect page at %p", addr);
}
}  // namespace marl::detail
#elif defined(__Fuchsia__)
#include <unistd.h>
#include <zircon/process.h>
#include <zircon/syscalls.h>
namespace marl::detail {
// This was a static in pageSize(), but due to the following TSAN false-positive
// bug, this has been moved out to a global.
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=68338
const size_t kPageSize = sysconf(_SC_PAGESIZE);
inline size_t pageSize() {
  return kPageSize;
}
inline void* allocatePages(size_t count) {
  auto length = count * kPageSize;
  zx_handle_t vmo;
  if (zx_vmo_create(length, 0, &vmo) != ZX_OK) {
    return nullptr;
  }
  zx_vaddr_t reservation;
  zx_status_t status =
      zx_vmar_map(zx_vmar_root_self(), ZX_VM_PERM_READ | ZX_VM_PERM_WRITE, 0,
                  vmo, 0, length, &reservation);
  zx_handle_close(vmo);
  (void)status;
  MARL_ASSERT(status == ZX_OK, "Failed to allocate %d pages", int(count));
  return reinterpret_cast<void*>(reservation);
}
inline void freePages(void* ptr, size_t count) {
  auto length = count * kPageSize;
  zx_status_t status = zx_vmar_unmap(zx_vmar_root_self(),
                                     reinterpret_cast<zx_vaddr_t>(ptr), length);
  (void)status;
  MARL_ASSERT(status == ZX_OK, "Failed to free %d pages at %p", int(count),
              ptr);
}
inline void protectPage(void* addr) {
  zx_status_t status = zx_vmar_protect(
      zx_vmar_root_self(), 0, reinterpret_cast<zx_vaddr_t>(addr), kPageSize);
  (void)status;
  MARL_ASSERT(status == ZX_OK, "Failed to protect page at %p", addr);
}
}  // namespace marl::detail
#elif defined(_WIN32)
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
namespace marl::detail {
inline size_t pageSize() {
  static auto size = [] {
    SYSTEM_INFO systemInfo = {};
    GetSystemInfo(&systemInfo);
    return systemInfo.dwPageSize;
  }();
  return size;
}
inline void* allocatePages(size_t count) {
  auto mapping = VirtualAlloc(nullptr, count * pageSize(),
                              MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  MARL_ASSERT(mapping != nullptr, "Failed to allocate %d pages", int(count));
  return mapping;
}
inline void freePages(void* ptr, size_t count) {
  (void)count;
  auto res = VirtualFree(ptr, 0, MEM_RELEASE);
  (void)res;
  MARL_ASSERT(res != 0, "Failed to free %d pages at %p", int(count), ptr);
}
inline void protectPage(void* addr) {
  DWORD oldVal = 0;
  auto res = VirtualProtect(addr, pageSize(), PAGE_NOACCESS, &oldVal);
  (void)res;
  MARL_ASSERT(res != 0, "Failed to protect page at %p", addr);
}
}  // namespace marl::detail
#else
#error "Page based allocation not implemented for this platform"
#endif

namespace marl::detail {

// pagedMalloc() allocates size bytes of uninitialized storage with the
// specified minimum byte alignment using OS specific page mapping calls.
// If guardLow is true then reads or writes to the page below the returned
// address will cause a page fault.
// If guardHigh is true then reads or writes to the page above the allocated
// block will cause a page fault.
// The pointer returned must be freed with pagedFree().
void* pagedMalloc(size_t alignment,
                  size_t size,
                  bool guardLow,
                  bool guardHigh) {
  (void)alignment;
  MARL_ASSERT(alignment < pageSize(),
              "alignment (0x%x) must be less than the page size (0x%x)",
              int(alignment), int(pageSize()));
  auto numRequestedPages = (size + pageSize() - 1) / pageSize();
  auto numTotalPages =
      numRequestedPages + (guardLow ? 1 : 0) + (guardHigh ? 1 : 0);
  auto mem = reinterpret_cast<uint8_t*>(allocatePages(numTotalPages));
  if (guardLow) {
    protectPage(mem);
    mem += pageSize();
  }
  if (guardHigh) {
    protectPage(mem + numRequestedPages * pageSize());
  }
  return mem;
}

// pagedFree() frees the memory allocated with pagedMalloc().
void pagedFree(void* ptr,
               size_t alignment,
               size_t size,
               bool guardLow,
               bool guardHigh) {
  (void)alignment;
  MARL_ASSERT(alignment < pageSize(),
              "alignment (0x%x) must be less than the page size (0x%x)",
              int(alignment), int(pageSize()));
  auto numRequestedPages = (size + pageSize() - 1) / pageSize();
  auto numTotalPages =
      numRequestedPages + (guardLow ? 1 : 0) + (guardHigh ? 1 : 0);
  if (guardLow) {
    ptr = reinterpret_cast<uint8_t*>(ptr) - pageSize();
  }
  freePages(ptr, numTotalPages);
}

}  // namespace marl::detail

namespace marl {
marl::Allocation Allocator::allocate(const marl::Allocation::Request& request) {
  void* ptr = nullptr;

  if (request.useGuards) {
    ptr = detail::pagedMalloc(request.alignment, request.size, true, true);
  } else {
#ifdef MARL_USE_SYSTEM_STL
#ifdef _WIN32
    ptr = _aligned_malloc(request.size, request.alignment);
#else
    ptr = ::aligned_alloc(request.alignment, request.size);
#endif
#else
    ptr = eastl::GetDefaultAllocator()->allocate(request.size,
                                                 request.alignment, 0u);
#endif
  }

  MARL_ASSERT(ptr != nullptr, "Allocation failed");
  MARL_ASSERT(reinterpret_cast<uintptr_t>(ptr) % request.alignment == 0,
              "Allocation gave incorrect alignment");

  marl::Allocation allocation;
  allocation.ptr = ptr;
  allocation.request = request;
  return allocation;
}

void Allocator::free(const marl::Allocation& allocation) {
  if (allocation.request.useGuards) {
    detail::pagedFree(allocation.ptr, allocation.request.alignment,
                      allocation.request.size, true, true);
  } else {
#ifdef MARL_USE_SYSTEM_STL
#ifdef _WIN32
      _aligned_free(allocation.ptr);
#else
      ::free(allocation.ptr);
#endif
#else
    eastl::GetDefaultAllocator()->deallocate(allocation.ptr, allocation.request.size);
#endif
  }
}
static Allocator alloc;
Allocator* Allocator::Default = &alloc;

size_t pageSize() {
  return detail::pageSize();
}

}  // namespace marl