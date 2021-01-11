// Copyright 2021 The Tint Authors.
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

#ifndef SRC_WRITER_TEST_NAMER_H_
#define SRC_WRITER_TEST_NAMER_H_

#include <string>

#include "src/namer.h"

namespace tint {
namespace writer {

/// A namer which returns the provided name prefixed with `test_`.
class TestNamer : public Namer {
 public:
  /// Constructor
  /// @param mod the module to retrieve names from
  explicit TestNamer(const ast::Module* mod);
  /// Destructor
  ~TestNamer() override;

  /// Returns `name`
  /// @param sym the symbol
  /// @returns `name` or "" if not found
  std::string NameFor(const Symbol& sym) override;
};

}  // namespace writer
}  // namespace tint

#endif  // SRC_WRITER_TEST_NAMER_H_