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

#ifdef MACE_ENABLE_OPENCL

#include "mace/kernels/resize_bilinear.h"
#include "mace/core/runtime/opencl/opencl_runtime.h"
#include "mace/core/tensor.h"
#include "mace/kernels/opencl/helper.h"
#include "mace/utils/tuner.h"
#include "mace/utils/utils.h"

namespace mace {
namespace kernels {

namespace {
std::vector<uint32_t> LocalWS(const uint32_t *gws, const uint32_t kwg_size) {
  std::vector<uint32_t> lws(4, 0);
  uint64_t cache_size = OpenCLRuntime::Global()->device_global_mem_cache_size();
  uint32_t base = std::max<uint32_t>(cache_size / kBaseGPUMemCacheSize, 1);
  lws[1] = std::min<uint32_t>(gws[1], kwg_size);
  if (lws[1] >= base) {
    lws[0] = std::min<uint32_t>(gws[0], base);
  } else {
    lws[0] = gws[0] / 8;
    if (lws[0] == 0) {
      lws[0] = gws[0];
    }
  }
  lws[0] = std::min<uint32_t>(lws[0], kwg_size / lws[1]);
  const uint32_t lws_size = lws[0] * lws[1];
  lws[2] = gws[2] / 8;
  if (lws[2] == 0) {
    lws[2] = gws[2];
  }
  lws[2] = std::max<uint32_t>(std::min<uint32_t>(lws[2], kwg_size / lws_size),
                              1);
  return lws;
}

}  // namespace

template <typename T>
MaceStatus ResizeBilinearFunctor<DeviceType::GPU, T>::operator()(
    const Tensor *input, Tensor *output, StatsFuture *future) {
  const index_t batch = input->dim(0);
  const index_t in_height = input->dim(1);
  const index_t in_width = input->dim(2);
  const index_t channels = input->dim(3);

  const index_t channel_blocks = RoundUpDiv4(channels);
  const index_t out_height = out_height_;
  const index_t out_width = out_width_;

  const uint32_t gws[3] = {static_cast<uint32_t>(channel_blocks),
                           static_cast<uint32_t>(out_width),
                           static_cast<uint32_t>(out_height * batch)};

  auto runtime = OpenCLRuntime::Global();

  if (kernel_.get() == nullptr) {
    std::set<std::string> built_options;
    std::string kernel_name = MACE_OBFUSCATE_SYMBOL("resize_bilinear_nocache");
    built_options.emplace("-Dresize_bilinear_nocache=" + kernel_name);
    auto dt = DataTypeToEnum<T>::value;
    built_options.emplace("-DDATA_TYPE=" + DtToUpstreamCLDt(dt));
    built_options.emplace("-DCMD_DATA_TYPE=" + DtToUpstreamCLCMDDt(dt));
    if (runtime->IsOutOfRangeCheckEnabled()) {
      built_options.emplace("-DOUT_OF_RANGE_CHECK");
      kernel_error_ = std::move(std::unique_ptr<Buffer>(
          new Buffer(GetDeviceAllocator(DeviceType::GPU))));
      MACE_RETURN_IF_ERROR(kernel_error_->Allocate(1));
      kernel_error_->Map(nullptr);
      *(kernel_error_->mutable_data<char>()) = 0;
      kernel_error_->UnMap();
    }
    if (runtime->IsNonUniformWorkgroupsSupported()) {
      built_options.emplace("-DNON_UNIFORM_WORK_GROUP");
    }
    kernel_ =
        runtime->BuildKernel("resize_bilinear", kernel_name, built_options);

    kwg_size_ =
        static_cast<uint32_t>(runtime->GetKernelMaxWorkGroupSize(kernel_));
  }
  if (!IsVecEqual(input_shape_, input->shape())) {
    MACE_CHECK(out_height > 0 && out_width > 0);
    std::vector<index_t> output_shape{batch, out_height, out_width, channels};

    std::vector<size_t> output_image_shape;
    CalImage2DShape(output_shape, BufferType::IN_OUT_CHANNEL,
                    &output_image_shape);
    MACE_RETURN_IF_ERROR(output->ResizeImage(output_shape, output_image_shape));

    float height_scale =
        CalculateResizeScale(in_height, out_height, align_corners_);
    float width_scale =
        CalculateResizeScale(in_width, out_width, align_corners_);

    uint32_t idx = 0;
    if (runtime->IsOutOfRangeCheckEnabled()) {
      kernel_.setArg(idx++,
                     *(static_cast<cl::Buffer *>(kernel_error_->buffer())));
    }
    if (!runtime->IsNonUniformWorkgroupsSupported()) {
      kernel_.setArg(idx++, gws[0]);
      kernel_.setArg(idx++, gws[1]);
      kernel_.setArg(idx++, gws[2]);
    }
    kernel_.setArg(idx++, *(input->opencl_image()));
    kernel_.setArg(idx++, *(output->opencl_image()));
    kernel_.setArg(idx++, height_scale);
    kernel_.setArg(idx++, width_scale);
    kernel_.setArg(idx++, static_cast<int32_t>(in_height));
    kernel_.setArg(idx++, static_cast<int32_t>(in_width));
    kernel_.setArg(idx++, static_cast<int32_t>(out_height));

    input_shape_ = input->shape();
  }

  const std::vector<uint32_t> lws = LocalWS(gws, kwg_size_);
  std::string tuning_key =
      Concat("resize_bilinear_opencl_kernel", output->dim(0), output->dim(1),
             output->dim(2), output->dim(3));
  TuningOrRun3DKernel(kernel_, tuning_key, gws, lws, future);

  if (runtime->IsOutOfRangeCheckEnabled()) {
    kernel_error_->Map(nullptr);
    char *kerror_code = kernel_error_->mutable_data<char>();
    MACE_CHECK(*kerror_code == 0) << "Kernel error code: " << *kerror_code;
    kernel_error_->UnMap();
  }

  return MACE_SUCCESS;
}

template struct ResizeBilinearFunctor<DeviceType::GPU, float>;
template struct ResizeBilinearFunctor<DeviceType::GPU, half>;

}  // namespace kernels
}  // namespace mace

#endif