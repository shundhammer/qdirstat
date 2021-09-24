# QDirStat Debugging Tips

## Problem: Directory reading is too fast to debug anything

### Symptom

Only the very first time, reading a large directory like / takes long enough to
experiment with anything in the tree widget. Any subsequent time, it's just too
fast - one or two seconds, and *bam* it's finished already.

### Reason

The Linux kernel has become incredibly good at caching directory information,
and today's PCs have so much RAM that the kernel tends to use a large amount of
it as cache - files, inodes, dentries (directories). Once a directory is read,
it remains in the cache for a long time, so the speedup upon a subsequent read
is enormous.

### Fix / Workaround

Drop the kernel caches (as root):

    su -
    echo 3 > /proc/sys/vm/drop_caches

or

    echo 3 | sudo tee /proc/sys/vm/drop_caches

(`sudo echo 3 > /proc/sys/vm/drop_caches` would NOT work because your non-root
shell would do the I/O redirection, so it would not have sufficient privileges)


### Reference

https://unix.stackexchange.com/questions/87908/how-do-you-empty-the-buffers-and-cache-on-a-linux-system


## Profiling QDirStat with Valgrind / KCachegrind

### Prerequisites

Install vallgrind and kcachgrind:

    sudo apt install valgrind kcachegrind


### Profile

Start QDirStat with the valgrind profiler:

    valgrind --tool=callgrind --dump-instr=yes --simulate-cache=yes --collect-jumps=yes qdirstat ~

Don't use a huge directory since the profiling slows down everything
considerably!

The results go to a file `callgrind.out.*` in that directory. Visualize with

    kcachegrind


### Reference

https://developer.mantidproject.org/ProfilingWithValgrind.html
