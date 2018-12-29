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

#include "gmock/gmock.h"
#include "mace/core/operator.h"
#include "mace/ops/ops_test_util.h"

namespace mace {
namespace ops {
namespace test {

class CastOpTest : public OpsTestBase {};

namespace {
template <typename SrcType, typename DstType>
void TestCast(const std::vector<index_t> &input_shape,
              const std::vector<SrcType> &input) {
  // Construct graph
  OpsTestNet net;
  OpDefBuilder("Cast", "CastTest")
      .Input("Input")
      .OutputType({DataTypeToEnum<DstType>::v()})
      .Output("Output")
      .AddIntArg("T", DataTypeToEnum<SrcType>::v())
      .Finalize(net.NewOperatorDef());

  // Add input data
  net.AddInputFromArray<DeviceType::CPU, SrcType>("Input", input_shape, input);

  // Run
  net.RunOp();

  auto input_tensor = net.GetTensor("Input");
  auto output_tensor = net.GetTensor("Output");

  EXPECT_THAT(output_tensor->shape(), ::testing::ContainerEq(input_shape));

  const int size = output_tensor->size();
  for (int i = 0; i < size; ++i) {
    Expector<SrcType, DstType, true>::Near(
        *input_tensor, *output_tensor, 1e-5, 1.f);
  }
}
}  // namespace

TEST_F(CastOpTest, TestCastFromFloatToInt32) {
  TestCast<float, int32_t>({1, 2, 3}, {1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f});
  TestCast<float, int32_t>({}, {3.3f});
}

TEST_F(CastOpTest, TestCastFromInt32ToFloat) {
  TestCast<int32_t, float>({1, 2, 3}, {1, 2, 3, 4, 5, 6});
  TestCast<int32_t, float>({}, {3});
}

}  // namespace test
}  // namespace ops
}  // namespace mace
