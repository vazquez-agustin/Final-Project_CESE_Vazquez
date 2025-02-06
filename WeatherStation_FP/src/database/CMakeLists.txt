# Crear la biblioteca Database
add_library(Database influxdb.cpp)

# Incluir directorios de encabezados
target_include_directories(Database PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Permitir que sensor_manager use esta biblioteca
target_link_libraries(Database PUBLIC)