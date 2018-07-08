// Copyright 2018 Xiaomi, Inc.  All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#if defined(MACE_ENABLE_OPENCL)

#include "mace/core/runtime/opencl/opencl_wrapper.h"
#include <CL/opencl.h>
#include <string>
#include <vector>
#include "mace/utils/logging.h"

#ifdef _WIN32

namespace mace
{
    void LoadOpenCLLibrary()
    {

    }

    void UnloadOpenCLLibrary()
    {

    }
}

#else
#include <dlfcn.h>
/**
 * Wrapper of OpenCL 2.0, based on file opencl20/CL/cl.h
 */
namespace mace {

namespace {
class OpenCLLibraryImpl final {
 public:
  bool Load();
  void Unload();

  using clGetPlatformIDsFunc = cl_int (*)(cl_uint, cl_platform_id *, cl_uint *);
  using clGetPlatformInfoFunc =
      cl_int (*)(cl_platform_id, cl_platform_info, size_t, void *, size_t *);
  using clBuildProgramFunc = cl_int (*)(cl_program,
                                        cl_uint,
                                        const cl_device_id *,
                                        const char *,
                                        void (*pfn_notify)(cl_program, void *),
                                        void *);
  using clEnqueueNDRangeKernelFunc = cl_int (*)(cl_command_queue,
                                                cl_kernel,
                                                cl_uint,
                                                const size_t *,
                                                const size_t *,
                                                const size_t *,
                                                cl_uint,
                                                const cl_event *,
                                                cl_event *);
  using clSetKernelArgFunc = cl_int (*)(cl_kernel,
                                        cl_uint,
                                        size_t,
                                        const void *);
  using clRetainMemObjectFunc = cl_int (*)(cl_mem);
  using clReleaseMemObjectFunc = cl_int (*)(cl_mem);
  using clEnqueueUnmapMemObjectFunc = cl_int (*)(
      cl_command_queue, cl_mem, void *, cl_uint, const cl_event *, cl_event *);
  using clRetainCommandQueueFunc = cl_int (*)(cl_command_queue command_queue);
  using clCreateContextFunc =
      cl_context (*)(const cl_context_properties *,
                     cl_uint,
                     const cl_device_id *,
                     void(CL_CALLBACK *)(  // NOLINT(readability/casting)
                         const char *, const void *, size_t, void *),
                     void *,
                     cl_int *);
  using clCreateContextFromTypeFunc =
      cl_context (*)(const cl_context_properties *,
                     cl_device_type,
                     void(CL_CALLBACK *)(  // NOLINT(readability/casting)
                         const char *, const void *, size_t, void *),
                     void *,
                     cl_int *);
  using clReleaseContextFunc = cl_int (*)(cl_context);
  using clWaitForEventsFunc = cl_int (*)(cl_uint, const cl_event *);
  using clReleaseEventFunc = cl_int (*)(cl_event);
  using clEnqueueWriteBufferFunc = cl_int (*)(cl_command_queue,
                                              cl_mem,
                                              cl_bool,
                                              size_t,
                                              size_t,
                                              const void *,
                                              cl_uint,
                                              const cl_event *,
                                              cl_event *);
  using clEnqueueReadBufferFunc = cl_int (*)(cl_command_queue,
                                             cl_mem,
                                             cl_bool,
                                             size_t,
                                             size_t,
                                             void *,
                                             cl_uint,
                                             const cl_event *,
                                             cl_event *);
  using clGetProgramBuildInfoFunc = cl_int (*)(cl_program,
                                               cl_device_id,
                                               cl_program_build_info,
                                               size_t,
                                               void *,
                                               size_t *);
  using clRetainProgramFunc = cl_int (*)(cl_program program);
  using clEnqueueMapBufferFunc = void *(*)(cl_command_queue,
                                           cl_mem,
                                           cl_bool,
                                           cl_map_flags,
                                           size_t,
                                           size_t,
                                           cl_uint,
                                           const cl_event *,
                                           cl_event *,
                                           cl_int *);
  using clEnqueueMapImageFunc = void *(*)(cl_command_queue,
                                          cl_mem,
                                          cl_bool,
                                          cl_map_flags,
                                          const size_t *,
                                          const size_t *,
                                          size_t *,
                                          size_t *,
                                          cl_uint,
                                          const cl_event *,
                                          cl_event *,
                                          cl_int *);
  using clCreateCommandQueueFunc = cl_command_queue(CL_API_CALL *)(  // NOLINT
      cl_context, cl_device_id, cl_command_queue_properties, cl_int *);
#if CL_HPP_TARGET_OPENCL_VERSION > 120
  using clCreateCommandQueueWithPropertiesFunc = cl_command_queue (*)(
      cl_context, cl_device_id, const cl_queue_properties *, cl_int *);
#endif
  using clReleaseCommandQueueFunc = cl_int (*)(cl_command_queue);
  using clCreateProgramWithBinaryFunc = cl_program (*)(cl_context,
                                                       cl_uint,
                                                       const cl_device_id *,
                                                       const size_t *,
                                                       const unsigned char **,
                                                       cl_int *,
                                                       cl_int *);
  using clRetainContextFunc = cl_int (*)(cl_context context);
  using clGetContextInfoFunc =
      cl_int (*)(cl_context, cl_context_info, size_t, void *, size_t *);
  using clReleaseProgramFunc = cl_int (*)(cl_program program);
  using clFlushFunc = cl_int (*)(cl_command_queue command_queue);
  using clFinishFunc = cl_int (*)(cl_command_queue command_queue);
  using clGetProgramInfoFunc =
      cl_int (*)(cl_program, cl_program_info, size_t, void *, size_t *);
  using clCreateKernelFunc = cl_kernel (*)(cl_program, const char *, cl_int *);
  using clRetainKernelFunc = cl_int (*)(cl_kernel kernel);
  using clCreateBufferFunc =
      cl_mem (*)(cl_context, cl_mem_flags, size_t, void *, cl_int *);
  using clCreateImage2DFunc = cl_mem(CL_API_CALL *)(cl_context,  // NOLINT
                                                    cl_mem_flags,
                                                    const cl_image_format *,
                                                    size_t,
                                                    size_t,
                                                    size_t,
                                                    void *,
                                                    cl_int *);
#if CL_HPP_TARGET_OPENCL_VERSION > 110
  using clCreateImageFunc = cl_mem (*)(cl_context,
                                       cl_mem_flags,
                                       const cl_image_format *,
                                       const cl_image_desc *,
                                       void *,
                                       cl_int *);
#endif
  using clCreateProgramWithSourceFunc = cl_program (*)(
      cl_context, cl_uint, const char **, const size_t *, cl_int *);
  using clReleaseKernelFunc = cl_int (*)(cl_kernel kernel);
  using clGetDeviceInfoFunc =
      cl_int (*)(cl_device_id, cl_device_info, size_t, void *, size_t *);
  using clGetDeviceIDsFunc = cl_int (*)(
      cl_platform_id, cl_device_type, cl_uint, cl_device_id *, cl_uint *);
  using clRetainDeviceFunc = cl_int (*)(cl_device_id);
  using clReleaseDeviceFunc = cl_int (*)(cl_device_id);
  using clRetainEventFunc = cl_int (*)(cl_event);
  using clGetKernelWorkGroupInfoFunc = cl_int (*)(cl_kernel,
                                                  cl_device_id,
                                                  cl_kernel_work_group_info,
                                                  size_t,
                                                  void *,
                                                  size_t *);
  using clGetEventInfoFunc = cl_int (*)(cl_event event,
                                        cl_event_info param_name,
                                        size_t param_value_size,
                                        void *param_value,
                                        size_t *param_value_size_ret);
  using clGetEventProfilingInfoFunc = cl_int (*)(cl_event event,
                                                 cl_profiling_info param_name,
                                                 size_t param_value_size,
                                                 void *param_value,
                                                 size_t *param_value_size_ret);
  using clGetImageInfoFunc =
      cl_int (*)(cl_mem, cl_image_info, size_t, void *, size_t *);

#define MACE_CL_DEFINE_FUNC_PTR(func) func##Func func = nullptr

  MACE_CL_DEFINE_FUNC_PTR(clGetPlatformIDs);
  MACE_CL_DEFINE_FUNC_PTR(clGetPlatformInfo);
  MACE_CL_DEFINE_FUNC_PTR(clBuildProgram);
  MACE_CL_DEFINE_FUNC_PTR(clEnqueueNDRangeKernel);
  MACE_CL_DEFINE_FUNC_PTR(clSetKernelArg);
  MACE_CL_DEFINE_FUNC_PTR(clReleaseKernel);
  MACE_CL_DEFINE_FUNC_PTR(clCreateProgramWithSource);
  MACE_CL_DEFINE_FUNC_PTR(clCreateBuffer);
#if CL_HPP_TARGET_OPENCL_VERSION > 110
  MACE_CL_DEFINE_FUNC_PTR(clCreateImage);
#endif
  MACE_CL_DEFINE_FUNC_PTR(clCreateImage2D);
  MACE_CL_DEFINE_FUNC_PTR(clRetainKernel);
  MACE_CL_DEFINE_FUNC_PTR(clCreateKernel);
  MACE_CL_DEFINE_FUNC_PTR(clGetProgramInfo);
  MACE_CL_DEFINE_FUNC_PTR(clFlush);
  MACE_CL_DEFINE_FUNC_PTR(clFinish);
  MACE_CL_DEFINE_FUNC_PTR(clReleaseProgram);
  MACE_CL_DEFINE_FUNC_PTR(clRetainContext);
  MACE_CL_DEFINE_FUNC_PTR(clGetContextInfo);
  MACE_CL_DEFINE_FUNC_PTR(clCreateProgramWithBinary);
  MACE_CL_DEFINE_FUNC_PTR(clCreateCommandQueue);
#if CL_HPP_TARGET_OPENCL_VERSION > 120
  MACE_CL_DEFINE_FUNC_PTR(clCreateCommandQueueWithProperties);
#endif
  MACE_CL_DEFINE_FUNC_PTR(clReleaseCommandQueue);
  MACE_CL_DEFINE_FUNC_PTR(clEnqueueMapBuffer);
  MACE_CL_DEFINE_FUNC_PTR(clEnqueueMapImage);
  MACE_CL_DEFINE_FUNC_PTR(clRetainProgram);
  MACE_CL_DEFINE_FUNC_PTR(clGetProgramBuildInfo);
  MACE_CL_DEFINE_FUNC_PTR(clEnqueueReadBuffer);
  MACE_CL_DEFINE_FUNC_PTR(clEnqueueWriteBuffer);
  MACE_CL_DEFINE_FUNC_PTR(clWaitForEvents);
  MACE_CL_DEFINE_FUNC_PTR(clReleaseEvent);
  MACE_CL_DEFINE_FUNC_PTR(clCreateContext);
  MACE_CL_DEFINE_FUNC_PTR(clCreateContextFromType);
  MACE_CL_DEFINE_FUNC_PTR(clReleaseContext);
  MACE_CL_DEFINE_FUNC_PTR(clRetainCommandQueue);
  MACE_CL_DEFINE_FUNC_PTR(clEnqueueUnmapMemObject);
  MACE_CL_DEFINE_FUNC_PTR(clRetainMemObject);
  MACE_CL_DEFINE_FUNC_PTR(clReleaseMemObject);
  MACE_CL_DEFINE_FUNC_PTR(clGetDeviceInfo);
  MACE_CL_DEFINE_FUNC_PTR(clGetDeviceIDs);
  MACE_CL_DEFINE_FUNC_PTR(clRetainDevice);
  MACE_CL_DEFINE_FUNC_PTR(clReleaseDevice);
  MACE_CL_DEFINE_FUNC_PTR(clRetainEvent);
  MACE_CL_DEFINE_FUNC_PTR(clGetKernelWorkGroupInfo);
  MACE_CL_DEFINE_FUNC_PTR(clGetEventInfo);
  MACE_CL_DEFINE_FUNC_PTR(clGetEventProfilingInfo);
  MACE_CL_DEFINE_FUNC_PTR(clGetImageInfo);

#undef MACE_CL_DEFINE_FUNC_PTR

 private:
  void *LoadFromPath(const std::string &path);
  void *handle_ = nullptr;
};

bool OpenCLLibraryImpl::Load() {
  if (handle_ != nullptr) {
    return true;
  }

  const std::vector<std::string> paths = {
    "libOpenCL.so",
#if defined(__aarch64__)
    // Qualcomm Adreno
    "/system/vendor/lib64/libOpenCL.so",
    "/system/lib64/libOpenCL.so",
    // Mali
    "/system/vendor/lib64/egl/libGLES_mali.so",
    "/system/lib64/egl/libGLES_mali.so",
#else
    // Qualcomm Adreno
    "/system/vendor/lib/libOpenCL.so",
    "/system/lib/libOpenCL.so",
    // Mali
    "/system/vendor/lib/egl/libGLES_mali.so",
    "/system/lib/egl/libGLES_mali.so",
#endif
  };

  for (const auto &path : paths) {
    VLOG(2) << "Loading OpenCL from " << path;
    void *handle = LoadFromPath(path);
    if (handle != nullptr) {
      handle_ = handle;
      break;
    }
  }

  if (handle_ == nullptr) {
    LOG(ERROR) << "Failed to load OpenCL library, "
        "please make sure there exist OpenCL library on your device, "
        "and your APP have right to access the library.";
    return false;
  }

  return true;
}

void OpenCLLibraryImpl::Unload() {
  if (handle_ != nullptr) {
    if (dlclose(handle_) != 0) {
      LOG(ERROR) << "dlclose failed for OpenCL library";
    }
    handle_ = nullptr;
  }
}

void *OpenCLLibraryImpl::LoadFromPath(const std::string &path) {
  void *handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);

  if (handle == nullptr) {
    VLOG(2) << "Failed to load OpenCL library from path " << path
            << " error code: " << dlerror();
    return nullptr;
  }

#define MACE_CL_ASSIGN_FROM_DLSYM(func)                          \
  do {                                                           \
    void *ptr = dlsym(handle, #func);                            \
    if (ptr == nullptr) {                                        \
      VLOG(1) << "Failed to load " << #func << " from " << path; \
      continue;                                                  \
    }                                                            \
    func = reinterpret_cast<func##Func>(ptr);                    \
    VLOG(2) << "Loaded " << #func << " from " << path;           \
  } while (false)

  MACE_CL_ASSIGN_FROM_DLSYM(clGetPlatformIDs);
  MACE_CL_ASSIGN_FROM_DLSYM(clGetPlatformInfo);
  MACE_CL_ASSIGN_FROM_DLSYM(clBuildProgram);
  MACE_CL_ASSIGN_FROM_DLSYM(clEnqueueNDRangeKernel);
  MACE_CL_ASSIGN_FROM_DLSYM(clSetKernelArg);
  MACE_CL_ASSIGN_FROM_DLSYM(clReleaseKernel);
  MACE_CL_ASSIGN_FROM_DLSYM(clCreateProgramWithSource);
  MACE_CL_ASSIGN_FROM_DLSYM(clCreateBuffer);
#if CL_HPP_TARGET_OPENCL_VERSION > 110
  MACE_CL_ASSIGN_FROM_DLSYM(clCreateImage);
#endif
  MACE_CL_ASSIGN_FROM_DLSYM(clCreateImage2D);
  MACE_CL_ASSIGN_FROM_DLSYM(clRetainKernel);
  MACE_CL_ASSIGN_FROM_DLSYM(clCreateKernel);
  MACE_CL_ASSIGN_FROM_DLSYM(clGetProgramInfo);
  MACE_CL_ASSIGN_FROM_DLSYM(clFlush);
  MACE_CL_ASSIGN_FROM_DLSYM(clFinish);
  MACE_CL_ASSIGN_FROM_DLSYM(clReleaseProgram);
  MACE_CL_ASSIGN_FROM_DLSYM(clRetainContext);
  MACE_CL_ASSIGN_FROM_DLSYM(clGetContextInfo);
  MACE_CL_ASSIGN_FROM_DLSYM(clCreateProgramWithBinary);
  MACE_CL_ASSIGN_FROM_DLSYM(clCreateCommandQueue);
#if CL_HPP_TARGET_OPENCL_VERSION > 120
  MACE_CL_ASSIGN_FROM_DLSYM(clCreateCommandQueueWithProperties);
#endif
  MACE_CL_ASSIGN_FROM_DLSYM(clReleaseCommandQueue);
  MACE_CL_ASSIGN_FROM_DLSYM(clEnqueueMapBuffer);
  MACE_CL_ASSIGN_FROM_DLSYM(clEnqueueMapImage);
  MACE_CL_ASSIGN_FROM_DLSYM(clRetainProgram);
  MACE_CL_ASSIGN_FROM_DLSYM(clGetProgramBuildInfo);
  MACE_CL_ASSIGN_FROM_DLSYM(clEnqueueReadBuffer);
  MACE_CL_ASSIGN_FROM_DLSYM(clEnqueueWriteBuffer);
  MACE_CL_ASSIGN_FROM_DLSYM(clWaitForEvents);
  MACE_CL_ASSIGN_FROM_DLSYM(clReleaseEvent);
  MACE_CL_ASSIGN_FROM_DLSYM(clCreateContext);
  MACE_CL_ASSIGN_FROM_DLSYM(clCreateContextFromType);
  MACE_CL_ASSIGN_FROM_DLSYM(clReleaseContext);
  MACE_CL_ASSIGN_FROM_DLSYM(clRetainCommandQueue);
  MACE_CL_ASSIGN_FROM_DLSYM(clEnqueueUnmapMemObject);
  MACE_CL_ASSIGN_FROM_DLSYM(clRetainMemObject);
  MACE_CL_ASSIGN_FROM_DLSYM(clReleaseMemObject);
  MACE_CL_ASSIGN_FROM_DLSYM(clGetDeviceInfo);
  MACE_CL_ASSIGN_FROM_DLSYM(clGetDeviceIDs);
  MACE_CL_ASSIGN_FROM_DLSYM(clRetainDevice);
  MACE_CL_ASSIGN_FROM_DLSYM(clReleaseDevice);
  MACE_CL_ASSIGN_FROM_DLSYM(clRetainEvent);
  MACE_CL_ASSIGN_FROM_DLSYM(clGetKernelWorkGroupInfo);
  MACE_CL_ASSIGN_FROM_DLSYM(clGetEventInfo);
  MACE_CL_ASSIGN_FROM_DLSYM(clGetEventProfilingInfo);
  MACE_CL_ASSIGN_FROM_DLSYM(clGetImageInfo);

#undef MACE_CL_ASSIGN_FROM_DLSYM

  return handle;
}

OpenCLLibraryImpl *openclLibraryImpl = nullptr;
}  // namespace

void LoadOpenCLLibrary() {
  MACE_CHECK(openclLibraryImpl == nullptr);
  openclLibraryImpl = new OpenCLLibraryImpl();
  MACE_CHECK(openclLibraryImpl->Load());
}

void UnloadOpenCLLibrary() {
  MACE_CHECK_NOTNULL(openclLibraryImpl);
  openclLibraryImpl->Unload();
  delete openclLibraryImpl;
  openclLibraryImpl = nullptr;
}

}  // namespace mace

// Platform APIs
CL_API_ENTRY cl_int clGetPlatformIDs(cl_uint num_entries,
                                     cl_platform_id *platforms,
                                     cl_uint *num_platforms)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clGetPlatformIDs;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clGetPlatformIDs");
  return func(num_entries, platforms, num_platforms);
}

CL_API_ENTRY cl_int clGetPlatformInfo(cl_platform_id platform,
                                      cl_platform_info param_name,
                                      size_t param_value_size,
                                      void *param_value,
                                      size_t *param_value_size_ret)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clGetPlatformInfo;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clGetPlatformInfo");
  return func(platform, param_name, param_value_size, param_value,
              param_value_size_ret);
}

// Device APIs
CL_API_ENTRY cl_int clGetDeviceIDs(cl_platform_id platform,
                                   cl_device_type device_type,
                                   cl_uint num_entries,
                                   cl_device_id *devices,
                                   cl_uint *num_devices)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clGetDeviceIDs;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clGetDeviceIDs");
  return func(platform, device_type, num_entries, devices, num_devices);
}

CL_API_ENTRY cl_int clGetDeviceInfo(cl_device_id device,
                                    cl_device_info param_name,
                                    size_t param_value_size,
                                    void *param_value,
                                    size_t *param_value_size_ret)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clGetDeviceInfo;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clGetDeviceInfo");
  return func(device, param_name, param_value_size, param_value,
              param_value_size_ret);
}

#if CL_HPP_TARGET_OPENCL_VERSION > 110

CL_API_ENTRY cl_int clRetainDevice(cl_device_id device)
    CL_API_SUFFIX__VERSION_1_2 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clRetainDevice;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clRetainDevice");
  return func(device);
}

CL_API_ENTRY cl_int clReleaseDevice(cl_device_id device)
    CL_API_SUFFIX__VERSION_1_2 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clReleaseDevice;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clReleaseDevice");
  return func(device);
}

#endif

// Context APIs
CL_API_ENTRY cl_context clCreateContext(
    const cl_context_properties *properties,
    cl_uint num_devices,
    const cl_device_id *devices,
    void(CL_CALLBACK *pfn_notify)(const char *, const void *, size_t, void *),
    void *user_data,
    cl_int *errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clCreateContext;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clCreateContext");
  return func(properties, num_devices, devices, pfn_notify, user_data,
              errcode_ret);
}

CL_API_ENTRY cl_context clCreateContextFromType(
    const cl_context_properties *properties,
    cl_device_type device_type,
    void(CL_CALLBACK *pfn_notify)(const char *, const void *, size_t, void *),
    void *user_data,
    cl_int *errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clCreateContextFromType;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clCreateContextFromType");
  return func(properties, device_type, pfn_notify, user_data, errcode_ret);
}

CL_API_ENTRY cl_int clRetainContext(cl_context context)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clRetainContext;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clRetainContext");
  return func(context);
}

CL_API_ENTRY cl_int clReleaseContext(cl_context context)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clReleaseContext;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clReleaseContext");
  return func(context);
}

CL_API_ENTRY cl_int clGetContextInfo(cl_context context,
                                     cl_context_info param_name,
                                     size_t param_value_size,
                                     void *param_value,
                                     size_t *param_value_size_ret)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clGetContextInfo;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clGetContextInfo");
  return func(context, param_name, param_value_size, param_value,
              param_value_size_ret);
}

// Program Object APIs
CL_API_ENTRY cl_program clCreateProgramWithSource(cl_context context,
                                                  cl_uint count,
                                                  const char **strings,
                                                  const size_t *lengths,
                                                  cl_int *errcode_ret)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clCreateProgramWithSource;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clCreateProgramWithSource");
  return func(context, count, strings, lengths, errcode_ret);
}

CL_API_ENTRY cl_program
clCreateProgramWithBinary(cl_context context,
                          cl_uint num_devices,
                          const cl_device_id *device_list,
                          const size_t *lengths,
                          const unsigned char **binaries,
                          cl_int *binary_status,
                          cl_int *errcode_ret) CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clCreateProgramWithBinary;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clCreateProgramWithBinary");
  return func(context, num_devices, device_list, lengths, binaries,
              binary_status, errcode_ret);
}

CL_API_ENTRY cl_int clGetProgramInfo(cl_program program,
                                     cl_program_info param_name,
                                     size_t param_value_size,
                                     void *param_value,
                                     size_t *param_value_size_ret)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clGetProgramInfo;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clGetProgramInfo");
  return func(program, param_name, param_value_size, param_value,
              param_value_size_ret);
}

CL_API_ENTRY cl_int clGetProgramBuildInfo(cl_program program,
                                          cl_device_id device,
                                          cl_program_build_info param_name,
                                          size_t param_value_size,
                                          void *param_value,
                                          size_t *param_value_size_ret)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clGetProgramBuildInfo;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clGetProgramBuildInfo");
  return func(program, device, param_name, param_value_size, param_value,
              param_value_size_ret);
}

CL_API_ENTRY cl_int clRetainProgram(cl_program program)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clRetainProgram;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clRetainProgram");
  return func(program);
}

CL_API_ENTRY cl_int clReleaseProgram(cl_program program)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clReleaseProgram;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clReleaseProgram");
  return func(program);
}

CL_API_ENTRY cl_int clBuildProgram(
    cl_program program,
    cl_uint num_devices,
    const cl_device_id *device_list,
    const char *options,
    void(CL_CALLBACK *pfn_notify)(cl_program program, void *user_data),
    void *user_data) CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clBuildProgram;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clBuildProgram");
  return func(program, num_devices, device_list, options, pfn_notify,
              user_data);
}

// Kernel Object APIs
CL_API_ENTRY cl_kernel clCreateKernel(cl_program program,
                                      const char *kernel_name,
                                      cl_int *errcode_ret)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clCreateKernel;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clCreateKernel");
  return func(program, kernel_name, errcode_ret);
}

CL_API_ENTRY cl_int clRetainKernel(cl_kernel kernel)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clRetainKernel;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clRetainKernel");
  return func(kernel);
}

CL_API_ENTRY cl_int clReleaseKernel(cl_kernel kernel)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clReleaseKernel;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clReleaseKernel");
  return func(kernel);
}

CL_API_ENTRY cl_int clSetKernelArg(cl_kernel kernel,
                                   cl_uint arg_index,
                                   size_t arg_size,
                                   const void *arg_value)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clSetKernelArg;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clSetKernelArg");
  return func(kernel, arg_index, arg_size, arg_value);
}

// Memory Object APIs
CL_API_ENTRY cl_mem clCreateBuffer(cl_context context,
                                   cl_mem_flags flags,
                                   size_t size,
                                   void *host_ptr,
                                   cl_int *errcode_ret)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clCreateBuffer;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clCreateBuffer");
  return func(context, flags, size, host_ptr, errcode_ret);
}

#if CL_HPP_TARGET_OPENCL_VERSION > 110
CL_API_ENTRY cl_mem clCreateImage(cl_context context,
                                  cl_mem_flags flags,
                                  const cl_image_format *image_format,
                                  const cl_image_desc *image_desc,
                                  void *host_ptr,
                                  cl_int *errcode_ret)
    CL_API_SUFFIX__VERSION_1_2 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clCreateImage;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clCreateImage");
  return func(context, flags, image_format, image_desc, host_ptr, errcode_ret);
}
#endif

CL_API_ENTRY cl_int clRetainMemObject(cl_mem memobj)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clRetainMemObject;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clRetainMemObject");
  return func(memobj);
}

CL_API_ENTRY cl_int clReleaseMemObject(cl_mem memobj)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clReleaseMemObject;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clReleaseMemObject");
  return func(memobj);
}

CL_API_ENTRY cl_int clGetImageInfo(cl_mem image,
                                   cl_image_info param_name,
                                   size_t param_value_size,
                                   void *param_value,
                                   size_t *param_value_size_ret)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clGetImageInfo;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clGetImageInfo");
  return func(image, param_name, param_value_size, param_value,
              param_value_size_ret);
}

#if CL_HPP_TARGET_OPENCL_VERSION > 120
// Command Queue APIs
CL_API_ENTRY cl_command_queue clCreateCommandQueueWithProperties(
    cl_context context,
    cl_device_id device,
    const cl_queue_properties *properties,
    cl_int *errcode_ret) CL_API_SUFFIX__VERSION_2_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clCreateCommandQueueWithProperties;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clCreateCommandQueueWithProperties");
  return func(context, device, properties, errcode_ret);
}
#endif

CL_API_ENTRY cl_int clRetainCommandQueue(cl_command_queue command_queue)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clRetainCommandQueue;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clRetainCommandQueue");
  return func(command_queue);
}

CL_API_ENTRY cl_int clReleaseCommandQueue(cl_command_queue command_queue)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clReleaseCommandQueue;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clReleaseCommandQueue");
  return func(command_queue);
}

// Enqueued Commands APIs
CL_API_ENTRY cl_int clEnqueueReadBuffer(cl_command_queue command_queue,
                                        cl_mem buffer,
                                        cl_bool blocking_read,
                                        size_t offset,
                                        size_t size,
                                        void *ptr,
                                        cl_uint num_events_in_wait_list,
                                        const cl_event *event_wait_list,
                                        cl_event *event)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clEnqueueReadBuffer;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clEnqueueReadBuffer");
  return func(command_queue, buffer, blocking_read, offset, size, ptr,
              num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY cl_int clEnqueueWriteBuffer(cl_command_queue command_queue,
                                         cl_mem buffer,
                                         cl_bool blocking_write,
                                         size_t offset,
                                         size_t size,
                                         const void *ptr,
                                         cl_uint num_events_in_wait_list,
                                         const cl_event *event_wait_list,
                                         cl_event *event)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clEnqueueWriteBuffer;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clEnqueueWriteBuffer");
  return func(command_queue, buffer, blocking_write, offset, size, ptr,
              num_events_in_wait_list, event_wait_list, event);
}

CL_API_ENTRY void *clEnqueueMapBuffer(cl_command_queue command_queue,
                                      cl_mem buffer,
                                      cl_bool blocking_map,
                                      cl_map_flags map_flags,
                                      size_t offset,
                                      size_t size,
                                      cl_uint num_events_in_wait_list,
                                      const cl_event *event_wait_list,
                                      cl_event *event,
                                      cl_int *errcode_ret)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clEnqueueMapBuffer;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clEnqueueMapBuffer");
  return func(command_queue, buffer, blocking_map, map_flags, offset, size,
              num_events_in_wait_list, event_wait_list, event, errcode_ret);
}

CL_API_ENTRY void *clEnqueueMapImage(cl_command_queue command_queue,
                                     cl_mem image,
                                     cl_bool blocking_map,
                                     cl_map_flags map_flags,
                                     const size_t *origin,
                                     const size_t *region,
                                     size_t *image_row_pitch,
                                     size_t *image_slice_pitch,
                                     cl_uint num_events_in_wait_list,
                                     const cl_event *event_wait_list,
                                     cl_event *event,
                                     cl_int *errcode_ret)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clEnqueueMapImage;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clEnqueueMapImage");
  return func(command_queue, image, blocking_map, map_flags, origin, region,
              image_row_pitch, image_slice_pitch, num_events_in_wait_list,
              event_wait_list, event, errcode_ret);
}

CL_API_ENTRY cl_int clEnqueueUnmapMemObject(cl_command_queue command_queue,
                                            cl_mem memobj,
                                            void *mapped_ptr,
                                            cl_uint num_events_in_wait_list,
                                            const cl_event *event_wait_list,
                                            cl_event *event)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clEnqueueUnmapMemObject;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clEnqueueUnmapMemObject");
  return func(command_queue, memobj, mapped_ptr, num_events_in_wait_list,
              event_wait_list, event);
}

CL_API_ENTRY cl_int clGetKernelWorkGroupInfo(
    cl_kernel kernel,
    cl_device_id device,
    cl_kernel_work_group_info param_name,
    size_t param_value_size,
    void *param_value,
    size_t *param_value_size_ret) CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clGetKernelWorkGroupInfo;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clGetKernelWorkGroupInfo");
  return func(kernel, device, param_name, param_value_size, param_value,
              param_value_size_ret);
}

CL_API_ENTRY cl_int clEnqueueNDRangeKernel(cl_command_queue command_queue,
                                           cl_kernel kernel,
                                           cl_uint work_dim,
                                           const size_t *global_work_offset,
                                           const size_t *global_work_size,
                                           const size_t *local_work_size,
                                           cl_uint num_events_in_wait_list,
                                           const cl_event *event_wait_list,
                                           cl_event *event)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clEnqueueNDRangeKernel;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clEnqueueNDRangeKernel");
  return func(command_queue, kernel, work_dim, global_work_offset,
              global_work_size, local_work_size, num_events_in_wait_list,
              event_wait_list, event);
}

// Event Object APIs
CL_API_ENTRY cl_int clWaitForEvents(
    cl_uint num_events, const cl_event *event_list) CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clWaitForEvents;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clWaitForEvents");
  return func(num_events, event_list);
}

CL_API_ENTRY cl_int clRetainEvent(cl_event event) CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clRetainEvent;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clRetainEvent");
  return func(event);
}

CL_API_ENTRY cl_int clReleaseEvent(cl_event event) CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clReleaseEvent;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clReleaseEvent");
  return func(event);
}

// Event API
CL_API_ENTRY cl_int clGetEventInfo(cl_event event,
                                   cl_event_info param_name,
                                   size_t param_value_size,
                                   void *param_value,
                                   size_t *param_value_size_ret)
CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clGetEventInfo;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clGetEventInfo");
  return func(event, param_name, param_value_size, param_value,
              param_value_size_ret);
}

// Profiling APIs
CL_API_ENTRY cl_int clGetEventProfilingInfo(cl_event event,
                                            cl_profiling_info param_name,
                                            size_t param_value_size,
                                            void *param_value,
                                            size_t *param_value_size_ret)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clGetEventProfilingInfo;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clGetEventProfilingInfo");
  return func(event, param_name, param_value_size, param_value,
              param_value_size_ret);
}

// Flush and Finish APIs
CL_API_ENTRY cl_int clFlush(cl_command_queue command_queue)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clFlush;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clFlush");
  return func(command_queue);
}

CL_API_ENTRY cl_int clFinish(cl_command_queue command_queue)
    CL_API_SUFFIX__VERSION_1_0 {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clFinish;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clFinish");
  return func(command_queue);
}

// Deprecated OpenCL 1.1 APIs
CL_API_ENTRY /* CL_EXT_PREFIX__VERSION_1_1_DEPRECATED */ cl_mem clCreateImage2D(
    cl_context context,
    cl_mem_flags flags,
    const cl_image_format *image_format,
    size_t image_width,
    size_t image_height,
    size_t image_row_pitch,
    void *host_ptr,
    cl_int *errcode_ret) /* CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED */ {
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clCreateImage2D;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clCreateImage2D");
  return func(context, flags, image_format, image_width, image_height,
              image_row_pitch, host_ptr, errcode_ret);
}

// Deprecated OpenCL 2.0 APIs
CL_API_ENTRY /*CL_EXT_PREFIX__VERSION_1_2_DEPRECATED*/ cl_command_queue
clCreateCommandQueue(cl_context context,
                     cl_device_id device,
                     cl_command_queue_properties properties,
                     cl_int *errcode_ret)
/* CL_EXT_SUFFIX__VERSION_1_2_DEPRECATED */ {  // NOLINT
  MACE_CHECK_NOTNULL(mace::openclLibraryImpl);
  auto func = mace::openclLibraryImpl->clCreateCommandQueue;
  MACE_CHECK_NOTNULL(func);
  MACE_LATENCY_LOGGER(3, "clCreateCommandQueue");
  return func(context, device, properties, errcode_ret);
}

#endif

#endif