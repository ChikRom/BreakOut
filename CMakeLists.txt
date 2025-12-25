cmake_minimum_required(VERSION 4.2.1)

project(BreakOut)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# all cpp and h files

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE  HEADER_FILES "src/*.h" "src/*.hpp")

#create executable file
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

# all libraries
file (GLOB LIB_FILES "lib/*.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB_FILES} opengl32)

# header files
target_include_directories(BreakOut PRIVATE include src)

# include support for Debug/Release
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# directory with executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/Release")

# Separate Debug and Release
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_definitions(_DEBUG)
	add_compile_options(/Od)
else()
	add_compile_definitions(NDEBUG)
	add_compile_options(/O2)
endif()

function(copy_resources)
    set(RESOURCE_DIRS
        shaders
        audio
        levels  
        resources/fonts
        textures
    )
    
    foreach(dir ${RESOURCE_DIRS})
        if(EXISTS ${CMAKE_SOURCE_DIR}/${dir})
            file(GLOB_RECURSE FILES_TO_COPY "${CMAKE_SOURCE_DIR}/${dir}/*")
            
            foreach(file ${FILES_TO_COPY})
                get_filename_component(filename ${file} NAME)
                get_filename_component(filedir ${file} DIRECTORY)
                
                file(RELATIVE_PATH rel_dir ${CMAKE_SOURCE_DIR}/${dir} ${filedir})

                add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory
                    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${dir}/${rel_dir}"
                    
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${file}"
                    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${dir}/${rel_dir}/${filename}"
                )
            endforeach()
        endif()
    endforeach()
endfunction()

copy_resources()

# copying dll files
# copying dll files
file(GLOB DLL_FILES "lib/*.dll")

foreach(dll ${DLL_FILES})
    get_filename_component(dll_name ${dll} NAME)
    
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${dll}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${dll_name}"
        COMMENT "Copying ${dll_name}"
    )
endforeach()
