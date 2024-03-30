#include "path.h"
#include <string>
#include <vector>
#include "env-inl.h"
#include "node_internals.h"
#include "util.h"

namespace node {

#ifdef _WIN32
bool IsPathSeparator(const char c) noexcept {
  return c == kPathSeparator || c == '/';
}
#else   // POSIX
bool IsPathSeparator(const char c) noexcept {
  return c == kPathSeparator;
}
#endif  // _WIN32

std::string NormalizeString(const std::string_view path,
                            bool allowAboveRoot,
                            const std::string_view separator) {
  std::string res;
  int lastSegmentLength = 0;
  int lastSlash = -1;
  int dots = 0;
  char code = 0;
  for (size_t i = 0; i <= path.size(); ++i) {
    if (i < path.size()) {
      code = path[i];
    } else if (IsPathSeparator(code)) {
      break;
    } else {
      code = '/';
    }

    if (IsPathSeparator(code)) {
      if (lastSlash == static_cast<int>(i - 1) || dots == 1) {
        // NOOP
      } else if (dots == 2) {
        int len = res.length();
        if (len < 2 || lastSegmentLength != 2 || res[len - 1] != '.' ||
            res[len - 2] != '.') {
          if (len > 2) {
            auto lastSlashIndex = res.find_last_of(separator);
            if (lastSlashIndex == std::string::npos) {
              res = "";
              lastSegmentLength = 0;
            } else {
              res = res.substr(0, lastSlashIndex);
              len = res.length();
              lastSegmentLength = len - 1 - res.find_last_of(separator);
            }
            lastSlash = i;
            dots = 0;
            continue;
          } else if (len != 0) {
            res = "";
            lastSegmentLength = 0;
            lastSlash = i;
            dots = 0;
            continue;
          }
        }

        if (allowAboveRoot) {
          res += res.length() > 0 ? std::string(separator) + ".." : "..";
          lastSegmentLength = 2;
        }
      } else {
        if (!res.empty()) {
          res += std::string(separator) +
                 std::string(path.substr(lastSlash + 1, i - (lastSlash + 1)));
        } else {
          res = path.substr(lastSlash + 1, i - (lastSlash + 1));
        }
        lastSegmentLength = i - lastSlash - 1;
      }
      lastSlash = i;
      dots = 0;
    } else if (code == '.' && dots != -1) {
      ++dots;
    } else {
      dots = -1;
    }
  }

  return res;
}

#ifdef _WIN32
bool IsWindowsDeviceRoot(const char c) noexcept {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

std::string PathResolve(Environment* env,
                        const std::vector<std::string_view>& paths) {
  std::string resolvedDevice = "";
  std::string resolvedTail = "";
  bool resolvedAbsolute = false;
  const size_t numArgs = paths.size();
  auto cwd = env->GetCwd(env->exec_path());

  for (int i = numArgs - 1; i >= -1; i--) {
    std::string path;
    if (i >= 0) {
      path = std::string(paths[i]);
    } else if (resolvedDevice.empty()) {
      path = cwd;
    } else {
      // Windows has the concept of drive-specific current working
      // directories. If we've resolved a drive letter but not yet an
      // absolute path, get cwd for that drive, or the process cwd if
      // the drive cwd is not available. We're sure the device is not
      // a UNC path at this points, because UNC paths are always absolute.
      std::string resolvedDevicePath;
      const std::string envvar = "=" + resolvedDevice;
      credentials::SafeGetenv(envvar.c_str(), &resolvedDevicePath);
      path = resolvedDevicePath.empty() ? cwd : resolvedDevicePath;

      // Verify that a cwd was found and that it actually points
      // to our drive. If not, default to the drive's root.
      if (path.empty() ||
          (ToLower(path.substr(0, 2)) != ToLower(resolvedDevice) &&
           path[2] == '/')) {
        path = resolvedDevice + "\\";
      }
    }

    const size_t len = path.length();
    int rootEnd = 0;
    std::string device = "";
    bool isAbsolute = false;
    const char code = path[0];

    // Try to match a root
    if (len == 1) {
      if (IsPathSeparator(code)) {
        // `path` contains just a path separator
        rootEnd = 1;
        isAbsolute = true;
      }
    } else if (IsPathSeparator(code)) {
      // Possible UNC root

      // If we started with a separator, we know we at least have an
      // absolute path of some kind (UNC or otherwise)
      isAbsolute = true;

      if (IsPathSeparator(path[1])) {
        // Matched double path separator at beginning
        size_t j = 2;
        size_t last = j;
        // Match 1 or more non-path separators
        while (j < len && !IsPathSeparator(path[j])) {
          j++;
        }
        if (j < len && j != last) {
          const std::string firstPart = path.substr(last, j - last);
          // Matched!
          last = j;
          // Match 1 or more path separators
          while (j < len && IsPathSeparator(path[j])) {
            j++;
          }
          if (j < len && j != last) {
            // Matched!
            last = j;
            // Match 1 or more non-path separators
            while (j < len && !IsPathSeparator(path[j])) {
              j++;
            }
            if (j == len || j != last) {
              // We matched a UNC root
              device = "\\\\" + firstPart + "\\" + path.substr(last, j - last);
              rootEnd = j;
            }
          }
        }
      }
    } else if (IsWindowsDeviceRoot(code) && path[1] == ':') {
      // Possible device root
      device = path.substr(0, 2);
      rootEnd = 2;
      if (len > 2 && IsPathSeparator(path[2])) {
        // Treat separator following drive name as an absolute path
        // indicator
        isAbsolute = true;
        rootEnd = 3;
      }
    }

    if (!device.empty()) {
      if (!resolvedDevice.empty()) {
        if (ToLower(device) != ToLower(resolvedDevice)) {
          // This path points to another device so it is not applicable
          continue;
        }
      } else {
        resolvedDevice = device;
      }
    }

    if (resolvedAbsolute) {
      if (!resolvedDevice.empty()) {
        break;
      }
    } else {
      resolvedTail = path.substr(rootEnd) + "\\" + resolvedTail;
      resolvedAbsolute = isAbsolute;
      if (isAbsolute && !resolvedDevice.empty()) {
        break;
      }
    }
  }

  // At this point the path should be resolved to a full absolute path,
  // but handle relative paths to be safe (might happen when process.cwd()
  // fails)

  // Normalize the tail path
  resolvedTail = NormalizeString(resolvedTail, !resolvedAbsolute, "\\");

  if (resolvedAbsolute) {
    return resolvedDevice + "\\" + resolvedTail;
  }

  if (!resolvedDevice.empty() || !resolvedTail.empty()) {
    return resolvedDevice + resolvedTail;
  }

  return ".";
}
#else   // _WIN32
std::string PathResolve(Environment* env,
                        const std::vector<std::string_view>& paths) {
  std::string resolvedPath;
  bool resolvedAbsolute = false;
  auto cwd = env->GetCwd(env->exec_path());
  const size_t numArgs = paths.size();

  for (int i = numArgs - 1; i >= -1 && !resolvedAbsolute; i--) {
    const std::string& path =
        (i >= 0) ? std::string(paths[i]) : env->GetCwd(env->exec_path());

    if (!path.empty()) {
      resolvedPath = std::string(path) + "/" + resolvedPath;

      if (path.front() == '/') {
        resolvedAbsolute = true;
        break;
      }
    }
  }

  // Normalize the path
  auto normalizedPath = NormalizeString(resolvedPath, !resolvedAbsolute, "/");

  if (resolvedAbsolute) {
    return "/" + normalizedPath;
  }

  if (normalizedPath.empty()) {
    return ".";
  }

  return normalizedPath;
}
#endif  // _WIN32

}  // namespace node
