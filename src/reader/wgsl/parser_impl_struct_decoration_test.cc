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
#include "src/ast/struct_block_decoration.h"
#include "src/ast/struct_decoration.h"
#include "src/reader/wgsl/parser_impl.h"
#include "src/reader/wgsl/parser_impl_test_helper.h"

namespace tint {
namespace reader {
namespace wgsl {
namespace {

struct StructDecorationData {
  const char* input;
  bool is_block;
};
inline std::ostream& operator<<(std::ostream& out, StructDecorationData data) {
  out << std::string(data.input);
  return out;
}

class StructDecorationTest
    : public ParserImplTestWithParam<StructDecorationData> {};

TEST_P(StructDecorationTest, Parses) {
  auto params = GetParam();
  auto p = parser(params.input);

  auto deco = p->decoration();
  ASSERT_FALSE(p->has_error());
  EXPECT_TRUE(deco.matched);
  EXPECT_FALSE(deco.errored);
  ASSERT_NE(deco.value, nullptr);
  auto* struct_deco = deco.value->As<ast::StructDecoration>();
  ASSERT_NE(struct_deco, nullptr);
  EXPECT_EQ(struct_deco->Is<ast::StructBlockDecoration>(), params.is_block);
}
INSTANTIATE_TEST_SUITE_P(ParserImplTest,
                         StructDecorationTest,
                         testing::Values(StructDecorationData{"block", true}));

TEST_F(ParserImplTest, StructDecoration_NoMatch) {
  auto p = parser("not-a-stage");
  auto deco = p->decoration();
  EXPECT_FALSE(deco.matched);
  EXPECT_FALSE(deco.errored);
  ASSERT_EQ(deco.value, nullptr);
}

}  // namespace
}  // namespace wgsl
}  // namespace reader
}  // namespace tint
