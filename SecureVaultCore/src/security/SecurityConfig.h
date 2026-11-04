#pragma once

#include <cstddef>
#include <cstdint>

namespace SecurityConfig {

	/* Password hashing (authentication) */

	// Salt size for stored password hashes
	constexpr std::size_t PASSWORD_SALT_SIZE_BYTES = 16;

	// PBKDF2 iterations for password hashing
	constexpr std::uint32_t PASSWORD_HASH_ITERATIONS = 200'000;

	// Output size of password hash
	constexpr std::size_t PASSWORD_HASH_SIZE_BYTES = 32;


	/* Key derivation (encryption keys) */

	// Salt size for deriving encryption keys
	constexpr std::size_t ENCRYPTION_SALT_SIZE_BYTES = 16;

	// PBKDF2 iterations for encryption key derivation
	constexpr std::uint32_t KEY_DERIVATION_ITERATIONS = 150'000;

	/* Symmetric encryption(AES) */

	// AES-256 key size
	constexpr std::size_t AES_KEY_SIZE_BYTES = 32;

	// AES block size (CBC mode)
	constexpr std::size_t AES_BLOCK_SIZE_BYTES = 16;

}
