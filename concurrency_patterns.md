# The 3 Classic Concurrency Patterns: Conceptual Guide

```
                       THE 3 CLASSIC CONCURRENCY PATTERNS
                                       │
     ┌─────────────────────────────────┼─────────────────────────────────┐
     ▼                                 ▼                                 ▼
1. PRODUCER-CONSUMER          2. DINING PHILOSOPHERS            3. READERS-WRITERS
 (Bounded Buffer)             (Resource Allocation)             (Asymmetric Access)
 State / Capacity             Multiple Shared Locks             Access Permission
```

---

## 1. Producer-Consumer (The Bounded Buffer Pattern)
*A model of **Asynchronous Pipelines & Capacity Management***

### The Real-World Scenario
Think of an assembly line: Factory workers (**Producers**) assemble parts and place them onto a conveyor belt (**Buffer**). Packaging workers (**Consumers**) take parts off the conveyor belt and box them up. The conveyor belt has fixed physical capacity ($N$ slots).

### The Inherent Conflict
- Producers and Consumers work at **unpredictable, independent speeds**.
- A sudden burst of production can overwhelm the belt.
- A sudden slowdown in production leaves packaging workers with nothing to do.

### The Failures We Encounter (Without Proper Coordination):
1. **Buffer Overflow:** A Producer drops an item when the belt is already full $\rightarrow$ the item falls off or overwrites another item (**Data Loss**).
2. **Buffer Underflow:** A Consumer grabs at the belt when it is empty $\rightarrow$ attempts to box up thin air or previous garbage (**Ghost Reading / Null Processing**).
3. **Pointer Corruption:** Two Producers push onto the same slot, or two Consumers pull from the same slot at the same time (**Race Condition on `head`/`tail`**).
4. **Wasted Energy (Busy-Wait):** Workers constantly leaning in every second to peer at the belt, burning out their energy (**CPU spikes to 100%**).

---

## 2. Dining Philosophers (The Resource Allocation & Deadlock Pattern)
*A model of **Multi-Resource Contention & Circular Dependencies***

### The Real-World Scenario
Five philosophers sit around a round dining table with a bowl of spaghetti in the middle. Between each pair of adjacent plates lies a single chopstick/fork ($5$ plates, $5$ forks total).

Each philosopher alternates between two life states:
1. **Thinking** (Requires zero forks).
2. **Eating** (Requires **both** the left fork AND the right fork simultaneously).

After eating, a philosopher puts down both forks and goes back to thinking.

```
                  Philosopher 0
                 /             \
            Fork 4             Fork 0
             /                     \
    Philosopher 4               Philosopher 1
           |                         |
        Fork 3                     Fork 1
             \                     /
    Philosopher 3 ——— Fork 2 ——— Philosopher 2
```

### The Inherent Conflict
- Adjacent philosophers share a fork. If Philosopher 0 is eating, neither Philosopher 4 nor Philosopher 1 can eat.
- To eat, a thread must acquire **multiple independent locks** at the same time.

### The Failures We Encounter (The Disasters):
1. **The Classic Circular Deadlock:**
   - Every philosopher gets hungry at the exact same moment.
   - Every philosopher simultaneously picks up their **left fork**.
   - Now, every fork is taken ($5$ philosophers hold $1$ fork each).
   - Every philosopher reaches for their **right fork**... but it is held by their neighbor!
   - Everyone waits indefinitely for their right neighbor to put down their fork.
   - **Result:** Nobody can eat, nobody will drop their fork. **All 5 philosophers starve to death in permanent freeze (Deadlock).**
2. **Livelock:**
   - If philosophers are programmed to put down their left fork if the right fork isn't available: they all pick up the left fork, notice the right is busy, put down the left fork, wait 1 ms, pick up the left fork again... repeating in lockstep forever without eating.
3. **Starvation (Unfairness):**
   - Two fast, greedy neighbors (e.g., Philosophers 0 and 2) eat alternately in such a way that Philosopher 1 never finds both forks free at the same time.

---

## 3. Readers-Writers (The Shared vs Exclusive Access Pattern)
*A model of **Asymmetric Permissions & Database Locking***

### The Real-World Scenario
Think of a shared Wikipedia page, flight booking system, or database record:
- **Readers:** Threads that only inspect/view the record (they do not alter any data).
- **Writers:** Threads that edit, insert, or delete data in the record.

### The Inherent Conflict & The Golden Rule of Storage:
- **Read-Read is Safe:** 100 readers can read the exact same memory at the exact same time without causing any corruption. There is **no need** for readers to block each other!
- **Read-Write is Fatal:** If a reader reads while a writer is modifying a struct, the reader gets torn/corrupted data.
- **Write-Write is Fatal:** If two writers write simultaneously, the data becomes corrupted garbage.
- **Rule:** *Multiple Concurrent Readers OR One Exclusive Writer (Never both).*

### The Failures We Encounter (The Disasters):
1. **Over-Constrained Inefficiency (Naive Mutex):**
   - If you use a simple Mutex, only 1 thread can read at a time.
   - If 99% of your traffic is readers, multi-core parallelism is destroyed. Readers wait in line behind each other for no reason.
2. **Torn Reads / Data Inconsistency:**
   - If readers don't lock properly, a reader reads half of an updated record (e.g., reads updated `name` but old `bank_balance`), creating silent financial/data corruption.
3. **Writer Starvation (The Reader-Bias Disaster):**
   - Reader 1 starts reading.
   - A Writer arrives and waits for Reader 1 to finish.
   - While Reader 1 is reading, Reader 2 arrives and joins in (since readers can share).
   - Reader 1 leaves, but Reader 3 arrives and joins Reader 2.
   - A never-ending trickle of readers keeps the database locked in "Read Mode".
   - **The Writer waits forever and starves!** The system's updates freeze completely.

---

## Summary Comparison Matrix

| Pattern | Shared Resource | Threads Involved | Primary Failure to Observe |
| :--- | :--- | :--- | :--- |
| **1. Producer-Consumer** | Fixed-capacity queue / buffer | Producers & Consumers (Cooperative) | **Overflow / Underflow / Spin-polling** |
| **2. Dining Philosophers** | Multi-lock ring (Forks) | Identical Symmetric Workers | **Circular Deadlock & Hold-and-Wait freeze** |
| **3. Readers-Writers** | Single database record / file | Asymmetric Readers & Writers | **Writer Starvation vs Data Race Corruption** |
