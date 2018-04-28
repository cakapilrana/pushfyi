################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../router/AndOrSubscription.cpp \
../router/AndOrUnsubscribe.cpp \
../router/AndSubscription.cpp \
../router/PubSubCmd.cpp \
../router/PubSubEntry.cpp \
../router/PubSubFuture.cpp \
../router/PubSubMaps.cpp \
../router/PubSubRouter.cpp \
../router/PubTimerTask.cpp \
../router/Publication.cpp \
../router/SubTimerTask.cpp \
../router/Subscription.cpp \
../router/Unpublish.cpp \
../router/Unsubscribe.cpp 

OBJS += \
./router/AndOrSubscription.o \
./router/AndOrUnsubscribe.o \
./router/AndSubscription.o \
./router/PubSubCmd.o \
./router/PubSubEntry.o \
./router/PubSubFuture.o \
./router/PubSubMaps.o \
./router/PubSubRouter.o \
./router/PubTimerTask.o \
./router/Publication.o \
./router/SubTimerTask.o \
./router/Subscription.o \
./router/Unpublish.o \
./router/Unsubscribe.o 

CPP_DEPS += \
./router/AndOrSubscription.d \
./router/AndOrUnsubscribe.d \
./router/AndSubscription.d \
./router/PubSubCmd.d \
./router/PubSubEntry.d \
./router/PubSubFuture.d \
./router/PubSubMaps.d \
./router/PubSubRouter.d \
./router/PubTimerTask.d \
./router/Publication.d \
./router/SubTimerTask.d \
./router/Subscription.d \
./router/Unpublish.d \
./router/Unsubscribe.d 


# Each subdirectory must supply rules for building sources it contributes
router/%.o: ../router/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -I/home/vikas/svn/pushfyi/trunk/src/pushfyi/3rdparty/tinyxml/include -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/clients" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/reader" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/listener" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/defs" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/manager" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/router" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/logger" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/util" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/network" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/tls" -I"/home/vikas/svn/pushfyi/trunk/src/pushfyi/protocol" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


