# Create the ExternalInterface library and add the source files
add_library(ExternalInterface ADC.cpp WiFi.cpp)

# Include directories
target_include_directories(ExternalInterface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# When needed I can link dependencies below here
