#pragma once

#include <cassert>

#if defined NDEBUG
# define X_ASSERT(CHECK) void(0)
#else
# define X_ASSERT(CHECK) \
    ( (CHECK) ? void(0) : []{assert(!#CHECK);}() )
#endif
