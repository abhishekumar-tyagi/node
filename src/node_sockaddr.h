#ifndef SRC_NODE_SOCKADDR_H_
#define SRC_NODE_SOCKADDR_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "env.h"
#include "memory_tracker.h"
#include "base_object.h"
#include "node.h"
#include "uv.h"
#include "v8.h"

#include <memory>
#include <string>
#include <list>
#include <unordered_map>

namespace node {

class Environment;

class SocketAddress : public MemoryRetainer {
 public:
  enum class CompareResult {
    NOT_COMPARABLE = -2,
    LESS_THAN,
    SAME,
    GREATER_THAN
  };

  struct Hash {
    size_t operator()(const SocketAddress& addr) const;
  };

  inline bool operator==(const SocketAddress& other) const;
  inline bool operator!=(const SocketAddress& other) const;

  inline bool operator<(const SocketAddress& other) const;
  inline bool operator>(const SocketAddress& other) const;
  inline bool operator<=(const SocketAddress& other) const;
  inline bool operator>=(const SocketAddress& other) const;

  inline static bool is_numeric_host(const char* hostname);
  inline static bool is_numeric_host(const char* hostname, int family);

  // Returns true if converting {family, host, port} to *addr succeeded.
  static bool ToSockAddr(
      int32_t family,
      const char* host,
      uint32_t port,
      sockaddr_storage* addr);

  // Returns true if converting {family, host, port} to *addr succeeded.
  static bool New(
      int32_t family,
      const char* host,
      uint32_t port,
      SocketAddress* addr);

  static bool New(
      const char* host,
      uint32_t port,
      SocketAddress* addr);

  // Returns the port for an IPv4 or IPv6 address.
  inline static int GetPort(const sockaddr* addr);
  inline static int GetPort(const sockaddr_storage* addr);

  // Returns the numeric host as a string for an IPv4 or IPv6 address.
  inline static std::string GetAddress(const sockaddr* addr);
  inline static std::string GetAddress(const sockaddr_storage* addr);

  // Returns the struct length for an IPv4, IPv6 or UNIX domain.
  inline static size_t GetLength(const sockaddr* addr);
  inline static size_t GetLength(const sockaddr_storage* addr);

  SocketAddress() = default;

  inline explicit SocketAddress(const sockaddr* addr);
  inline SocketAddress(const SocketAddress& addr);
  inline SocketAddress& operator=(const sockaddr* other);
  inline SocketAddress& operator=(const SocketAddress& other);

  inline const sockaddr& operator*() const;
  inline const sockaddr* operator->() const;

  inline const sockaddr* data() const;
  inline const uint8_t* raw() const;
  inline sockaddr* storage();
  inline size_t length() const;

  inline int family() const;
  inline std::string address() const;
  inline int port() const;

  // Returns true if the given other SocketAddress is a match
  // for this one. The addresses are a match if:
  // 1. They are the same family and match identically
  // 2. They are different family but match semantically (
  //     for instance, an IPv4 addres in IPv6 notation)
  bool is_match(const SocketAddress& other) const;

  // Compares this SocketAddress to the given other SocketAddress.
  CompareResult compare(const SocketAddress& other) const;

  // Returns true if this SocketAddress is within the subnet
  // identified by the given network address and CIDR prefix.
  bool is_in_network(const SocketAddress& network, int prefix) const;

  // If the SocketAddress is an IPv6 address, returns the
  // current value of the IPv6 flow label, if set. Otherwise
  // returns 0.
  inline uint32_t flow_label() const;

  // If the SocketAddress is an IPv6 address, sets the
  // current value of the IPv6 flow label. If not an
  // IPv6 address, set_flow_label is a non-op. It
  // is important to note that the flow label,
  // while represented as an uint32_t, the flow
  // label is strictly limited to 20 bits, and
  // this will assert if any value larger than
  // 20-bits is specified.
  inline void set_flow_label(uint32_t label = 0);

  inline void Update(uint8_t* data, size_t len);

  static SocketAddress FromSockName(const uv_udp_t& handle);
  static SocketAddress FromSockName(const uv_tcp_t& handle);
  static SocketAddress FromPeerName(const uv_udp_t& handle);
  static SocketAddress FromPeerName(const uv_tcp_t& handle);

  inline v8::Local<v8::Object> ToJS(
      Environment* env,
      v8::Local<v8::Object> obj = v8::Local<v8::Object>()) const;

  inline std::string ToString() const;

  SET_NO_MEMORY_INFO()
  SET_MEMORY_INFO_NAME(SocketAddress)
  SET_SELF_SIZE(SocketAddress)

  template <typename T>
  using Map = std::unordered_map<SocketAddress, T, Hash>;

 private:
  sockaddr_storage address_;
};

template <typename T>
class SocketAddressLRU : public MemoryRetainer {
 public:
  using Type = typename T::Type;

  inline explicit SocketAddressLRU(size_t max_size);

  // If the item already exists, returns a reference to
  // the existing item, adjusting items position in the
  // LRU. If the item does not exist, emplaces the item
  // and returns the new item.
  Type* Upsert(const SocketAddress& address);

  // Returns a reference to the item if it exists, or
  // nullptr. The position in the LRU is not modified.
  Type* Peek(const SocketAddress& address) const;

  size_t size() const { return map_.size(); }
  size_t max_size() const { return max_size_; }

  void MemoryInfo(MemoryTracker* tracker) const override;
  SET_MEMORY_INFO_NAME(SocketAddressLRU)
  SET_SELF_SIZE(SocketAddressLRU)

 private:
  using Pair = std::pair<SocketAddress, Type>;
  using Iterator = typename std::list<Pair>::iterator;

  void CheckExpired();

  std::list<Pair> list_;
  SocketAddress::Map<Iterator> map_;
  size_t max_size_;
};

// A BlockList is used to evaluate whether a given
// SocketAddress should be accepted for inbound or
// outbound network activity.
class SocketAddressBlockList : public MemoryRetainer {
 public:
  explicit SocketAddressBlockList(
      std::shared_ptr<SocketAddressBlockList> parent = {});
  ~SocketAddressBlockList() = default;

  void AddSocketAddress(
      const SocketAddress& address);

  void RemoveSocketAddress(
      const SocketAddress& address);

  void AddSocketAddressRange(
      const SocketAddress& start,
      const SocketAddress& end);

  void AddSocketAddressMask(
      const SocketAddress& address,
      int prefix);

  bool Apply(const SocketAddress& address);

  size_t size() const { return rules_.size(); }

  v8::MaybeLocal<v8::Array> ListRules(Environment* env);

  struct Rule : public MemoryRetainer {
    virtual bool Apply(const SocketAddress& address) = 0;
    inline v8::MaybeLocal<v8::Value> ToV8String(Environment* env);
    virtual std::string ToString() = 0;
  };

  struct SocketAddressRule final : Rule {
    SocketAddress address;

    explicit SocketAddressRule(const SocketAddress& address);

    bool Apply(const SocketAddress& address) override;
    std::string ToString() override;

    void MemoryInfo(node::MemoryTracker* tracker) const override;
    SET_MEMORY_INFO_NAME(SocketAddressRule)
    SET_SELF_SIZE(SocketAddressRule)
  };

  struct SocketAddressRangeRule final : Rule {
    SocketAddress start;
    SocketAddress end;

    SocketAddressRangeRule(
        const SocketAddress& start,
        const SocketAddress& end);

    bool Apply(const SocketAddress& address) override;
    std::string ToString() override;

    void MemoryInfo(node::MemoryTracker* tracker) const override;
    SET_MEMORY_INFO_NAME(SocketAddressRangeRule)
    SET_SELF_SIZE(SocketAddressRangeRule)
  };

  struct SocketAddressMaskRule final : Rule {
    SocketAddress network;
    int prefix;

    SocketAddressMaskRule(
        const SocketAddress& address,
        int prefix);

    bool Apply(const SocketAddress& address) override;
    std::string ToString() override;

    void MemoryInfo(node::MemoryTracker* tracker) const override;
    SET_MEMORY_INFO_NAME(SocketAddressMaskRule)
    SET_SELF_SIZE(SocketAddressMaskRule)
  };

  void MemoryInfo(node::MemoryTracker* tracker) const override;
  SET_MEMORY_INFO_NAME(SocketAddressBlockList)
  SET_SELF_SIZE(SocketAddressBlockList)

 private:
  std::shared_ptr<SocketAddressBlockList> parent_;
  std::list<std::unique_ptr<Rule>> rules_;
  SocketAddress::Map<std::list<std::unique_ptr<Rule>>::iterator> address_rules_;
};

class SocketAddressBlockListWrap :
    public BaseObject,
    public SocketAddressBlockList {
 public:
  static void Initialize(v8::Local<v8::Object> target,
                         v8::Local<v8::Value> unused,
                         v8::Local<v8::Context> context,
                         void* priv);

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void AddAddress(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void AddRange(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void AddSubnet(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Check(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void GetRules(const v8::FunctionCallbackInfo<v8::Value>& args);

  SocketAddressBlockListWrap(
      Environment* env,
      v8::Local<v8::Object> wrap);

  void MemoryInfo(node::MemoryTracker* tracker) const override {
    SocketAddressBlockList::MemoryInfo(tracker);
  }
  SET_MEMORY_INFO_NAME(SocketAddressBlockListWrap)
  SET_SELF_SIZE(SocketAddressBlockListWrap)
};

}  // namespace node

#endif  // NOE_WANT_INTERNALS

#endif  // SRC_NODE_SOCKADDR_H_
