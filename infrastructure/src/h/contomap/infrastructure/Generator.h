#pragma once

// The following block based on a workaround
// See https://github.com/msys2/MINGW-packages/issues/7565
#if defined __GNUC__ && defined _WIN32
#if __GNUC__ == 10
namespace std
{
inline namespace __n4861
{
/* workaround for Windows linker error "multiple definition of ..."
   with GCC10: these two symbols are declared as "weak", but only
   adding "inline" and "selectany" makes the linker failure go away */
inline void __dummy_resume_destroy();
extern struct __noop_coro_frame __noop_coro_fr __attribute__((selectany));
}
}
#endif
#endif

#include <coroutine>
#include <optional>

namespace contomap::infrastructure
{

/**
 * Generator type to support std::coroutine implementations.
 *
 * @tparam T the result type that is produced with the generator.
 */
template <std::movable T> class Generator
{
public:
   /**
    * The mandatory type to support coroutines.
    */
   struct promise_type
   {
      /**
       * @return generator result as per contract.
       */
      Generator<T> get_return_object()
      {
         return Generator { Handle::from_promise(*this) };
      }
      /**
       * @return result as per contract.
       */
      static std::suspend_always initial_suspend() noexcept
      {
         return {};
      }
      /**
       * @return result as per contract.
       */
      static std::suspend_always final_suspend() noexcept
      {
         return {};
      }
      /**
       * Assigns the given value to be the currently held value.
       *
       * @param value the value to report.
       * @return as per contract.
       */
      std::suspend_always yield_value(T value) noexcept
      {
         currentValue = std::move(value);
         return {};
      }
      /**
       * Deleted support for co_await in generator coroutines.
       */
      void await_transform() = delete;
      /**
       * Implementation as per contract.
       */
      [[noreturn]] static void unhandled_exception()
      {
         throw;
      }

      /**
       * @return the current value
       */
      std::optional<T> const &current() const
      {
         return currentValue;
      }

   private:
      std::optional<T> currentValue;
   };

   /**
    * Handle represents a shortcut for the promise_type expressed as coroutine handle.
    */
   using Handle = std::coroutine_handle<promise_type>;

   /**
    * Default constructor.
    */
   Generator() = default;
   /**
    * Constructor that initializes the handle.
    *
    * @param coroutineHandle the handle to assign to this generator.
    */
   explicit Generator(const Handle coroutineHandle)
      : coroutineHandle { coroutineHandle }
   {
   }
   /**
    * Deleted assignment constructor.
    */
   Generator(Generator const &) = delete;
   /**
    * Move constructor.
    *
    * @param other the other generator to take over.
    */
   Generator(Generator &&other) noexcept
      : coroutineHandle { other.coroutineHandle }
   {
      other.coroutineHandle = {};
   }
   ~Generator()
   {
      if (coroutineHandle)
      {
         coroutineHandle.destroy();
      }
   }

   /**
    * Deleted assignment operator.
    *
    * @return nothing.
    */
   Generator &operator=(Generator const &) = delete;
   /**
    * Move operator.
    *
    * @param other the other generator to move from.
    *
    * @return this.
    */
   Generator &operator=(Generator &&other) noexcept
   {
      if (this != &other)
      {
         if (coroutineHandle)
         {
            coroutineHandle.destroy();
         }
         coroutineHandle = other.coroutineHandle;
         other.coroutineHandle = {};
      }
      return *this;
   }

   /**
    * Iterator implements the support for for-range-loop support
    */
   class Iterator
   {
   public:
      /**
       * Constructor.
       *
       * @param handle the coroutine handle to cover
       */
      explicit Iterator(Handle const handle)
         : coroutineHandle { handle }
      {
      }

      /**
       * The increment operator advances to the next item from the generator.
       */
      void operator++()
      {
         coroutineHandle.resume();
      }
      /**
       * The dereference operator returns the currently contained value.
       *
       * @return the currently contained value.
       */
      T const &operator*() const
      {
         return *coroutineHandle.promise().current();
      }
      /**
       * The equality operator is implemented against the sentinel value to indicate the end of the sequence.
       *
       * @return true if the generator has reached the end.
       */
      bool operator==(std::default_sentinel_t) const
      {
         return !coroutineHandle || coroutineHandle.done();
      }

   private:
      Handle coroutineHandle;
   };

   /**
    * @return the start iteration of the generator.
    */
   Iterator begin()
   {
      if (coroutineHandle)
      {
         coroutineHandle.resume();
      }
      return Iterator { coroutineHandle };
   }

   /**
    * @return the default sentinel value for terminating an iteration.
    */
   std::default_sentinel_t end()
   {
      return {};
   }

private:
   Handle coroutineHandle;
};

/**
 * A Search is a generator for constant references of a certain type.
 */
template <class T> using Search = contomap::infrastructure::Generator<std::reference_wrapper<T>>;

}
