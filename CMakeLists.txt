cmake_minimum_required(VERSION 3.24)
project(LangulusModAssetsGeometry
    VERSION         1.0.0
    DESCRIPTION     "Langulus plug-in module for generating and managing geometry assets"
    HOMEPAGE_URL    https://langulus.com
)

# Build the module                                                              
add_library(LangulusModAssetsGeometry SHARED
    source/MeshLibrary.cpp
    source/Mesh.cpp
)

target_link_libraries(LangulusModAssetsGeometry 
    PRIVATE     Langulus
)

if(LANGULUS_TESTING)
    enable_testing()
	add_subdirectory(test)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(LangulusModAssetsGeometry
        PRIVATE -Wno-backslash-newline-escape
    )
endif()