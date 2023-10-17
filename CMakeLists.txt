cmake_minimum_required(VERSION 3.12)

project("1101 CG Sample Code"
    VERSION 0.1.0
    LANGUAGES CXX C
    DESCRIPTION "1101 NTOU Computer Graphics Sample Code"
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

#[[
Define Compiler Flags

Depending on the compiler you're using, this block of code sets the corresponding compiler flags. You can ignore this
part and look into it later.
]]
# GNU Compiler Collection (GCC)
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    # General
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic-errors")

    # Debug mode
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -ggdb -D_GLIBC_DEBUG")

# LLVM Clang
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    # General
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic-errors")

    # Debug mode
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -ggdb")

# Microsoft Visual C++
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    # General
    if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
    endif()
endif()

#[[ End of Compiler Flags ]]

# Print out compiler settings
message(STATUS "C Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "C Flags: ${CMAKE_C_FLAGS}")
message(STATUS "C Debug Flags: ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "C Release Flags: ${CMAKE_C_FLAGS_RELEASE}")

# Find external libraries
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenCV 4 REQUIRED)

set(Codes
    "main"
)

foreach(ex IN LISTS Codes)
    message(STATUS "Configuring ${ex}...")

    add_executable(${ex})
    target_sources(${ex} PRIVATE "${ex}.cpp")
    target_link_libraries(${ex} PRIVATE GLUT::GLUT)

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_link_libraries(${ex} PRIVATE
            OpenGL::OpenGL
            OpenGL::GLU
            m # Required by math.h
            X11 # Required by freeglut static linkage
        )
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        target_link_libraries(${ex} PRIVATE
            OpenGL32
            ${OpenCV_LIBS} # 这里链接OpenCV库
        )
    endif()
endforeach()
