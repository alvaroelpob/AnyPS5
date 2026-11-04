#include "prx/libSceJxr/libSceJxr.hpp"
#include "prx/libc/include/Api.hpp"
#include "prx/libc/include/ApplicationHeap.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <source_location>

namespace {

void Require(bool condition, std::source_location location = std::source_location::current()) {
    if (condition) return;
    std::fprintf(stderr, "JXR image encode check failed at line %u\n", location.line());
    std::abort();
}

int liveBlocks = 0;
int zeroedCallsBeforeFailure = -1;
int closed = 0;
std::int64_t positionResult = 0;

void* APS5_VABI Allocate(std::size_t bytes) {
    ++liveBlocks;
    return std::malloc(bytes ? bytes : 1);
}

void APS5_VABI Release(void* block) {
    if (block) --liveBlocks;
    std::free(block);
}

void* APS5_VABI AllocateZeroed(std::size_t count, std::size_t bytes) {
    if (zeroedCallsBeforeFailure == 0) {
        zeroedCallsBeforeFailure = -1;
        return nullptr;
    }
    if (zeroedCallsBeforeFailure > 0) --zeroedCallsBeforeFailure;
    ++liveBlocks;
    return std::calloc(count ? count : 1, bytes ? bytes : 1);
}

void* APS5_VABI Reallocate(void*, std::size_t) { return nullptr; }
void* APS5_VABI Align(std::size_t, std::size_t) { return nullptr; }
void* APS5_VABI Realign(void*, std::size_t, std::size_t) { return nullptr; }
int APS5_VABI PosixAlign(void**, std::size_t, std::size_t) { return 12; }

void InitializeHeap() {
    void* const api[10] {
        reinterpret_cast<void*>(&Allocate), reinterpret_cast<void*>(&Release), reinterpret_cast<void*>(&AllocateZeroed),
        reinterpret_cast<void*>(&Reallocate), reinterpret_cast<void*>(&Align), reinterpret_cast<void*>(&Realign),
        reinterpret_cast<void*>(&PosixAlign),
    };
    ApplicationHeapRegister_nid_no_patch(api);
}

std::int64_t APS5_VABI GetPosition(Jxr::Stream*, std::size_t* position) {
    *position = 42;
    return positionResult;
}

std::int64_t APS5_VABI CloseStream(Jxr::Stream** stream) {
    ++closed;
    *stream = nullptr;
    return -7;
}

constexpr JxrGuestGuid DontCare {0x6fddc324, 0x4e03, 0x4bfe, {0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x00}};
constexpr JxrGuestGuid Rgb24 {0x6fddc324, 0x4e03, 0x4bfe, {0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x0d}};

Jxr::ImageEncode* NewEncoder() {
    auto* encoder = static_cast<Jxr::ImageEncode*>(calloc_nid_postfix(1, sizeof(Jxr::ImageEncode)));
    Require(encoder != nullptr);
    return encoder;
}

}

int main() {
    InitializeHeap();
    {
        auto* encoder = NewEncoder();
        Require(PKImageEncode_SetPixelFormat_nid_postfix(encoder, Rgb24) == 0);
        Require(std::memcmp(&encoder->pixelFormat, &Rgb24, 16) == 0);
        Require(encoder->width == 0 && encoder->stream == nullptr);
        free_nid_postfix(encoder);
    }
    {
        auto* encoder = NewEncoder();
        Require(PKImageEncode_SetSize_nid_postfix(encoder, 640, 360) == 0 && encoder->width == 640 && encoder->height == 360);
        Require(PKImageEncode_SetSize_nid_postfix(encoder, -1, -2) == 0 && encoder->width == 0xffffffff && encoder->height == 0xfffffffe);
        Require(encoder->currentLine == 0);
        free_nid_postfix(encoder);
    }
    {
        auto* encoder = NewEncoder();
        Require(PKImageEncode_SetResolution_nid_postfix(encoder, 72.5f, -300.0f) == 0);
        Require(encoder->resolutionX == 72.5f && encoder->resolutionY == -300.0f && encoder->frames == 0);
        free_nid_postfix(encoder);
    }
    {
        const std::uint8_t context[4] {1, 2, 3, 4};
        Require(PKImageEncode_SetColorContext_nid_postfix(nullptr, context, 4) == -2);
        Require(PKImageEncode_SetColorContext_nid_postfix(nullptr, nullptr, 0) == -2);
    }
}
