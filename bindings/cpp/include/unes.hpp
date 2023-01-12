#pragma once

#include <unes.h>
#include "ppu.hpp"

namespace unes {
    inline void init() {
        unes_init();
    }

    inline void finish() {
        unes_finish();
    }
}
