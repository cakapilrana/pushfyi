################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../clients/ClientFactory.cpp \
../clients/HeartbeatTimerTask.cpp \
../clients/PushFYIClient.cpp \
../clients/TLSClient.cpp \
../clients/TextClient.cpp \
../clients/UnknownClient.cpp 

OBJS += \
./clients/ClientFactory.o \
./clients/HeartbeatTimerTask.o \
./clients/PushFYIClient.o \
./clients/TLSClient.o \
./clients/TextClient.o \
./clients/UnknownClient.o 

CPP_DEPS += \
./clients/ClientFactory.d \
./clients/HeartbeatTimerTask.d \
./clients/PushFYIClient.d \
./clients/TLSClient.d \
./clients/TextClient.d \
./clients/UnknownClient.d 


# Each subdirectory must supply rules for building sources it contributes
clients/%.o: ../clients/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -I/home/vikas/svn/pushfyi/trunk/src/pushfyi/3rdparty/tinyxml/include -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/clients" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/reader" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/listener" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/defs" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/manager" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/router" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/logger" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/util" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/network" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/tls" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/protocol" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


