#pragma once

#include <functional>
#include <memory>
#include <optional>

namespace contomap::infrastructure
{

class Links;

/**
 * A link provides a bi-directional reference between two instances.
 * Links come in pairs, and as soon as one goes out of scope, the other is informed.
 *
 * @tparam LinkedType the type of the referred party.
 */
template <class LinkedType> class Link
{
public:
   /**
    * Deleted default constructor.
    */
   Link() = delete;
   /**
    * Deleted copy constructor.
    */
   Link(Link const &) = delete;
   /**
    * Deleted move constructor.
    */
   Link(Link &&) = delete;
   ~Link()
   {
      if (linked.has_value())
      {
         auto remote = std::move(remoteUnlink);
         reset();
         remote();
      }
   }

   /**
    * Deleted copy assignment operator.
    * @return this.
    */
   Link &operator=(Link const &) = delete;
   /**
    * Deleted move assignment operator.
    * @return this.
    */
   Link &operator=(Link &&) = delete;

   /**
    * @return a reference to the linked type. Throws in case the link was detached.
    */
   LinkedType &getLinked() const
   {
      return linked.value();
   }

private:
   friend Links;

   explicit Link(LinkedType &linked, std::function<void()> localUnlinked)
      : linked(linked)
      , localUnlinked(std::move(localUnlinked))
   {
   }

   void unlink()
   {
      if (linked.has_value())
      {
         auto local = std::move(localUnlinked);
         reset();
         local();
      }
   }

   std::function<void()> wrapUnlink()
   {
      return [this]() { unlink(); };
   }

   void reset()
   {
      linked.reset();
      localUnlinked = []() {};
      remoteUnlink = []() {};
   }

   std::optional<std::reference_wrapper<LinkedType>> linked;
   std::function<void()> localUnlinked;
   std::function<void()> remoteUnlink;
};

/**
 * Links provides factory functions for Link instances.
 */
class Links
{
public:
   /**
    * Deleted default constructor.
    */
   Links() = delete;

   /**
    * Factory function to create a pair of links.
    *
    * @param a the reference to the left side.
    * @param aUnlinked the callback to be called if the right side goes out of scope.
    * @param b the reference to the right side.
    * @param bUnlinked the callback to be called if the the left side goes out of scope.
    * @tparam A the type of the left side.
    * @tparam B the type of the right side.
    * @return a pair of link instances that can call each other when going out of scope.
    */
   template <class A, class B>
   [[nodiscard]] static std::pair<std::unique_ptr<Link<A>>, std::unique_ptr<Link<B>>> between(
      A &a, std::function<void()> aUnlinked, B &b, std::function<void()> bUnlinked)
   {
      std::unique_ptr<Link<A>> aLink(new Link<A>(a, std::move(aUnlinked)));
      std::unique_ptr<Link<B>> bLink(new Link<B>(b, std::move(bUnlinked)));
      aLink->remoteUnlink = bLink->wrapUnlink();
      bLink->remoteUnlink = aLink->wrapUnlink();
      return { std::move(aLink), std::move(bLink) };
   }
};

}
