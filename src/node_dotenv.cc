#include "node_dotenv.h"
#include "env-inl.h"
#include "node_file.h"
#include "uv.h"

namespace node {

using v8::NewStringType;
using v8::String;

std::optional<std::string> Dotenv::GetPathFromArgs(
    const std::vector<std::string>& args) {
  std::string_view flag = "--env-file";
  // Match the last `--env-file`
  // This is required to imitate the default behavior of Node.js CLI argument
  // matching.
  auto path =
      std::find_if(args.rbegin(), args.rend(), [&flag](const std::string& arg) {
        return strncmp(arg.c_str(), flag.data(), flag.size()) == 0;
      });

  if (path == args.rend()) {
    return std::nullopt;
  }

  auto equal_char = path->find('=');

  if (equal_char != std::string::npos) {
    return path->substr(equal_char + 1);
  }

  auto next_arg = std::prev(path);

  if (next_arg == args.rend()) {
    return std::nullopt;
  }

  return *next_arg;
}

void Dotenv::SetEnvironment(node::Environment* env) {
  if (store_.empty()) {
    return;
  }

  auto isolate = env->isolate();

  for (const auto& entry : store) {
    auto key = entry.first;
    auto value = entry.second;
    env->env_vars()->Set(
        isolate,
        v8::String::NewFromUtf8(
            isolate, key.data(), NewStringType::kNormal, key.size())
            .ToLocalChecked(),
        v8::String::NewFromUtf8(
            isolate, value.data(), NewStringType::kNormal, value.size())
            .ToLocalChecked());
  }
}

void Dotenv::ParsePath(const std::string_view path) {
  uv_fs_t req;
  auto defer_req_cleanup = OnScopeLeave([&req]() { uv_fs_req_cleanup(&req); });

  uv_file file = uv_fs_open(nullptr, &req, path.data(), 0, 438, nullptr);
  if (req.result < 0) {
    // req will be cleaned up by scope leave.
    return;
  }
  uv_fs_req_cleanup(&req);

  auto defer_close = OnScopeLeave([file]() {
    uv_fs_t close_req;
    CHECK_EQ(0, uv_fs_close(nullptr, &close_req, file, nullptr));
    uv_fs_req_cleanup(&close_req);
  });

  std::string result{};
  char buffer[8192];
  uv_buf_t buf = uv_buf_init(buffer, sizeof(buffer));

  while (true) {
    auto r = uv_fs_read(nullptr, &req, file, &buf, 1, -1, nullptr);
    if (req.result < 0) {
      // req will be cleaned up by scope leave.
      return;
    }
    uv_fs_req_cleanup(&req);
    if (r <= 0) {
      break;
    }
    result.append(buf.base, r);
  }

  using std::string_view_literals::operator""sv;
  auto lines = SplitString(result, "\n"sv);

  for (const auto& line : lines) {
    ParseLine(line);
  }
}

void Dotenv::AssignNodeOptionsIfAvailable(std::string* node_options) {
  auto match = store_.find("NODE_OPTIONS");

  if (match != store_.end()) {
    *node_options = match->second;
  }
}

void Dotenv::ParseLine(const std::string_view line) {
  auto equal_index = line.find('=');

  if (equal_index == std::string_view::npos) {
    return;
  }

  auto key = line.substr(0, equal_index);

  // Remove leading and trailing space characters from key.
  while (!key.empty() && std::isspace(key.front())) key.remove_prefix(1);
  while (!key.empty() && std::isspace(key.back())) key.remove_suffix(1);

  // Omit lines with comments
  if (key.front() == '#' || key.empty()) {
    return;
  }

  auto value = std::string(line.substr(equal_index + 1));

  // Might start and end with `"' characters.
  auto quotation_index = value.find_first_of("`\"'");

  if (quotation_index == 0) {
    auto quote_character = value[quotation_index];
    value.erase(0, 1);

    auto end_quotation_index = value.find_last_of(quote_character);

    // We couldn't find the closing quotation character. Terminate.
    if (end_quotation_index == std::string::npos) {
      return;
    }

    value.erase(end_quotation_index);
  } else {
    auto hash_index = value.find('#');

    // Remove any inline comments
    if (hash_index != std::string::npos) {
      value.erase(hash_index);
    }

    // Remove any leading/trailing spaces from unquoted values.
    while (!value.empty() && std::isspace(value.front())) value.erase(0, 1);
    while (!value.empty() && std::isspace(value.back()))
      value.erase(value.size() - 1);
  }

  store_.emplace(key, value);
}

}  // namespace node
