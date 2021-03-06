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
#include "src/ast/assignment_statement.h"
#include "src/ast/identifier_expression.h"
#include "src/ast/location_decoration.h"
#include "src/ast/member_accessor_expression.h"
#include "src/ast/pipeline_stage.h"
#include "src/ast/stage_decoration.h"
#include "src/ast/variable.h"
#include "src/program.h"
#include "src/type/f32_type.h"
#include "src/type/i32_type.h"
#include "src/type/vector_type.h"
#include "src/type/void_type.h"
#include "src/type_determiner.h"
#include "src/writer/msl/generator_impl.h"
#include "src/writer/msl/test_helper.h"

namespace tint {
namespace writer {
namespace msl {
namespace {

using MslGeneratorImplTest = TestHelper;

TEST_F(MslGeneratorImplTest, Emit_Function_EntryPointData_Vertex_Input) {
  // [[location 0]] var<in> foo : f32;
  // [[location 1]] var<in> bar : i32;
  //
  // struct vtx_main_in {
  //   float foo [[attribute(0)]];
  //   int bar [[attribute(1)]];
  // };

  auto* foo_var =
      Var("foo", ast::StorageClass::kInput, ty.f32(), nullptr,
          ast::VariableDecorationList{create<ast::LocationDecoration>(0)});

  auto* bar_var =
      Var("bar", ast::StorageClass::kInput, ty.i32(), nullptr,
          ast::VariableDecorationList{create<ast::LocationDecoration>(1)});

  td.RegisterVariableForTesting(foo_var);
  td.RegisterVariableForTesting(bar_var);

  AST().AddGlobalVariable(foo_var);
  AST().AddGlobalVariable(bar_var);

  auto body = ast::StatementList{
      create<ast::AssignmentStatement>(Expr("foo"), Expr("foo")),
      create<ast::AssignmentStatement>(Expr("bar"), Expr("bar")),
  };
  auto* func =
      Func("vtx_main", ast::VariableList{}, ty.f32(), body,
           ast::FunctionDecorationList{
               create<ast::StageDecoration>(ast::PipelineStage::kVertex),
           });

  AST().Functions().Add(func);

  ASSERT_TRUE(td.Determine()) << td.error();

  GeneratorImpl& gen = Build();

  ASSERT_TRUE(gen.EmitEntryPointData(func)) << gen.error();
  EXPECT_EQ(gen.result(), R"(struct vtx_main_in {
  float foo [[attribute(0)]];
  int bar [[attribute(1)]];
};

)");
}

TEST_F(MslGeneratorImplTest, Emit_Function_EntryPointData_Vertex_Output) {
  // [[location 0]] var<out> foo : f32;
  // [[location 1]] var<out> bar : i32;
  //
  // struct vtx_main_out {
  //   float foo [[user(locn0)]];
  //   int bar [[user(locn1)]];
  // };

  auto* foo_var =
      Var("foo", ast::StorageClass::kOutput, ty.f32(), nullptr,
          ast::VariableDecorationList{create<ast::LocationDecoration>(0)});

  auto* bar_var =
      Var("bar", ast::StorageClass::kOutput, ty.i32(), nullptr,
          ast::VariableDecorationList{create<ast::LocationDecoration>(1)});

  td.RegisterVariableForTesting(foo_var);
  td.RegisterVariableForTesting(bar_var);

  AST().AddGlobalVariable(foo_var);
  AST().AddGlobalVariable(bar_var);

  auto body = ast::StatementList{
      create<ast::AssignmentStatement>(Expr("foo"), Expr("foo")),
      create<ast::AssignmentStatement>(Expr("bar"), Expr("bar")),
  };
  auto* func =
      Func("vtx_main", ast::VariableList{}, ty.f32(), body,
           ast::FunctionDecorationList{
               create<ast::StageDecoration>(ast::PipelineStage::kVertex),
           });

  AST().Functions().Add(func);

  ASSERT_TRUE(td.Determine()) << td.error();

  GeneratorImpl& gen = Build();

  ASSERT_TRUE(gen.EmitEntryPointData(func)) << gen.error();
  EXPECT_EQ(gen.result(), R"(struct vtx_main_out {
  float foo [[user(locn0)]];
  int bar [[user(locn1)]];
};

)");
}

TEST_F(MslGeneratorImplTest, Emit_Function_EntryPointData_Fragment_Input) {
  // [[location 0]] var<in> foo : f32;
  // [[location 1]] var<in> bar : i32;
  //
  // struct frag_main_in {
  //   float foo [[user(locn0)]];
  //   int bar [[user(locn1)]];
  // };

  auto* foo_var =
      Var("foo", ast::StorageClass::kInput, ty.f32(), nullptr,
          ast::VariableDecorationList{create<ast::LocationDecoration>(0)});

  auto* bar_var =
      Var("bar", ast::StorageClass::kInput, ty.i32(), nullptr,
          ast::VariableDecorationList{create<ast::LocationDecoration>(1)});

  td.RegisterVariableForTesting(foo_var);
  td.RegisterVariableForTesting(bar_var);

  AST().AddGlobalVariable(foo_var);
  AST().AddGlobalVariable(bar_var);

  auto body = ast::StatementList{
      create<ast::AssignmentStatement>(Expr("foo"), Expr("foo")),
      create<ast::AssignmentStatement>(Expr("bar"), Expr("bar")),
  };
  auto* func =
      Func("main", ast::VariableList{}, ty.f32(), body,
           ast::FunctionDecorationList{
               create<ast::StageDecoration>(ast::PipelineStage::kFragment),
           });

  AST().Functions().Add(func);

  ASSERT_TRUE(td.Determine()) << td.error();

  GeneratorImpl& gen = Build();

  ASSERT_TRUE(gen.EmitEntryPointData(func)) << gen.error();
  EXPECT_EQ(gen.result(), R"(struct main_in {
  float foo [[user(locn0)]];
  int bar [[user(locn1)]];
};

)");
}

TEST_F(MslGeneratorImplTest, Emit_Function_EntryPointData_Fragment_Output) {
  // [[location 0]] var<out> foo : f32;
  // [[location 1]] var<out> bar : i32;
  //
  // struct frag_main_out {
  //   float foo [[color(0)]];
  //   int bar [[color(1)]];
  // };

  auto* foo_var =
      Var("foo", ast::StorageClass::kOutput, ty.f32(), nullptr,
          ast::VariableDecorationList{create<ast::LocationDecoration>(0)});

  auto* bar_var =
      Var("bar", ast::StorageClass::kOutput, ty.i32(), nullptr,
          ast::VariableDecorationList{create<ast::LocationDecoration>(1)});

  td.RegisterVariableForTesting(foo_var);
  td.RegisterVariableForTesting(bar_var);

  AST().AddGlobalVariable(foo_var);
  AST().AddGlobalVariable(bar_var);

  auto body = ast::StatementList{
      create<ast::AssignmentStatement>(Expr("foo"), Expr("foo")),
      create<ast::AssignmentStatement>(Expr("bar"), Expr("bar")),
  };
  auto* func =
      Func("main", ast::VariableList{}, ty.f32(), body,
           ast::FunctionDecorationList{
               create<ast::StageDecoration>(ast::PipelineStage::kFragment),
           });

  AST().Functions().Add(func);

  ASSERT_TRUE(td.Determine()) << td.error();

  GeneratorImpl& gen = Build();

  ASSERT_TRUE(gen.EmitEntryPointData(func)) << gen.error();
  EXPECT_EQ(gen.result(), R"(struct main_out {
  float foo [[color(0)]];
  int bar [[color(1)]];
};

)");
}

TEST_F(MslGeneratorImplTest, Emit_Function_EntryPointData_Compute_Input) {
  // [[location 0]] var<in> foo : f32;
  // [[location 1]] var<in> bar : i32;
  //
  // -> Error, not allowed

  auto* foo_var =
      Var("foo", ast::StorageClass::kInput, ty.f32(), nullptr,
          ast::VariableDecorationList{create<ast::LocationDecoration>(0)});

  auto* bar_var =
      Var("bar", ast::StorageClass::kInput, ty.i32(), nullptr,
          ast::VariableDecorationList{create<ast::LocationDecoration>(1)});

  td.RegisterVariableForTesting(foo_var);
  td.RegisterVariableForTesting(bar_var);

  AST().AddGlobalVariable(foo_var);
  AST().AddGlobalVariable(bar_var);

  auto body = ast::StatementList{
      create<ast::AssignmentStatement>(Expr("foo"), Expr("foo")),
      create<ast::AssignmentStatement>(Expr("bar"), Expr("bar")),
  };
  auto* func =
      Func("main", ast::VariableList{}, ty.f32(), body,
           ast::FunctionDecorationList{
               create<ast::StageDecoration>(ast::PipelineStage::kCompute),
           });

  AST().Functions().Add(func);

  ASSERT_TRUE(td.Determine()) << td.error();

  GeneratorImpl& gen = Build();

  ASSERT_FALSE(gen.EmitEntryPointData(func)) << gen.error();
  EXPECT_EQ(gen.error(), R"(invalid location variable for pipeline stage)");
}

TEST_F(MslGeneratorImplTest, Emit_Function_EntryPointData_Compute_Output) {
  // [[location 0]] var<out> foo : f32;
  // [[location 1]] var<out> bar : i32;
  //
  // -> Error not allowed

  auto* foo_var =
      Var("foo", ast::StorageClass::kOutput, ty.f32(), nullptr,
          ast::VariableDecorationList{create<ast::LocationDecoration>(0)});

  auto* bar_var =
      Var("bar", ast::StorageClass::kOutput, ty.i32(), nullptr,
          ast::VariableDecorationList{create<ast::LocationDecoration>(1)});

  td.RegisterVariableForTesting(foo_var);
  td.RegisterVariableForTesting(bar_var);

  AST().AddGlobalVariable(foo_var);
  AST().AddGlobalVariable(bar_var);

  auto body = ast::StatementList{
      create<ast::AssignmentStatement>(Expr("foo"), Expr("foo")),
      create<ast::AssignmentStatement>(Expr("bar"), Expr("bar")),
  };
  auto* func =
      Func("main", ast::VariableList{}, ty.f32(), body,
           ast::FunctionDecorationList{
               create<ast::StageDecoration>(ast::PipelineStage::kCompute),
           });

  AST().Functions().Add(func);

  ASSERT_TRUE(td.Determine()) << td.error();

  GeneratorImpl& gen = Build();

  ASSERT_FALSE(gen.EmitEntryPointData(func)) << gen.error();
  EXPECT_EQ(gen.error(), R"(invalid location variable for pipeline stage)");
}

TEST_F(MslGeneratorImplTest, Emit_Function_EntryPointData_Builtins) {
  // Output builtins go in the output struct, input builtins will be passed
  // as input parameters to the entry point function.

  // [[builtin frag_coord]] var<in> coord : vec4<f32>;
  // [[builtin frag_depth]] var<out> depth : f32;
  //
  // struct main_out {
  //   float depth [[depth(any)]];
  // };

  auto* coord_var =
      Var("coord", ast::StorageClass::kInput, ty.vec4<f32>(), nullptr,
          ast::VariableDecorationList{
              create<ast::BuiltinDecoration>(ast::Builtin::kFragCoord)});

  auto* depth_var =
      Var("depth", ast::StorageClass::kOutput, ty.f32(), nullptr,
          ast::VariableDecorationList{
              create<ast::BuiltinDecoration>(ast::Builtin::kFragDepth)});

  td.RegisterVariableForTesting(coord_var);
  td.RegisterVariableForTesting(depth_var);

  AST().AddGlobalVariable(coord_var);
  AST().AddGlobalVariable(depth_var);

  auto body = ast::StatementList{create<ast::AssignmentStatement>(
      Expr("depth"), MemberAccessor("coord", "x"))};
  auto* func =
      Func("main", ast::VariableList{}, ty.void_(), body,
           ast::FunctionDecorationList{
               create<ast::StageDecoration>(ast::PipelineStage::kFragment),
           });

  AST().Functions().Add(func);

  ASSERT_TRUE(td.Determine()) << td.error();

  GeneratorImpl& gen = Build();

  ASSERT_TRUE(gen.EmitEntryPointData(func)) << gen.error();
  EXPECT_EQ(gen.result(), R"(struct main_out {
  float depth [[depth(any)]];
};

)");
}

}  // namespace
}  // namespace msl
}  // namespace writer
}  // namespace tint
