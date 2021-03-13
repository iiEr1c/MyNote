# memory order
std::memory_order ```specifies``` how memory accesses, including regular, non-atomic memory accesses, are to be ordered ```around``` an atomic operation. Absent any constraints on a multi-core system, when multiple threads simultaneously read and write to several variables, one thread can observe the values change in an order different from the order another thread wrote them. Indeed, the apparent order of changes can even differ among multiple reader threads. Some similar effects can occur even on uniprocessor systems due to compiler transformations allowed by the memory model.

## Formal description

```Inter-thread synchronization and memory ordering``` determine how evaluations and side effects of expressions are ordered between different threads of execution. They are defined in the following terms:

+ Sequenced-before
+ Carries dependency
+ Modification order
+ Release sequence
+ Dependency-ordered before
+ Dependency-ordered before
+ Happens-before
  + Simply happens-before
  + Strongly happens-before
+ Visible side-effects
+ Consume operation
+ Acquire operation
+ Release operation

### Sequenced-before

```Within the same thread```, evaluation A may be sequenced-before evaluation B, as described in evaluation order. Usually, the sequence is determined by character ```;```'s sequence

```cpp
// evaluation order
#include <cstdio>
int a() { return std::puts("a"); }
int b() { return std::puts("b"); }
int c() { return std::puts("c"); }
void z(int, int, int) {}
int main() {
    z(a(), b(), c());       // all 6 permutations of output are allowed
    return a() + b() + c(); // all 6 permutations of output are allowed
}
```

### Carries dependency
Within the same thread, evaluation A that is sequenced-before evaluation B may also carry a dependency into B (that is, B depends on A), if any of the following is true:

1. The value of A is used as an operand of B, except
   1. if B is a call to std::kill_dependency
   2. if A is the left operand of the built-in &&, ||, ?:, or , operators.
2. A writes to a scalar object M, B reads from M
3. A carries dependency into another evaluation X, and X carries dependency into B

### Modification order
All modifications to any particular atomic variable occur in a total order that is specific to this one atomic variable.

The following four requirements are guaranteed for all atomic operations:
1. ```Write-write coherence```: If evaluation A that modifies some atomic M (a write) happens-before evaluation B that modifies M, then A appears earlier than B in the modification order of M
2. ```Read-read coherence```: if a value computation A of some atomic M (a read) happens-before a value computation B on M, and if the value of A comes from a write X on M, then the value of B is either the value stored by X, or the value stored by a side effect Y on M that appears later than X in the modification order of M.
3. ```Read-write coherence```: if a value computation A of some atomic M (a read) happens-before an operation B on M (a write), then the value of A comes from a side-effect (a write) X that appears earlier than B in the modification order of M
4. ```Write-read coherence```: if a side effect (a write) X on an atomic object M happens-before a value computation (a read) B of M, then the evaluation B shall take its value from X or from a side effect Y that follows X in the modification order of M

### Release sequence
After a release operation A is performed on an atomic object M, ```the longest continuous subsequence of``` the modification order of M that consists of
+ Writes performed by the same thread that performed A
+ ```Atomic read-modify-write operations``` made to M by any thread

is known as ```release sequence headed by A```

### Dependency-ordered before
```Between threads```, evaluation A is dependency-ordered before evaluation B if any of the following is true:
+ A performs a release operation on some atomic M, and, ```in a different thread```, B performs ```a consume operation on the same atomic M```, and B reads a value written by ```any part of the release sequence headed``` by A.
+ A is dependency-ordered before X and X carries a dependency into B.

### Inter-thread happens-before(Transitivity)
```Between threads```, evaluation A inter-thread happens before evaluation B if any of the following is true:
+ A synchronizes-with B
+ A is dependency-ordered before B
+ A synchronizes-with some evaluation X, and X is sequenced-before B
+ A is sequenced-before some evaluation X, and X inter-thread happens-before B
+ A inter-thread happens-before some evaluation X, and X inter-thread happens-before B

### Happens-before
Regardless of threads, evaluation A happens-before evaluation B if any of the following is true:
+ A is sequenced-before B
+ ```A inter-thread happens before B```

### Visible side-effects
The side-effect A on a scalar M (a write) is visible with respect to value computation B on M (a read) if both of the following are true:
+ A happens-before B
+ There is no other side effect X to M where A happens-before X and X happens-before B


## memory order

| Value | Explanation |
| :----: | :----: |
| memory_order_relaxed | Relaxed operation: there are no synchronization or ordering constraints imposed on other reads or writes, only this operation's atomicity is guaranteed |
| memory_order_consume | ```A load operation``` with this memory order performs a consume operation on the affected memory location: no reads or writes in the current thread dependent on the value currently loaded can be reordered before this load. Writes to data-dependent variables in other threads that release the same atomic variable are visible in the current thread. On most platforms, this affects compiler optimizations only |
| memory_order_acquire | ```A load operation``` with this memory order performs the acquire operation on the affected memory location: no reads or writes in the current thread can be reordered before this load. All writes in other threads that release the same atomic variable are visible in the current thread |
| memory_order_release | ```A store operation``` with this memory order performs the release operation: no reads or writes in the current thread can be reordered after this store. All writes in the current thread are visible in other threads that acquire the same atomic variable (see Release-Acquire ordering below) and writes that carry a dependency into the atomic variable become visible in other threads that consume the same atomic |
| memory_order_acq_rel | ```A read-modify-write``` operation with this memory order is both an acquire operation and a release operation. No memory reads or writes in the current thread can be reordered before or after this store. All writes in other threads that release the same atomic variable are visible before the modification and the modification is visible in other threads that acquire the same atomic variable. |
| memory_order_seq_cst | ```A load operation``` with this memory order performs an acquire operation, a store performs a release operation, and read-modify-write performs both an acquire operation and a release operation, plus a single total order exists in which all threads observe all modifications in the same order |


#### reference
+ [sequence-points](https://stackoverflow.com/questions/4176328/what-are-sequence-points-and-how-do-they-affect-undefined-behavior)
+ [cpp reference](https://en.cppreference.com/w/cpp/atomic/memory_order)
+ [C++ memory order](https://blog.csdn.net/wxj1992/article/details/103656486)