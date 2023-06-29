#pragma once

#include <stdint.h>
#include <string>

#include "crc32_table.h"

#define ENABLE_HASH_DEBUG (1) // 1: Hash�̃f�o�b�O�@�\��L��, 0: ����

namespace Utility {

    class Hash
    {
    public:
        Hash(const char* str);

        //! �n�b�V���l���擾
        uint32_t GetDigest() const { return m_digest; }

        //! �����񂩂�n�b�V���l���쐬���Ď擾
        static constexpr uint32_t GetDigest(const char* str, const size_t length)
        {
            uint32_t digest = 0xffffffff;
            for (size_t i = 0; i < length; i++)
            {
                digest = (digest << 8) ^ s_crc32_table[((digest >> 24) ^ str[i]) & 0xff];
            }

            return digest;
        }

        //! �����񂩂�n�b�V���l���쐬���Ď擾 string�p.
        static constexpr uint32_t GetDigest(const std::string& str)
        {
            return GetDigest(str.c_str(), str.length());
        }

    private:
        uint32_t m_digest = 0; // �n�b�V���l

#if ENABLE_HASH_DEBUG
    public:
        //! (�f�o�b�O�p) ��������擾
        const char* GetDebugStr() const { return m_debug_str.c_str(); }

    private:
        std::string m_debug_str = ""; // (�f�o�b�O�p) ������
#endif
    };

    // ��������n�b�V���l�ɕϊ�����B
    // �g�p��: HASH_DIGEST("STAGE_FOREST")
    // sizeof��null�I�[���܂ݕ�����+1�����̂�-1���ĕ������݂̂ɂ��Ă����B 
#define HASH_DIGEST(str) Utility::Hash::GetDigest(str, (sizeof(str)-1))

}