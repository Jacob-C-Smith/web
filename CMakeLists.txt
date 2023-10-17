# Need CMake 3.20
cmake_minimum_required (VERSION 3.20)

# This is the name of the repository
project ("web server")

# Set the correct paths
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/extern/)

# Comment out for Debug mode
set(IS_DEBUG_BUILD CMAKE_BUILD_TYPE STREQUAL "Debug")

# Set for debug mode
if (${IS_DEBUG_BUILD})
    add_compile_definitions(NDEBUG)
endif ()


# Find the array module
if ( NOT "${HAS_ARRAY}")
    
    # Log
    message("[web-server] Adding array project")
    
    # Add the array directory
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/array ${CMAKE_SOURCE_DIR}/build/array)

    # Let any parent project know that this project has array
    set(HAS_ARRAY true PARENT_SCOPE)
endif()

# Find the stack module
if ( NOT "${HAS_STACK}")

    # Log
    message("[web-server] Adding stack project")

    # Add the stack directory
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/stack ${CMAKE_SOURCE_DIR}/build/stack)

    # Let any parent project know that this project has stack
    set(HAS_STACK true PARENT_SCOPE)
endif()

# Find the base64 module
if ( NOT "${HAS_BASE64}")

    # Log
    message("[web-server] Adding base64 project")

    # Add the base64 directory
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/base-64 ${CMAKE_SOURCE_DIR}/build/base64)

    # Let any parent project know that this project has base64
    set(HAS_BASE64 true PARENT_SCOPE)
endif()

# Find the dict module
if ( NOT "${HAS_DICT}")
    
    # Log
    message("[web-server] Adding dict project")
    
    # Add the dict directory
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/dict ${CMAKE_SOURCE_DIR}/build/dict)

    # Let any parent project know that this project has dict
    set(HAS_DICT true PARENT_SCOPE)
endif()

# Find the crypto module
if ( NOT "${HAS_CRYPTO}")
    
    # Log
    message("[web-server] Adding crypto project")
    
    # Add the crypto directory
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/crypto ${CMAKE_SOURCE_DIR}/build/crypto)

    # Let any parent project know that this project has crypto
    set(HAS_CRYPTO true PARENT_SCOPE)
endif()

# Find the sync module
if ( NOT "${HAS_SYNC}")

    # Log
    message("[web-server] Adding sync project")

    # Add the sync directory
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/sync ${CMAKE_SOURCE_DIR}/build/sync)

    # Let any parent project know that this project has sync
    set(HAS_SYNC true PARENT_SCOPE)
endif()

# Find the http module
if ( NOT "${HAS_HTTP}")

    # Log
    message("[web-server] Adding http project")

    # Add the http directory
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/http ${CMAKE_SOURCE_DIR}/build/http)

    # Let any parent project know that this project has http
    set(HAS_HTTP true PARENT_SCOPE)
endif()

# Find the socket module
if ( NOT "${HAS_SOCKET}")

    # Log
    message("[web-server] Adding socket project")

    # Add the socket directory
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/socket ${CMAKE_SOURCE_DIR}/build/socket)

    # Let any parent project know that this project has socket
    set(HAS_SOCKET true PARENT_SCOPE)
endif()

# Find the queue module
if ( NOT "${HAS_QUEUE}")

    # Log
    message("[web-server] Adding queue project")

    # Add the queue directory
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/queue ${CMAKE_SOURCE_DIR}/build/queue)

    # Let any parent project know that this project has queue
    set(HAS_QUEUE true PARENT_SCOPE)
endif()

# Find the tuple module
if ( NOT "${HAS_TUPLE}")

    # Log
    message("[web-server] Adding tuple project")

    # Add the tuple directory
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/tuple ${CMAKE_SOURCE_DIR}/build/tuple)

    # Let any parent project know that this project has tuple
    set(HAS_TUPLE true PARENT_SCOPE)
endif()

# Find the json module
if ( NOT "${HAS_JSON}")

    # Log
    message("[web-server] Adding json project")

    # Add the json directory
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/json ${CMAKE_SOURCE_DIR}/build/json)

    # Let any parent project know that this project has json
    set(HAS_JSON true PARENT_SCOPE)
endif()

# Set the HAS_WEB
if ( NOT "${HAS_WEB}")

    # Log
    message("[web-server] Adding web project")

    # Let any parent project know that it has the web project
    set(HAS_WEB true PARENT_SCOPE)
endif()

# Add source to this project's executables
add_executable (web_server_example "main.c")
add_dependencies(web_server_example json queue array dict sync stack crypto socket tuple http)
target_include_directories(web_server_example PUBLIC include include/web ${CMAKE_SOURCE_DIR}/extern/array/include/ ${CMAKE_SOURCE_DIR}/extern/dict/include/ ${CMAKE_SOURCE_DIR}/extern/sync/include/ ${CMAKE_SOURCE_DIR}/extern/json/include/ ${CMAKE_SOURCE_DIR}/extern/socket/include/ ${CMAKE_SOURCE_DIR}/extern/queue/include/ ${CMAKE_SOURCE_DIR}/extern/tuple/include/ ${CMAKE_SOURCE_DIR}/extern/stack/include/ ${CMAKE_SOURCE_DIR}/extern/http/include/)
target_link_libraries(web_server_example web_server json queue array dict sync stack crypto socket tuple http)

# Add source to this project's library
add_library (web_server SHARED "server.c" "route.c" "web.c" "template.c")
add_dependencies(web_server json queue array dict sync stack crypto socket tuple http)
target_include_directories(web_server PUBLIC include include/web)
target_link_libraries(web_server PRIVATE json queue array dict sync stack crypto socket tuple http)
