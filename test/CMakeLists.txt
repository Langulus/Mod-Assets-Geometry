project(LangulusModAssetsGeometryTest)

add_executable(LangulusModAssetsGeometryTest
	Main.cpp
	"TestGeometry.cpp"
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