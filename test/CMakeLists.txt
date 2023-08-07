project(LangulusModAssetsGeometryTest)

add_executable(LangulusModAssetsGeometryTest
	Main.cpp
	TestGeometry.cpp
)

target_link_libraries(LangulusModAssetsGeometryTest
	PRIVATE		Langulus
				Catch2
)

add_test(
	NAME		LangulusModAssetsGeometryTest
	COMMAND		LangulusModAssetsGeometryTest
)

langulus_copy_dlls(LangulusModAssetsGeometryTest ON LangulusModAssetsGeometryTest)
langulus_copy_dlls_advanced(LangulusModAssetsGeometryTest
	TO			LangulusModAssetsGeometryTest
	FROM		LangulusModAssetsGeometry
				LangulusModFileSystem
)


# Make the write and read data dir for PhysFS, because it doesn't have access   
add_custom_command(
    TARGET LangulusModAssetsGeometryTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:LangulusModAssetsGeometryTest>/data/assets/meshes"
)