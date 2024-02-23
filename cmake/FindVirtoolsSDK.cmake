# Find the Virtools SDK header + import library
#
#  VIRTOOLS_SDK_PATH        - Path to Virtools SDK.
#  VIRTOOLS_SDK_INCLUDE_DIR - Where to find CKAll.h
#  VIRTOOLS_SDK_LIBRARIES   - List of libraries when using VirtoolsSDK.
#  VirtoolsSDK_FOUND        - True if Virtools SDK found.
#  VirtoolsSDK::CK2         - Imported library of CK2
#  VirtoolsSDK::VxMath      - Imported library of VxMath

find_path(VIRTOOLS_SDK_INCLUDE_DIR CKAll.h
        HINTS $ENV{VIRTOOLS_SDK_PATH} ${VIRTOOLS_SDK_PATH}
        PATH_SUFFIXES Include Includes
        DOC "Where the Virtools SDK headers can be found"
        )

set(VIRTOOLS_SDK_LIBRARY_NAMES CK2 VxMath)

set(VIRTOOLS_SDK_LIBRARIES)
foreach (LIBNAME IN LISTS VIRTOOLS_SDK_LIBRARY_NAMES)
    find_library(VIRTOOLS_SDK_${LIBNAME} NAMES ${LIBNAME}
            HINTS $ENV{VIRTOOLS_SDK_PATH} ${VIRTOOLS_SDK_PATH}
            PATH_SUFFIXES Lib
            DOC "Where the Virtools SDK libraries can be found"
            )
    list(APPEND VIRTOOLS_SDK_LIBRARIES ${VIRTOOLS_SDK_${LIBNAME}})
endforeach ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(VirtoolsSDK DEFAULT_MSG VIRTOOLS_SDK_LIBRARIES VIRTOOLS_SDK_INCLUDE_DIR)

if (VirtoolsSDK_FOUND)
    foreach (LIBNAME IN LISTS VIRTOOLS_SDK_LIBRARY_NAMES)
        if (NOT TARGET ${LIBNAME})
            add_library(${LIBNAME} UNKNOWN IMPORTED)
            add_library(VirtoolsSDK::${LIBNAME} ALIAS ${LIBNAME})
            set_target_properties(${LIBNAME} PROPERTIES
                    IMPORTED_LOCATION ${VIRTOOLS_SDK_${LIBNAME}}
                    INTERFACE_INCLUDE_DIRECTORIES ${VIRTOOLS_SDK_INCLUDE_DIR}
                    )
            # Disable strict const-qualification conformance for pointers initialized by using string literals
            target_compile_options(${LIBNAME} INTERFACE
                    $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:MSVC>:/Zc:strictStrings->>
                    $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:GNU>:-fpermissive>>
                    $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:GNU>:-Wno-write-strings>>
                    )
        endif ()
    endforeach ()
endif ()