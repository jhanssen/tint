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

#ifndef SRC_AST_FUNCTION_H_
#define SRC_AST_FUNCTION_H_

#include <memory>
#include <ostream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "src/ast/binding_decoration.h"
#include "src/ast/block_statement.h"
#include "src/ast/builtin_decoration.h"
#include "src/ast/expression.h"
#include "src/ast/function_decoration.h"
#include "src/ast/group_decoration.h"
#include "src/ast/location_decoration.h"
#include "src/ast/node.h"
#include "src/ast/pipeline_stage.h"
#include "src/ast/statement.h"
#include "src/ast/variable.h"
#include "src/symbol.h"
#include "src/type/sampler_type.h"
#include "src/type/type.h"

namespace tint {
namespace ast {

/// A Function statement.
class Function : public Castable<Function, Node> {
 public:
  /// Information about a binding
  struct BindingInfo {
    /// The binding decoration
    BindingDecoration* binding = nullptr;
    /// The group decoration
    GroupDecoration* group = nullptr;
  };

  /// Create a function
  /// @param source the variable source
  /// @param symbol the function symbol
  /// @param params the function parameters
  /// @param return_type the return type
  /// @param body the function body
  /// @param decorations the function decorations
  Function(const Source& source,
           Symbol symbol,
           VariableList params,
           type::Type* return_type,
           BlockStatement* body,
           FunctionDecorationList decorations);
  /// Move constructor
  Function(Function&&);

  ~Function() override;

  /// @returns the function symbol
  Symbol symbol() const { return symbol_; }
  /// @returns the function params
  const VariableList& params() const { return params_; }

  /// @returns the decorations attached to this function
  const FunctionDecorationList& decorations() const { return decorations_; }

  /// @returns the workgroup size {x, y, z} for the function. {1, 1, 1} will be
  /// return if no workgroup size was set.
  std::tuple<uint32_t, uint32_t, uint32_t> workgroup_size() const;

  /// @returns the functions pipeline stage or None if not set
  PipelineStage pipeline_stage() const;

  /// @returns true if this function is an entry point
  bool IsEntryPoint() const { return pipeline_stage() != PipelineStage::kNone; }

  /// Adds the given variable to the list of referenced module variables if it
  /// is not already included.
  /// @param var the module variable to add
  void add_referenced_module_variable(Variable* var);
  /// Adds the given variable to the list of locally referenced module variables
  /// if it is not already included.
  /// @param var the module variable to add
  void add_local_referenced_module_variable(Variable* var);
  /// Note: If this function calls other functions, the return will also include
  /// all of the referenced variables from the callees.
  /// @returns the referenced module variables
  const std::vector<Variable*>& referenced_module_variables() const {
    return referenced_module_vars_;
  }
  /// @returns the locally referenced module variables
  const std::vector<Variable*>& local_referenced_module_variables() const {
    return local_referenced_module_vars_;
  }
  /// Retrieves any referenced location variables
  /// @returns the <variable, decoration> pair.
  const std::vector<std::pair<Variable*, LocationDecoration*>>
  referenced_location_variables() const;
  /// Retrieves any referenced builtin variables
  /// @returns the <variable, decoration> pair.
  const std::vector<std::pair<Variable*, BuiltinDecoration*>>
  referenced_builtin_variables() const;
  /// Retrieves any referenced uniform variables. Note, the variables must be
  /// decorated with both binding and group decorations.
  /// @returns the referenced uniforms
  const std::vector<std::pair<Variable*, Function::BindingInfo>>
  referenced_uniform_variables() const;
  /// Retrieves any referenced storagebuffer variables. Note, the variables
  /// must be decorated with both binding and group decorations.
  /// @returns the referenced storagebuffers
  const std::vector<std::pair<Variable*, Function::BindingInfo>>
  referenced_storagebuffer_variables() const;
  /// Retrieves any referenced regular Sampler variables. Note, the
  /// variables must be decorated with both binding and group decorations.
  /// @returns the referenced storagebuffers
  const std::vector<std::pair<Variable*, Function::BindingInfo>>
  referenced_sampler_variables() const;
  /// Retrieves any referenced comparison Sampler variables. Note, the
  /// variables must be decorated with both binding and group decorations.
  /// @returns the referenced storagebuffers
  const std::vector<std::pair<Variable*, Function::BindingInfo>>
  referenced_comparison_sampler_variables() const;
  /// Retrieves any referenced sampled textures variables. Note, the
  /// variables must be decorated with both binding and group decorations.
  /// @returns the referenced sampled textures
  const std::vector<std::pair<Variable*, Function::BindingInfo>>
  referenced_sampled_texture_variables() const;
  /// Retrieves any referenced multisampled textures variables. Note, the
  /// variables must be decorated with both binding and group decorations.
  /// @returns the referenced sampled textures
  const std::vector<std::pair<Variable*, Function::BindingInfo>>
  referenced_multisampled_texture_variables() const;

  /// Retrieves any locally referenced builtin variables
  /// @returns the <variable, decoration> pairs.
  const std::vector<std::pair<Variable*, BuiltinDecoration*>>
  local_referenced_builtin_variables() const;

  /// Adds an ancestor entry point
  /// @param ep the entry point ancestor
  void add_ancestor_entry_point(Symbol ep);
  /// @returns the ancestor entry points
  const std::vector<Symbol>& ancestor_entry_points() const {
    return ancestor_entry_points_;
  }
  /// Checks if the given entry point is an ancestor
  /// @param sym the entry point symbol
  /// @returns true if `sym` is an ancestor entry point of this function
  bool HasAncestorEntryPoint(Symbol sym) const;

  /// @returns the function return type.
  type::Type* return_type() const { return return_type_; }
  /// @returns a pointer to the last statement of the function or nullptr if
  // function is empty
  const Statement* get_last_statement() const;

  /// @returns the function body
  const BlockStatement* body() const { return body_; }
  /// @returns the function body
  BlockStatement* body() { return body_; }

  /// Clones this node and all transitive child nodes using the `CloneContext`
  /// `ctx`.
  /// @note Semantic information such as resolved expression type and intrinsic
  /// information is not cloned.
  /// @param ctx the clone context
  /// @return the newly cloned node
  Function* Clone(CloneContext* ctx) const override;

  /// @returns true if the symbol and type are both present
  bool IsValid() const override;

  /// Writes a representation of the node to the output stream
  /// @param out the stream to write to
  /// @param indent number of spaces to indent the node when writing
  void to_str(std::ostream& out, size_t indent) const override;

  /// @returns the type name for this function
  std::string type_name() const;

 private:
  Function(const Function&) = delete;
  const std::vector<std::pair<Variable*, Function::BindingInfo>>
  ReferencedSamplerVariablesImpl(type::SamplerKind kind) const;
  const std::vector<std::pair<Variable*, Function::BindingInfo>>
  ReferencedSampledTextureVariablesImpl(bool multisampled) const;

  Symbol const symbol_;
  VariableList const params_;
  type::Type* const return_type_;
  BlockStatement* const body_;

  std::vector<Variable*> referenced_module_vars_;        // Semantic info
  std::vector<Variable*> local_referenced_module_vars_;  // Semantic info
  std::vector<Symbol> ancestor_entry_points_;            // Semantic info
  FunctionDecorationList decorations_;                   // Semantic info
};

/// A list of functions
class FunctionList : public std::vector<Function*> {
 public:
  /// Appends f to the end of the list
  /// @param f the function to append to this list
  void Add(Function* f) { this->emplace_back(f); }

  /// Returns the function with the given name
  /// @param sym the function symbol to search for
  /// @returns the associated function or nullptr if none exists
  Function* Find(Symbol sym) const;

  /// Returns the function with the given name
  /// @param sym the function symbol to search for
  /// @param stage the pipeline stage
  /// @returns the associated function or nullptr if none exists
  Function* Find(Symbol sym, PipelineStage stage) const;

  /// @param stage the pipeline stage
  /// @returns true if the Builder contains an entrypoint function with
  /// the given stage
  bool HasStage(PipelineStage stage) const;
};

}  // namespace ast
}  // namespace tint

#endif  // SRC_AST_FUNCTION_H_
