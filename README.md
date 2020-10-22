# Install

To build the project install cmake then type in the source directory :

    cmake .

On windows you can open the vcproj and compile using Visual Studio.

On Linux just type :

    make

# Run

Type the following command to run the proxy server :

    ./video_stream_proxy <server_url>

Default port is 8080. It can be changed in main.cpp (#define PORT)

# Example

Run the proxy server :

    ./video_stream_proxy http://bitdash-a.akamaihd.net

Then in VLC, Open Network :

    http://localhost:8080/content/MI201109210084_1/m3u8s/f08e80da-bf1d-4e3d-8899-f0f6155f6efa.m3u8

The video should play in VLC, and request IN/OUT should be printed in the server logs.

Other sources for testing :

    http://bitdash-a.akamaihd.net/content/sintel/hls/playlist.m3u8
    http://wowza-test.streamroot.io/liveOriginAbsolute/_definst_/sintel-live.smil/chunklist_b1128000.m3u8
    http://test-streams.mux.dev/x36xhzz/x36xhzz.m3u8

# Limitations

* Restricted only to HLS format
* Doesn't support https yet
* [TRACK SWITCH] is printed for each connection (thread) opened by the client
