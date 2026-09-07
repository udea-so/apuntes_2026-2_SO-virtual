# Segmentation

## Overview

Let’s explore how to support a larger address space.

This section should help us answer the following question:
* **How can we support a large address space that could possibly have a lot of free space between the stack and the heap?**
  
## Introduction

### So far, we’ve stored each process’s complete address space in memory.

The OS can simply move processes around in physical memory using the **base and bounds registers**.

<p align="center">
  <img src="img/segmentation1.gif" alt="Segmentation 1">
</p>


In the above graphic, you'll notice that there is a large *"free"* area between the stack and the heap. However, even though the space is "free", it's still taking up physical memory when we move the whole address space to another spot in physical memory.

In this case, using a base and bounds register for memory virtualization is wasteful. This also makes it difficult to run a program when the whole address space doesn't fit into memory.

Let's further explore how to support a larger address space.

## Segmentation: Generalized Base/Bounds

**Segmentation** was created to counteract **internal fragmentation** which occurs when a process gets allocated to a memory block that has more memory than necessary. This results in wasted memory, as seen in a previous example. In segmentation, memory is divided into **varied** segment sizes. A **segment** is an uninterrupted piece of the address space of a particular length. Each of these segments has its own base and bounds pair rather than the entire memory management unit having just one pair.

There are three different segment types that can occupy the address space:
* Code
* Stack
* Heap

**Segmentation** lets the OS put each one of those segments into different parts of physical memory and avoid filling the physical memory with unused virtual address space.

Let’s say we want to put the address space from our previous graphic into physical memory. If we have a base and bounds pair for every segment, we can put each one independently into physical memory.

In the graphic to the left, we see a `64KB`  physical memory with our three segments in it. Huge address spaces with large amounts of **sparse address space** can be accommodated because only used memory is allocated space in physical memory.

Our MMU’s hardware structure needs a set of three base and bounds register pairs to handle segmentation. The table below shows the register values for this example. Each bounds register holds the size of a segment.

| Segment | Base | Size |
|---|---|---|
|Code|`32KB`|`2KB`|
|Heap|`34KB`|`3KB`|
|Stack|`28KB`|`2KB`|

<p align="center">
  <img src="img/segmentation2.gif" alt="Segmentation 2">
</p>

* The code segment is placed at physical address `32KB` and has a size of `2KB`
* The heap segment is placed at `34KB` and has a size of `3KB`
* The stack segment is placed at `28KB` and has a size of `2KB`

The size segment is identical to the bounds register we mentioned previously. It tells the hardware of the number of valid bytes in the segment. This allows the hardware to detect when a software has accessed data outside of these bounds without permission.

### Question

Fill in the blank to complete the statement below.

Click the button below to submit.

In a system using **`segmentation`**, programs are represented as a collection of **`segments`** stored in a 
**`contiguous`** memory block.

> **Solution**
> 
> In a system using **`segmentation`**, programs are represented as a collection of **`segments`** stored in a 
**`contiguous`** memory block.

## Segmentation Fault

Let’s translate the address space in the graphic to the left.

Assume `100` is the virtual address (which is inside the code segment). When the reference happens (like on an instruction fetch), the hardware will add the base value to the offset into this segment (`100`) to get the desired physical address:

$$
100 + 32KB = 32868
$$

The hardware will then check if the address is within bounds (is `100 < 2KB`), see that it is, and issue the reference to physical memory address `32868`.

What about a virtual address of `4200` in the heap? Adding the virtual address `4200` to the heap’s base (`34KB`) gives us a physical address of `39016`, which is incorrect.

The first step is to get the **offset** into the heap, which tells us which byte(s) in this segment the address belongs to. Since the heap begins at virtual address `4KB` (`4096`), the `4200` offset is really `4200 - 4096`, or `104`. **We then add this offset (`104`) to the physical address of the base register (`34KB`) to get the desired result: `34920`**.

**What if we tried to refer to an illegal address past the end of the heap(i.e., a virtual address of `7KB` or more)?**

You can probably guess what happens next. The hardware determines that the address is out of bounds and will likely terminate the process. This results in what is commonly known as **segmentation fault**.

### Question

Which of the following occurs when we refer to an address space that lies beyond the end of the heap?

Select an answer and click the button below to submit.
- [x] Segmentation Fault
- [ ] Address Translation
- [ ] Paging
- [ ] Non-Contiguous Segmentation

> **Solution**
> 
> When an address space lies beyond the end of the heap, the hardware determines that the address is out of bounds and will likely terminate the process. This is called **`Segmentation Fault`**.


## Which Segment Do We Mean?

During translation, the hardware makes use of **segment registers**. How does it determine the offset into a segment, as well as which segment an address relates to? One common approach, known as an **explicit approach**, is to divide the address space into segments based on the first few bits of the virtual address.

```c
// get top 2 bits of 14-bit VA
Segment = (VirtualAddress & SEG_MASK) >> SEG_SHIFT
// now get offset
Offset  = VirtualAddress & OFFSET_MASK
if (Offset >= Bounds[Segment])
    RaiseException(PROTECTION_FAULT)
else
    PhysAddr = Base[Segment] + Offset
    Register = AccessMemory(PhysAddr)
```

In our previous example, we have three segments, so we only need two bits to complete our assignment. If we pick the segment using the first two bits of our 14-bit virtual address, our virtual address will look like this:

<p align="center">
  <img src="img/dir1.webp" alt="Dir 1">
</p>


If the top two bits are `00`, the hardware understands the virtual address is in the **code** segment and uses the code base and bounds pair to relocate it. If the top two bits are `01`, the hardware uses the **heap** base and bounds.

To clarify, let’s translate our previous heap virtual address (`4200`). Here is the virtual address `4200` in binary form:

<p align="center">
  <img src="img/dir2.webp" alt="Dir 2">
</p>

So, the first two bits (`01`) indicate the hardware which section we’re talking about. The last `12` bits indicate the segment offset: `000001101000`, hex `0x86`, decimal `104`.

So the hardware uses the first two bits to select the **segment register**, and the next `12` bits to **offset into the segment**. The final physical address is obtained by adding the base register to the offset.

The offset also simplifies the bounds check. If the offset is not less than the bounds, the address is illegal.

### Questions

Fill in the blank to complete the statement below.

Click the button below to submit.

The **`offset`**  is added to a segment’s base register to create the complete physical address.


> **Solution**:
> 
> The **`offset`** is added to a segment’s base register to create the complete *physical address*.

## Example

To get the desired physical address, the hardware would do something like the code segment to the left if the base and bounds were arrays (with one entry per segment).

In our continuous example, we can fill in the values for the constants in the code segment to the left.
* `SEG_MASK` would be set to `0x300`
* `SEG_SHIFT` is set to `12`
* `OFFSET_MASK` is set to `0xFFF`

You may have noticed that when we use the top two bits and there are just three segments (code, heap, and stack), **one segment of the address space is left unused**. Some systems put code in the same segment as the heap to fully use the virtual address space (and avoid an unused segment) and use only one bit to decide which segment to use.

**Using so many bits to pick a segment also limits the use of virtual address space**. Each segment is limited to a maximum size. In our example, `4KB` (using the top two bits to choose segments implies the `16KB` address space gets chopped into four pieces, or `4KB` in this example). A program that wants to expand a segment (like the heap or the stack) beyond that limit is out of luck.

The hardware can also determine the segment an address belongs to. The implicit approach determines the segment by examining the address. If the address came from the program counter (i.e., an instruction fetch), it’s in the **code** segment. If it came from the stack or base pointer, it’s in the **stack** segment. All others are in the **heap**.

### Questions

Select **all** of the valid types of segments below.
- [x] heap
- [x] code
- [ ] array
- [ ] counter
- [x] stack
- [ ] base
- [ ] bound

> **Solution**:
>
> There are three valid segments types: `code`, `stack`, and `heap`.

## What About the Stack?

<p align="center">
  <img src="img/segmentation3.gif" alt="segmentation 3">
</p>


Next, let’s talk about the **stack**. If we revisit our previous graphic, the stack has been shifted to physical address `28KB`, but with one major difference: it now grows backwards (towards lower addresses). It “starts” at `28 KB` and expands back to `26 KB` in physical memory, corresponding to virtual addresses `16 KB` to `14 KB`. Translation has to proceed in a different way.


The first thing we need is some more hardware support. In addition to the base and boundary numbers, the hardware also has to know which way the segment will grow (a bit, for example, that is set to `1` when the segment grows in the positive direction, and `0` for negative). The table below shows our modified view of the hardware tracks:

| Segment | Base | Size (max `4K`) | Grows positive? |
|---|---|---|---|
|`Code_00`|`32KB`|`2KB`|`1`|
|`Heap_01`|`34KB`|`3KB`|`1`|
|`Stack_11`|`28KB`|`2KB`|`0`|

Because the hardware now recognizes that segments can grow in the opposite direction, it has to now translate virtual addresses in a different way. Let’s look at an example stack virtual address and translate it.

Say we want to access virtual address `15KB`, which should correspond to physical address `27KB` in this example. In binary representation, our virtual address looks like this:

```
11 1100 0000 0000 (hex 0x3C00)
```

The first two bits (`11`) are used by the hardware to designate the segment, but we are left with a `3KB` offset. To get the right negative offset, subtract the maximum segment size from `3KB`. A segment can be `4KB` in this case, so the correct negative offset is:

$$
3KB - 4KB = -1KB
$$

To get the right physical address, we add the negative offset (`-1KB`) to the base (`28KB`). The bounds check is done by confirming that the negative offset’s absolute value is less than or equal to the segment’s current size (in this case, `2 KB`).

### Questions

By adding the negative offset to the base, we are able to calculate what?
- [ ] base and boundary number
- [x] physical address
- [ ] positive or negative growth
- [ ] segment size

> **Answer**
> 
> To get the right **`physical address`**, we add the negative offset to the base.

## OS Support

You should now understand the basics of **segmentation**. As the system operates, pieces of the address space are relocated into physical memory, saving a lot of space compared to using a single base/bounds pair for the whole address space. Specifically, the empty space between the stack and the heap does not need to be allocated in physical memory, allowing us to support larger virtual address spaces per process.

But segmentation presents new challenges for the OS.
1. **What should the OS do on a context switch?** The segment registers must be saved and restored. Each process has its own virtual address space, which the OS must correctly set up before continuing execution.
2. **When segments grow, the OS interacts (or perhaps shrink)**. To allocate an object, a program can use `malloc()`. In some cases, the current heap can satisfy the request, and `malloc()` will find free space for the object and return a pointer to the caller. In others, the heap segment may need to increase.
   * In this case, the memory-allocation library will use a system call to expand the heap (e.g., `sbrk()`). As a result, the OS normally provides more space, updating the segment size register to the new (larger) size, and informing the library of success. The OS may deny the request if no more physical memory is available or if the calling process has too much.
3. Finally, and maybe most importantly, **managing physical memory free space**. The OS must be able to find physical memory space for new address spaces. Before, we assumed that each address space had the same size, and physical memory could therefore be described as a series of slots for processes. Now we have multiple segments per process, each with a different size.

The main issue is that **physical memory soon fills up with pockets of free space**, making it impossible to assign new segments or expand old ones. We call this **external fragmentation**. We can see an example of this in the next figure.

<p align="center">
  <img src="img/segmentation5.png" alt="segmentation 5">
</p>

In this case, a process requests a `20 KB` section. In this case, there is `24 KB` free, but not in one piece (rather, in three non-contiguous chunks). So the OS can’t handle the `20 KB`. If the next so many bytes of physical space are not accessible, **the OS must deny the request**, even if there are free bytes elsewhere in physical memory.

**Rearranging existing memory parts could help compact physical memory**. For example, the OS may stop all current processes, copy their data to one contiguous region of memory, and update their segment register values to point to the new physical addresses. So the OS lets the next allocation request succeed. However, copying segments is memory-intensive and takes up a lot of processor time. Compaction also makes requests to grow current segments difficult to meet.

### Questions

When physical memory is so full of empty space that assigning new segments or expanding old ones becomes impossible, which of the following happens?

Select an answer and click the button below to submit.
- [x] External Fragmentation
- [ ] Segmentation Fault
- [ ] Length Limit
- [ ] Bounds Breach

> **Answer**:
>
> When physical memory fills up with pockets of free space, this makes it impossible to assign new segments or expand old ones. We call this **`External Fragmentation`**.

## Summary

# Segmentation solves many difficulties and improves memory virtualization.
* It is also fast since the arithmetic segmentation is simple and hardware-friendly. Translation overheads are minor.
* Beyond dynamic relocation, segmentation can help sparse address spaces by reducing the amount of memory wasted across logical address space segments.
* However, as we discovered, allocating variable-sized segments in memory causes certain issues.
* The first is external fragmentation. Because segments vary in size, free memory is divided into odd-sized chunks, making memory allocation challenging.

The problem is fundamental and hard to avoid using smart algorithms or periodically compact memory.
* Second, and perhaps more importantly, segmentation still isn’t flexible enough to support our fully generalized, sparse address space.
* If we have a huge but rarely used heap in one logical segment, we must access the entire heap in memory.

The address space model we use does not exactly match how the underlying segmentation was designed to support it, so we need fresh solutions.