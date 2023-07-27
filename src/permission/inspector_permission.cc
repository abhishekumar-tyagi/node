#include "inspector_permission.h"

#include <string>

namespace node {

namespace permission {

// Currently, Inspector manage a single state
// Once denied, it's always denied
void InspectorPermission::Apply(
    const std::string& allow,
    PermissionScope scope,
    const std::unordered_map<std::string, std::string>& options) {
  deny_all_ = true;
}

bool InspectorPermission::is_granted(PermissionScope perm,
                                     const std::string_view& param) {
  return deny_all_ == false;
}

}  // namespace permission
}  // namespace node
