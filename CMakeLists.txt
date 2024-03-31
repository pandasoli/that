cmake_minimum_required(VERSION 3.28.1)

project(ThatLang VERSION 1.0.0)
set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -ggdb -std=c18")
set(CMAKE_CTEST_ARGUMENTS "--output-on-failure")

add_library(
	th SHARED

	src/diags/bag.c
	src/diags/diag.c
	src/diags/span.c

	src/lexer/lexer.c
	src/lexer/token.c

	src/intern/utils.c
	src/intern/debug.c)

target_link_libraries(
	th
	PRIVATE m)

target_include_directories(
	th
	PUBLIC include)

install(
	DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/thatlang
	DESTINATION /usr/include)

install (
	TARGETS th
	DESTINATION /usr/lib64)

# -- Tests --
enable_testing()

function(mk_test name command)
	add_executable(
		${command}
		tests/${command}.c)

	target_include_directories(
		${command}
		PUBLIC include/thatlang include/intern)

	target_link_libraries(
		${command}
		PRIVATE th check)

	add_test(NAME ${name} COMMAND ${command})
endfunction()

mk_test("Test Lexer" "check_lexer")
