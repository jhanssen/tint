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

#ifndef SRC_AST_NODE_H_
#define SRC_AST_NODE_H_

#include <ostream>
#include <string>
#include <vector>

#include "src/castable.h"
#include "src/source.h"

namespace tint {

// Forward declarations
class CloneContext;
namespace type {
class Type;
}

namespace ast {

/// AST base class node
class Node : public Castable<Node> {
 public:
  ~Node() override;

  /// Clones this node and all transitive child nodes using the `CloneContext`
  /// `ctx`.
  /// @note Semantic information such as resolved expression type and intrinsic
  /// information is not cloned.
  /// @param ctx the clone context
  /// @return the newly cloned node
  virtual Node* Clone(CloneContext* ctx) const = 0;

  /// @returns the node source data
  const Source& source() const { return source_; }

  /// @returns true if the node is valid
  virtual bool IsValid() const = 0;

  /// Writes a representation of the node to the output stream
  /// @param out the stream to write to
  /// @param indent number of spaces to indent the node when writing
  virtual void to_str(std::ostream& out, size_t indent) const = 0;

  /// Convenience wrapper around the to_str() method.
  /// @returns the node as a string
  std::string str() const;

 protected:
  /// Create a new node
  /// @param source the input source for the node
  explicit Node(const Source& source);
  /// Move constructor
  Node(Node&&);

  /// Writes indent into stream
  /// @param out the stream to write to
  /// @param indent the number of spaces to write
  void make_indent(std::ostream& out, size_t indent) const;

 private:
  Node(const Node&) = delete;

  Source const source_;
};

}  // namespace ast
}  // namespace tint

#endif  // SRC_AST_NODE_H_
