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

#ifndef SRC_TYPE_TYPE_H_
#define SRC_TYPE_TYPE_H_

#include <string>

#include "src/castable.h"
#include "src/clone_context.h"

namespace tint {

// Forward declarations
class ProgramBuilder;

namespace type {

/// Supported memory layouts for calculating sizes
enum class MemoryLayout { kUniformBuffer, kStorageBuffer };

/// Base class for a type in the system
class Type : public Castable<Type> {
 public:
  /// Move constructor
  Type(Type&&);
  ~Type() override;

  /// Clones this type and all transitive types using the `CloneContext` `ctx`.
  /// @param ctx the clone context
  /// @return the newly cloned type
  virtual Type* Clone(CloneContext* ctx) const = 0;

  /// @returns the name for this type. The type name is unique over all types.
  virtual std::string type_name() const = 0;

  /// @param mem_layout type of memory layout to use in calculation.
  /// @returns minimum size required for this type, in bytes.
  ///          0 for non-host shareable types.
  virtual uint64_t MinBufferBindingSize(MemoryLayout mem_layout) const;

  /// @param mem_layout type of memory layout to use in calculation.
  /// @returns base alignment for the type, in bytes.
  ///          0 for non-host shareable types.
  virtual uint64_t BaseAlignment(MemoryLayout mem_layout) const;

  /// @returns the pointee type if this is a pointer, `this` otherwise
  Type* UnwrapPtrIfNeeded();

  /// Removes all levels of aliasing and access control.
  /// This is just enough to assist with WGSL translation
  /// in that you want see through one level of pointer to get from an
  /// identifier-like expression as an l-value to its corresponding r-value,
  /// plus see through the wrappers on either side.
  /// @returns the completely unaliased type.
  Type* UnwrapIfNeeded();

  /// Returns the type found after:
  /// - removing all layers of aliasing and access control if they exist, then
  /// - removing the pointer, if it exists, then
  /// - removing all further layers of aliasing or access control, if they exist
  /// @returns the unwrapped type
  Type* UnwrapAll();

  /// @returns true if this type is a scalar
  bool is_scalar();
  /// @returns true if this type is a float scalar
  bool is_float_scalar();
  /// @returns true if this type is a float matrix
  bool is_float_matrix();
  /// @returns true if this type is a float vector
  bool is_float_vector();
  /// @returns true if this type is a float scalar or vector
  bool is_float_scalar_or_vector();
  /// @returns ture if this type is an integer scalar
  bool is_integer_scalar();
  /// @returns true if this type is a signed integer vector
  bool is_signed_integer_vector();
  /// @returns true if this type is an unsigned vector
  bool is_unsigned_integer_vector();
  /// @returns true if this type is an unsigned scalar or vector
  bool is_unsigned_scalar_or_vector();
  /// @returns true if this type is a signed scalar or vector
  bool is_signed_scalar_or_vector();
  /// @returns true if this type is an integer scalar or vector
  bool is_integer_scalar_or_vector();

 protected:
  Type();

  /// A helper method for cloning the `Type` `t` if it is not null.
  /// If `t` is null, then `Clone()` returns null.
  /// @param b the program builder to clone `n` into
  /// @param t the `Type` to clone (if not null)
  /// @return the cloned type
  template <typename T>
  static T* Clone(ProgramBuilder* b, const T* t) {
    return (t != nullptr) ? static_cast<T*>(t->Clone(b)) : nullptr;
  }
};

}  // namespace type
}  // namespace tint

#endif  // SRC_TYPE_TYPE_H_
