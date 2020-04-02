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
#include <vector>

#include "gtest/gtest.h"
#include "src/ast/break_statement.h"
#include "src/ast/case_statement.h"
#include "src/ast/identifier_expression.h"
#include "src/ast/int_literal.h"
#include "src/ast/type/i32_type.h"
#include "src/writer/wgsl/generator_impl.h"

namespace tint {
namespace writer {
namespace wgsl {
namespace {

using GeneratorImplTest = testing::Test;

TEST_F(GeneratorImplTest, Emit_Case) {
  ast::type::I32Type i32;
  auto cond = std::make_unique<ast::IntLiteral>(&i32, 5);

  std::vector<std::unique_ptr<ast::Statement>> body;
  body.push_back(std::make_unique<ast::BreakStatement>());

  ast::CaseStatement c(std::move(cond), std::move(body));

  GeneratorImpl g;
  g.increment_indent();

  ASSERT_TRUE(g.EmitCase(&c)) << g.error();
  EXPECT_EQ(g.result(), R"(  case 5: {
    break;
  }
)");
}

TEST_F(GeneratorImplTest, Emit_Case_Default) {
  ast::CaseStatement c;

  std::vector<std::unique_ptr<ast::Statement>> body;
  body.push_back(std::make_unique<ast::BreakStatement>());

  c.set_body(std::move(body));

  GeneratorImpl g;
  g.increment_indent();

  ASSERT_TRUE(g.EmitCase(&c)) << g.error();
  EXPECT_EQ(g.result(), R"(  default: {
    break;
  }
)");
}

}  // namespace
}  // namespace wgsl
}  // namespace writer
}  // namespace tint