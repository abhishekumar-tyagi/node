﻿#ifndef ENVVALUE_H
#define ENVVALUE_H
#include <string>
#include <vector>

#include "VariablePosition.h"
namespace cppnv {
struct EnvValue
{
  std::string* value;
  bool is_parsing_variable = false;
  std::vector<VariablePosition*>* interpolations;
  int interpolation_index = 0;
  bool quoted = false;
  bool triple_quoted = false;
  bool double_quoted = false;
  bool triple_double_quoted = false;
  bool implicit_double_quote = false;
  int value_index = 0;
  bool is_already_interpolated = false;
  bool is_being_interpolated = false;
  bool did_over_flow = false;
  int back_slash_streak = 0;
  int single_quote_streak = 0;
  int double_quote_streak = 0;
  std::string* own_buffer;

  //todo: remove
  void clip_own_buffer(int length) const
  {
    own_buffer->resize(length);
  }

  bool has_own_buffer() const
  {
    return own_buffer != nullptr;
  }

  void set_own_buffer(std::string* buff)
  {
    delete own_buffer;
    own_buffer = buff;
    value = buff;
  }

  EnvValue(): value(nullptr), own_buffer(nullptr)
  {
    interpolations = new std::vector<VariablePosition*>();
  }

  ~EnvValue()
  {
    for (const auto interpolation : *interpolations)
    {
      delete interpolation;
    }
    delete own_buffer;
    delete interpolations;
  }
};
}  //namespace cppnv
#endif  // ENVVALUE_H
