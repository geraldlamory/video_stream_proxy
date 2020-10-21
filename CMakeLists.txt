cmake_minimum_required(VERSION 3.5)
project (video_stream_proxy C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(video_stream_proxy main.cpp Socket.cpp Url.cpp HttpProxy.cpp HttpStream.cpp Utils.cpp HLSProxy.cpp)

target_link_libraries(${PROJECT_NAME} Threads::Threads)

if(WIN32)
  target_link_libraries(video_stream_proxy wsock32 ws2_32)
endif()
