#include "CryptoManager.h"
#include "CryptoUtils.h"
#include "SecurityConfig.h"

#include <windows.h>
#include <bcrypt.h>

#pragma comment(lib, "bcrypt.lib")

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)


CryptoManager::EncryptedData
CryptoManager::encrypt(const QByteArray& plainText, const QByteArray& key)
{

    EncryptedData result;

    if (plainText.isEmpty()) {
        return result;
    }

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    BCRYPT_KEY_HANDLE hKey = nullptr;

    NTSTATUS status;

    result.iv = CryptoUtils::generateRandomBytes(SecurityConfig::AES_BLOCK_SIZE_BYTES);

    status = BCryptOpenAlgorithmProvider(
        &hAlg,
        BCRYPT_AES_ALGORITHM,
        nullptr,
        0
    );

    if (!NT_SUCCESS(status)) {
        return {};
    }

    status = BCryptSetProperty(
        hAlg,
        BCRYPT_CHAINING_MODE,
        (PUCHAR)BCRYPT_CHAIN_MODE_CBC,
        sizeof(BCRYPT_CHAIN_MODE_CBC),
        0
    );

    if (!NT_SUCCESS(status)) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return {};
    }

    status = BCryptGenerateSymmetricKey(
        hAlg,
        &hKey,
        nullptr,
        0,
        (PUCHAR)key.data(),
        SecurityConfig::AES_KEY_SIZE_BYTES,
        0
    );

    if (!NT_SUCCESS(status)) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return {};
    }

    DWORD cipherSize = 0;
    QByteArray ivCopy = result.iv;

    // Get output size
    status = BCryptEncrypt(
        hKey,
        (PUCHAR)plainText.data(),
        plainText.size(),
        nullptr,
        (PUCHAR)ivCopy.data(),
        ivCopy.size(),
        nullptr,
        0,
        &cipherSize,
        BCRYPT_BLOCK_PADDING
    );

    if (!NT_SUCCESS(status) || cipherSize == 0) {
        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return {};
    }

    result.cipherText.resize(cipherSize);

    ivCopy = result.iv;

    status = BCryptEncrypt(
        hKey,
        reinterpret_cast<PUCHAR>(
            const_cast<char*>(plainText.data())
            ),
        static_cast<ULONG>(plainText.size()),
        nullptr,
        reinterpret_cast<PUCHAR>(ivCopy.data()),
        static_cast<ULONG>(ivCopy.size()),
        reinterpret_cast<PUCHAR>(result.cipherText.data()),
        cipherSize,
        &cipherSize,
        BCRYPT_BLOCK_PADDING
    );

    if (!NT_SUCCESS(status)) {
        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return {};
    }

    result.cipherText.resize(cipherSize);

    BCryptDestroyKey(hKey);
    BCryptCloseAlgorithmProvider(hAlg, 0);

    return result;
}

QByteArray CryptoManager::decrypt(
    const QByteArray& cipherText,
    const QByteArray& iv,
    const QByteArray& key
)
{

    if (cipherText.isEmpty()) {
        return {};
    }

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    BCRYPT_KEY_HANDLE hKey = nullptr;

    NTSTATUS status;

    status = BCryptOpenAlgorithmProvider(
        &hAlg,
        BCRYPT_AES_ALGORITHM,
        nullptr,
        0
    );

    if (!NT_SUCCESS(status)) {
        return {};
    }

    status = BCryptSetProperty(
        hAlg,
        BCRYPT_CHAINING_MODE,
        reinterpret_cast<PUCHAR>(
            const_cast<wchar_t*>(BCRYPT_CHAIN_MODE_CBC)
            ),
        sizeof(BCRYPT_CHAIN_MODE_CBC),
        0
    );

    if (!NT_SUCCESS(status)) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return {};
    }

    status = BCryptGenerateSymmetricKey(
        hAlg,
        &hKey,
        nullptr,
        0,
        reinterpret_cast<PUCHAR>(
            const_cast<char*>(key.data())
            ),
        static_cast<ULONG>(key.size()),
        0
    );

    if (!NT_SUCCESS(status)) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return {};
    }

    DWORD plainSize = 0;

    QByteArray ivCopy = iv;

    status = BCryptDecrypt(
        hKey,
        reinterpret_cast<PUCHAR>(
            const_cast<char*>(cipherText.data())
            ),
        static_cast<ULONG>(cipherText.size()),
        nullptr,
        reinterpret_cast<PUCHAR>(
            ivCopy.data()
            ),
        static_cast<ULONG>(ivCopy.size()),
        nullptr,
        0,
        &plainSize,
        BCRYPT_BLOCK_PADDING
    );

    if (!NT_SUCCESS(status) || plainSize == 0) {
        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return {};
    }

    QByteArray plainText;
    ivCopy = iv;
    plainText.resize(plainSize);

    status = BCryptDecrypt(
        hKey,
        reinterpret_cast<PUCHAR>(
            const_cast<char*>(cipherText.data())
            ),
        static_cast<ULONG>(cipherText.size()),
        nullptr,
        reinterpret_cast<PUCHAR>(ivCopy.data()),
        static_cast<ULONG>(ivCopy.size()),
        reinterpret_cast<PUCHAR>(plainText.data()),
        plainSize,
        &plainSize,
        BCRYPT_BLOCK_PADDING
    );

    if (!NT_SUCCESS(status)) {
        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return {};
    }

    plainText.resize(plainSize);

    BCryptDestroyKey(hKey);
    BCryptCloseAlgorithmProvider(hAlg, 0);

    return plainText;
}
