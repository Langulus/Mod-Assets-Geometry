cmake_minimum_required(VERSION 3.24)
project(LangulusModAssetsGeometry)

# Build the module                                                          
add_library(LangulusModAssetsGeometry SHARED
    source/GeometryLibrary.cpp
    source/Model.cpp
)

target_link_libraries(LangulusModAssetsGeometry 
    PRIVATE     Langulus
)

if(LANGULUS_ENABLE_TESTING)
    enable_testing()
	add_subdirectory(test)
endif()