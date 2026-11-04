#include "CryptoUtils.h"

#include <windows.h>
#include <bcrypt.h>

#pragma comment(lib, "bcrypt.lib")

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

namespace CryptoUtils {

    /* Function to generate random bytes*/
    QByteArray generateRandomBytes(size_t size)
    {
        QByteArray buffer(size, Qt::Uninitialized);

        NTSTATUS status = BCryptGenRandom(
            nullptr,
            reinterpret_cast<PUCHAR>(buffer.data()),
            static_cast<ULONG>(size),
            BCRYPT_USE_SYSTEM_PREFERRED_RNG
        );

        if (!NT_SUCCESS(status)) {
            return {};
        }

        return buffer;
    }

    /* Function to derive key */
    QByteArray pbkdf2Sha256(
        const QByteArray& password,
        const QByteArray& salt,
        uint32_t iterations,
        size_t outputLen
    )
    {
        if (password.isEmpty() || salt.isEmpty() || iterations == 0 || outputLen == 0) {
            return {};
        }

        BCRYPT_ALG_HANDLE hAlg = nullptr;
        NTSTATUS status = BCryptOpenAlgorithmProvider(
            &hAlg,
            BCRYPT_SHA256_ALGORITHM,
            nullptr,
            BCRYPT_ALG_HANDLE_HMAC_FLAG
        );

        if (!NT_SUCCESS(status)) {
            return {};
        }

        QByteArray derivedKey(outputLen, Qt::Uninitialized);

         status = BCryptDeriveKeyPBKDF2(
            hAlg,
            reinterpret_cast<PUCHAR>(
                const_cast<char*>(password.data())
                ),
            static_cast<ULONG>(password.size()),
            reinterpret_cast<PUCHAR>(
                const_cast<char*>(salt.data())
                ),
            static_cast<ULONG>(salt.size()),
            iterations,
            reinterpret_cast<PUCHAR>(derivedKey.data()),
            static_cast<ULONG>(outputLen),
            0
        );

        BCryptCloseAlgorithmProvider(hAlg, 0);

        if (!NT_SUCCESS(status)) {
            return {};
        }

        return derivedKey;
    }

}
