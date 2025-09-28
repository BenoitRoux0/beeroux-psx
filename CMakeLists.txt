# PSn00bSDK example CMake script
# (C) 2021 spicyjpeg - MPL licensed

cmake_minimum_required(VERSION 3.21)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
	beeroux-project
	LANGUAGES    C CXX ASM
	VERSION      1.0.0
	DESCRIPTION  "My beeroux project"
	HOMEPAGE_URL "http://lameguy64.net/?page=psn00bsdk"
)

psn00bsdk_add_executable(beeroux GPREL src/main.c)


psn00bsdk_add_cd_image(
	iso
	beeroux
	iso.xml
	DEPENDS beeroux system.cnf
)

install(
	FILES
		${PROJECT_BINARY_DIR}/beeroux.bin
		${PROJECT_BINARY_DIR}/beeroux.cue
	TYPE BIN
)
