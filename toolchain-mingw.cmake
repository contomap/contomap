set(CMAKE_SYSTEM_NAME Windows)

# Forcing '-posix' suffix in order to support new C++ headers
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc-posix)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++-posix)
