#include <runtime/core/logging.h>
#include <runtime/dsl/func.h>

namespace luisa::compute::detail {

void CallableInvoke::_error_too_many_arguments() noexcept {
    LUISA_ERROR_WITH_LOCATION("Too many arguments for callable.");
}

}// namespace luisa::compute::detail
