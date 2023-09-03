
if(ANDROID_ABI MATCHES "x86_64" OR ANDROID_ABI MATCHES "x86")
        target_compile_options(${PROJECT_NAME} PUBLIC -mmmx -msse -msse2 -msse3 -msse4.1 -mfpmath=sse -minline-all-stringops -finline-functions)
elseif(ANDROID_ABI MATCHES "arm64-v8a" OR ANDROID_ABI MATCHES "armeabi-v7a")
        target_compile_options(${PROJECT_NAME} PUBLIC -mfpu=neon -finline-functions)
endif()
