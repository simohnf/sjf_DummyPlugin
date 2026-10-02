# SJF Audio Audio Plugin CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# The top-level CMakeLists.txt file for a project must contain a literal, direct call to the
# `project()` command. `project()` sets up some helpful variables that describe source/binary
# directories, and the current project version. This is a standard CMake command.

project(DummyPlugin VERSION 0.0.1)

# ------------------------------------------------------------------------------
# Sanitizer Configuration Option
# ------------------------------------------------------------------------------
option(ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)

if(ENABLE_SANITIZERS)
    if(MSVC)
        add_compile_options(/fsanitize=address)
    else()
        # macOS / Clang / GCC
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()

set(PLUGIN_MANUFACTURER_CODE "SjfA")
# --- Set Plugin Metadata (Updated by Python script) ---
set(PLUGIN_CODE "Dumm")
set(PLUGIN_NAME "Dummy Plugin")

# 2. ACTIVATE C++20 HERE (Before adding any subdirectories or targets)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_subdirectory(JUCE)                    #
add_subdirectory(sjf_audio)

# If you are building a VST2, CMake needs to be told where to find the SDK on your
# system. This setup should be done before calling `juce_add_plugin`.

# juce_set_vst2_sdk_path(...)

# `juce_add_plugin` adds a static library target with the name passed as the first argument
# (AudioPluginExample here). This target is a normal CMake target, but has a lot of extra properties set
# up by default. As well as this shared code static library, this function adds targets for each of
# the formats specified by the FORMATS arguments. This function accepts many optional arguments.
# Check the readme at `docs/CMake API.md` in the JUCE repo for the full list.

juce_add_plugin(${PROJECT_NAME}
        # VERSION ...                               # Set this if the plugin version is different to the project version
        # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
        # ICON_SMALL ...
        COMPANY_NAME sjf_plugins                          # Specify the name of the plugin's author
        # IS_SYNTH TRUE/FALSE                       # Is this a synth or an effect?
        # NEEDS_MIDI_INPUT TRUE/FALSE               # Does the plugin need midi input?
        # NEEDS_MIDI_OUTPUT TRUE/FALSE              # Does the plugin need midi output?
        # IS_MIDI_EFFECT TRUE/FALSE                 # Is this plugin a MIDI effect?
        # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE    # Does the editor need keyboard focus?
        COPY_PLUGIN_AFTER_BUILD TRUE        # Should the plugin be installed to a default location after building?
        PLUGIN_MANUFACTURER_CODE ${PLUGIN_MANUFACTURER_CODE}               # A four-character manufacturer id with at least one upper-case character
        PLUGIN_CODE ${PLUGIN_CODE}                            # A unique four-character plugin id with exactly one upper-case character
        # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
        FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
        PRODUCT_NAME ${PLUGIN_NAME}         # The name of the final executable, which can differ from the target name
        NEEDS_WEB_BROWSER FALSE                     # Set to TRUE if you need to use webviews
        NEEDS_CURL FALSE)                           # Set to TRUE if you need to use curl

# `juce_generate_juce_header` will create a JuceHeader.h for a given target, which will be generated
# into your build tree. This should be included with `#include <JuceHeader.h>`. The include path for
# this header will be automatically added to the target. The main function of the JuceHeader is to
# include all your JUCE module headers; if you're happy to include module headers directly, you
# probably don't need to call this.

juce_generate_juce_header(${PROJECT_NAME})

# `target_sources` adds source files to a target. We pass the target that needs the sources as the
# first argument, then a visibility parameter for the sources which should normally be PRIVATE.
# Finally, we supply a list of source files that will be built into the target. This is a standard
# CMake command.

target_sources(${PROJECT_NAME}
        PRIVATE
        PluginEditor.cpp
        PluginProcessor.cpp)

# `target_compile_definitions` adds some preprocessor definitions to our target. In a Projucer
# project, these might be passed in the 'Preprocessor Definitions' field. JUCE modules also make use
# of compile definitions to switch certain features on/off, so if there's a particular feature you
# need that's not on by default, check the module header for the correct flag to set here. These
# definitions will be visible both to your code, and also the JUCE module code, so for new
# definitions, pick unique names that are unlikely to collide! This is a standard CMake command.

target_compile_definitions(${PROJECT_NAME}
        PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0)

# If your target needs extra binary assets, you can add them here. The first argument is the name of
# a new static library target that will include all the binary resources. There is an optional
# `NAMESPACE` argument that can specify the namespace of the generated binary data class. Finally,
# the SOURCES argument should be followed by a list of source files that should be built into the
# static library. These source files can be of any kind (wav data, images, fonts, icons etc.).
# Conversion to binary-data will happen when your target is built.

# juce_add_binary_data(AudioPluginData SOURCES ...)


# `target_link_libraries` links libraries and JUCE modules to other libraries or executables. Here,
# we're linking our executable target to the `juce::juce_audio_utils` module. Inter-module
# dependencies are resolved automatically, so `juce_core`, `juce_events` and so on will also be
# linked automatically. If we'd generated a binary data target above, we would need to link to it
# here too. This is a standard CMake command.

target_include_directories(${PROJECT_NAME} PRIVATE Source)

set(MODULES
        juce::juce_analytics
        juce::juce_animation
        juce::juce_audio_utils
        juce::juce_box2d
        juce::juce_build_tools
        juce::juce_dsp
        juce::juce_javascript
        juce::juce_midi_ci
        juce::juce_opengl
        juce::juce_osc
        juce::juce_product_unlocking
        juce::juce_video


        sjf_helpers
        sjf_dsp
        sjf_gui
        sjf_modulation
)

set(FLAGS
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        # AudioPluginData           # If we'd created a binary data target, we'd link to it here
        ${MODULES}

        PUBLIC
        ${FLAGS}
)

include(sjf_audio/sjf_UnitTester/cmake/unit_tester.cmake)
sjf_add_juce_unit_test_target("${PROJECT_NAME}"
        MODULES
        ${MODULES}
        FLAGS
        ${FLAGS}
)



#===============#===============#===============#===============#===============
#===============#===============#===============#===============#===============
#===============#===============#===============#===============#===============
#===============#===============#===============#===============#===============

add_subdirectory ("modules/pluginval")

if(MSVC)
    # 1. Force pluginval's third-party dependencies back to C++17
    # (This natively removes char8_t without needing /Zc:char8_t-)
    foreach(vst3_target validator sdk_hosting base pluginterfaces sdk_common)
        if(TARGET ${vst3_target})
            set_property(TARGET ${vst3_target} PROPERTY CXX_STANDARD 17)
            target_compile_definitions(${vst3_target} PRIVATE _SILENCE_CXX20_U8PATH_DEPRECATION_WARNING)
        endif()
    endforeach()
endif()


if(APPLE)
    # 1. Validate VST3
    add_custom_target(${PROJECT_NAME}_validate_VST3
            COMMAND $<TARGET_FILE:pluginval> --validate "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}_VST3>" --strictness-level 10 --timeout-ms 120000 --validate-in-process --verbose
            DEPENDS ${PROJECT_NAME}_VST3 pluginval
            COMMENT "Validating VST3 bundle with pluginval..."
            VERBATIM
    )

    # 2. Validate AU
    add_custom_target(${PROJECT_NAME}_validate_AU
            # 1. Kill the background registrar so macOS refreshes its AU database
            COMMAND killall -9 AudioComponentRegistrar 2>/dev/null || true
            # 2. Targeted rescan ONLY for your specific plugin's type, subtype, and manufacturer codes
            COMMAND AU_TYPE=$(/usr/libexec/PlistBuddy -c "Print :AudioComponents:0:type" "$ENV{HOME}/Library/Audio/Plug-Ins/Components/$<TARGET_FILE_BASE_NAME:${PROJECT_NAME}_AU>.component/Contents/Info.plist") && auval -v $AU_TYPE ${PLUGIN_CODE} ${PLUGIN_MANUFACTURER_CODE} 2>/dev/null || true
            # 3. Run pluginval
            COMMAND $<TARGET_FILE:pluginval> --validate "$ENV{HOME}/Library/Audio/Plug-Ins/Components/$<TARGET_FILE_BASE_NAME:${PROJECT_NAME}_AU>.component" --strictness-level 10 --timeout-ms 120000 --validate-in-process --verbose
            DEPENDS ${PROJECT_NAME}_AU pluginval
            COMMENT "Validating AU bundle with pluginval..."
            VERBATIM
    )
    set(VALIDATE_DEPENDENCIES ${PROJECT_NAME}_validate_VST3 ${PROJECT_NAME}_validate_AU)
else ()
    # 1. Validate VST3 (Linux & Windows)
    if(UNIX AND NOT APPLE)
        add_custom_target(${PROJECT_NAME}_validate_VST3
                COMMAND /bin/sh -c "xvfb-run -a '$<TARGET_FILE:pluginval>' --validate '$ENV{HOME}/.vst3/${PLUGIN_NAME}.vst3' --strictness-level 10 --timeout-ms 120000 --validate-in-process --verbose"
                DEPENDS ${PROJECT_NAME}_VST3 pluginval
                COMMENT "Validating installed VST3 bundle with pluginval under Xvfb..."
                VERBATIM
        )
    else()
        add_custom_target(${PROJECT_NAME}_validate_VST3
                COMMAND $<TARGET_FILE:pluginval> --validate "$<TARGET_FILE:${PROJECT_NAME}_VST3>" --strictness-level 10 --timeout-ms 120000 --validate-in-process --verbose
                DEPENDS ${PROJECT_NAME}_VST3 pluginval
                COMMENT "Validating VST3 with pluginval..."
                VERBATIM
        )
    endif()
    set(VALIDATE_DEPENDENCIES ${PROJECT_NAME}_validate_VST3)
endif()

# 3. Combined Target for CLion
add_custom_target(${PROJECT_NAME}_validate_All
        DEPENDS ${VALIDATE_DEPENDENCIES}
        COMMENT "Running pluginval on ALL formats..."
        VERBATIM
)


# 4. Build all, run unit tests, validate with pluginval
add_custom_target(${PROJECT_NAME}_Build_then_validate_All
        DEPENDS ${PROJECT_NAME}_Tests ${PROJECT_NAME}_All ${PROJECT_NAME}_validate_All
        COMMAND $<TARGET_FILE:${PROJECT_NAME}_Tests>
        COMMENT "Building all targets, then validating..."
        VERBATIM
)



#===============#===============#===============#===============#===============
#===============#===============#===============#===============#===============
#===============#===============#===============#===============#===============
#===============#===============#===============#===============#===============

# 5. Sanitised validation
if(ENABLE_SANITIZERS OR ENABLE_TSAN)
    set(SANITIZED_TARGETS
            ${PROJECT_NAME}
            ${PROJECT_NAME}_VST3
            ${PROJECT_NAME}_AU
            ${PROJECT_NAME}_Standalone
            ${PROJECT_NAME}_Tests
    )

    if(ENABLE_SANITIZERS)
        foreach(target IN LISTS SANITIZED_TARGETS)
            if(TARGET ${target})
                if(MSVC)
                    target_compile_options(${target} PRIVATE /fsanitize=address)
                    target_link_options(${target} PRIVATE /fsanitize=address)
                else()
                    target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
                    target_link_options(${target} PRIVATE -fsanitize=address,undefined)
                endif()
            endif()
        endforeach()
    else() # TSAN
        foreach(target IN LISTS SANITIZED_TARGETS)
            if(TARGET ${target})
                if(NOT MSVC)
                    target_compile_options(${target} PRIVATE -fsanitize=thread)
                    target_link_options(${target} PRIVATE -fsanitize=thread)
                endif()
            endif()
        endforeach()
    endif()
endif()

# ------------------------------------------------------------------------------
# Locate Sanitizer Dynamic Preload Libraries (macOS & Linux)
# ------------------------------------------------------------------------------
if(NOT CMAKE_BINARY_DIR MATCHES "/build-(san|tsan)$")
    if(APPLE)
        execute_process(
                COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libclang_rt.asan_osx_dynamic.dylib
                OUTPUT_VARIABLE ASAN_DYLIB_PATH OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        execute_process(
                COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libclang_rt.tsan_osx_dynamic.dylib
                OUTPUT_VARIABLE TSAN_DYLIB_PATH OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        set(ASAN_PRELOAD_ENV "DYLD_INSERT_LIBRARIES=${ASAN_DYLIB_PATH}")
        set(TSAN_PRELOAD_ENV "DYLD_INSERT_LIBRARIES=${TSAN_DYLIB_PATH}")
    elseif(UNIX AND NOT APPLE)
        execute_process(
                COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libasan.so
                OUTPUT_VARIABLE ASAN_DYLIB_PATH OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        execute_process(
                COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libtsan.so
                OUTPUT_VARIABLE TSAN_DYLIB_PATH OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        set(ASAN_PRELOAD_ENV "LD_PRELOAD=${ASAN_DYLIB_PATH}")
        set(TSAN_PRELOAD_ENV "LD_PRELOAD=${TSAN_DYLIB_PATH}")
    endif()
endif()

# ------------------------------------------------------------------------------
# Sanitized Validation Targets
# ------------------------------------------------------------------------------
# ==============================================================================
# Isolated Sanitizer Validation Targets for CLion
# ==============================================================================
if(NOT CMAKE_BINARY_DIR MATCHES "/build-(san|tsan)$")
    # 1. Locate Sanitizer Dynamic Libraries on Host
    if(APPLE)
        execute_process(COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libclang_rt.asan_osx_dynamic.dylib OUTPUT_VARIABLE ASAN_LIB OUTPUT_STRIP_TRAILING_WHITESPACE)
        execute_process(COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libclang_rt.tsan_osx_dynamic.dylib OUTPUT_VARIABLE TSAN_LIB OUTPUT_STRIP_TRAILING_WHITESPACE)
    elseif(UNIX AND NOT APPLE)
        execute_process(COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libasan.so OUTPUT_VARIABLE ASAN_LIB OUTPUT_STRIP_TRAILING_WHITESPACE)
        execute_process(COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libtsan.so OUTPUT_VARIABLE TSAN_LIB OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif()

    # 2. ThreadSanitizer Isolated Validation Target
    add_custom_target(${PROJECT_NAME}_Validate_Thread_Sanitised
            # Step A: Configure isolated sub-build in build-tsan
            COMMAND ${CMAKE_COMMAND} -B "${CMAKE_CURRENT_BINARY_DIR}/build-tsan" -S "${CMAKE_CURRENT_SOURCE_DIR}" -DCMAKE_BUILD_TYPE=$<CONFIG> -DENABLE_TSAN=ON

            # Step B: Build and run Unit Tests
            COMMAND ${CMAKE_COMMAND} --build "${CMAKE_CURRENT_BINARY_DIR}/build-tsan" --config $<CONFIG> --target ${PROJECT_NAME}_Tests
            COMMAND "${CMAKE_CURRENT_BINARY_DIR}/build-tsan/${PROJECT_NAME}_Tests_artefacts/$<CONFIG>/${PROJECT_NAME}_Tests"

            # Step C: Build Plugin Formats and pluginval
            COMMAND ${CMAKE_COMMAND} --build "${CMAKE_CURRENT_BINARY_DIR}/build-tsan" --config $<CONFIG> --target ${PROJECT_NAME}_All --target pluginval

            # Step D: Run pluginval directly in shell with preloaded TSan runtime
            COMMAND /bin/sh -c "DYLD_INSERT_LIBRARIES='${TSAN_LIB}' TSAN_OPTIONS='halt_on_error=0' '${CMAKE_CURRENT_BINARY_DIR}/build-tsan/modules/pluginval/pluginval_artefacts/$<CONFIG>/pluginval.app/Contents/MacOS/pluginval' --validate '$ENV{HOME}/Library/Audio/Plug-Ins/Components/$<TARGET_FILE_BASE_NAME:${PROJECT_NAME}_AU>.component' --strictness-level 10 --timeout-ms 120000 --validate-in-process --verbose"

            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            COMMENT "Building TSan binaries in isolated directory and validating with pluginval..."
            USES_TERMINAL
            VERBATIM
    )

    # 3. AddressSanitizer + UBSan Isolated Validation Target
    add_custom_target(${PROJECT_NAME}_Validate_Sanitised
            # Step A: Configure isolated sub-build in build-san
            COMMAND ${CMAKE_COMMAND} -B "${CMAKE_CURRENT_BINARY_DIR}/build-san" -S "${CMAKE_CURRENT_SOURCE_DIR}" -DCMAKE_BUILD_TYPE=$<CONFIG> -DENABLE_SANITIZERS=ON

            # Step B: Build and run Unit Tests
            COMMAND ${CMAKE_COMMAND} --build "${CMAKE_CURRENT_BINARY_DIR}/build-san" --config $<CONFIG> --target ${PROJECT_NAME}_Tests
            COMMAND "${CMAKE_CURRENT_BINARY_DIR}/build-san/${PROJECT_NAME}_Tests_artefacts/$<CONFIG>/${PROJECT_NAME}_Tests"

            # Step C: Build Plugin Formats and pluginval
            COMMAND ${CMAKE_COMMAND} --build "${CMAKE_CURRENT_BINARY_DIR}/build-san" --config $<CONFIG> --target ${PROJECT_NAME}_All --target pluginval

            # Step D: Run pluginval directly in shell with preloaded ASan runtime
            COMMAND /bin/sh -c "DYLD_INSERT_LIBRARIES='${ASAN_LIB}' ASAN_OPTIONS='halt_on_error=0' '${CMAKE_CURRENT_BINARY_DIR}/build-san/modules/pluginval/pluginval_artefacts/$<CONFIG>/pluginval.app/Contents/MacOS/pluginval' --validate '$ENV{HOME}/Library/Audio/Plug-Ins/Components/$<TARGET_FILE_BASE_NAME:${PROJECT_NAME}_AU>.component' --strictness-level 10 --timeout-ms 120000 --validate-in-process --verbose"

            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            COMMENT "Building ASan binaries in isolated directory and validating with pluginval..."
            USES_TERMINAL
            VERBATIM
    )
endif()

#===============#===============#===============#===============#===============
#===============#===============#===============#===============#===============
#===============#===============#===============#===============#===============
#===============#===============#===============#===============#===============

# Installer target: build all, run all tests (no sanitisers), build installer package
if(APPLE)
    # Find Python 3 installed on the system
    find_package(Python3 REQUIRED COMPONENTS Interpreter)

    # Custom target to invoke installer_mac.py
    add_custom_target(${PROJECT_NAME}_create_installer
            COMMAND ${Python3_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/scripts/installer_mac.py"
            --name "${PLUGIN_NAME}"
            --version "${PROJECT_VERSION}"
            --manufacturer "${PLUGIN_MANUFACTURER_CODE}"
            --plugin-code "${PLUGIN_CODE}"
            --build-dir "${CMAKE_CURRENT_BINARY_DIR}"
            --vst3-path "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}_VST3>"
            --au-path "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}_AU>"
            --stand-path "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}_Standalone>"
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            DEPENDS ${PROJECT_NAME}_All
            COMMENT "Generating macOS installer package (.pkg)..."
            VERBATIM
    )
endif()