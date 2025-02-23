add_library(ExternalInterface ADC.cpp WiFi.cpp)

target_include_directories(ExternalInterface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
