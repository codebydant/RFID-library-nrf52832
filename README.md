# RFID Library nRF52832
This is a C library to program a RFIDRW-E-TTL board with a **nRF52832 board** 

## How to compile and flash
1. Download:
	
	* gcc arm none eabi toolchain x86-64-linux: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads
	* SDK for the nRF52 16.0: https://www.nordicsemi.com/Software-and-tools/Software/nRF5-SDK/Download#infotabs
	* Programmer NRFJPROG: https://www.nordicsemi.com/Software-and-tools/Development-Tools/nRF-Command-Line-Tools/Download#infotabs
	* JLink: https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack

2. Create a folder in /opt to unzip files, e.g. */opt/SEGGER/*
3. Change lines **19**,**20**,**21** and **22** in the *CMakeLists.txt* file at the root project with the path for each one (toolchain,SDK and Programmer) and save it.
	e.g. 

		set(NRF_TARGET 	"nrf52")							  # nrf51 or nrf52
		set(ARM_NONE_EABI_TOOLCHAIN_PATH "/opt/SEGGER/gcc-arm-none-eabi-9-2019-q4-major") # Path to root folder 
		set(NRF5_SDK_PATH "/opt/SEGGER/nRF5SDK"	)					  # Path to root folder
		set(NRFJPROG "/opt/SEGGER/Toolchain/nrfjprog/nrfjprog")				  # Path to .bin file

4. Create a *build* folder and,
	