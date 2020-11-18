// Copyright 2020 The Tint Authors.
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

#include <string>

#include "gmock/gmock.h"
#include "src/reader/spirv/function.h"
#include "src/reader/spirv/parser_impl.h"
#include "src/reader/spirv/parser_impl_test_helper.h"
#include "src/reader/spirv/spirv_tools_helpers_test.h"

namespace tint {
namespace reader {
namespace spirv {
namespace {

using ::testing::Eq;
using ::testing::HasSubstr;
using ::testing::Not;

std::string Preamble() {
  return R"(
    OpCapability Shader
    OpCapability Sampled1D
    OpCapability Image1D
    OpCapability StorageImageExtendedFormats
    OpMemoryModel Logical Simple
  )";
}

std::string CommonTypes() {
  return R"(
    %void = OpTypeVoid
    %voidfn = OpTypeFunction %void

    %float = OpTypeFloat 32
    %uint = OpTypeInt 32 0
    %int = OpTypeInt 32 1

    %uint_1 = OpConstant %uint 1
    %uint_2 = OpConstant %uint 2
    %uint_100 = OpConstant %uint 100

    %v2uint = OpTypeVector %uint 2
    %v4uint = OpTypeVector %uint 4
    %v4int = OpTypeVector %int 4
    %v2float = OpTypeVector %float 2
    %v3float = OpTypeVector %float 3
    %v4float = OpTypeVector %float 4

    %float_null = OpConstantNull %float
    %v2float_null = OpConstantNull %v2float
    %v3float_null = OpConstantNull %v3float
    %v4float_null = OpConstantNull %v4float

    %depth = OpConstant %float 0.2

; Define types for all sampler and texture types that can map to WGSL,
; modulo texel formats for storage textures. For now, we limit
; ourselves to 2-channel 32-bit texel formats.

; Because the SPIR-V reader also already generalizes so it can work with
; combined image-samplers, we also test that too.

    %sampler = OpTypeSampler

    ; sampled images
    %f_texture_1d          = OpTypeImage %float 1D   0 0 0 1 Unknown
    %f_texture_1d_array    = OpTypeImage %float 1D   0 1 0 1 Unknown
    %f_texture_2d          = OpTypeImage %float 2D   0 0 0 1 Unknown
    %f_texture_2d_ms       = OpTypeImage %float 2D   0 0 1 1 Unknown
    %f_texture_2d_array    = OpTypeImage %float 2D   0 1 0 1 Unknown
    %f_texture_2d_ms_array = OpTypeImage %float 2D   0 1 1 1 Unknown ; not in WebGPU
    %f_texture_3d          = OpTypeImage %float 3D   0 0 0 1 Unknown
    %f_texture_cube        = OpTypeImage %float Cube 0 0 0 1 Unknown
    %f_texture_cube_array  = OpTypeImage %float Cube 0 1 0 1 Unknown

    ; storage images
    %f_storage_1d         = OpTypeImage %float 1D   0 0 0 1 Rg32f
    %f_storage_1d_array   = OpTypeImage %float 1D   0 1 0 1 Rg32f
    %f_storage_2d         = OpTypeImage %float 2D   0 0 0 1 Rg32f
    %f_storage_2d_array   = OpTypeImage %float 2D   0 1 0 1 Rg32f
    %f_storage_3d         = OpTypeImage %float 3D   0 0 0 1 Rg32f

    ; Now all the same, but for unsigned integer sampled type.

    %u_texture_1d          = OpTypeImage %uint  1D   0 0 0 1 Unknown
    %u_texture_1d_array    = OpTypeImage %uint  1D   0 1 0 1 Unknown
    %u_texture_2d          = OpTypeImage %uint  2D   0 0 0 1 Unknown
    %u_texture_2d_ms       = OpTypeImage %uint  2D   0 0 1 1 Unknown
    %u_texture_2d_array    = OpTypeImage %uint  2D   0 1 0 1 Unknown
    %u_texture_2d_ms_array = OpTypeImage %uint  2D   0 1 1 1 Unknown ; not in WebGPU
    %u_texture_3d          = OpTypeImage %uint  3D   0 0 0 1 Unknown
    %u_texture_cube        = OpTypeImage %uint  Cube 0 0 0 1 Unknown
    %u_texture_cube_array  = OpTypeImage %uint  Cube 0 1 0 1 Unknown

    %u_storage_1d         = OpTypeImage %uint  1D   0 0 0 1 Rg32ui
    %u_storage_1d_array   = OpTypeImage %uint  1D   0 1 0 1 Rg32ui
    %u_storage_2d         = OpTypeImage %uint  2D   0 0 0 1 Rg32ui
    %u_storage_2d_array   = OpTypeImage %uint  2D   0 1 0 1 Rg32ui
    %u_storage_3d         = OpTypeImage %uint  3D   0 0 0 1 Rg32ui

    ; Now all the same, but for signed integer sampled type.

    %i_texture_1d          = OpTypeImage %int  1D   0 0 0 1 Unknown
    %i_texture_1d_array    = OpTypeImage %int  1D   0 1 0 1 Unknown
    %i_texture_2d          = OpTypeImage %int  2D   0 0 0 1 Unknown
    %i_texture_2d_ms       = OpTypeImage %int  2D   0 0 1 1 Unknown
    %i_texture_2d_array    = OpTypeImage %int  2D   0 1 0 1 Unknown
    %i_texture_2d_ms_array = OpTypeImage %int  2D   0 1 1 1 Unknown ; not in WebGPU
    %i_texture_3d          = OpTypeImage %int  3D   0 0 0 1 Unknown
    %i_texture_cube        = OpTypeImage %int  Cube 0 0 0 1 Unknown
    %i_texture_cube_array  = OpTypeImage %int  Cube 0 1 0 1 Unknown

    %i_storage_1d         = OpTypeImage %int  1D   0 0 0 1 Rg32i
    %i_storage_1d_array   = OpTypeImage %int  1D   0 1 0 1 Rg32i
    %i_storage_2d         = OpTypeImage %int  2D   0 0 0 1 Rg32i
    %i_storage_2d_array   = OpTypeImage %int  2D   0 1 0 1 Rg32i
    %i_storage_3d         = OpTypeImage %int  3D   0 0 0 1 Rg32i

    ;; Now pointers to each of the above, so we can declare variables for them.

    %ptr_sampler = OpTypePointer UniformConstant %sampler

    %ptr_f_texture_1d          = OpTypePointer UniformConstant %f_texture_1d
    %ptr_f_texture_1d_array    = OpTypePointer UniformConstant %f_texture_1d_array
    %ptr_f_texture_2d          = OpTypePointer UniformConstant %f_texture_2d
    %ptr_f_texture_2d_ms       = OpTypePointer UniformConstant %f_texture_2d_ms
    %ptr_f_texture_2d_array    = OpTypePointer UniformConstant %f_texture_2d_array
    %ptr_f_texture_2d_ms_array = OpTypePointer UniformConstant %f_texture_2d_ms_array
    %ptr_f_texture_3d          = OpTypePointer UniformConstant %f_texture_3d
    %ptr_f_texture_cube        = OpTypePointer UniformConstant %f_texture_cube
    %ptr_f_texture_cube_array  = OpTypePointer UniformConstant %f_texture_cube_array

    ; storage images
    %ptr_f_storage_1d         = OpTypePointer UniformConstant %f_storage_1d
    %ptr_f_storage_1d_array   = OpTypePointer UniformConstant %f_storage_1d_array
    %ptr_f_storage_2d         = OpTypePointer UniformConstant %f_storage_2d
    %ptr_f_storage_2d_array   = OpTypePointer UniformConstant %f_storage_2d_array
    %ptr_f_storage_3d         = OpTypePointer UniformConstant %f_storage_3d

    ; Now all the same, but for unsigned integer sampled type.

    %ptr_u_texture_1d          = OpTypePointer UniformConstant %u_texture_1d
    %ptr_u_texture_1d_array    = OpTypePointer UniformConstant %u_texture_1d_array
    %ptr_u_texture_2d          = OpTypePointer UniformConstant %u_texture_2d
    %ptr_u_texture_2d_ms       = OpTypePointer UniformConstant %u_texture_2d_ms
    %ptr_u_texture_2d_array    = OpTypePointer UniformConstant %u_texture_2d_array
    %ptr_u_texture_2d_ms_array = OpTypePointer UniformConstant %u_texture_2d_ms_array
    %ptr_u_texture_3d          = OpTypePointer UniformConstant %u_texture_3d
    %ptr_u_texture_cube        = OpTypePointer UniformConstant %u_texture_cube
    %ptr_u_texture_cube_array  = OpTypePointer UniformConstant %u_texture_cube_array

    %ptr_u_storage_1d         = OpTypePointer UniformConstant %u_storage_1d
    %ptr_u_storage_1d_array   = OpTypePointer UniformConstant %u_storage_1d_array
    %ptr_u_storage_2d         = OpTypePointer UniformConstant %u_storage_2d
    %ptr_u_storage_2d_array   = OpTypePointer UniformConstant %u_storage_2d_array
    %ptr_u_storage_3d         = OpTypePointer UniformConstant %u_storage_3d

    ; Now all the same, but for signed integer sampled type.

    %ptr_i_texture_1d          = OpTypePointer UniformConstant %i_texture_1d
    %ptr_i_texture_1d_array    = OpTypePointer UniformConstant %i_texture_1d_array
    %ptr_i_texture_2d          = OpTypePointer UniformConstant %i_texture_2d
    %ptr_i_texture_2d_ms       = OpTypePointer UniformConstant %i_texture_2d_ms
    %ptr_i_texture_2d_array    = OpTypePointer UniformConstant %i_texture_2d_array
    %ptr_i_texture_2d_ms_array = OpTypePointer UniformConstant %i_texture_2d_ms_array
    %ptr_i_texture_3d          = OpTypePointer UniformConstant %i_texture_3d
    %ptr_i_texture_cube        = OpTypePointer UniformConstant %i_texture_cube
    %ptr_i_texture_cube_array  = OpTypePointer UniformConstant %i_texture_cube_array

    %ptr_i_storage_1d         = OpTypePointer UniformConstant %i_storage_1d
    %ptr_i_storage_1d_array   = OpTypePointer UniformConstant %i_storage_1d_array
    %ptr_i_storage_2d         = OpTypePointer UniformConstant %i_storage_2d
    %ptr_i_storage_2d_array   = OpTypePointer UniformConstant %i_storage_2d_array
    %ptr_i_storage_3d         = OpTypePointer UniformConstant %i_storage_3d

  )";
}

TEST_F(SpvParserTest,
       GetMemoryObjectDeclarationForHandle_WellFormedButNotAHandle) {
  const auto assembly = Preamble() + CommonTypes() + R"(
     %10 = OpConstantNull %ptr_sampler
     %20 = OpConstantNull %ptr_f_texture_1d
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  const auto* sampler = p->GetMemoryObjectDeclarationForHandle(10, false);
  const auto* image = p->GetMemoryObjectDeclarationForHandle(20, true);

  EXPECT_EQ(sampler, nullptr);
  EXPECT_EQ(image, nullptr);
  EXPECT_TRUE(p->error().empty());
}

TEST_F(SpvParserTest, GetMemoryObjectDeclarationForHandle_Variable_Direct) {
  const auto assembly = Preamble() + CommonTypes() + R"(
     %10 = OpVariable %ptr_sampler UniformConstant
     %20 = OpVariable %ptr_f_texture_1d UniformConstant
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->error().empty());
  const auto* sampler = p->GetMemoryObjectDeclarationForHandle(10, false);
  const auto* image = p->GetMemoryObjectDeclarationForHandle(20, true);

  ASSERT_TRUE(sampler != nullptr);
  EXPECT_EQ(sampler->result_id(), 10u);

  ASSERT_TRUE(image != nullptr);
  EXPECT_EQ(image->result_id(), 20u);
}

TEST_F(SpvParserTest,
       GetMemoryObjectDeclarationForHandle_Variable_AccessChain) {
  // Show that we would generalize to arrays of handles, even though that
  // is not supported in WGSL MVP.
  const auto assembly = Preamble() + CommonTypes() + R"(

     %sampler_array = OpTypeArray %sampler %uint_100
     %image_array = OpTypeArray %f_texture_1d %uint_100

     %ptr_sampler_array = OpTypePointer UniformConstant %sampler_array
     %ptr_image_array = OpTypePointer UniformConstant %image_array

     %10 = OpVariable %ptr_sampler_array UniformConstant
     %20 = OpVariable %ptr_image_array UniformConstant

     %main = OpFunction %void None %voidfn
     %entry = OpLabel

     %110 = OpAccessChain %ptr_sampler %10 %uint_1
     %120 = OpAccessChain %ptr_f_texture_1d %20 %uint_2

     OpReturn
     OpFunctionEnd
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->error().empty());
  const auto* sampler = p->GetMemoryObjectDeclarationForHandle(110, false);
  const auto* image = p->GetMemoryObjectDeclarationForHandle(120, true);

  ASSERT_TRUE(sampler != nullptr);
  EXPECT_EQ(sampler->result_id(), 10u);

  ASSERT_TRUE(image != nullptr);
  EXPECT_EQ(image->result_id(), 20u);
}

TEST_F(SpvParserTest,
       GetMemoryObjectDeclarationForHandle_Variable_InBoundsAccessChain) {
  const auto assembly = Preamble() + CommonTypes() + R"(

     %sampler_array = OpTypeArray %sampler %uint_100
     %image_array = OpTypeArray %f_texture_1d %uint_100

     %ptr_sampler_array = OpTypePointer UniformConstant %sampler_array
     %ptr_image_array = OpTypePointer UniformConstant %image_array

     %10 = OpVariable %ptr_sampler_array UniformConstant
     %20 = OpVariable %ptr_image_array UniformConstant

     %main = OpFunction %void None %voidfn
     %entry = OpLabel

     %110 = OpInBoundsAccessChain %ptr_sampler %10 %uint_1
     %120 = OpInBoundsAccessChain %ptr_f_texture_1d %20 %uint_2

     OpReturn
     OpFunctionEnd
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->error().empty());
  const auto* sampler = p->GetMemoryObjectDeclarationForHandle(110, false);
  const auto* image = p->GetMemoryObjectDeclarationForHandle(120, true);

  ASSERT_TRUE(sampler != nullptr);
  EXPECT_EQ(sampler->result_id(), 10u);

  ASSERT_TRUE(image != nullptr);
  EXPECT_EQ(image->result_id(), 20u);
}

TEST_F(SpvParserTest,
       GetMemoryObjectDeclarationForHandle_Variable_PtrAccessChain) {
  // Show that we would generalize to arrays of handles, even though that
  // is not supported in WGSL MVP.
  const auto assembly = Preamble() + CommonTypes() + R"(

     %sampler_array = OpTypeArray %sampler %uint_100
     %image_array = OpTypeArray %f_texture_1d %uint_100

     %ptr_sampler_array = OpTypePointer UniformConstant %sampler_array
     %ptr_image_array = OpTypePointer UniformConstant %image_array

     %10 = OpVariable %ptr_sampler_array UniformConstant
     %20 = OpVariable %ptr_image_array UniformConstant

     %main = OpFunction %void None %voidfn
     %entry = OpLabel

     %110 = OpPtrAccessChain %ptr_sampler %10 %uint_1 %uint_1
     %120 = OpPtrAccessChain %ptr_f_texture_1d %20 %uint_1 %uint_2

     OpReturn
     OpFunctionEnd
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->error().empty());
  const auto* sampler = p->GetMemoryObjectDeclarationForHandle(110, false);
  const auto* image = p->GetMemoryObjectDeclarationForHandle(120, true);

  ASSERT_TRUE(sampler != nullptr);
  EXPECT_EQ(sampler->result_id(), 10u);

  ASSERT_TRUE(image != nullptr);
  EXPECT_EQ(image->result_id(), 20u);
}

TEST_F(SpvParserTest,
       GetMemoryObjectDeclarationForHandle_Variable_InBoundsPtrAccessChain) {
  const auto assembly = Preamble() + CommonTypes() + R"(

     %sampler_array = OpTypeArray %sampler %uint_100
     %image_array = OpTypeArray %f_texture_1d %uint_100

     %ptr_sampler_array = OpTypePointer UniformConstant %sampler_array
     %ptr_image_array = OpTypePointer UniformConstant %image_array

     %10 = OpVariable %ptr_sampler_array UniformConstant
     %20 = OpVariable %ptr_image_array UniformConstant

     %main = OpFunction %void None %voidfn
     %entry = OpLabel

     %110 = OpInBoundsPtrAccessChain %ptr_sampler %10 %uint_1 %uint_1
     %120 = OpInBoundsPtrAccessChain %ptr_f_texture_1d %20 %uint_1 %uint_2

     OpReturn
     OpFunctionEnd
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->error().empty());
  const auto* sampler = p->GetMemoryObjectDeclarationForHandle(110, false);
  const auto* image = p->GetMemoryObjectDeclarationForHandle(120, true);

  ASSERT_TRUE(sampler != nullptr);
  EXPECT_EQ(sampler->result_id(), 10u);

  ASSERT_TRUE(image != nullptr);
  EXPECT_EQ(image->result_id(), 20u);
}

TEST_F(SpvParserTest, GetMemoryObjectDeclarationForHandle_Variable_CopyObject) {
  const auto assembly = Preamble() + CommonTypes() + R"(

     %10 = OpVariable %ptr_sampler UniformConstant
     %20 = OpVariable %ptr_f_texture_1d UniformConstant

     %main = OpFunction %void None %voidfn
     %entry = OpLabel

     %110 = OpCopyObject %ptr_sampler %10
     %120 = OpCopyObject %ptr_f_texture_1d %20

     OpReturn
     OpFunctionEnd
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->error().empty());
  const auto* sampler = p->GetMemoryObjectDeclarationForHandle(110, false);
  const auto* image = p->GetMemoryObjectDeclarationForHandle(120, true);

  ASSERT_TRUE(sampler != nullptr);
  EXPECT_EQ(sampler->result_id(), 10u);

  ASSERT_TRUE(image != nullptr);
  EXPECT_EQ(image->result_id(), 20u);
}

TEST_F(SpvParserTest, GetMemoryObjectDeclarationForHandle_Variable_Load) {
  const auto assembly = Preamble() + CommonTypes() + R"(

     %10 = OpVariable %ptr_sampler UniformConstant
     %20 = OpVariable %ptr_f_texture_1d UniformConstant

     %main = OpFunction %void None %voidfn
     %entry = OpLabel

     %110 = OpLoad %sampler %10
     %120 = OpLoad %f_texture_1d %20

     OpReturn
     OpFunctionEnd
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->error().empty());
  const auto* sampler = p->GetMemoryObjectDeclarationForHandle(110, false);
  const auto* image = p->GetMemoryObjectDeclarationForHandle(120, true);

  ASSERT_TRUE(sampler != nullptr);
  EXPECT_EQ(sampler->result_id(), 10u);

  ASSERT_TRUE(image != nullptr);
  EXPECT_EQ(image->result_id(), 20u);
}

TEST_F(SpvParserTest,
       GetMemoryObjectDeclarationForHandle_Variable_SampledImage) {
  // Trace through the sampled image instruction, but in two different
  // directions.
  const auto assembly = Preamble() + CommonTypes() + R"(
     %sampled_image_type = OpTypeSampledImage %f_texture_1d

     %10 = OpVariable %ptr_sampler UniformConstant
     %20 = OpVariable %ptr_f_texture_1d UniformConstant

     %main = OpFunction %void None %voidfn
     %entry = OpLabel

     %s = OpLoad %sampler %10
     %im = OpLoad %f_texture_1d %20
     %100 = OpSampledImage %sampled_image_type %im %s

     OpReturn
     OpFunctionEnd
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->error().empty());
  const auto* sampler = p->GetMemoryObjectDeclarationForHandle(100, false);
  const auto* image = p->GetMemoryObjectDeclarationForHandle(100, true);

  ASSERT_TRUE(sampler != nullptr);
  EXPECT_EQ(sampler->result_id(), 10u);

  ASSERT_TRUE(image != nullptr);
  EXPECT_EQ(image->result_id(), 20u);
}

TEST_F(SpvParserTest, GetMemoryObjectDeclarationForHandle_Variable_Image) {
  const auto assembly = Preamble() + CommonTypes() + R"(
     %sampled_image_type = OpTypeSampledImage %f_texture_1d

     %10 = OpVariable %ptr_sampler UniformConstant
     %20 = OpVariable %ptr_f_texture_1d UniformConstant

     %main = OpFunction %void None %voidfn
     %entry = OpLabel

     %s = OpLoad %sampler %10
     %im = OpLoad %f_texture_1d %20
     %100 = OpSampledImage %sampled_image_type %im %s
     %200 = OpImage %im %100

     OpReturn
     OpFunctionEnd
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->error().empty());

  const auto* image = p->GetMemoryObjectDeclarationForHandle(200, true);
  ASSERT_TRUE(image != nullptr);
  EXPECT_EQ(image->result_id(), 20u);
}

TEST_F(SpvParserTest, GetMemoryObjectDeclarationForHandle_FuncParam_Direct) {
  const auto assembly = Preamble() + CommonTypes() + R"(
     %fty = OpTypeFunction %void %ptr_sampler %ptr_f_texture_1d

     %func = OpFunction %void None %fty
     %10 = OpFunctionParameter %ptr_sampler
     %20 = OpFunctionParameter %ptr_f_texture_1d
     %entry = OpLabel
     OpReturn
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->error().empty());
  const auto* sampler = p->GetMemoryObjectDeclarationForHandle(10, false);
  const auto* image = p->GetMemoryObjectDeclarationForHandle(20, true);

  ASSERT_TRUE(sampler != nullptr);
  EXPECT_EQ(sampler->result_id(), 10u);

  ASSERT_TRUE(image != nullptr);
  EXPECT_EQ(image->result_id(), 20u);
}

TEST_F(SpvParserTest,
       GetMemoryObjectDeclarationForHandle_FuncParam_AccessChain) {
  // Show that we would generalize to arrays of handles, even though that
  // is not supported in WGSL MVP.
  const auto assembly = Preamble() + CommonTypes() + R"(
     %sampler_array = OpTypeArray %sampler %uint_100
     %image_array = OpTypeArray %f_texture_1d %uint_100

     %ptr_sampler_array = OpTypePointer UniformConstant %sampler_array
     %ptr_image_array = OpTypePointer UniformConstant %image_array

     %fty = OpTypeFunction %void %ptr_sampler_array %ptr_image_array

     %func = OpFunction %void None %fty
     %10 = OpFunctionParameter %ptr_sampler_array
     %20 = OpFunctionParameter %ptr_image_array
     %entry = OpLabel

     %110 = OpAccessChain %ptr_sampler %10 %uint_1
     %120 = OpAccessChain %ptr_f_texture_1d %20 %uint_2

     OpReturn
     OpFunctionEnd
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->error().empty());
  const auto* sampler = p->GetMemoryObjectDeclarationForHandle(110, false);
  const auto* image = p->GetMemoryObjectDeclarationForHandle(120, true);

  ASSERT_TRUE(sampler != nullptr);
  EXPECT_EQ(sampler->result_id(), 10u);

  ASSERT_TRUE(image != nullptr);
  EXPECT_EQ(image->result_id(), 20u);
}

TEST_F(SpvParserTest,
       GetMemoryObjectDeclarationForHandle_FuncParam_InBoundsAccessChain) {
  const auto assembly = Preamble() + CommonTypes() + R"(
     %sampler_array = OpTypeArray %sampler %uint_100
     %image_array = OpTypeArray %f_texture_1d %uint_100

     %ptr_sampler_array = OpTypePointer UniformConstant %sampler_array
     %ptr_image_array = OpTypePointer UniformConstant %image_array

     %fty = OpTypeFunction %void %ptr_sampler_array %ptr_image_array

     %func = OpFunction %void None %fty
     %10 = OpFunctionParameter %ptr_sampler_array
     %20 = OpFunctionParameter %ptr_image_array
     %entry = OpLabel

     %110 = OpInBoundsAccessChain %ptr_sampler %10 %uint_1
     %120 = OpInBoundsAccessChain %ptr_f_texture_1d %20 %uint_2

     OpReturn
     OpFunctionEnd
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->error().empty());
  const auto* sampler = p->GetMemoryObjectDeclarationForHandle(110, false);
  const auto* image = p->GetMemoryObjectDeclarationForHandle(120, true);

  ASSERT_TRUE(sampler != nullptr);
  EXPECT_EQ(sampler->result_id(), 10u);

  ASSERT_TRUE(image != nullptr);
  EXPECT_EQ(image->result_id(), 20u);
}

TEST_F(SpvParserTest,
       GetMemoryObjectDeclarationForHandle_FuncParam_PtrAccessChain) {
  // Show that we would generalize to arrays of handles, even though that
  // is not supported in WGSL MVP.
  const auto assembly = Preamble() + CommonTypes() + R"(
     %sampler_array = OpTypeArray %sampler %uint_100
     %image_array = OpTypeArray %f_texture_1d %uint_100

     %ptr_sampler_array = OpTypePointer UniformConstant %sampler_array
     %ptr_image_array = OpTypePointer UniformConstant %image_array

     %fty = OpTypeFunction %void %ptr_sampler_array %ptr_image_array

     %func = OpFunction %void None %fty
     %10 = OpFunctionParameter %ptr_sampler_array
     %20 = OpFunctionParameter %ptr_image_array
     %entry = OpLabel

     %110 = OpPtrAccessChain %ptr_sampler %10 %uint_1 %uint_1
     %120 = OpPtrAccessChain %ptr_f_texture_1d %20 %uint_1 %uint_2

     OpReturn
     OpFunctionEnd
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->error().empty());
  const auto* sampler = p->GetMemoryObjectDeclarationForHandle(110, false);
  const auto* image = p->GetMemoryObjectDeclarationForHandle(120, true);

  ASSERT_TRUE(sampler != nullptr);
  EXPECT_EQ(sampler->result_id(), 10u);

  ASSERT_TRUE(image != nullptr);
  EXPECT_EQ(image->result_id(), 20u);
}

TEST_F(SpvParserTest,
       GetMemoryObjectDeclarationForHandle_FuncParam_InBoundsPtrAccessChain) {
  const auto assembly = Preamble() + CommonTypes() + R"(
     %sampler_array = OpTypeArray %sampler %uint_100
     %image_array = OpTypeArray %f_texture_1d %uint_100

     %ptr_sampler_array = OpTypePointer UniformConstant %sampler_array
     %ptr_image_array = OpTypePointer UniformConstant %image_array

     %fty = OpTypeFunction %void %ptr_sampler_array %ptr_image_array

     %func = OpFunction %void None %fty
     %10 = OpFunctionParameter %ptr_sampler_array
     %20 = OpFunctionParameter %ptr_image_array
     %entry = OpLabel

     %110 = OpInBoundsPtrAccessChain %ptr_sampler %10 %uint_1 %uint_1
     %120 = OpInBoundsPtrAccessChain %ptr_f_texture_1d %20 %uint_1 %uint_2

     OpReturn
     OpFunctionEnd
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->error().empty());
  const auto* sampler = p->GetMemoryObjectDeclarationForHandle(110, false);
  const auto* image = p->GetMemoryObjectDeclarationForHandle(120, true);

  ASSERT_TRUE(sampler != nullptr);
  EXPECT_EQ(sampler->result_id(), 10u);

  ASSERT_TRUE(image != nullptr);
  EXPECT_EQ(image->result_id(), 20u);
}

TEST_F(SpvParserTest,
       GetMemoryObjectDeclarationForHandle_FuncParam_CopyObject) {
  const auto assembly = Preamble() + CommonTypes() + R"(
     %fty = OpTypeFunction %void %ptr_sampler %ptr_f_texture_1d

     %func = OpFunction %void None %fty
     %10 = OpFunctionParameter %ptr_sampler
     %20 = OpFunctionParameter %ptr_f_texture_1d
     %entry = OpLabel

     %110 = OpCopyObject %ptr_sampler %10
     %120 = OpCopyObject %ptr_f_texture_1d %20

     OpReturn
     OpFunctionEnd
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->error().empty());
  const auto* sampler = p->GetMemoryObjectDeclarationForHandle(110, false);
  const auto* image = p->GetMemoryObjectDeclarationForHandle(120, true);

  ASSERT_TRUE(sampler != nullptr);
  EXPECT_EQ(sampler->result_id(), 10u);

  ASSERT_TRUE(image != nullptr);
  EXPECT_EQ(image->result_id(), 20u);
}

TEST_F(SpvParserTest, GetMemoryObjectDeclarationForHandle_FuncParam_Load) {
  const auto assembly = Preamble() + CommonTypes() + R"(
     %fty = OpTypeFunction %void %ptr_sampler %ptr_f_texture_1d

     %func = OpFunction %void None %fty
     %10 = OpFunctionParameter %ptr_sampler
     %20 = OpFunctionParameter %ptr_f_texture_1d
     %entry = OpLabel

     %110 = OpLoad %sampler %10
     %120 = OpLoad %f_texture_1d %20

     OpReturn
     OpFunctionEnd
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->error().empty());
  const auto* sampler = p->GetMemoryObjectDeclarationForHandle(110, false);
  const auto* image = p->GetMemoryObjectDeclarationForHandle(120, true);

  ASSERT_TRUE(sampler != nullptr);
  EXPECT_EQ(sampler->result_id(), 10u);

  ASSERT_TRUE(image != nullptr);
  EXPECT_EQ(image->result_id(), 20u);
}

TEST_F(SpvParserTest,
       GetMemoryObjectDeclarationForHandle_FuncParam_SampledImage) {
  // Trace through the sampled image instruction, but in two different
  // directions.
  const auto assembly = Preamble() + CommonTypes() + R"(
     %sampled_image_type = OpTypeSampledImage %f_texture_1d

     %fty = OpTypeFunction %void %ptr_sampler %ptr_f_texture_1d

     %func = OpFunction %void None %fty
     %10 = OpFunctionParameter %ptr_sampler
     %20 = OpFunctionParameter %ptr_f_texture_1d
     %entry = OpLabel

     %s = OpLoad %sampler %10
     %im = OpLoad %f_texture_1d %20
     %100 = OpSampledImage %sampled_image_type %im %s

     OpReturn
     OpFunctionEnd
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->error().empty());
  const auto* sampler = p->GetMemoryObjectDeclarationForHandle(100, false);
  const auto* image = p->GetMemoryObjectDeclarationForHandle(100, true);

  ASSERT_TRUE(sampler != nullptr);
  EXPECT_EQ(sampler->result_id(), 10u);

  ASSERT_TRUE(image != nullptr);
  EXPECT_EQ(image->result_id(), 20u);
}

TEST_F(SpvParserTest, GetMemoryObjectDeclarationForHandle_FuncParam_Image) {
  const auto assembly = Preamble() + CommonTypes() + R"(
     %sampled_image_type = OpTypeSampledImage %f_texture_1d

     %fty = OpTypeFunction %void %ptr_sampler %ptr_f_texture_1d

     %func = OpFunction %void None %fty
     %10 = OpFunctionParameter %ptr_sampler
     %20 = OpFunctionParameter %ptr_f_texture_1d
     %entry = OpLabel

     %s = OpLoad %sampler %10
     %im = OpLoad %f_texture_1d %20
     %100 = OpSampledImage %sampled_image_type %im %s
     %200 = OpImage %im %100

     OpReturn
     OpFunctionEnd
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->error().empty());

  const auto* image = p->GetMemoryObjectDeclarationForHandle(200, true);
  ASSERT_TRUE(image != nullptr);
  EXPECT_EQ(image->result_id(), 20u);
}

// Test RegisterHandleUsage, sampled image cases

struct SampledImageCase {
  std::string inst;
  std::string expected_sampler_usage;
  std::string expected_image_usage;
};
inline std::ostream& operator<<(std::ostream& out, const SampledImageCase& c) {
  out << "SampledImageCase(" << c.inst << ", " << c.expected_sampler_usage
      << ", " << c.expected_image_usage << ")";
  return out;
}

using SpvParserTest_RegisterHandleUsage_SampledImage =
    SpvParserTestBase<::testing::TestWithParam<SampledImageCase>>;

TEST_P(SpvParserTest_RegisterHandleUsage_SampledImage, Variable) {
  const auto assembly = Preamble() + CommonTypes() + R"(
     %si_ty = OpTypeSampledImage %f_texture_2d
     %coords = OpConstantNull %v2float

     %10 = OpVariable %ptr_sampler UniformConstant
     %20 = OpVariable %ptr_f_texture_2d UniformConstant

     %main = OpFunction %void None %voidfn
     %entry = OpLabel

     %sam = OpLoad %sampler %10
     %im = OpLoad %f_texture_2d %20
     %sampled_image = OpSampledImage %si_ty %im %sam
)" + GetParam().inst + R"(

     OpReturn
     OpFunctionEnd
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->RegisterHandleUsage());
  EXPECT_TRUE(p->error().empty());
  Usage su = p->GetHandleUsage(10);
  Usage iu = p->GetHandleUsage(20);

  EXPECT_THAT(su.to_str(), Eq(GetParam().expected_sampler_usage));
  EXPECT_THAT(iu.to_str(), Eq(GetParam().expected_image_usage));
}

TEST_P(SpvParserTest_RegisterHandleUsage_SampledImage, FunctionParam) {
  const auto assembly = Preamble() + CommonTypes() + R"(
     %f_ty = OpTypeFunction %void %ptr_sampler %ptr_f_texture_2d
     %si_ty = OpTypeSampledImage %f_texture_2d
     %coords = OpConstantNull %v2float
     %component = OpConstant %uint 1

     %10 = OpVariable %ptr_sampler UniformConstant
     %20 = OpVariable %ptr_f_texture_2d UniformConstant

     %func = OpFunction %void None %f_ty
     %110 = OpFunctionParameter %ptr_sampler
     %120 = OpFunctionParameter %ptr_f_texture_2d
     %func_entry = OpLabel
     %sam = OpLoad %sampler %110
     %im = OpLoad %f_texture_2d %120
     %sampled_image = OpSampledImage %si_ty %im %sam

)" + GetParam().inst + R"(

     OpReturn
     OpFunctionEnd

     %main = OpFunction %void None %voidfn
     %entry = OpLabel
     %foo = OpFunctionCall %void %func %10 %20
     OpReturn
     OpFunctionEnd
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule()) << p->error() << assembly << std::endl;
  EXPECT_TRUE(p->RegisterHandleUsage()) << p->error() << assembly << std::endl;
  EXPECT_TRUE(p->error().empty()) << p->error() << assembly << std::endl;
  Usage su = p->GetHandleUsage(10);
  Usage iu = p->GetHandleUsage(20);

  std::cout << p->GetHandleUsage(10) << std::endl;
  std::cout << p->GetHandleUsage(20) << std::endl;
  std::cout << p->GetHandleUsage(110) << std::endl;
  std::cout << p->GetHandleUsage(120) << std::endl;

  EXPECT_THAT(su.to_str(), Eq(GetParam().expected_sampler_usage));
  EXPECT_THAT(iu.to_str(), Eq(GetParam().expected_image_usage));
}

INSTANTIATE_TEST_SUITE_P(
    Samples,
    SpvParserTest_RegisterHandleUsage_SampledImage,
    ::testing::Values(

        // OpImageGather
        SampledImageCase{"%result = OpImageGather "
                         "%v4float %sampled_image %coords %uint_1",
                         "Usage(Sampler( ))", "Usage(Texture( is_sampled ))"},
        // OpImageDrefGather
        SampledImageCase{"%result = OpImageDrefGather "
                         "%v4float %sampled_image %coords %depth",
                         "Usage(Sampler( comparison ))",
                         "Usage(Texture( is_sampled depth ))"},

        // Sample the texture.

        // OpImageSampleImplicitLod
        SampledImageCase{"%result = OpImageSampleImplicitLod "
                         "%v4float %sampled_image %coords",
                         "Usage(Sampler( ))", "Usage(Texture( is_sampled ))"},
        // OpImageSampleExplicitLod
        SampledImageCase{"%result = OpImageSampleExplicitLod "
                         "%v4float %sampled_image %coords Lod %float_null",
                         "Usage(Sampler( ))", "Usage(Texture( is_sampled ))"},
        // OpImageSampleDrefImplicitLod
        SampledImageCase{"%result = OpImageSampleDrefImplicitLod "
                         "%v4float %sampled_image %coords %depth",
                         "Usage(Sampler( comparison ))",
                         "Usage(Texture( is_sampled depth ))"},
        // OpImageSampleDrefExplicitLod
        SampledImageCase{
            "%result = OpImageSampleDrefExplicitLod "
            "%v4float %sampled_image %coords %depth Lod %float_null",
            "Usage(Sampler( comparison ))",
            "Usage(Texture( is_sampled depth ))"},

        // Sample the texture, with *Proj* variants, even though WGSL doesn't
        // support them.

        // OpImageSampleProjImplicitLod
        SampledImageCase{"%result = OpImageSampleProjImplicitLod "
                         "%v4float %sampled_image %coords",
                         "Usage(Sampler( ))", "Usage(Texture( is_sampled ))"},
        // OpImageSampleProjExplicitLod
        SampledImageCase{"%result = OpImageSampleProjExplicitLod "
                         "%v4float %sampled_image %coords Lod %float_null",
                         "Usage(Sampler( ))", "Usage(Texture( is_sampled ))"},
        // OpImageSampleProjDrefImplicitLod
        SampledImageCase{"%result = OpImageSampleProjDrefImplicitLod "
                         "%v4float %sampled_image %coords %depth",
                         "Usage(Sampler( comparison ))",
                         "Usage(Texture( is_sampled depth ))"},
        // OpImageSampleProjDrefExplicitLod
        SampledImageCase{
            "%result = OpImageSampleProjDrefExplicitLod "
            "%v4float %sampled_image %coords %depth Lod %float_null",
            "Usage(Sampler( comparison ))",
            "Usage(Texture( is_sampled depth ))"},

        // OpImageQueryLod
        SampledImageCase{
            "%result = OpImageQueryLod %v2float %sampled_image %coords",
            "Usage(Sampler( ))", "Usage(Texture( is_sampled ))"}

        ));

// Test RegisterHandleUsage, raw image cases.
// For these we test the use of an image value directly, and not combined
// with the sampler. The image still could be of sampled image type.

struct RawImageCase {
  std::string type;  // Example: f_storage_1d or f_texture_1d
  std::string inst;
  std::string expected_image_usage;
};
inline std::ostream& operator<<(std::ostream& out, const RawImageCase& c) {
  out << "RawImageCase(" << c.type << ", " << c.inst << ", "
      << c.expected_image_usage << ")";
  return out;
}

using SpvParserTest_RegisterHandleUsage_RawImage =
    SpvParserTestBase<::testing::TestWithParam<RawImageCase>>;

TEST_P(SpvParserTest_RegisterHandleUsage_RawImage, Variable) {
  const auto assembly = Preamble() + CommonTypes() + R"(
     %20 = OpVariable %ptr_)" +
                        GetParam().type + R"( UniformConstant

     %main = OpFunction %void None %voidfn
     %entry = OpLabel

     %im = OpLoad %)" + GetParam().type +
                        R"( %20
)" + GetParam().inst + R"(

     OpReturn
     OpFunctionEnd
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->RegisterHandleUsage());
  EXPECT_TRUE(p->error().empty());

  Usage iu = p->GetHandleUsage(20);
  EXPECT_THAT(iu.to_str(), Eq(GetParam().expected_image_usage));

  Usage su = p->GetHandleUsage(20);
}

TEST_P(SpvParserTest_RegisterHandleUsage_RawImage, FunctionParam) {
  const auto assembly = Preamble() + CommonTypes() + R"(
     %f_ty = OpTypeFunction %void %ptr_)" +
                        GetParam().type + R"(

     %20 = OpVariable %ptr_)" +
                        GetParam().type + R"( UniformConstant

     %func = OpFunction %void None %f_ty
     %i_param = OpFunctionParameter %ptr_)" +
                        GetParam().type + R"(
     %func_entry = OpLabel
     %im = OpLoad %)" + GetParam().type +
                        R"( %i_param

)" + GetParam().inst + R"(

     OpReturn
     OpFunctionEnd

     %main = OpFunction %void None %voidfn
     %entry = OpLabel
     %foo = OpFunctionCall %void %func %20
     OpReturn
     OpFunctionEnd
  )";
  auto* p = parser(test::Assemble(assembly));
  ASSERT_TRUE(p->BuildInternalModule());
  EXPECT_TRUE(p->RegisterHandleUsage());
  EXPECT_TRUE(p->error().empty());
  Usage iu = p->GetHandleUsage(20);

  EXPECT_THAT(iu.to_str(), Eq(GetParam().expected_image_usage));
}

INSTANTIATE_TEST_SUITE_P(
    Samples,
    SpvParserTest_RegisterHandleUsage_RawImage,
    ::testing::Values(

        // OpImageRead
        RawImageCase{"f_storage_1d",
                     "%result = OpImageRead %v4float %im %uint_1",
                     "Usage(Texture( read ))"},

        // OpImageWrite
        RawImageCase{"f_storage_1d", "OpImageWrite %im %uint_1 %v4float_null",
                     "Usage(Texture( write ))"},

        // OpImageFetch
        RawImageCase{"f_texture_1d",
                     "%result = OpImageFetch "
                     "%v4float %im %float_null",
                     "Usage(Texture( is_sampled ))"},

        // Image queries

        // OpImageQuerySizeLod
        // Applies to NonReadable, hence write-only storage
        RawImageCase{"f_storage_2d",
                     "%result = OpImageQuerySizeLod "
                     "%v2uint %im %uint_1",
                     "Usage(Texture( write ))"},

        // OpImageQuerySize
        // Applies to NonReadable, hence write-only storage
        RawImageCase{"f_storage_2d",
                     "%result = OpImageQuerySize "
                     "%v2uint %im",
                     "Usage(Texture( write ))"},

        // OpImageQueryLevels
        RawImageCase{"f_texture_2d",
                     "%result = OpImageQueryLevels "
                     "%uint %im",
                     "Usage(Texture( ))"},

        // OpImageQuerySamples
        RawImageCase{"f_texture_2d_ms",
                     "%result = OpImageQuerySamples "
                     "%uint %im",
                     "Usage(Texture( is_sampled ms ))"}

        ));

}  // namespace
}  // namespace spirv
}  // namespace reader
}  // namespace tint