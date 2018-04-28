################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../util/Bool.cpp \
../util/ConditionVar.cpp \
../util/Double.cpp \
../util/Event.cpp \
../util/EventList.cpp \
../util/Float.cpp \
../util/Int.cpp \
../util/Long.cpp \
../util/Metric.cpp \
../util/MetricManager.cpp \
../util/Mutex.cpp \
../util/Object.cpp \
../util/Params.cpp \
../util/ProcessInit.cpp \
../util/Properties.cpp \
../util/RE.cpp \
../util/REException.cpp \
../util/Ref.cpp \
../util/RefVar.cpp \
../util/Runnable.cpp \
../util/String.cpp \
../util/Synchronized.cpp \
../util/SystemProperties.cpp \
../util/SystemTimer.cpp \
../util/Thread.cpp \
../util/Time.cpp \
../util/TimerCb.cpp \
../util/TimerSet.cpp \
../util/UInt.cpp \
../util/ULong.cpp \
../util/Var.cpp \
../util/pushfyi-types.cpp 

OBJS += \
./util/Bool.o \
./util/ConditionVar.o \
./util/Double.o \
./util/Event.o \
./util/EventList.o \
./util/Float.o \
./util/Int.o \
./util/Long.o \
./util/Metric.o \
./util/MetricManager.o \
./util/Mutex.o \
./util/Object.o \
./util/Params.o \
./util/ProcessInit.o \
./util/Properties.o \
./util/RE.o \
./util/REException.o \
./util/Ref.o \
./util/RefVar.o \
./util/Runnable.o \
./util/String.o \
./util/Synchronized.o \
./util/SystemProperties.o \
./util/SystemTimer.o \
./util/Thread.o \
./util/Time.o \
./util/TimerCb.o \
./util/TimerSet.o \
./util/UInt.o \
./util/ULong.o \
./util/Var.o \
./util/pushfyi-types.o 

CPP_DEPS += \
./util/Bool.d \
./util/ConditionVar.d \
./util/Double.d \
./util/Event.d \
./util/EventList.d \
./util/Float.d \
./util/Int.d \
./util/Long.d \
./util/Metric.d \
./util/MetricManager.d \
./util/Mutex.d \
./util/Object.d \
./util/Params.d \
./util/ProcessInit.d \
./util/Properties.d \
./util/RE.d \
./util/REException.d \
./util/Ref.d \
./util/RefVar.d \
./util/Runnable.d \
./util/String.d \
./util/Synchronized.d \
./util/SystemProperties.d \
./util/SystemTimer.d \
./util/Thread.d \
./util/Time.d \
./util/TimerCb.d \
./util/TimerSet.d \
./util/UInt.d \
./util/ULong.d \
./util/Var.d \
./util/pushfyi-types.d 


# Each subdirectory must supply rules for building sources it contributes
util/%.o: ../util/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -I/home/vikas/svn/pushfyi/trunk/src/pushfyi/3rdparty/tinyxml/include -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/clients" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/reader" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/listener" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/defs" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/manager" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/router" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/logger" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/util" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/network" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/tls" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/protocol" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


