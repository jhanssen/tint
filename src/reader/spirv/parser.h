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

#ifndef SRC_READER_SPIRV_PARSER_H_
#define SRC_READER_SPIRV_PARSER_H_

#include <cstdint>
#include <memory>
#include <vector>

#include "src/reader/reader.h"

namespace tint {
namespace reader {
namespace spirv {

class ParserImpl;

/// Parser for SPIR-V source data
/// [DEPRECATED] - Use Parse()
class Parser : public Reader {
 public:
  /// Creates a new parser
  /// @param input the input data to parse
  explicit Parser(const std::vector<uint32_t>& input);
  /// Destructor
  ~Parser() override;

  /// Run the parser
  /// @returns true if the parse was successful, false otherwise.
  bool Parse() override;

  /// @returns the program. The program builder in the parser will be reset
  /// after this.
  Program program() override;

 private:
  std::unique_ptr<ParserImpl> impl_;
};

/// Parses the SPIR-V source data, returning the parsed program.
/// If the source data fails to parse then the returned
/// `program.Diagnostics.contains_errors()` will be true, and the
/// `program.Diagnostics()` will describe the error.
/// @param input the source data
/// @returns the parsed program
Program Parse(const std::vector<uint32_t>& input);

}  // namespace spirv
}  // namespace reader
}  // namespace tint

#endif  // SRC_READER_SPIRV_PARSER_H_
