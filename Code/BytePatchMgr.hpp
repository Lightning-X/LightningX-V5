#pragma once
#include <cstdint>
extern "C" std::uint64_t g_sound_overload_ret_addr;
namespace base
{
    class byte_patch_manager
    {
    public:
        byte_patch_manager();
        ~byte_patch_manager();
    };

    inline byte_patch_manager* g_byte_patch_manager;
}
