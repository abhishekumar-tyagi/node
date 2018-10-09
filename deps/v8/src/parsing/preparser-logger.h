// Copyright 2018 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_PARSING_PREPARSER_LOGGER_H_
#define V8_PARSING_PREPARSER_LOGGER_H_

namespace v8 {
namespace internal {

class PreParserLogger final {
 public:
  PreParserLogger() : end_(-1), num_parameters_(-1), num_inner_functions_(-1) {}

  void LogFunction(int end, int num_parameters, int num_inner_functions) {
    end_ = end;
    num_parameters_ = num_parameters;
    num_inner_functions_ = num_inner_functions;
  }

  int end() const { return end_; }
  int num_parameters() const { return num_parameters_; }
  int num_inner_functions() const { return num_inner_functions_; }

 private:
  int end_;
  // For function entries.
  int num_parameters_;
  int num_inner_functions_;
};

}  // namespace internal
}  // namespace v8.

#endif  // V8_PARSING_PREPARSER_LOGGER_H_
