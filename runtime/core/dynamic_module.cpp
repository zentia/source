#include <runtime/core/dynamic_module.h>
#include <runtime/core/clock.h>
#include <runtime/core/logging.h>
#include <runtime/core/stl/filesystem.h>
#ifdef LUISA_PLATFORM_WINDOWS
#include <Windows.h>
#endif
namespace luisa {

DynamicModule &DynamicModule::operator=(DynamicModule &&rhs) noexcept {
    if (this != &rhs) {
        dispose();
        _handle = rhs._handle;
        rhs._handle = nullptr;
    }
    return *this;
}

DynamicModule::DynamicModule(DynamicModule &&another) noexcept
    : _handle{another._handle} { another._handle = nullptr; }

DynamicModule::~DynamicModule() noexcept { dispose(); }

void DynamicModule::dispose() noexcept {
    if (_handle) {
        dynamic_module_destroy(_handle);
        _handle = nullptr;
    }
}

[[nodiscard]] static std::mutex &dynamic_module_search_path_mutex() noexcept {
    static std::mutex mutex;
    return mutex;
}

[[nodiscard]] static auto &dynamic_module_search_paths() noexcept {
    static luisa::vector<std::pair<luisa::filesystem::path, std::size_t>> paths;
    return paths;
}

#ifdef LUISA_PLATFORM_WINDOWS
[[nodiscard]] static auto &dynamic_module_search_path_cookies() noexcept {
    static luisa::vector<DLL_DIRECTORY_COOKIE> cookies;
    return cookies;
}
#endif

void DynamicModule::reset_search_paths() noexcept {
    dynamic_module_search_paths() = luisa::vector<std::pair<luisa::filesystem::path, std::size_t>>{};
#ifdef LUISA_PLATFORM_WINDOWS
    dynamic_module_search_path_cookies() = luisa::vector<DLL_DIRECTORY_COOKIE>{};
#endif
}

void DynamicModule::add_search_path(const luisa::filesystem::path &path) noexcept {
    std::lock_guard lock{dynamic_module_search_path_mutex()};
    auto canonical_path = luisa::filesystem::canonical(path);
    auto &&paths = dynamic_module_search_paths();
    if (
        auto iter = std::find_if(
            paths.begin(),
            paths.end(),
            [&canonical_path](auto &&p) noexcept {
                return p.first == canonical_path;
            });
        iter != paths.end()) {
        iter->second++;
    } else {
#ifdef LUISA_PLATFORM_WINDOWS
        auto &&cookies = dynamic_module_search_path_cookies();
        cookies.emplace_back(AddDllDirectory(canonical_path.c_str()));
#endif
        paths.emplace_back(std::move(canonical_path), 0);
    }
}

void DynamicModule::remove_search_path(const std::filesystem::path &path) noexcept {
    std::lock_guard lock{dynamic_module_search_path_mutex()};
    auto canonical_path = std::filesystem::canonical(path);
    auto &&paths = dynamic_module_search_paths();
    if (auto iter = std::find_if(paths.begin(), paths.end(), [&canonical_path](auto &&p) noexcept {
            return p.first == canonical_path;
        });
        iter != paths.end()) {
        if (--iter->second == 0u) {
            auto index = std::distance(paths.begin(), iter);
            paths.erase(iter);
#ifdef LUISA_PLATFORM_WINDOWS
            auto &&cookies = dynamic_module_search_path_cookies();
            RemoveDllDirectory(cookies[index]);
            cookies.erase(cookies.begin() + index);
#endif
        }
    }
}

DynamicModule DynamicModule::load(std::string_view name) noexcept {
    std::lock_guard lock{dynamic_module_search_path_mutex()};
    Clock clock;
    auto &&paths = dynamic_module_search_paths();
    for (auto iter = paths.crbegin(); iter != paths.crend(); iter++) {
        auto p = iter->first / dynamic_module_name(name);
        if (auto handle = dynamic_module_load(p)) {
            LUISA_INFO(
                "Loaded dynamic module '{}' in {} ms.",
                to_string(p), clock.toc());
            return DynamicModule{handle};
        }
    }
    auto module_name = dynamic_module_name(name);
    if (auto handle = dynamic_module_load(module_name)) {
        LUISA_INFO(
            "Loaded dynamic module '{}' in {} ms.",
            module_name, clock.toc());
        return DynamicModule{handle};
    }
    return DynamicModule{nullptr};
}

DynamicModule DynamicModule::load(
    const luisa::filesystem::path &folder,
    luisa::string_view name) noexcept {
    Clock clock;
    auto p = folder / dynamic_module_name(name);
    if (auto handle = dynamic_module_load(p)) {
        LUISA_INFO(
            "Loaded dynamic module '{}' in {} ms.",
            to_string(p), clock.toc());
        return DynamicModule{handle};
    }

    return DynamicModule{nullptr};
}

}// namespace luisa
