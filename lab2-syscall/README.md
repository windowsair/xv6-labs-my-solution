# Foreword

This lab is mainly used to get familiar with syscall. All the hints are available on the lab website. Here are some key points that I have summarized:

1. It is better to have a general understanding of the virtual memory mechanism of RISC-V.
2. Use spinlock carefully.


# System call tracing

All the hints are available on the lab website.

# sysinfo

If you don't understand the page table at all, there may be some trouble in implementing statistics on free memory. Reading section 3.5 in Chapter 3 of the xv6 book can help you understand what's going on.

> In fact, the page tables are stored as list(struct run), each with a fixed size.