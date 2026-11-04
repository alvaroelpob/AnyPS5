#include "prx/libSceJxr/include/Api.hpp"
#include "prx/libc/include/Api.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace {

constexpr std::int64_t Success = 0;
constexpr std::int64_t FileError = -102;
constexpr int FromStart = 0;

FileStream* File(Jxr::Stream* stream, const char* function) {
    if (!stream) throw std::invalid_argument(std::string(function) + " requires a stream");
    return reinterpret_cast<FileStream*>(stream->state[0]);
}

}

extern "C" {

std::int64_t APS5_VABI WriteWS_File_nid_postfix(Jxr::Stream* stream, const void* data, std::size_t size) {
    auto* file = File(stream, __func__);
    if (size != 0 && fwrite_nid_postfix(data, size, 1, file) != 1) return FileError;
    return Success;
}

std::int64_t APS5_VABI ReadWS_File_nid_postfix(Jxr::Stream* stream, void* data, std::size_t size) {
    return fread_nid_postfix(data, size, 1, File(stream, __func__)) == 1 ? Success : FileError;
}

}
