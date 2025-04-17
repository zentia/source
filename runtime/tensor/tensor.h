#pragma once
#include <luisa/core/dll_export.h>
#include <luisa/core/stl/memory.h>
#include <luisa/core/pool.h>
#include <luisa/runtime/buffer.h>
#include <luisa/runtime/byte_buffer.h>
namespace luisa::compute {

enum struct TensorElementType : uint8_t {
    Float16,
    Float32,
    Float64,
};
class LC_TENSOR_API TensorData {
    luisa::span<uint32_t const> _sizes;
    TensorElementType _type;
    uint64_t _idx;
    size_t _size_bytes;

public:
    TensorData(luisa::span<uint32_t const> sizes,
               TensorElementType element_type,
               uint64_t uid) noexcept;
    TensorData(TensorData &&rhs) noexcept;

    TensorData(TensorData const &rhs) = delete;

    [[nodiscard]] uint64_t idx() const noexcept {
        return _idx;
    }
    [[nodiscard]] luisa::span<uint32_t const> sizes() const noexcept {
        return _sizes;
    }
    [[nodiscard]] size_t dimension() const noexcept {
        return _sizes.size();
    }
    [[nodiscard]] size_t size_bytes() const noexcept {
        return _size_bytes;
    }
};

class TensorBuilder;

class LC_TENSOR_API Tensor {
    friend class TensorBuilder;

    TensorData *_data;
    bool _contained;
    Tensor(TensorData *data,
           bool contained) noexcept;

public:
    explicit Tensor(TensorData *data) noexcept : Tensor(data, false) {}
    Tensor(Tensor &&rhs) noexcept;
    ~Tensor() noexcept;
    Tensor &operator=(Tensor &&rhs) noexcept {
        if (&rhs == this) [[unlikely]]
            return *this;
        std::destroy_at(this);
        new (this) Tensor(std::move(rhs));
    }
    [[nodiscard]] auto data() const noexcept { return _data; }
    void dispose() noexcept;

    // TODO: arithmetic
    // Tensor operator*(Tensor const &rhs) const noexcept;
    // Tensor &operator*=(Tensor const &rhs) noexcept;
    // Tensor operator+(Tensor const &rhs) const noexcept;
    // Tensor &operator+=(Tensor const &rhs) noexcept;
};

// class DTensor {
//     Device &device;
//     bool _requires_grad = false;
//     bool _reserve_memory = false;
//     bool _dirty = false;
//     std::array<size_t, 3> _shape;
//     std::array<size_t, 3> _stride;
//     luisa::optional<Buffer<T>> _buffer;
//     luisa::optional<Var<T>> _var;
// public:
//     using shape_type = std::array<size_t, 3>;
//     using value_type = T;

// private:
//     static size_t compute_size(shape_type s) {
//         size_t size = 1;
//         for (auto i : s) {
//             size *= i;
//         }
//         return size;
//     }
// public:

//     explicit DTensor(Device &device) : device(device) {}

//     DTensor(Device &device, Buffer<T> buffer, shape_type shape) : device(device), _buffer(buffer), _shape(shape) {
//     }
//     static DTensor zeros(Device &device, shape_type shape) noexcept {
//         auto size = compute_size(shape);
//         auto tensor = Tensor{device, device.create_buffer<T>(size), shape};
//     }
//     void fill(const T &value) noexcept {
//     }
//     [[nodiscard]] luisa::optional<Buffer<T> &> buffer() noexcept;
//     [[nodiscard]] Tensor &requires_grad(bool requires_grad) noexcept {
//         _requires_grad = requires_grad;
//         return *this;
//     }
//     [[nodiscard]] Tensor &reserve_memory(bool reserve_memory) noexcept {
//         _reserve_memory = reserve_memory;
//         return *this;
//     }
//     // inplace operations
//     void scatter_(const Tensor<uint, Dim> &index, const Tensor<T, Dim> &src) noexcept {
//     }
//     [[nodiscard]] DTensor<T> gather(const DTensor<uint> &index) const noexcept {
//         // TODO: implement
//         return DTensor<T>{device};
//     }

//     // template<size_t... Is>
//     // [[nodiscard]] Tensor<T, Dim + sizeof...(Is)> repeat(Is...) {
//     //     // TODO: implement
//     //     return Tensor<T, Dim>{device};
//     // }
// };

// template<class R, size_t Dim, class... Ts>
// Tensor<R, Dim> map(const Tensor<Ts, Dim> &... ts) noexcept {
//     // TODO: implement
// }

}// namespace luisa::compute