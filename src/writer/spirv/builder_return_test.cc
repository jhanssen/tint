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

#include <memory>

#include "gtest/gtest.h"
#include "src/ast/float_literal.h"
#include "src/ast/return_statement.h"
#include "src/ast/scalar_constructor_expression.h"
#include "src/ast/type/f32_type.h"
#include "src/ast/type/vector_type.h"
#include "src/ast/type_constructor_expression.h"
#include "src/writer/spirv/builder.h"
#include "src/writer/spirv/spv_dump.h"

namespace tint {
namespace writer {
namespace spirv {
namespace {

using BuilderTest = testing::Test;

TEST_F(BuilderTest, Return) {
  ast::ReturnStatement ret;

  Builder b;
  b.push_function(Function{});
  EXPECT_TRUE(b.GenerateReturnStatement(&ret));
  ASSERT_FALSE(b.has_error()) << b.error();

  EXPECT_EQ(DumpInstructions(b.functions()[0].instructions()), R"(OpReturn
)");
}

TEST_F(BuilderTest, Return_WithValue) {
  ast::type::F32Type f32;
  ast::type::VectorType vec(&f32, 3);

  std::vector<std::unique_ptr<ast::Expression>> vals;
  vals.push_back(std::make_unique<ast::ScalarConstructorExpression>(
      std::make_unique<ast::FloatLiteral>(&f32, 1.0f)));
  vals.push_back(std::make_unique<ast::ScalarConstructorExpression>(
      std::make_unique<ast::FloatLiteral>(&f32, 1.0f)));
  vals.push_back(std::make_unique<ast::ScalarConstructorExpression>(
      std::make_unique<ast::FloatLiteral>(&f32, 3.0f)));

  auto val =
      std::make_unique<ast::TypeConstructorExpression>(&vec, std::move(vals));

  ast::ReturnStatement ret(std::move(val));

  Builder b;
  b.push_function(Function{});
  EXPECT_TRUE(b.GenerateReturnStatement(&ret));
  ASSERT_FALSE(b.has_error()) << b.error();

  EXPECT_EQ(DumpInstructions(b.types()), R"(%2 = OpTypeFloat 32
%1 = OpTypeVector %2 3
%3 = OpConstant %2 1
%4 = OpConstant %2 3
%5 = OpConstantComposite %1 %3 %3 %4
)");
  EXPECT_EQ(DumpInstructions(b.functions()[0].instructions()),
            R"(OpReturnValue %5
)");
}

}  // namespace
}  // namespace spirv
}  // namespace writer
}  // namespace tint