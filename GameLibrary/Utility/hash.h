#pragma once

#include <stdint.h>
#include <string>

#include "crc32_table.h"

#define ENABLE_HASH_DEBUG (1) // 1: Hashのデバッグ機能を有効, 0: 無効

namespace Utility {

    class Hash
    {
    public:
        Hash(const char* str);

        //! ハッシュ値を取得
        uint32_t GetDigest() const { return m_digest; }

        //! 文字列からハッシュ値を作成して取得
        static constexpr uint32_t GetDigest(const char* str, const size_t length)
        {
            uint32_t digest = 0xffffffff;
            for (size_t i = 0; i < length; i++)
            {
                digest = (digest << 8) ^ s_crc32_table[((digest >> 24) ^ str[i]) & 0xff];
            }

            return digest;
        }

        //! 文字列からハッシュ値を作成して取得 string用.
        static constexpr uint32_t GetDigest(const std::string& str)
        {
            return GetDigest(str.c_str(), str.length());
        }

    private:
        uint32_t m_digest = 0; // ハッシュ値

#if ENABLE_HASH_DEBUG
    public:
        //! (デバッグ用) 文字列を取得
        const char* GetDebugStr() const { return m_debug_str.c_str(); }

    private:
        std::string m_debug_str = ""; // (デバッグ用) 文字列
#endif
    };

    // 文字列をハッシュ値に変換する。
    // 使用例: HASH_DIGEST("STAGE_FOREST")
    // sizeofはnull終端を含み文字数+1されるので-1して文字数のみにしておく。 
#define HASH_DIGEST(str) Utility::Hash::GetDigest(str, (sizeof(str)-1))

}