add_subdirectory(union)

include(tinyjson.cmake)
include(SHA256.cmake)

target_link_libraries(${CMAKE_PROJECT_NAME}
	PRIVATE
		union
		TinyJSON
		sha256
)