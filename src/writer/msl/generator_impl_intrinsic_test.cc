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

#include "gtest/gtest.h"
#include "src/ast/call_expression.h"
#include "src/ast/identifier_expression.h"
#include "src/program.h"
#include "src/type/f32_type.h"
#include "src/type/vector_type.h"
#include "src/type_determiner.h"
#include "src/writer/msl/generator_impl.h"
#include "src/writer/msl/test_helper.h"

namespace tint {
namespace writer {
namespace msl {
namespace {

using MslGeneratorImplTest = TestHelper;

struct IntrinsicData {
  ast::Intrinsic intrinsic;
  const char* msl_name;
};
inline std::ostream& operator<<(std::ostream& out, IntrinsicData data) {
  out << data.msl_name;
  return out;
}
using MslIntrinsicTest = TestParamHelper<IntrinsicData>;
TEST_P(MslIntrinsicTest, Emit) {
  auto param = GetParam();

  GeneratorImpl& gen = Build();

  EXPECT_EQ(gen.generate_intrinsic_name(param.intrinsic), param.msl_name);
}
INSTANTIATE_TEST_SUITE_P(
    MslGeneratorImplTest,
    MslIntrinsicTest,
    testing::Values(IntrinsicData{ast::Intrinsic::kAny, "any"},
                    IntrinsicData{ast::Intrinsic::kAll, "all"},
                    IntrinsicData{ast::Intrinsic::kCountOneBits, "popcount"},
                    IntrinsicData{ast::Intrinsic::kDot, "dot"},
                    IntrinsicData{ast::Intrinsic::kDpdx, "dfdx"},
                    IntrinsicData{ast::Intrinsic::kDpdxCoarse, "dfdx"},
                    IntrinsicData{ast::Intrinsic::kDpdxFine, "dfdx"},
                    IntrinsicData{ast::Intrinsic::kDpdy, "dfdy"},
                    IntrinsicData{ast::Intrinsic::kDpdyCoarse, "dfdy"},
                    IntrinsicData{ast::Intrinsic::kDpdyFine, "dfdy"},
                    IntrinsicData{ast::Intrinsic::kFwidth, "fwidth"},
                    IntrinsicData{ast::Intrinsic::kFwidthCoarse, "fwidth"},
                    IntrinsicData{ast::Intrinsic::kFwidthFine, "fwidth"},
                    IntrinsicData{ast::Intrinsic::kIsFinite, "isfinite"},
                    IntrinsicData{ast::Intrinsic::kIsInf, "isinf"},
                    IntrinsicData{ast::Intrinsic::kIsNan, "isnan"},
                    IntrinsicData{ast::Intrinsic::kIsNormal, "isnormal"},
                    IntrinsicData{ast::Intrinsic::kReverseBits, "reverse_bits"},
                    IntrinsicData{ast::Intrinsic::kSelect, "select"}));

TEST_F(MslGeneratorImplTest, Intrinsic_Bad_Name) {
  GeneratorImpl& gen = Build();

  EXPECT_EQ(gen.generate_intrinsic_name(ast::Intrinsic::kNone), "");
}

TEST_F(MslGeneratorImplTest, Intrinsic_Call) {
  auto* call = Call("dot", "param1", "param2");

  auto* v1 = Var("param1", ast::StorageClass::kFunction, ty.vec2<f32>());
  auto* v2 = Var("param2", ast::StorageClass::kFunction, ty.vec2<f32>());

  td.RegisterVariableForTesting(v1);
  td.RegisterVariableForTesting(v2);

  ASSERT_TRUE(td.DetermineResultType(call)) << td.error();

  GeneratorImpl& gen = Build();

  gen.increment_indent();
  ASSERT_TRUE(gen.EmitExpression(call)) << gen.error();
  EXPECT_EQ(gen.result(), "  dot(param1, param2)");
}

}  // namespace
}  // namespace msl
}  // namespace writer
}  // namespace tint
