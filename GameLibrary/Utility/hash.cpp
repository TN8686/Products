#include "hash.h"

namespace Utility {

    Hash::Hash(const char* str)
    {
        m_digest = GetDigest(str, std::string(str).length());

#if ENABLE_HASH_DEBUG
        m_debug_str = str;
#endif
    }
}