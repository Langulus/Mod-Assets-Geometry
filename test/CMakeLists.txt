file(GLOB_RECURSE
	LANGULUS_MOD_ASSETS_GEOMETRY_TEST_SOURCES 
	LIST_DIRECTORIES FALSE CONFIGURE_DEPENDS
	*.cpp *.hpp
)

add_executable(LangulusModAssetsGeometryTest ${LANGULUS_MOD_ASSETS_GEOMETRY_TEST_SOURCES})

target_link_libraries(LangulusModAssetsGeometryTest
	PRIVATE		Langulus
				Catch2
)

add_dependencies(LangulusModAssetsGeometryTest
	LangulusModAssetsGeometry
	LangulusModFileSystem
)

add_test(
	NAME		LangulusModAssetsGeometryTest
	COMMAND		LangulusModAssetsGeometryTest
	WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# Make the write and read data dir for PhysFS, because it doesn't have access   
add_custom_command(
    TARGET LangulusModAssetsGeometryTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
		"$<TARGET_FILE_DIR:LangulusModAssetsGeometryTest>/data/assets/meshes"
)