#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_UPSCALEMODE_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_UPSCALEMODE_HPP

#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <stdexcept>
#include <string_view>

namespace AgcDriver {

struct UpscaleMode {
    bool fsr = false;
    float sharpness = 0.2f;
};

inline UpscaleMode SelectUpscaleMode(const char* upscale, const char* sharpness) {
    UpscaleMode mode;
    if (upscale != nullptr && *upscale != '\0') {
        if (std::string_view(upscale) != "fsr") throw std::runtime_error("Vulkan presentation: ANYPS5_UPSCALE must be fsr or unset");
        mode.fsr = true;
    }
    if (sharpness != nullptr && *sharpness != '\0') {
        const std::string_view text(sharpness);
        bool composite = [](const std::string_view value) {
            const std::string_view digits("0123456789");
            std::size_t position = 0;
            if (!value.empty() && value.front() == '-') position = 1;
            const std::size_t start = position;
            while (position < value.size() && digits.find(value[position]) != std::string_view::npos) position++;
            if (position < value.size() && value[position] == '.') {
                position++;
                while (position < value.size() && digits.find(value[position]) != std::string_view::npos) position++;
            }
            if (position == start) return false;
            if (position < value.size() && (value[position] == 'e' || value[position] == 'E')) {
                position++;
                if (position < value.size() && (value[position] == '+' || value[position] == '-')) position++;
                const std::size_t exponent = position;
                while (position < value.size() && digits.find(value[position]) != std::string_view::npos) position++;
                if (position == exponent) return false;
            }
            return position == value.size();
        }(text);
        errno = 0;
        char* end = nullptr;
        const float value = composite ? std::strtof(text.data(), &end) : 0.0f;
        if (!composite || errno == ERANGE || end != text.data() + text.size() || !std::isfinite(value) || value < 0.0f || value > 2.0f) throw std::runtime_error("Vulkan presentation: ANYPS5_FSR_SHARPNESS must be a number from 0.0 to 2.0");
        mode.sharpness = value;
    }
    return mode;
}

}

#endif
