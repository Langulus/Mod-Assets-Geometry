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

# Make the write and read data dir for PhysFS, because it doesn't have access   
add_custom_command(
    TARGET LangulusModAssetsGeometryTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:LangulusModAssetsGeometryTest>/data/assets/meshes"
)