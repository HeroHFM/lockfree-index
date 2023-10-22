cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(lockfree-index/ LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

# Threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Sanitizers
# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)

#add_compile_options(-fsanitize=undefined)
#add_link_options(-fsanitize=undefined)

# add_compile_options(-fsanitize=leak)
# add_link_options(-fsanitize=leak)

#add_compile_options(-fsanitize=thread)
#add_link_options(-fsanitize=thread)

add_compile_options(-Wall -Wextra  -Wshadow -Wpedantic)

# Add executable program.
add_executable(tester app/Tester.cpp lib/Geometry.cpp)
target_include_directories(tester PUBLIC include/)
# target_link_libraries(test_queue Threads::Threads)
