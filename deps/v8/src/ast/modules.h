// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_AST_MODULES_H_
#define V8_AST_MODULES_H_

#include "src/parsing/scanner.h"  // Only for Scanner::Location.
#include "src/pending-compilation-error-handler.h"
#include "src/zone/zone-containers.h"

namespace v8 {
namespace internal {


class AstRawString;
class ModuleInfoEntry;

class ModuleDescriptor : public ZoneObject {
 public:
  explicit ModuleDescriptor(Zone* zone)
      : module_requests_(zone),
        special_exports_(1, zone),
        namespace_imports_(1, zone),
        regular_exports_(zone),
        regular_imports_(zone) {}

  // The following Add* methods are high-level convenience functions for use by
  // the parser.

  // import x from "foo.js";
  // import {x} from "foo.js";
  // import {x as y} from "foo.js";
  void AddImport(
    const AstRawString* import_name, const AstRawString* local_name,
    const AstRawString* module_request, const Scanner::Location loc,
    Zone* zone);

  // import * as x from "foo.js";
  void AddStarImport(
    const AstRawString* local_name, const AstRawString* module_request,
    const Scanner::Location loc, Zone* zone);

  // import "foo.js";
  // import {} from "foo.js";
  // export {} from "foo.js";  (sic!)
  void AddEmptyImport(const AstRawString* module_request);

  // export {x};
  // export {x as y};
  // export VariableStatement
  // export Declaration
  // export default ...
  void AddExport(
    const AstRawString* local_name, const AstRawString* export_name,
    const Scanner::Location loc, Zone* zone);

  // export {x} from "foo.js";
  // export {x as y} from "foo.js";
  void AddExport(
    const AstRawString* export_name, const AstRawString* import_name,
    const AstRawString* module_request, const Scanner::Location loc,
    Zone* zone);

  // export * from "foo.js";
  void AddStarExport(
    const AstRawString* module_request, const Scanner::Location loc,
    Zone* zone);

  // Check if module is well-formed and report error if not.
  // Also canonicalize indirect exports.
  bool Validate(ModuleScope* module_scope,
                PendingCompilationErrorHandler* error_handler, Zone* zone);

  struct Entry : public ZoneObject {
    const Scanner::Location location;
    const AstRawString* export_name;
    const AstRawString* local_name;
    const AstRawString* import_name;
    // The module_request value records the order in which modules are
    // requested. It also functions as an index into the ModuleInfo's array of
    // module specifiers and into the Module's array of requested modules.  A
    // negative value means no module request.
    int module_request;

    // TODO(neis): Remove local_name component?
    explicit Entry(Scanner::Location loc)
        : location(loc),
          export_name(nullptr),
          local_name(nullptr),
          import_name(nullptr),
          module_request(-1) {}

    // (De-)serialization support.
    // Note that the location value is not preserved as it's only needed by the
    // parser.  (A Deserialize'd entry has an invalid location.)
    Handle<ModuleInfoEntry> Serialize(Isolate* isolate) const;
    static Entry* Deserialize(Isolate* isolate, AstValueFactory* avfactory,
                              Handle<ModuleInfoEntry> entry);
  };

  // Module requests.
  const ZoneMap<const AstRawString*, int>& module_requests() const {
    return module_requests_;
  }

  // Namespace imports.
  const ZoneList<const Entry*>& namespace_imports() const {
    return namespace_imports_;
  }

  // All the remaining imports, indexed by local name.
  const ZoneMap<const AstRawString*, const Entry*>& regular_imports() const {
    return regular_imports_;
  }

  // Star exports and explicitly indirect exports.
  const ZoneList<const Entry*>& special_exports() const {
    return special_exports_;
  }

  // All the remaining exports, indexed by local name.
  // After canonicalization (see Validate), these are exactly the local exports.
  const ZoneMultimap<const AstRawString*, Entry*>& regular_exports() const {
    return regular_exports_;
  }

  void AddRegularExport(Entry* entry) {
    DCHECK_NOT_NULL(entry->export_name);
    DCHECK_NOT_NULL(entry->local_name);
    DCHECK_NULL(entry->import_name);
    DCHECK_LT(entry->module_request, 0);
    regular_exports_.insert(std::make_pair(entry->local_name, entry));
  }

  void AddSpecialExport(const Entry* entry, Zone* zone) {
    DCHECK_NULL(entry->local_name);
    DCHECK_LE(0, entry->module_request);
    special_exports_.Add(entry, zone);
  }

  void AddRegularImport(const Entry* entry) {
    DCHECK_NOT_NULL(entry->import_name);
    DCHECK_NOT_NULL(entry->local_name);
    DCHECK_NULL(entry->export_name);
    DCHECK_LE(0, entry->module_request);
    regular_imports_.insert(std::make_pair(entry->local_name, entry));
    // We don't care if there's already an entry for this local name, as in that
    // case we will report an error when declaring the variable.
  }

  void AddNamespaceImport(const Entry* entry, Zone* zone) {
    DCHECK_NULL(entry->import_name);
    DCHECK_NULL(entry->export_name);
    DCHECK_NOT_NULL(entry->local_name);
    DCHECK_LE(0, entry->module_request);
    namespace_imports_.Add(entry, zone);
  }

  Handle<FixedArray> SerializeRegularExports(Isolate* isolate,
                                             Zone* zone) const;
  void DeserializeRegularExports(Isolate* isolate, AstValueFactory* avfactory,
                                 Handle<FixedArray> data);

 private:
  // TODO(neis): Use STL datastructure instead of ZoneList?
  ZoneMap<const AstRawString*, int> module_requests_;
  ZoneList<const Entry*> special_exports_;
  ZoneList<const Entry*> namespace_imports_;
  ZoneMultimap<const AstRawString*, Entry*> regular_exports_;
  ZoneMap<const AstRawString*, const Entry*> regular_imports_;

  // If there are multiple export entries with the same export name, return the
  // last of them (in source order).  Otherwise return nullptr.
  const Entry* FindDuplicateExport(Zone* zone) const;

  // Find any implicitly indirect exports and make them explicit.
  //
  // An explicitly indirect export is an export entry arising from an export
  // statement of the following form:
  //   export {a as c} from "X";
  // An implicitly indirect export corresponds to
  //   export {b as c};
  // in the presence of an import statement of the form
  //   import {a as b} from "X";
  // This function finds such implicitly indirect export entries and rewrites
  // them by filling in the import name and module request, as well as nulling
  // out the local name.  Effectively, it turns
  //   import {a as b} from "X"; export {b as c};
  // into:
  //   import {a as b} from "X"; export {a as c} from "X";
  // (The import entry is never deleted.)
  void MakeIndirectExportsExplicit(Zone* zone);

  int AddModuleRequest(const AstRawString* specifier) {
    DCHECK_NOT_NULL(specifier);
    auto it = module_requests_
                  .insert(std::make_pair(specifier, module_requests_.size()))
                  .first;
    return it->second;
  }
};

}  // namespace internal
}  // namespace v8

#endif  // V8_AST_MODULES_H_
