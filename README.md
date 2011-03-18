# CSOS

## Memory Map

* `00000000` - `000FFFFF` (1MiB) **Low Memory**. I may want to use this later for V86 stuff

* `00000500` - `0000053B` (i dunno) **Saved registers when dropping to real mode**

* `00000600` - `000015FF` (4KiB) **My GDT**. Space reserved for my GDT.

* `00001600` - `00001D00` (2KiB) **My IDT**.

* `00002000` - `00002400` (1 KiB) **Real/Protected communication**

* `00120000` - `001FFFFF` **My kernel!**

* `00200000` - `003FFFFF` **Kernel Heap**

* `00400000` - `007FFFFF` **Page tables**