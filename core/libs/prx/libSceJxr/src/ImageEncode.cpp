#include "prx/libSceJxr/include/Api.hpp"
#include "prx/libc/include/Api.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace {

constexpr std::int64_t Success = 0;
constexpr std::int64_t NotYetImplemented = -2;

Jxr::ImageEncode* Encoder(Jxr::ImageEncode* encoder, const char* function) {
    if (!encoder) throw std::invalid_argument(std::string(function) + " requires an encoder");
    return encoder;
}

}

extern "C" {

std::int64_t APS5_VABI PKImageEncode_SetPixelFormat_nid_postfix(Jxr::ImageEncode* encoder, JxrGuestGuid format) {
    Encoder(encoder, __func__)->pixelFormat = format;
    return Success;
}

std::int64_t APS5_VABI PKImageEncode_SetSize_nid_postfix(Jxr::ImageEncode* encoder, std::int32_t width, std::int32_t height) {
    Encoder(encoder, __func__)->width = static_cast<std::uint32_t>(width);
    encoder->height = static_cast<std::uint32_t>(height);
    return Success;
}

std::int64_t APS5_VABI PKImageEncode_SetResolution_nid_postfix(Jxr::ImageEncode* encoder, float horizontal, float vertical) {
    Encoder(encoder, __func__)->resolutionX = horizontal;
    encoder->resolutionY = vertical;
    return Success;
}

std::int64_t APS5_VABI PKImageEncode_SetColorContext_nid_postfix(Jxr::ImageEncode*, const std::uint8_t*, std::uint32_t) {
    return NotYetImplemented;
}

std::int64_t APS5_VABI PKImageEncode_SetDescriptiveMetadata_nid_postfix(Jxr::ImageEncode*, const void*) {
    return NotYetImplemented;
}

}
