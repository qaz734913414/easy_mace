LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := easy_mace

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../external/half \
                    $(LOCAL_PATH)/../external/opencl \
                    $(LOCAL_PATH)/../external/opencl/opencl12 \
                    $(LOCAL_PATH)/..
                    
# core
SOURCE_LIST := $(wildcard $(LOCAL_PATH)/core/*.cc)
LOCAL_SRC_FILES += $(SOURCE_LIST:$(LOCAL_PATH)/%=%)

# core/runtime/cpu
SOURCE_LIST := $(wildcard $(LOCAL_PATH)/core/runtime/cpu/*.cc)
LOCAL_SRC_FILES += $(SOURCE_LIST:$(LOCAL_PATH)/%=%)

# core/runtime/opencl
SOURCE_LIST := $(wildcard $(LOCAL_PATH)/core/runtime/opencl/*.cc)
LOCAL_SRC_FILES += $(SOURCE_LIST:$(LOCAL_PATH)/%=%)

# kernels
SOURCE_LIST := $(wildcard $(LOCAL_PATH)/kernels/*.cc)
LOCAL_SRC_FILES += $(SOURCE_LIST:$(LOCAL_PATH)/%=%)

# kernels/arm
SOURCE_LIST := $(wildcard $(LOCAL_PATH)/kernels/arm/*.cc)
LOCAL_SRC_FILES += $(SOURCE_LIST:$(LOCAL_PATH)/%=%)

# kernels/opencl
SOURCE_LIST := $(wildcard $(LOCAL_PATH)/kernels/opencl/*.cc)
LOCAL_SRC_FILES += $(SOURCE_LIST:$(LOCAL_PATH)/%=%)

# ops
SOURCE_LIST := $(wildcard $(LOCAL_PATH)/ops/*.cc)
LOCAL_SRC_FILES += $(SOURCE_LIST:$(LOCAL_PATH)/%=%)

# proto
SOURCE_LIST := $(wildcard $(LOCAL_PATH)/proto/*.cc)
LOCAL_SRC_FILES += $(SOURCE_LIST:$(LOCAL_PATH)/%=%)

# utils
SOURCE_LIST := $(wildcard $(LOCAL_PATH)/utils/*.cc)
LOCAL_SRC_FILES += $(SOURCE_LIST:$(LOCAL_PATH)/%=%)

# codegen/version
SOURCE_LIST := $(wildcard $(LOCAL_PATH)/codegen/version/*.cc)
LOCAL_SRC_FILES += $(SOURCE_LIST:$(LOCAL_PATH)/%=%)

# codegen/opencl
SOURCE_LIST := $(wildcard $(LOCAL_PATH)/codegen/opencl/*.cc)
LOCAL_SRC_FILES += $(SOURCE_LIST:$(LOCAL_PATH)/%=%)

# libmace
SOURCE_LIST := $(wildcard $(LOCAL_PATH)/libmace/*.cc)
LOCAL_SRC_FILES += $(SOURCE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_CFLAGS += -Os -Wall -fPIC
LOCAL_CPPFLAGS += -Os -std=c++11 -Wall -fPIC -D__STDC_LIMIT_MACROS -fexceptions

ifeq ($(USE_OPENMP), on)
	LOCAL_CFLAGS += -DMACE_ENABLE_OPENMP -fopenmp
	LOCAL_CPPFLAGS += -DMACE_ENABLE_OPENMP -fopenmp
endif

ifeq ($(USE_NEON), on)
    LOCAL_ARM_NEON := true
	LOCAL_CFLAGS += -DMACE_ENABLE_NEON
	LOCAL_CPPFLAGS += -DMACE_ENABLE_NEON
endif

ifeq ($(USE_OPENCL), on)
	LOCAL_CFLAGS += -DMACE_ENABLE_OPENCL
	LOCAL_CPPFLAGS += -DMACE_ENABLE_OPENCL
endif

#LOCAL_LDLIBS += -llog -fopenmp
#LOCAL_STATIC_LIBRARIES += protobuf-lite
#include $(BUILD_SHARED_LIBRARY)

LOCAL_STATIC_LIBRARIES += protobuf-lite
include $(BUILD_STATIC_LIBRARY)

include $(LOCAL_PATH)/../external/protobuf-3.4.0/Android.mk