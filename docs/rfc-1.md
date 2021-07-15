# RFC-1: Terminology

Status: draft

## Interface & ownership

In some situations, an interface needs to represent ownership. In others, it is unneeded.

When an interface tells nothing about the ownership, it means that the user is responsible for the instance's lifetime.

The following gradation is applicable:

```cpp
struct interface_type_1 {
  int (*method)();
};

struct interface_type_2 {
  int (*method)(struct interface_type_2* self);
};

struct interface_type_3 {
  int (*destroy)(struct interface_type_3* self);
  int (*method)(struct interface_type_3* self);
};
```

`interface_type_1` tells nothing about ownership. In particular, the interface provides no method to signal that an instance is no longer needed. Moreover, methods have no access to the instance's state.

`interface_type_2` says nothing about the ownership. The methods can access the instance's state.

`interface_type_3` there is a destroy method => client can destroy an instance. This interface represents ownership.


### What in C++

 * Destructor is always available.
 * Ownership depends on how you have/access the object:
   * you have a value -> you own the value and must destroy it in the end
   * a reference to value -> you don't have ownership
   * unique_ptr -> you own the value and must destroy it in the end, and also you must deallocate the memory
   * shared_ptr -> you own the value, but the value's destruction is proxied through the ref-counter; if you are the last owner, you must destroy the object in the end and also deallocate its memory

### What in Java

GC owns all the instances and monitors whether they are still reachable. Unreachable instances disappear without additional actions from the user; in particular, no proper "destruction" is possible.
