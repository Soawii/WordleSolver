cmake_minimum_required(VERSION 3.10)

project(WordleSolver)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER "g++")

add_executable(WordleSolver main.cpp)

target_compile_options(${PROJECT_NAME} PRIVATE -O3 -fopenmp -march=native)
target_link_options(${PROJECT_NAME} PRIVATE -fopenmp)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/valid_words.txt
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/valid_words.txt
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/best_first_guess.txt
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/best_first_guess.txt
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/answer_words.txt
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/answer_words.txt
)