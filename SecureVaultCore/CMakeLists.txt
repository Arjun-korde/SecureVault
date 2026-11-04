# SecureVaultCore - shared static library
find_package(Qt6 REQUIRED COMPONENTS Core Sql)
add_library(SecureVaultCore STATIC)

target_sources(SecureVaultCore
    PRIVATE
         # models
        src/models/User.cpp
        src/models/PasswordEntry.cpp

         # security
        src/security/CryptoManager.cpp
        src/security/CryptoUtils.cpp
        src/security/KeyDerivation.cpp
        src/security/PasswordHasher.cpp

        #userSession
        src/app/UserSession.cpp

        # database
        src/database/DatabaseManager.cpp
        src/database/PasswordRepository.cpp
        src/database/UserRepository.cpp
)

target_include_directories(SecureVaultCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(SecureVaultCore
    PUBLIC
        Qt6::Core
        Qt6::Sql
)

set_target_properties(SecureVaultCore PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)