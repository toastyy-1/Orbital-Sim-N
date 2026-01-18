cmake_minimum_required(VERSION 3.20)

# Project definition
project(OrbitSimulation C)

# C Standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Force Release build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Find required packages
# Note: On Linux, ensure these are installed via package manager or built from source
# --- 1. FIND PACKAGES ---
if(APPLE)
    # Try common Homebrew installation paths
    if(EXISTS "/opt/homebrew")
        # Apple Silicon Macs
        list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
    elseif(EXISTS "/usr/local")
        # Intel Macs
        list(APPEND CMAKE_PREFIX_PATH "/usr/local")
    endif()
endif()

find_package(SDL3 CONFIG REQUIRED)
find_package(cJSON CONFIG REQUIRED)

if(NOT EMSCRIPTEN)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
endif()

if(NOT EMSCRIPTEN)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
endif()

# --- 2. DEFINE SOURCES ---
set(SOURCES
        src/main.c
        src/types.h
        src/globals.h
        src/sim/bodies.h
        src/sim/bodies.c
        src/sim/spacecraft.h
        src/sim/spacecraft.c
        src/gui/SDL_engine.h
        src/gui/SDL_engine.c
        src/utility/json_loader.h
        src/utility/json_loader.c
        src/sim/simulation.h
        src/sim/simulation.c
        src/utility/telemetry_export.c
        src/utility/telemetry_export.h
        src/gui/GL_renderer.h
        src/gui/GL_renderer.c
        src/math/matrix.h
        src/gui/models.c
        src/gui/models.h
        src/gui/stb_truetype.h
)

# --- 3. CREATE EXECUTABLE ---
add_executable(${PROJECT_NAME} ${SOURCES})

# Windows-specific: hide console window
#if(WIN32)
#    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
#endif()

# --- 4. COMPILER FLAGS & OPTIMIZATIONS ---
if(EMSCRIPTEN)
    # --- WEB ASSEMBLY FLAGS ---
    target_compile_options(${PROJECT_NAME} PRIVATE -msimd128 -pthread)

    # Linker flags for WebGL 2
    target_link_options(${PROJECT_NAME} PRIVATE
            "-sUSE_WEBGL2=1"
            "-sFULL_ES3=1"
            "-sUSE_PTHREADS=1"
            "-sASYNCIFY=1"
            "-sEXPORTED_RUNTIME_METHODS=['FS']"
            "-pthread"

            # Mappings for virtual filesystem
            "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/assets@/assets"
            "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/shaders/web@/shaders"
            "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/data@/"
    )

elseif(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-function)
    # Aggressive optimization flags for maximum performance
    add_compile_options(
            -O3
            -march=native
            -mtune=native
            -flto=auto
            -funroll-loops
            -fno-math-errno -fno-trapping-math
            -DNDEBUG
    )

    # Architecture-specific SIMD optimizations
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|i686")
        # x86/x64: Use AVX2 and FMA instructions
        add_compile_options(-mavx2 -mfma)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64|ARM64")
        # ARM: Use NEON instructions (enabled by -march=native on ARM)
        # No additional flags needed as -march=native handles this
    endif()

    add_link_options(-flto=auto)    # Enable LTO during linking

    # GCC-specific optimizations
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-fno-stack-protector)  # Remove stack protection overhead
    endif()
elseif(MSVC)
    # MSVC optimization flags (matched to Linux -O3 level)
    add_compile_options(
            /W4                    # High warning level
            /permissive-           # Standards conformance mode (pedantic)
            /Ox                    # Maximum optimization (aggressive, similar to -O3)
            /Ot                    # Favor fast code
            /Oi                    # Generate intrinsic functions
            /GL                    # Whole program optimization
            /arch:AVX2             # Use AVX2 instructions if available
            /fp:fast               # Fast floating-point model
            /GS-                   # Disable security checks for performance
    )
    add_link_options(/LTCG)    # Link-time code generation
endif()

# --- 5. INCLUDE PATHS (macOS) ---
if(APPLE)
    if(EXISTS "/opt/homebrew")
        target_include_directories(${PROJECT_NAME} PRIVATE "/opt/homebrew/include")
        link_directories("/opt/homebrew/lib")
    elseif(EXISTS "/usr/local")
        target_include_directories(${PROJECT_NAME} PRIVATE "/usr/local/include")
        link_directories("/usr/local/lib")
    endif()
endif()

# --- 6. LINK LIBRARIES ---
set(EXTRA_LIBS "")
if(NOT EMSCRIPTEN)
    list(APPEND EXTRA_LIBS OpenGL::GL GLEW::GLEW)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL3::SDL3
        cjson
        ${EXTRA_LIBS}
)

if(NOT EMSCRIPTEN)
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
    # Link math library on Unix-like systems (not needed on Windows)
    if(UNIX)
        target_link_libraries(${PROJECT_NAME} PRIVATE m)
    endif()
endif()

# --- 7. ASSET COPYING ---
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/data"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/"
        COMMENT "Copying data files"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/shaders"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
        COMMENT "Copying shader files"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        COMMENT "Copying assets"
)

# Windows: Copy SDL3 DLLs automatically
if(WIN32)
    # First check if using Conan (sdl::sdl target exists)
    if(TARGET sdl::sdl)
        get_target_property(SDL3_IMPORTED_LOCATION sdl::sdl IMPORTED_LOCATION_RELEASE)
        if(NOT SDL3_IMPORTED_LOCATION)
            get_target_property(SDL3_IMPORTED_LOCATION sdl::sdl IMPORTED_LOCATION)
        endif()
        if(SDL3_IMPORTED_LOCATION)
            get_filename_component(SDL3_LIB_DIR "${SDL3_IMPORTED_LOCATION}" DIRECTORY)
            file(GLOB SDL3_DLLS "${SDL3_LIB_DIR}/../bin/SDL3*.dll")
            if(SDL3_DLLS)
                add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        ${SDL3_DLLS}
                        $<TARGET_FILE_DIR:${PROJECT_NAME}>
                    COMMENT "Copying SDL3 DLLs from Conan package"
                )
            endif()
        endif()
    elseif(TARGET SDL3::SDL3)
        # Native SDL3 installation (vcpkg, manual, etc.)
        # Check if it's a real library target (has IMPORTED_LOCATION)
        get_target_property(SDL3_TYPE SDL3::SDL3 TYPE)
        if(SDL3_TYPE STREQUAL "SHARED_LIBRARY")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_FILE:SDL3::SDL3>
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>
                COMMENT "Copying SDL3 DLL"
            )
        else()
            # It's an interface/alias target, search for DLL manually
            get_target_property(SDL3_INCLUDE_DIRS SDL3::SDL3 INTERFACE_INCLUDE_DIRECTORIES)
            if(SDL3_INCLUDE_DIRS)
                list(GET SDL3_INCLUDE_DIRS 0 SDL3_FIRST_INCLUDE)
                get_filename_component(SDL3_ROOT "${SDL3_FIRST_INCLUDE}" DIRECTORY)
                file(GLOB SDL3_DLLS "${SDL3_ROOT}/bin/SDL3*.dll" "${SDL3_ROOT}/lib/SDL3*.dll")
                
                if(SDL3_DLLS)
                    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            ${SDL3_DLLS}
                            $<TARGET_FILE_DIR:${PROJECT_NAME}>
                        COMMENT "Copying SDL3 DLLs from installation"
                    )
                endif()
            endif()
        endif()
    endif()
endif()

# WASM: Copy files to build directory
if(EMSCRIPTEN)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/web/index.html"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/index.html"
            COMMENT "Copying index.html to build directory"
    )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/web/coi-serviceworker.js"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/coi-serviceworker.js"
            COMMENT "Copying coi-serviceworker.js to build directory"
    )
endif()
