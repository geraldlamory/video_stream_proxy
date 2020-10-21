cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project (video_stream_proxy C CXX)

add_executable(video_stream_proxy main.cpp Socket.cpp Url.cpp HttpProxy.cpp HttpStream.cpp Utils.cpp HLSProxy.cpp)

if(WIN32)
  target_link_libraries(video_stream_proxy wsock32 ws2_32)
endif()
