#ifndef SRC_CALLBACK_QUEUE_H_
#define SRC_CALLBACK_QUEUE_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include <atomic>

namespace node {

template <typename R, typename... Args>
class CallbackQueue {
 public:
  class Callback {
   public:
    explicit inline Callback(bool refed);

    virtual ~Callback() = default;
    virtual R Call(Args&&... args) = 0;

    inline bool is_refed() const;

   private:
    inline std::unique_ptr<Callback> get_next();
    inline void set_next(std::unique_ptr<Callback> next);

    bool refed_;
    std::unique_ptr<Callback> next_;

    friend class CallbackQueue;
  };

  template <typename Fn>
  inline std::unique_ptr<Callback> CreateCallback(Fn&& fn, bool refed);

  inline std::unique_ptr<Callback> Shift();
  inline void Push(std::unique_ptr<Callback> cb);
  // ConcatMove adds elements from 'other' to the end of this list, and clears
  // 'other' afterwards.
  inline void ConcatMove(CallbackQueue&& other);

  // size() is atomic and may be called from any thread.
  inline size_t size() const;

 private:
  template <typename Fn>
  class CallbackImpl final : public Callback {
   public:
    CallbackImpl(Fn&& callback, bool refed);
    R Call(Args&&... args) override;

   private:
    Fn callback_;
  };

  std::atomic<size_t> size_ {0};
  std::unique_ptr<Callback> head_;
  Callback* tail_ = nullptr;
};

}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#endif  // SRC_CALLBACK_QUEUE_H_
