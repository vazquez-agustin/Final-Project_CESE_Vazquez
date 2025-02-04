# Crear la biblioteca ExternalInterface
add_library(ExternalInterface ADC.cpp)

# Incluir directorios de encabezados
target_include_directories(ExternalInterface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Permitir que sensor_manager use esta biblioteca
target_link_libraries(ExternalInterface PUBLIC)
