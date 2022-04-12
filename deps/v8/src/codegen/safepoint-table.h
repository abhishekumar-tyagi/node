// Copyright 2011 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_CODEGEN_SAFEPOINT_TABLE_H_
#define V8_CODEGEN_SAFEPOINT_TABLE_H_

#include "src/base/bit-field.h"
#include "src/base/iterator.h"
#include "src/base/memory.h"
#include "src/common/assert-scope.h"
#include "src/utils/allocation.h"
#include "src/utils/bit-vector.h"
#include "src/utils/utils.h"
#include "src/zone/zone-chunk-list.h"
#include "src/zone/zone.h"

namespace v8 {
namespace internal {

namespace wasm {
class WasmCode;
}  // namespace wasm

class SafepointEntry {
 public:
  static constexpr int kNoDeoptIndex = -1;
  static constexpr int kNoTrampolinePC = -1;

  SafepointEntry() = default;

  SafepointEntry(int pc, int deopt_index, uint32_t tagged_register_indexes,
                 base::Vector<uint8_t> tagged_slots, int trampoline_pc)
      : pc_(pc),
        deopt_index_(deopt_index),
        tagged_register_indexes_(tagged_register_indexes),
        tagged_slots_(tagged_slots),
        trampoline_pc_(trampoline_pc) {
    DCHECK(is_initialized());
  }

  bool is_initialized() const { return tagged_slots_.begin() != nullptr; }

  bool operator==(const SafepointEntry& other) const {
    return pc_ == other.pc_ && deopt_index_ == other.deopt_index_ &&
           tagged_register_indexes_ == other.tagged_register_indexes_ &&
           tagged_slots_ == other.tagged_slots_ &&
           trampoline_pc_ == other.trampoline_pc_;
  }

  void Reset() {
    *this = SafepointEntry{};
    DCHECK(!is_initialized());
  }

  int pc() const { return pc_; }

  int trampoline_pc() const { return trampoline_pc_; }

  bool has_deoptimization_index() const {
    DCHECK(is_initialized());
    return deopt_index_ != kNoDeoptIndex;
  }

  int deoptimization_index() const {
    DCHECK(is_initialized() && has_deoptimization_index());
    return deopt_index_;
  }

  uint32_t tagged_register_indexes() const {
    DCHECK(is_initialized());
    return tagged_register_indexes_;
  }

  base::Vector<const uint8_t> tagged_slots() const {
    DCHECK(is_initialized());
    return tagged_slots_;
  }

 private:
  int pc_ = -1;
  int deopt_index_ = kNoDeoptIndex;
  uint32_t tagged_register_indexes_ = 0;
  base::Vector<uint8_t> tagged_slots_;
  int trampoline_pc_ = kNoTrampolinePC;
};

// A wrapper class for accessing the safepoint table embedded into the Code
// object.
class SafepointTable {
 public:
  // The isolate and pc arguments are used for figuring out whether pc
  // belongs to the embedded or un-embedded code blob.
  explicit SafepointTable(Isolate* isolate, Address pc, Code code);
#if V8_ENABLE_WEBASSEMBLY
  explicit SafepointTable(const wasm::WasmCode* code);
#endif  // V8_ENABLE_WEBASSEMBLY

  SafepointTable(const SafepointTable&) = delete;
  SafepointTable& operator=(const SafepointTable&) = delete;

  int length() const { return length_; }

  int byte_size() const {
    return kHeaderSize + length_ * (entry_size() + tagged_slots_bytes());
  }

  int find_return_pc(int pc_offset);

  SafepointEntry GetEntry(int index) const {
    DCHECK_GT(length_, index);
    Address entry_ptr =
        safepoint_table_address_ + kHeaderSize + index * entry_size();

    int pc = read_bytes(&entry_ptr, pc_size());
    int deopt_index = SafepointEntry::kNoDeoptIndex;
    int trampoline_pc = SafepointEntry::kNoTrampolinePC;
    if (has_deopt_data()) {
      STATIC_ASSERT(SafepointEntry::kNoDeoptIndex == -1);
      STATIC_ASSERT(SafepointEntry::kNoTrampolinePC == -1);
      // `-1` to restore the original value, see also
      // SafepointTableBuilder::Emit.
      deopt_index = read_bytes(&entry_ptr, deopt_index_size()) - 1;
      trampoline_pc = read_bytes(&entry_ptr, pc_size()) - 1;
      DCHECK(deopt_index >= 0 || deopt_index == SafepointEntry::kNoDeoptIndex);
      DCHECK(trampoline_pc >= 0 ||
             trampoline_pc == SafepointEntry::kNoTrampolinePC);
    }
    int tagged_register_indexes =
        read_bytes(&entry_ptr, register_indexes_size());

    // Entry bits start after the the vector of entries (thus the pc offset of
    // the non-existing entry after the last one).
    uint8_t* tagged_slots_start = reinterpret_cast<uint8_t*>(
        safepoint_table_address_ + kHeaderSize + length_ * entry_size());
    base::Vector<uint8_t> tagged_slots(
        tagged_slots_start + index * tagged_slots_bytes(),
        tagged_slots_bytes());

    return SafepointEntry(pc, deopt_index, tagged_register_indexes,
                          tagged_slots, trampoline_pc);
  }

  // Returns the entry for the given pc.
  SafepointEntry FindEntry(Address pc) const;

  void Print(std::ostream&) const;

 private:
  // Layout information.
  static constexpr int kLengthOffset = 0;
  static constexpr int kEntryConfigurationOffset = kLengthOffset + kIntSize;
  static constexpr int kHeaderSize = kEntryConfigurationOffset + kUInt32Size;

  using HasDeoptDataField = base::BitField<bool, 0, 1>;
  using RegisterIndexesSizeField = HasDeoptDataField::Next<int, 3>;
  using PcSizeField = RegisterIndexesSizeField::Next<int, 3>;
  using DeoptIndexSizeField = PcSizeField::Next<int, 3>;
  // In 22 bits, we can encode up to 4M bytes, corresponding to 32M frame slots,
  // which is 128MB on 32-bit and 256MB on 64-bit systems. The stack size is
  // limited to a bit below 1MB anyway (see FLAG_stack_size).
  using TaggedSlotsBytesField = DeoptIndexSizeField::Next<int, 22>;

  SafepointTable(Address instruction_start, Address safepoint_table_address);

  int entry_size() const {
    int deopt_data_size = has_deopt_data() ? pc_size() + deopt_index_size() : 0;
    return pc_size() + deopt_data_size + register_indexes_size();
  }

  int tagged_slots_bytes() const {
    return TaggedSlotsBytesField::decode(entry_configuration_);
  }
  bool has_deopt_data() const {
    return HasDeoptDataField::decode(entry_configuration_);
  }
  int pc_size() const { return PcSizeField::decode(entry_configuration_); }
  int deopt_index_size() const {
    return DeoptIndexSizeField::decode(entry_configuration_);
  }
  int register_indexes_size() const {
    return RegisterIndexesSizeField::decode(entry_configuration_);
  }

  static int read_bytes(Address* ptr, int bytes) {
    uint32_t result = 0;
    for (int b = 0; b < bytes; ++b, ++*ptr) {
      result |= uint32_t{*reinterpret_cast<uint8_t*>(*ptr)} << (8 * b);
    }
    return static_cast<int>(result);
  }

  DISALLOW_GARBAGE_COLLECTION(no_gc_)

  const Address instruction_start_;

  // Safepoint table layout.
  const Address safepoint_table_address_;
  const int length_;
  const uint32_t entry_configuration_;

  friend class SafepointTableBuilder;
  friend class SafepointEntry;
};

class SafepointTableBuilder {
 private:
  struct EntryBuilder {
    int pc;
    int deopt_index = SafepointEntry::kNoDeoptIndex;
    int trampoline = SafepointEntry::kNoTrampolinePC;
    GrowableBitVector* stack_indexes;
    uint32_t register_indexes = 0;
    EntryBuilder(Zone* zone, int pc)
        : pc(pc), stack_indexes(zone->New<GrowableBitVector>()) {}
  };

 public:
  explicit SafepointTableBuilder(Zone* zone) : entries_(zone), zone_(zone) {}

  SafepointTableBuilder(const SafepointTableBuilder&) = delete;
  SafepointTableBuilder& operator=(const SafepointTableBuilder&) = delete;

  bool emitted() const {
    return safepoint_table_offset_ != kNoSafepointTableOffset;
  }

  int safepoint_table_offset() const {
    DCHECK(emitted());
    return safepoint_table_offset_;
  }

  class Safepoint {
   public:
    void DefineTaggedStackSlot(int index) {
      // Note it is only valid to specify stack slots here that are *not* in
      // the fixed part of the frame (e.g. argc, target, context, stored rbp,
      // return address). Frame iteration handles the fixed part of the frame
      // with custom code, see CommonFrame::IterateCompiledFrame.
      entry_->stack_indexes->Add(index, table_->zone_);
      table_->UpdateMinMaxStackIndex(index);
    }

    void DefineTaggedRegister(int reg_code) {
      DCHECK_LT(reg_code,
                kBitsPerByte * sizeof(EntryBuilder::register_indexes));
      entry_->register_indexes |= 1u << reg_code;
    }

   private:
    friend class SafepointTableBuilder;
    Safepoint(EntryBuilder* entry, SafepointTableBuilder* table)
        : entry_(entry), table_(table) {}
    EntryBuilder* const entry_;
    SafepointTableBuilder* const table_;
  };

  // Define a new safepoint for the current position in the body.
  Safepoint DefineSafepoint(Assembler* assembler);

  // Emit the safepoint table after the body. The number of bits per
  // entry must be enough to hold all the pointer indexes.
  V8_EXPORT_PRIVATE void Emit(Assembler* assembler, int bits_per_entry);

  // Find the Deoptimization Info with pc offset {pc} and update its
  // trampoline field. Calling this function ensures that the safepoint
  // table contains the trampoline PC {trampoline} that replaced the
  // return PC {pc} on the stack.
  int UpdateDeoptimizationInfo(int pc, int trampoline, int start,
                               int deopt_index);

 private:
  // Remove consecutive identical entries.
  void RemoveDuplicates();

  void UpdateMinMaxStackIndex(int index) {
#ifdef DEBUG
    max_stack_index_ = std::max(max_stack_index_, index);
#endif  // DEBUG
    min_stack_index_ = std::min(min_stack_index_, index);
  }

  int min_stack_index() const {
    return min_stack_index_ == std::numeric_limits<int>::max()
               ? 0
               : min_stack_index_;
  }

  static constexpr int kNoSafepointTableOffset = -1;

  // Tracks the min/max stack slot index over all entries. We need the minimum
  // index when encoding the actual table since we shift all unused lower
  // indices out of the encoding. Tracking the indices during safepoint
  // construction means we don't have to iterate again later.
#ifdef DEBUG
  int max_stack_index_ = 0;
#endif  // DEBUG
  int min_stack_index_ = std::numeric_limits<int>::max();

  ZoneChunkList<EntryBuilder> entries_;
  int safepoint_table_offset_ = kNoSafepointTableOffset;
  Zone* const zone_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_CODEGEN_SAFEPOINT_TABLE_H_
