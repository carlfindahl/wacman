# Encryption source files 

target_sources(
    ${EXEC_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/base_encryptor.h

    ${CMAKE_CURRENT_LIST_DIR}/vignere_encryptor.h
    ${CMAKE_CURRENT_LIST_DIR}/vignere_encryptor.cpp
)