#pragma once

#include <unes.h>

namespace unes {
    inline void init() {
        unes_init();
    }

    inline void finish() {
        unes_finish();
    }
}
