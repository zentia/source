#pragma once
namespace luisa::compute {
struct FusedActivation {
    enum class Type : uint32_t {
        NONE,
        ELU,
        HARD_SIGMOID,
        IDENTITY,
        LEAKY_RELU,
        LINEAR,
        PARAMETRIC_SOFTPLUS,
        RELU,
        SCALED_ELU,
        SCALED_TANH,
        SIGMOID,
        SOFTPLUS,
        SOFTSIGN,
        TANH,
        THRESHOLDED_RELU,
        SHRINK,
        CELU,
    };
    Type type = Type::NONE;
    float param1 = 0.0f;
    float param2 = 0.0f;
    FusedActivation() = default;

    explicit FusedActivation(Type type, float param1 = 0.0f, float param2 = 0.0f)
        : type(type), param1(param1), param2(param2) {}

    static FusedActivation none() noexcept {
        return FusedActivation();
    }

    static FusedActivation elu(float alpha = 1.0f) noexcept {
        return FusedActivation(Type::ELU, alpha);
    }

    static FusedActivation hard_sigmoid(float alpha = 0.2f, float beta = 0.5f) noexcept {
        return FusedActivation(Type::HARD_SIGMOID, alpha, beta);
    }

    static FusedActivation identity() noexcept {
        return FusedActivation(Type::IDENTITY);
    }

    static FusedActivation leakly_relu(float alpha = 0.01f) noexcept {
        return FusedActivation(Type::LEAKY_RELU, alpha);
    }

    static FusedActivation linear(float alpha, float beta) noexcept {
        return FusedActivation(Type::LINEAR, alpha, beta);
    }

    static FusedActivation parametric_softplus(float alpha, float beta) noexcept {
        return FusedActivation(Type::PARAMETRIC_SOFTPLUS, alpha, beta);
    }

    static FusedActivation relu() noexcept {
        return FusedActivation(Type::RELU);
    }

    static FusedActivation scaled_elu(float alpha = 1.67326319217681884765625f, float gamma = 1.05070102214813232421875f) noexcept {
        return FusedActivation(Type::SCALED_ELU, alpha, gamma);
    }

    static FusedActivation scaled_tanh(float alpha = 1.0f, float beta = 0.5f) noexcept {
        return FusedActivation(Type::SCALED_TANH, alpha, beta);
    }

    static FusedActivation sigmoid() noexcept {
        return FusedActivation(Type::SIGMOID);
    }

    static FusedActivation softplus(float steepness = 1.0f) noexcept {
        return FusedActivation(Type::SOFTPLUS, steepness);
    }

    static FusedActivation softsign() noexcept {
        return FusedActivation(Type::SOFTSIGN);
    }

    static FusedActivation tanh() noexcept {
        return FusedActivation(Type::TANH);
    }

    static FusedActivation thresholded_relu(float alpha = 1.0f) noexcept {
        return FusedActivation(Type::THRESHOLDED_RELU, alpha);
    }

    static FusedActivation shrink(float bias = 0.0f, float threshold = 0.5f) noexcept {
        return FusedActivation(Type::SHRINK, bias, threshold);
    }

    static FusedActivation celu(float alpha = 1.0f) noexcept {
        return FusedActivation(Type::CELU, alpha);
    }
};

};// namespace luisa::compute