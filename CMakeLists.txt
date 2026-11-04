cmake_minimum_required(VERSION 3.21)

set(CMAKE_PREFIX_PATH "C:/Qt/6.8.3/msvc2022_64")

project(SecureVault LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Shared core library
add_subdirectory(SecureVaultCore)

# GUI application
add_subdirectory(SecureVaultGUI)

# Native Messaging host
add_subdirectory(SecureVaultNativeHost)
