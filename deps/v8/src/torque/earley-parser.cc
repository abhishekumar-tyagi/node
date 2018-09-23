// Copyright 2018 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "src/torque/ast.h"
#include "src/torque/earley-parser.h"
#include "src/torque/utils.h"

namespace v8 {
namespace internal {
namespace torque {

namespace {

void UpdateSourcePosition(InputPosition from, InputPosition to,
                          SourcePosition* pos) {
  while (from != to) {
    if (*from == '\n') {
      pos->line += 1;
      pos->column = 0;
    } else {
      pos->column += 1;
    }
    ++from;
  }
}

}  // namespace

base::Optional<ParseResult> Rule::RunAction(const Item* completed_item,
                                            const LexerResult& tokens) const {
  std::vector<ParseResult> results;
  for (const Item* child : completed_item->Children()) {
    if (!child) continue;
    base::Optional<ParseResult> child_result =
        child->left()->RunAction(child, tokens);
    if (child_result) results.push_back(std::move(*child_result));
  }
  MatchedInput matched_input = completed_item->GetMatchedInput(tokens);
  CurrentSourcePosition::Scope pos_scope(matched_input.pos);
  ParseResultIterator iterator(std::move(results), matched_input);
  return action_(&iterator);
}

Symbol& Symbol::operator=(std::initializer_list<Rule> rules) {
  rules_.clear();
  for (const Rule& rule : rules) {
    AddRule(rule);
  }
  return *this;
}

std::vector<const Item*> Item::Children() const {
  std::vector<const Item*> children;
  for (const Item* current = this; current->prev_; current = current->prev_) {
    children.push_back(current->child_);
  }
  // The above loop collects the child nodes in reversed order.
  std::reverse(children.begin(), children.end());
  DCHECK_EQ(children.size(), right().size());
  return children;
}

std::string Item::SplitByChildren(const LexerResult& tokens) const {
  if (right().size() == 1) {
    if (const Item* child = Children()[0])
      return child->SplitByChildren(tokens);
  }
  std::stringstream s;
  bool first = true;
  for (const Item* item : Children()) {
    if (!item) continue;
    if (!first) s << "  ";
    s << item->GetMatchedInput(tokens).ToString();
    first = false;
  }
  return s.str();
}

void Item::CheckAmbiguity(const Item& other, const LexerResult& tokens) const {
  DCHECK(*this == other);
  if (child_ != other.child_) {
    std::stringstream s;
    s << "Ambiguous grammer rules for \""
      << child_->GetMatchedInput(tokens).ToString() << "\":\n   "
      << child_->SplitByChildren(tokens) << "\nvs\n   "
      << other.child_->SplitByChildren(tokens);
    ReportError(s.str());
  }
  if (prev_ != other.prev_) {
    std::stringstream s;
    s << "Ambiguous grammer rules for \"" << GetMatchedInput(tokens).ToString()
      << "\":\n   " << SplitByChildren(tokens) << "  ...\nvs\n   "
      << other.SplitByChildren(tokens) << "  ...";
    ReportError(s.str());
  }
}

LexerResult Lexer::RunLexer(const std::string& input) {
  LexerResult result;
  InputPosition const begin = input.c_str();
  InputPosition const end = begin + input.size();
  InputPosition pos = begin;
  InputPosition token_start = pos;
  CurrentSourcePosition::Scope scope(
      SourcePosition{CurrentSourceFile::Get(), 0, 0});
  match_whitespace_(&pos);
  while (pos != end) {
    UpdateSourcePosition(token_start, pos, &CurrentSourcePosition::Get());
    token_start = pos;
    Symbol* symbol = MatchToken(&pos, end);
    if (!symbol) {
      ReportError("Lexer Error: unknown token " +
                  StringLiteralQuote(std::string(
                      token_start, token_start + std::min<ptrdiff_t>(
                                                     end - token_start, 10))));
    }
    result.token_symbols.push_back(symbol);
    result.token_contents.push_back(
        {token_start, pos, CurrentSourcePosition::Get()});
    match_whitespace_(&pos);
  }
  UpdateSourcePosition(token_start, pos, &CurrentSourcePosition::Get());
  // Add an additional token position to simplify corner cases.
  result.token_contents.push_back({pos, pos, CurrentSourcePosition::Get()});
  return result;
}

Symbol* Lexer::MatchToken(InputPosition* pos, InputPosition end) {
  InputPosition token_start = *pos;
  Symbol* symbol = nullptr;
  // Find longest matching pattern.
  for (std::pair<const PatternFunction, Symbol>& pair : patterns_) {
    InputPosition token_end = token_start;
    PatternFunction matchPattern = pair.first;
    if (matchPattern(&token_end) && token_end > *pos) {
      *pos = token_end;
      symbol = &pair.second;
    }
  }
  // Check if matched pattern coincides with a keyword. Prefer the keyword in
  // this case.
  if (*pos != token_start) {
    auto found_keyword = keywords_.find(std::string(token_start, *pos));
    if (found_keyword != keywords_.end()) {
      return &found_keyword->second;
    }
    return symbol;
  }
  // Now check for a keyword (that doesn't overlap with a pattern).
  // Iterate from the end to ensure that if one keyword is a prefix of another,
  // we first try to match the longer one.
  for (auto it = keywords_.rbegin(); it != keywords_.rend(); ++it) {
    const std::string& keyword = it->first;
    if (static_cast<size_t>(end - *pos) < keyword.size()) continue;
    if (keyword == std::string(*pos, *pos + keyword.size())) {
      *pos += keyword.size();
      return &it->second;
    }
  }
  return nullptr;
}

// This is an implementation of Earley's parsing algorithm
// (https://en.wikipedia.org/wiki/Earley_parser).
const Item* RunEarleyAlgorithm(
    Symbol* start, const LexerResult& tokens,
    std::unordered_set<Item, base::hash<Item>>* processed) {
  // Worklist for items at the current position.
  std::vector<Item> worklist;
  // Worklist for items at the next position.
  std::vector<Item> future_items;
  CurrentSourcePosition::Scope source_position(
      SourcePosition{CurrentSourceFile::Get(), 0, 0});
  std::vector<const Item*> completed_items;
  std::unordered_map<std::pair<size_t, Symbol*>, std::set<const Item*>,
                     base::hash<std::pair<size_t, Symbol*>>>
      waiting;

  std::vector<const Item*> debug_trace;

  // Start with one top_level symbol mapping to the start symbol of the grammar.
  // This simplifies things because the start symbol might have several
  // rules.
  Symbol top_level;
  top_level.AddRule(Rule({start}));
  worklist.push_back(Item{top_level.rule(0), 0, 0, 0});

  size_t input_length = tokens.token_symbols.size();

  for (size_t pos = 0; pos <= input_length; ++pos) {
    while (!worklist.empty()) {
      auto insert_result = processed->insert(worklist.back());
      const Item& item = *insert_result.first;
      DCHECK_EQ(pos, item.pos());
      MatchedInput last_token = tokens.token_contents[pos];
      CurrentSourcePosition::Get() = last_token.pos;
      bool is_new = insert_result.second;
      if (!is_new) item.CheckAmbiguity(worklist.back(), tokens);
      worklist.pop_back();
      if (!is_new) continue;

      debug_trace.push_back(&item);
      if (item.IsComplete()) {
        // 'Complete' phase: Advance all items that were waiting to match this
        // symbol next.
        for (const Item* parent : waiting[{item.start(), item.left()}]) {
          worklist.push_back(parent->Advance(pos, &item));
        }
      } else {
        Symbol* next = item.NextSymbol();
        // 'Scan' phase: Check if {next} is the next symbol in the input (this
        // is never the case if {next} is a non-terminal).
        if (pos < tokens.token_symbols.size() &&
            tokens.token_symbols[pos] == next) {
          future_items.push_back(item.Advance(pos + 1, nullptr));
        }
        // 'Predict' phase: Add items for every rule of the non-terminal.
        if (!next->IsTerminal()) {
          // Remember that this item is waiting for completion with {next}.
          waiting[{pos, next}].insert(&item);
        }
        for (size_t i = 0; i < next->rule_number(); ++i) {
          Rule* rule = next->rule(i);
          auto already_completed =
              processed->find(Item{rule, rule->right().size(), pos, pos});
          // As discussed in section 3 of
          //    Aycock, John, and R. Nigel Horspool. "Practical earley
          //    parsing." The Computer Journal 45.6 (2002): 620-630.
          // Earley parsing has the following problem with epsilon rules:
          // When we complete an item that started at the current position
          // (that is, it matched zero tokens), we might not yet have
          // predicted all items it can complete with. Thus we check for the
          // existence of such items here and complete them immediately.
          if (already_completed != processed->end()) {
            worklist.push_back(item.Advance(pos, &*already_completed));
          } else {
            worklist.push_back(Item{rule, 0, pos, pos});
          }
        }
      }
    }
    std::swap(worklist, future_items);
  }

  auto final_item =
      processed->find(Item{top_level.rule(0), 1, 0, input_length});
  if (final_item != processed->end()) {
    // Success: The {top_level} rule matches the complete input.
    return final_item->Children()[0];
  }
  std::string reason;
  const Item& last_item = *debug_trace.back();
  if (last_item.pos() < tokens.token_symbols.size()) {
    std::string next_token = tokens.token_contents[last_item.pos()].ToString();
    reason = "unexpected token \"" + next_token + "\"";
  } else {
    reason = "unexpected end of input";
  }
  ReportError("Parser Error: " + reason);
}

// static
bool Grammar::MatchChar(int (*char_class)(int), InputPosition* pos) {
  if (**pos && char_class(static_cast<unsigned char>(**pos))) {
    ++*pos;
    return true;
  }
  return false;
}

// static
bool Grammar::MatchChar(bool (*char_class)(char), InputPosition* pos) {
  if (**pos && char_class(**pos)) {
    ++*pos;
    return true;
  }
  return false;
}

// static
bool Grammar::MatchString(const char* s, InputPosition* pos) {
  InputPosition current = *pos;
  for (; *s != 0; ++s, ++current) {
    if (*s != *current) return false;
  }
  *pos = current;
  return true;
}

// static
bool Grammar::MatchAnyChar(InputPosition* pos) {
  return MatchChar([](char c) { return true; }, pos);
}

}  // namespace torque
}  // namespace internal
}  // namespace v8
