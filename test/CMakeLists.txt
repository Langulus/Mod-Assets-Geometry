file(GLOB_RECURSE
	LANGULUS_MOD_ASSETS_GEOMETRY_TEST_SOURCES 
	LIST_DIRECTORIES FALSE CONFIGURE_DEPENDS
	*.cpp
)

add_langulus_test(LangulusModAssetsGeometryTest
	SOURCES			${LANGULUS_MOD_ASSETS_GEOMETRY_TEST_SOURCES}
	LIBRARIES		Langulus
	DEPENDENCIES    LangulusModAssetsGeometry
					LangulusModAssetsImages
					LangulusModFileSystem
)

# Make the write and read data dir for PhysFS, because it doesn't have access   
add_custom_command(
    TARGET LangulusModAssetsGeometryTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
		"$<TARGET_FILE_DIR:LangulusModAssetsGeometryTest>/data/assets/meshes"
    COMMAND ${CMAKE_COMMAND} -E copy
		"${CMAKE_CURRENT_SOURCE_DIR}/data/maxwell.obj"
		"$<TARGET_FILE_DIR:LangulusModAssetsGeometryTest>/data/assets/meshes/maxwell/maxwell.obj"
    COMMAND ${CMAKE_COMMAND} -E copy
		"${CMAKE_CURRENT_SOURCE_DIR}/data/maxwell.mtl"
		"$<TARGET_FILE_DIR:LangulusModAssetsGeometryTest>/data/assets/meshes/maxwell/maxwell.mtl"
    COMMAND ${CMAKE_COMMAND} -E copy
		"${CMAKE_CURRENT_SOURCE_DIR}/data/dingus_nowhiskers.png"
		"$<TARGET_FILE_DIR:LangulusModAssetsGeometryTest>/data/assets/meshes/maxwell/dingus_nowhiskers.png"
    COMMAND ${CMAKE_COMMAND} -E copy
		"${CMAKE_CURRENT_SOURCE_DIR}/data/dingus_whiskers.png"
		"$<TARGET_FILE_DIR:LangulusModAssetsGeometryTest>/data/assets/meshes/maxwell/dingus_whiskers.png"
)