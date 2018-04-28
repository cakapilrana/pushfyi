################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../logger/CachedLog.cpp \
../logger/FileLogger.cpp \
../logger/Log.cpp \
../logger/LogEntry.cpp \
../logger/LogId.cpp \
../logger/LogStrategy.cpp \
../logger/StreamLogger.cpp 

OBJS += \
./logger/CachedLog.o \
./logger/FileLogger.o \
./logger/Log.o \
./logger/LogEntry.o \
./logger/LogId.o \
./logger/LogStrategy.o \
./logger/StreamLogger.o 

CPP_DEPS += \
./logger/CachedLog.d \
./logger/FileLogger.d \
./logger/Log.d \
./logger/LogEntry.d \
./logger/LogId.d \
./logger/LogStrategy.d \
./logger/StreamLogger.d 


# Each subdirectory must supply rules for building sources it contributes
logger/%.o: ../logger/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -I/home/vikas/svn/pushfyi/trunk/src/pushfyi/3rdparty/tinyxml/include -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/clients" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/reader" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/listener" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/defs" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/manager" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/router" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/logger" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/util" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/network" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/tls" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/protocol" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


