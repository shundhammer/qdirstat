# Btrfs and Free Size Reporting

Btrfs is a Linux filesystem with advanced features like copy-on-write and
snapshots. While that is very convenient to go back to a previous state of the
system after botched package upgrades or manual configurations, it also comes
at a cost: It consumes disk space.

However, disk space used in that way is **not** reported to the usual tools
like the `df` command or even system calls like `statfs()`; Btrfs only reports
disk space used by files and directories to them.

So it can easily happen (this is actually quite a common problem) that the `df`
command (or other similar commands that rely on Btrfs reporting sizes properly)
tells you that your Btrfs filesystem still has 15 GB of its total 30 GB
available, yet you get a "no space left on device" error: The remaining space
might be taken by filesystem snapshots and/or by copy-on-write.

One might argue that Btrfs is blatantly lying about its free space, that it
should really report the real free disk space to the underlying system calls
like `statfs()`, so the tools using that system call can give the user the real
information and not just some bogus numbers.


## Helpful Btrfs Commands

To find out more about available disk space on a Btrfs, you need to use special
Btrfs commands, and they _all_ require root permissions (which is one reason
why QDirStat does not do that internally).

Yes, this is most annoying.

Yes, this could be done better; a **lot** better.

Yes, this problem has been known for many years, and no progress is visible in
that area.


### btrfs fi usage

```
sudo btrfs filesystem usage /mybtrfs

Overall:
    Device size:		  40.00GiB
    Device allocated:		  30.06GiB
    Device unallocated:		   9.94GiB
    Device missing:		     0.00B
    Used:			  26.80GiB
    Free (estimated):		  10.92GiB	(min: 5.95GiB)
    Data ratio:			      1.00
    Metadata ratio:		      2.00
    Global reserve:		  63.12MiB	(used: 0.00B)
    Multiple profiles:		        no

Data,single: Size:25.00GiB, Used:24.02GiB (96.07%)
   /dev/sda2	  25.00GiB

Metadata,DUP: Size:2.50GiB, Used:1.39GiB (55.69%)
   /dev/sda2	   5.00GiB

System,DUP: Size:32.00MiB, Used:16.00KiB (0.05%)
   /dev/sda2	  64.00MiB

Unallocated:
   /dev/sda2	   9.94GiB

```

This can be slightly abbreviated to

```
sudo btrfs fi usage /mybtrfs
```

_Notice that all Btrfs commands always need the mount point as the argument, not the device._


### btrfs fi df

```
sudo btrfs filesystem df /mybtrfs

Data, single: total=25.00GiB, used=24.02GiB
System, DUP: total=32.00MiB, used=16.00KiB
Metadata, DUP: total=2.50GiB, used=1.39GiB
GlobalReserve, single: total=63.12MiB, used=0.00B
```

Shorter: `sudo btrfs fi df /mybtrfs`


### btrfs fi show

```
sudo btrfs filesystem show /mybtrfs

Label: 'mybtrfs'  uuid: a0be3e1e-d127-45b8-85d6-...
	Total devices 1 FS bytes used 25.41GiB
	devid    1 size 40.00GiB used 30.06GiB path /dev/sda2
```

Shorter: `sudo btrfs fi show /mybtrfs`


### btrfs balance

https://btrfs.wiki.kernel.org/index.php/FAQ#What_does_.22balance.22_do.3F

Greatly simplified, this is the Btrfs counterpart of defragmenting the
filesystem: It cleans up metadata.

How to invoke it, however, seems to be some black magic, and there are lots of
guides out on the web; google for "btrfs balance". Some search results:

- https://www.thegeekdiary.com/how-to-re-balancing-btrfs-to-free-disk-space/
- http://marc.merlins.org/perso/btrfs/post_2014-05-04_Fixing-Btrfs-Filesystem-Full-Problems.html


## Background Information


### Snapshots

A main reason for using Btrfs in the first place is using snapshots before
critical changes to the system, such as software package upgrades: Before
installing upgraded package versions, the old system state is saved into a
_snapshot_, then the upgrade is performed. If the upgrade resulted in problems,
you can _roll back_ to a previous snapshot.

This can be done manually or with tools such as _snapper_. On SUSE Linux, a
snapshot is created with _snapper_ for every set of package upgrades or when
certain system administration tasks are done with _YaST_, typically even a
"pre" snapshot before the action is performed and a "post" snapshot immediately
afterwards.

Since Btrfs uses CoW, this is not nearly as expensive as one might think: Only
changed disk blocks need to be actually copied, not the complete filesystem.

However, as snapshots accumulate, so does disk space usage. This is why
_snapper_ has multiple strategies to clean up snapshots. Even so, you can run
out of disk space if you don't keep track of your snapshots, in particular if
you use a rolling release like openSUSE Tumbleweed with regular updates every
few days, and free space on your Btrfs root filesystem wasn't too plentiful to
begin with.

You can use the `snapper` command line (`snapper ls`, `snapper rm`) or the YaST
snapper module to monitor and manage snapshots. If you find you have to clean
up snapshots manually very often, consider using a different automatic strategy
for cleaning them up.

See also `man snapper`.


### CoW (Copy-on-Write)

https://en.wikipedia.org/wiki/Copy-on-write

Btrfs by default uses CoW (copy-on-write) for write access: It can share disk
blocks among different _subvolumes_ or _snapshots_ (which are technically
little different from subvolumes). As long as the content of a file and its
disk blocks is identical between subvolumes or snapshots, there is no need to
copy all the blocks of that file, so all that Btrfs does is add a new reference
to it and increase a reference count.

When data are written to that file, however, the old data need to be saved to
remain available in the old version to that subvolume or snapshot; so at that
moment, the affected disk blocks are really copied, and only then new content
is written to the file.

This is a very efficient way of keeping copies of older data so you can go back
to a previous snapshot: Most data are identical between snapshots, so there is
no need to really copy them; a reference count does just nicely.

However, this can be a nightmare for data that change all the time, and change
in random places: Binary database files for RDBMS such as MariaDB (formerly
MySQL) are a typical example. Also, you never want to roll back such files
anyway because you cannot guarantee consistency of such binary data; they need
to be exempt from snapshots.

This is why there are _subvolumes_ with different mount options such as `noCoW`
for certain directories; not only would it be wasteful to keep older versions
of such files, it would also be counterproductive and endanger consistency.


## Further Reading

- https://www.linuxlinks.com/btrfs/
- https://btrfs.wiki.kernel.org/index.php/FAQ#How_much_free_space_do_I_have.3F
- https://btrfs.wiki.kernel.org/index.php/SysadminGuide
- https://en.opensuse.org/SDB:BTRFS
