# Finding Files that are Shadowed by a Mount

(c) 2021 Stefan Hundhammer <Stefan.Hundhammer@gmx.de>

License: GNU Free Documentation License


## Problem

If files are _shadowed_ by a mount, they are invisible, yet they still occupy
disk space.

For example, if your root filesystem contains an existing directory
`/home/kilroy` with 2 GB worth of files, but you also have a separate `/home`
filesystem that has an `/etc/fstab` entry, that separate `/home` filesystem is
mounted _over_ those files in `/home/kilroy`, making those 2 GB invisible and
unaccessible, no matter if the separate `/home` also has a `/home/kilroy`
directory. Of course those _shadowed_ files still occupy disk space; you just
can't see them.

What `QDirStat` or `du` report will be different from what `df` reports as
"used".

_Notice: If it's Btrfs, that is commonly caused by snapshots, subvolumes, CoW
and other Btrfs weirdness, so don't worry. See also
https://github.com/shundhammer/qdirstat/blob/master/doc/Btrfs-Free-Size.md._

## Low-tech Solution

Boot from live media (a Linux on a USB stick) or, if you have one, another
Linux installation on the same machine and mount that filesystem from there:

```console
tux@live-linux:~$ lsblk /dev/sdb
NAME   MAJ:MIN RM   SIZE RO TYPE MOUNTPOINT
sdb      8:16   0 931,5G  0 disk
├─sdb1   8:17   0     2G  0 part
├─sdb2   8:18   0    30G  0 part
├─sdb3   8:19   0    30G  0 part
├─sdb4   8:20   0     1K  0 part
└─sdb5   8:21   0 869,5G  0 part
tux@live-linux:~$ sudo mount /dev/sdb2 /mnt
tux@live-linux:~$ sudo du -hs /mnt/home
2.0G    /mnt/home
```

The `lsblk` command is to find out what partition the root filesystem was.
After booting from a live system, your single disk will probably be `/dev/sdb`
since `/dev/sda` is typically the USB stick that you booted from. If you have
more disks, just use a plain `lsblk` command to see all your disks with all
your partitions.

On many live systems you can even install software packages, so you may find it
worthwhile to install QDirStat on your Ubuntu live system:

```console
tux@live-linux:~$ sudo apt install qdirstat
tux@live-linux:~$ xhost +
tux@live-linux:~$ sudo qdirstat /mnt
```

(Starting QDirStat with `sudo` to have permissions for all subdirectories)

# The True Linux Nerd Solution

We don't need no stinkin' reboot. We can do that while the system keeps running.

Why? **_Because we can!_**

...or you may have a storage setup that is slightly too complex to mount it
casually by hand; if you have full disk encryption or LVM or RAID or any
combination of those.


## General Idea

- Bind-mount the filesystem to have it accessible a second time, but without
  other filesystems mounted on top of it.

- In that bind-mounted tree, bind-mount all subdirectories that are a mount
  point in the original tree. Mount them all to a common parent to make it
  easier to see all shadowed files (and only them) at once.

- Examine that tree, e.g. with QDirStat.


## WARNING

There is the potential of things going wrong in quite spectacular ways. If you
do things wrong, some bind-mounted directory might look like stuff that you
don't need, and you may feel compelled to delete a whole directory tree that
looks like a duplicated `/usr/lib` directory copied to a strange
location. Don't casually do that; it might be your _real_ `/usr/lib` that just
appeared a second time at that strange location, and if you delete it, you will
delete the real `/usr/lib`, and that will for sure wreck your system so you
will have to reinstall.

I have seen Linux system developers (not noobs!) do that by accident when
trying to reclaim disk space in their _Scratchbox_ cross-compilation
environment which also makes heavy use of bind-mounts (and symlinks).

Exercise extreme caution when handling anything that you found with this
method!

If you are not sure, better don't remove or even just rename or move
anything. Double-check what you are doing. Try to _create_ a file there
(`touch schlonz`) and then use a global `find / -name schlonz` to see where the
name appears; you may be surprised. **Then** make your decision.

Whatever you do with the instructions and script(s) presented here, **use at
your own risk**.


## Steps and Useful Commands

This is the manual solution. Scroll down for a simple script that does this for
the root filesystem and its direct mount points.


### 1. Bind-mount the Root Directory

```console
root@linux:~# mkdir -p /mnt/root
root@linux:~# mount -o bind / /mnt/root
```

### 2. Find all Real Mounts

- This lists the "real" mounts from `/proc/mounts`, ignoring the cruft (`/sys`,
  `/proc`, `/dev`, `/run`) and any bind-mounts in `/mnt` that we might already
  have created (in previous failed steps):

```console
root@linux:~# egrep -v ' /(sys|proc|dev|run|mnt)' /proc/mounts
/dev/sdc2 / ext4 rw,relatime,errors=remount-ro,data=ordered 0 0
/dev/sdb2 /hd-root-18-04 ext4 rw,relatime,data=ordered 0 0
/dev/sdb3 /hd-root-14-04 ext4 rw,relatime,data=ordered 0 0
/dev/sdc3 /ssd-root-20-04 ext4 rw,relatime,data=ordered 0 0
/dev/sdc4 /ssd-work ext4 rw,relatime,data=ordered 0 0
/dev/sda1 /win/boot fuseblk rw,relatime,user_id=0,group_id=0,default_permissions,allow_other,blksize=4096 0 0
/dev/sdb5 /work ext4 rw,relatime,data=ordered 0 0
/dev/sda2 /win/app fuseblk rw,relatime,user_id=0,group_id=0,default_permissions,allow_other,blksize=4096 0 0
```

- Use only the second field (the mount points) from that:

```console
root@linux:~# egrep -v ' /(sys|proc|dev|run|mnt)' /proc/mounts | cut -d ' ' -f 2
/
/hd-root-18-04
/hd-root-14-04
/ssd-root-20-04
/ssd-work
/win/boot
/work
/win/app
```

- Ignore the root directory from that:

```console
root@linux:~# egrep -v ' /(sys|proc|dev|run|mnt)' /proc/mounts | cut -d ' ' -f 2 | grep -v '^/$'
/hd-root-18-04
/hd-root-14-04
/ssd-root-20-04
/ssd-work
/win/boot
/work
/win/app
```

### 2a. Caveat: Btrfs

Btrfs, being the _enfant terrible_ of filesystems that it is, needs special
treatment, as always.

When you bind-mount a Btrfs, you implicitly bind-mount its _subvolumes_, too
(tested on a recent openSUSE Tumbleweed from 3/2021). That means there is no
reasonable way to check if underneath all those mounted subvolumes there are
any shadowed files since even a bind-mount will shadow them immediately
again. Use the low-tech solution for that (see above: Booting from a live
system).

You can still check if the _other_ mounts (non-subvolume mounts) are shadowing
any files, though; but you need a few more steps for that, or you will drown in
all the stuff that is on the subvolumes because that will all show up in the
final result.

Unless, of course, you explicitly exclude the subvolumes as well. For that we
first need to find out on what device the root filesystem is:

```console
tux@linux:~> grep ' / ' /proc/mounts | cut -d ' ' -f 1
/dev/sda2
```

Or stored in a shell variable for later use:

```console
tux@linux:~> ROOT_DEVICE=$(grep ' / ' /proc/mounts | cut -d ' ' -f 1)
tux@linux:~> echo $ROOT_DEVICE
/dev/sda2
```

All the subvolumes are listed in `/proc/mounts` with that same device:

```console
tux@linux:~> grep $ROOT_DEVICE /proc/mounts
/dev/sda2 / btrfs rw,relatime,space_cache,subvolid=256,subvol=/@ 0 0
/dev/sda2 /boot/grub2/i386-pc btrfs rw,relatime,space_cache,subvolid=264,subvol=/@/boot/grub2/i386-pc 0 0
/dev/sda2 /boot/grub2/x86_64-efi btrfs rw,relatime,space_cache,subvolid=263,subvol=/@/boot/grub2/x86_64-efi 0 0
/dev/sda2 /opt btrfs rw,relatime,space_cache,subvolid=262,subvol=/@/opt 0 0
/dev/sda2 /var btrfs rw,relatime,space_cache,subvolid=258,subvol=/@/var 0 0
/dev/sda2 /root btrfs rw,relatime,space_cache,subvolid=261,subvol=/@/root 0 0
/dev/sda2 /srv btrfs rw,relatime,space_cache,subvolid=260,subvol=/@/srv 0 0
/dev/sda2 /usr/local btrfs rw,relatime,space_cache,subvolid=259,subvol=/@/usr/local 0 0
```

Now we can extend our pipeline from the previous section to exclude them all:

```console
tux@linux:~> grep -v "^$ROOT_DEVICE" /proc/mounts | egrep -v " /(sys|proc|dev|run|mnt)" | cut -d ' ' -f 2 | grep -v '^/$'
/tmp
/work/tmp
/work/src
/home
```

That's the list we need.


### 3. Bind-mount those Real Mounts

- Bind-mount one of those mount points:

```console
root@linux:~# mkdir -p /mnt/shadowed/work
root@linux:~# mount -o bind /mnt/root/work /mnt/shadowed/work
```

- Bind-mount more (or all) of them:

```console
root@linux:~# mkdir -p /mnt/shadowed/win_app
root@linux:~# mount -o bind /mnt/root/win/app /mnt/shadowed/win_app
root@linux:~# mkdir -p /mnt/shadowed/win_boot
root@linux:~# mount -o bind /mnt/root/win/app /mnt/shadowed/win_boot
```

Make sure not to again shadow directories while doing that; that's why this
example uses `/mnt/shadowed/win_app` and `/mnt/shadowed/win_boot`, not
`/mnt/shadowed/win/app` and `/mnt/shadowed/win/boot`. Use a flat list of target
mount points, not a tree. It's not a problem here, but it might be a problem if
there is more nesting, e.g. `/mnt/shadowed/var` and `/mnt/shadow/var/log`.


### 4. Check for Shadowed Files

By bind-mounting the mount points a second time to `/mnt/shadowed`, all files
that are not shadowed are now excluded; each file below `/mnt/shadowed` is a
shadowed one.

- Quick check with `du`:

```console
root@linux:~# du -hs /mnt/shadowed
```

This will report _something_ because directories also need a couple of disk
blocks; but it shouldn't be much, just about one _cluster_ (typically 4k) for
each directory.

- Check with `tree` (if you have it installed):

```console
root@linux:~# tree /mnt/shadowed
```

If you see more than just directories, you found your trouble spot.

- Check for files with `find`:

```console
root@linux:~# find /mnt/shadowed -type f
```

This better not find anything.

- Run QDirStat to see all the shadowed files:

```console
root@linux:~# qdirstat /mnt/shadowed
```

Notice that you can of course delete files with QDirStat in that setup. As
usual, be careful; maybe just move the files to another directory (one that is
not shadowed).

_Notice that you cannot delete, rename or move any of the bind mount points as
long as a mount is active on them: You will get an error "Device or resource
busy" if you try._


### Misc

`lsblk` shows all block devices (i.e. all disks and all partitions), no matter
if they are mounted or not. It doesn't need root permissions.

```console
sh@balrog:~$ lsblk
NAME   MAJ:MIN RM   SIZE RO TYPE MOUNTPOINT
sda      8:0    0 931,5G  0 disk
├─sda1   8:1    0  97,7G  0 part /win/boot
└─sda2   8:2    0 833,9G  0 part /win/app
sdb      8:16   0 931,5G  0 disk
├─sdb1   8:17   0     2G  0 part
├─sdb2   8:18   0    30G  0 part /hd-root-18-04
├─sdb3   8:19   0    30G  0 part /hd-root-14-04
├─sdb4   8:20   0     1K  0 part
└─sdb5   8:21   0 869,5G  0 part /work
sdc      8:32   0 232,9G  0 disk
├─sdc1   8:33   0     2G  0 part [SWAP]
├─sdc2   8:34   0    30G  0 part /
├─sdc3   8:35   0    30G  0 part /ssd-root-20-04
└─sdc4   8:36   0 170,9G  0 part /ssd-work
sr0     11:0    1  1024M  0 rom
```

`findmnt` shows all mounts with their mount hierarchy, even if a filesystem is
mounted to a mount point that is itself a mounted filesystem (i.e. not on the
root filesystem). It doesn't need root permissions.

```console
sh@balrog:~$ findmnt -t ext4,ext3,ext2,xfs,btrfs,fuseblk,vfat,vboxsf
TARGET            SOURCE    FSTYPE  OPTIONS
/                 /dev/sdc2 ext4    rw,relatime,errors=remount-ro,data=ordered
├─/hd-root-14-04  /dev/sdb3 ext4    rw,relatime,data=ordered
├─/hd-root-18-04  /dev/sdb2 ext4    rw,relatime,data=ordered
├─/ssd-root-20-04 /dev/sdc3 ext4    rw,relatime,data=ordered
├─/ssd-work       /dev/sdc4 ext4    rw,relatime,data=ordered
├─/win/boot       /dev/sda1 fuseblk rw,relatime,user_id=0,group_id=0,default_permissions,allow_other
├─/win/app        /dev/sda2 fuseblk rw,relatime,user_id=0,group_id=0,default_permissions,allow_other
└─/work           /dev/sdb5 ext4    rw,relatime,data=ordered
```

Btrfs with mounted subvolumes and two VirtualBox _shared folders_:

```console
tux@linux:~> findmnt -t ext4,ext3,ext2,xfs,btrfs,fuseblk,vfat,vboxsf
TARGET                   SOURCE                              FSTYPE OPTIONS
/                        /dev/sda2[/@]                       btrfs  rw,relatime,space_cache,subvolid=256,subvol=/@
├─/opt                   /dev/sda2[/@/opt]                   btrfs  rw,relatime,space_cache,subvolid=262,subvol=/@/opt
├─/boot/grub2/i386-pc    /dev/sda2[/@/boot/grub2/i386-pc]    btrfs  rw,relatime,space_cache,subvolid=264,subvol=/@/boot/grub2/i386-pc
├─/boot/grub2/x86_64-efi /dev/sda2[/@/boot/grub2/x86_64-efi] btrfs  rw,relatime,space_cache,subvolid=263,subvol=/@/boot/grub2/x86_64-efi
├─/var                   /dev/sda2[/@/var]                   btrfs  rw,relatime,space_cache,subvolid=258,subvol=/@/var
├─/root                  /dev/sda2[/@/root]                  btrfs  rw,relatime,space_cache,subvolid=261,subvol=/@/root
├─/srv                   /dev/sda2[/@/srv]                   btrfs  rw,relatime,space_cache,subvolid=260,subvol=/@/srv
├─/usr/local             /dev/sda2[/@/usr/local]             btrfs  rw,relatime,space_cache,subvolid=259,subvol=/@/usr/local
├─/work/tmp              work_tmp                            vboxsf rw,nodev,relatime
├─/work/src              work_src                            vboxsf rw,nodev,relatime
└─/home                  /dev/sda4                           ext4   rw,relatime,data=ordered
```

There doesn't seem to be a simple way to exclude all the kernel pseudo filesystems, i.e. a plain `findmnt` shows all the cruft, too:

```console
sh@balrog:~$ findmnt
TARGET                                SOURCE      FSTYPE    OPTIONS
/                                     /dev/sdc2   ext4      rw,relatime,errors=remount-ro,data=order
├─/sys                                sysfs       sysfs     rw,nosuid,nodev,noexec,relatime
│ ├─/sys/kernel/security              securityfs  securityf rw,nosuid,nodev,noexec,relatime
│ ├─/sys/fs/cgroup                    tmpfs       tmpfs     ro,nosuid,nodev,noexec,mode=755
│ │ ├─/sys/fs/cgroup/unified          cgroup      cgroup2   rw,nosuid,nodev,noexec,relatime,nsdelega
│ │ ├─/sys/fs/cgroup/systemd          cgroup      cgroup    rw,nosuid,nodev,noexec,relatime,xattr,na
│ │ ├─/sys/fs/cgroup/blkio            cgroup      cgroup    rw,nosuid,nodev,noexec,relatime,blkio
│ │ ├─/sys/fs/cgroup/hugetlb          cgroup      cgroup    rw,nosuid,nodev,noexec,relatime,hugetlb
│ │ ├─/sys/fs/cgroup/perf_event       cgroup      cgroup    rw,nosuid,nodev,noexec,relatime,perf_eve
│ │ ├─/sys/fs/cgroup/cpu,cpuacct      cgroup      cgroup    rw,nosuid,nodev,noexec,relatime,cpu,cpua
│ │ ├─/sys/fs/cgroup/net_cls,net_prio cgroup      cgroup    rw,nosuid,nodev,noexec,relatime,net_cls,
│ │ ├─/sys/fs/cgroup/cpuset           cgroup      cgroup    rw,nosuid,nodev,noexec,relatime,cpuset
│ │ ├─/sys/fs/cgroup/pids             cgroup      cgroup    rw,nosuid,nodev,noexec,relatime,pids
│ │ ├─/sys/fs/cgroup/rdma             cgroup      cgroup    rw,nosuid,nodev,noexec,relatime,rdma
│ │ ├─/sys/fs/cgroup/memory           cgroup      cgroup    rw,nosuid,nodev,noexec,relatime,memory
│ │ ├─/sys/fs/cgroup/freezer          cgroup      cgroup    rw,nosuid,nodev,noexec,relatime,freezer
│ │ └─/sys/fs/cgroup/devices          cgroup      cgroup    rw,nosuid,nodev,noexec,relatime,devices
│ ├─/sys/fs/pstore                    pstore      pstore    rw,nosuid,nodev,noexec,relatime
│ ├─/sys/kernel/debug                 debugfs     debugfs   rw,relatime
│ ├─/sys/fs/fuse/connections          fusectl     fusectl   rw,relatime
│ └─/sys/kernel/config                configfs    configfs  rw,relatime
├─/proc                               proc        proc      rw,nosuid,nodev,noexec,relatime
│ └─/proc/sys/fs/binfmt_misc          systemd-1   autofs    rw,relatime,fd=32,pgrp=1,timeout=0,minpr
│   └─/proc/sys/fs/binfmt_misc        binfmt_misc binfmt_mi rw,relatime
├─/dev                                udev        devtmpfs  rw,nosuid,relatime,size=8168840k,nr_inod
│ ├─/dev/pts                          devpts      devpts    rw,nosuid,noexec,relatime,gid=5,mode=620
│ ├─/dev/shm                          tmpfs       tmpfs     rw,nosuid,nodev
│ ├─/dev/mqueue                       mqueue      mqueue    rw,relatime
│ └─/dev/hugepages                    hugetlbfs   hugetlbfs rw,relatime,pagesize=2M
├─/run                                tmpfs       tmpfs     rw,nosuid,noexec,relatime,size=1638392k,
│ ├─/run/lock                         tmpfs       tmpfs     rw,nosuid,nodev,noexec,relatime,size=512
│ └─/run/user/1000                    tmpfs       tmpfs     rw,nosuid,nodev,relatime,size=1638388k,m
│   └─/run/user/1000/gvfs             gvfsd-fuse  fuse.gvfs rw,nosuid,nodev,relatime,user_id=1000,gr
├─/hd-root-14-04                      /dev/sdb3   ext4      rw,relatime,data=ordered
├─/hd-root-18-04                      /dev/sdb2   ext4      rw,relatime,data=ordered
├─/ssd-root-20-04                     /dev/sdc3   ext4      rw,relatime,data=ordered
├─/ssd-work                           /dev/sdc4   ext4      rw,relatime,data=ordered
├─/win/boot                           /dev/sda1   fuseblk   rw,relatime,user_id=0,group_id=0,default
├─/win/app                            /dev/sda2   fuseblk   rw,relatime,user_id=0,group_id=0,default
└─/work                               /dev/sdb5   ext4      rw,relatime,data=ordered
```

That's why it is advisable to explicitly specify `-t` with the filesystem types
that should be listed, otherwise you get drowned in all that _cgroup_
etc. garbage.

### Clean Up (Unmount all the Bind Mounts)

- Find all the mounts in `/mnt/shadowed`
- Unmount each one of them
- Unmount `/mnt/root`

- Remove the mount directories:

```console
root@linux:~# rmdir /mnt/shadowed/*
root@linux:~# rmdir /mnt/shadowed
root@linux:~# rmdir /mnt/root
```

Don't just use `rm -rf`; if there is still anything else than those
directories, something (some `umount`?) went wrong. You might accidentally
remove a whole directory tree on your root filesystem, wrecking your system in
the process. `rmdir` on the other hand is safe: It complains if the directory
is not empty.

----------------------------------------------------------------------

# The Script

This is a script that works for standard situations where files on the root
filesystem are shadowed by one or more direct mounts.

This does not cover shadowed files on another filesystem, e.g. a separate
`/var` filesystem with files in `/var/log` shadowed by a separate `/var/log`
filesystem mounted over it. That's easy to recover from by simply unmounting
`/var/log`.

This also does not work with crazy mount point names (or paths) with blanks or
other characters that make the shell barf. _If you do that, you clearly deserve
punishment, so you have to do it manually._ ;-)

But it supports a Btrfs root filesystem (including handling the subvolumes).


## Fetch the Latest Version

```console
tux@linux:~/tmp> wget https://raw.githubusercontent.com/shundhammer/qdirstat/master/scripts/shadowed/unshadow-mount-points
```

or use `curl` if you prefer that. Or use your web browser: Go to
https://github.com/shundhammer/qdirstat/tree/master/scripts/shadowed
and use "Save linked content as..." from the browser's context menu (right
click).

In any case, make sure the script is executable:

```console
tux@linux:~/tmp> chmod 755 unshadow-mount-points
```

## Dry run

To see what it _would_ do without actually doing it, use the `-n` (dry run)
command line option and call it as a normal user:

```console
tux@linux:~/tmp> ./unshadow-mount-points -n

*** Dry run - not executing any dangerous commands. ***

mkdir -p /mnt/root
mount -o bind / /mnt/root
mkdir -p /mnt/shadowed
mkdir -p /mnt/shadowed/tmp
mount -o bind /mnt/root/tmp /mnt/shadowed/tmp
mkdir -p /mnt/shadowed/work_tmp
mount -o bind /mnt/root/work/tmp /mnt/shadowed/work_tmp
mkdir -p /mnt/shadowed/work_src
mount -o bind /mnt/root/work/src /mnt/shadowed/work_src
mkdir -p /mnt/shadowed/home
mount -o bind /mnt/root/home /mnt/shadowed/home
tux@linux:~/tmp> ls -lR /mnt
/mnt:
total 0
tux@linux:~/tmp> grep '/mnt' /proc/mounts
tux@linux:~/tmp>
```

It did not execute any of the `mount` of `mkdir` commands; it just shows you
what it would do. This gives you the chance to copy & paste them to a file and
execute them one by one if you prefer that.


## Execute the Commands

Start the script without the `-n` (dry run) command line option, but with root
privileges (`sudo` or `su`, whichever you prefer):

```console
tux@linux:~/tmp> ./unshadow-mount-points
unshadow-mount-points: FATAL: This needs root privileges.
```

_Nope, this doesn't work. Use `sudo` or `su`._


```console
tux@linux:~/tmp> sudo ./unshadow-mount-points
mkdir -p /mnt/root
mount -o bind / /mnt/root
mkdir -p /mnt/shadowed
mkdir -p /mnt/shadowed/tmp
mount -o bind /mnt/root/tmp /mnt/shadowed/tmp
mkdir -p /mnt/shadowed/work_tmp
mount -o bind /mnt/root/work/tmp /mnt/shadowed/work_tmp
mkdir -p /mnt/shadowed/work_src
mount -o bind /mnt/root/work/src /mnt/shadowed/work_src
mkdir -p /mnt/shadowed/home
mount -o bind /mnt/root/home /mnt/shadowed/home

=== Found 447 shadowed files.===

=== Disk space in shadowed directories:

0	/mnt/shadowed/home
0	/mnt/shadowed/tmp
0	/mnt/shadowed/work_src
49M	/mnt/shadowed/work_tmp

Now run    qdirstat /mnt/shadowed.


Remember to later clean everything up with

    unshadow-mount-points -c

```

## Analyze the Shadowed Files

If there were any shadowed files, use the tools of your choice to find out what
they are.

```console
tux@linux:~/tmp> qdirstat /mnt/shadowed
```

```console
tux@linux:~/tmp> tree -d /mnt/shadowed/
/mnt/shadowed/
├── home
├── tmp
├── work_src
└── work_tmp
    └── hidden-treasure
        └── qdirstat
            ├── debian
            │   └── source
            ├── doc
            │   └── stats
            ├── man
            ├── screenshots
            ├── scripts
            │   ├── pkg-tools
            │   └── shadowed
            ├── src
            │   └── icons
            │       ├── tree-medium
            │       └── tree-small
            └── test
                ├── data
                └── util

22 directories
```

Compared with its counterpart in the root filesystem that has another
filesystem mounted over it:

```console
tux@linux:~/tmp> tree -d /work/tmp
/work/tmp

0 directories
```

```console
tux@linux:~/tmp> du -h /mnt/shadowed/
0	/mnt/shadowed/tmp
4.0K	/mnt/shadowed/work_tmp/hidden-treasure/qdirstat/.github
4.0K	/mnt/shadowed/work_tmp/hidden-treasure/qdirstat/debian/source
24K	/mnt/shadowed/work_tmp/hidden-treasure/qdirstat/debian
100K	/mnt/shadowed/work_tmp/hidden-treasure/qdirstat/doc/stats
344K	/mnt/shadowed/work_tmp/hidden-treasure/qdirstat/doc
64K	/mnt/shadowed/work_tmp/hidden-treasure/qdirstat/man
8.0M	/mnt/shadowed/work_tmp/hidden-treasure/qdirstat/screenshots
40K	/mnt/shadowed/work_tmp/hidden-treasure/qdirstat/scripts/pkg-tools
8.0K	/mnt/shadowed/work_tmp/hidden-treasure/qdirstat/scripts/shadowed
112K	/mnt/shadowed/work_tmp/hidden-treasure/qdirstat/scripts
420K	/mnt/shadowed/work_tmp/hidden-treasure/qdirstat/src/.moc
56K	/mnt/shadowed/work_tmp/hidden-treasure/qdirstat/src/icons/tree-medium
56K	/mnt/shadowed/work_tmp/hidden-treasure/qdirstat/src/icons/tree-small
232K	/mnt/shadowed/work_tmp/hidden-treasure/qdirstat/src/icons
41M	/mnt/shadowed/work_tmp/hidden-treasure/qdirstat/src
28K	/mnt/shadowed/work_tmp/hidden-treasure/qdirstat/test/data
4.0K	/mnt/shadowed/work_tmp/hidden-treasure/qdirstat/test/util
32K	/mnt/shadowed/work_tmp/hidden-treasure/qdirstat/test
49M	/mnt/shadowed/work_tmp/hidden-treasure/qdirstat
49M	/mnt/shadowed/work_tmp/hidden-treasure
49M	/mnt/shadowed/work_tmp
0	/mnt/shadowed/work_src
0	/mnt/shadowed/home
49M	/mnt/shadowed/
```

```console
tux@linux:~/tmp> find /mnt/shadowed -type f | wc -l
447
```


## Delete or Move the Shadowed Files

You can delete or move anything **below** any of the directories in
`/mnt/shadowed` if you are absolutely sure what it is; read the
[warning above](#warning):

```console
tux@linux:~/tmp> sudo mv /mnt/shadowed/work_tmp/hidden-treasure .
```

or

```console
tux@linux:~/tmp> sudo rm -rf /mnt/shadowed/work_tmp/hidden-treasure
```

But you can't delete or move any of the bind mount points themselves (because
they are active mount points):

```console
tux@linux:~/tmp> sudo rm -rf /mnt/shadowed/work_tmp
rm: cannot remove '/mnt/shadowed/work_tmp': Device or resource busy
tux@linux:~/tmp> sudo mv /mnt/shadowed/work_tmp .
mv: cannot remove '/mnt/shadowed/work_tmp': Device or resource busy
```


## Clean Up: Unmount all those Bind Mounts

The same script that sets up the bind mounts can also tear them down; use the
`-c` (clean up) option. Of course that also needs root permissions. This also
supports a dry run (`-n`) so you can see what it _would_ do.

```console
tux@linux:~/tmp> ./unshadow-mount-points -n -c
Cleaning up


*** Dry run - not executing any dangerous commands. ***

umount /mnt/shadowed/tmp
umount /mnt/shadowed/work_src
umount /mnt/shadowed/work_tmp
umount /mnt/shadowed/home
rmdir /mnt/shadowed/*
rmdir /mnt/shadowed
umount /mnt/root
rmdir /mnt/root

Mounts below /mnt:

/dev/sda2 /mnt/root btrfs
/dev/sda2 /mnt/shadowed/tmp btrfs
/dev/sda2 /mnt/root/mnt/shadowed/tmp btrfs
/dev/sda2 /mnt/shadowed/work_src btrfs
/dev/sda2 /mnt/root/mnt/shadowed/work_src btrfs
/dev/sda2 /mnt/shadowed/work_tmp btrfs
/dev/sda2 /mnt/root/mnt/shadowed/work_tmp btrfs
/dev/sda2 /mnt/shadowed/home btrfs
/dev/sda2 /mnt/root/mnt/shadowed/home btrfs
```

```console
tux@linux:~/tmp> sudo ./unshadow-mount-points -c
Cleaning up

umount /mnt/shadowed/tmp
umount /mnt/shadowed/work_src
umount /mnt/shadowed/work_tmp
umount /mnt/shadowed/home
rmdir /mnt/shadowed/*
rmdir /mnt/shadowed
umount /mnt/root
rmdir /mnt/root

Mounts below /mnt:

tux@linux:~/tmp> ls -lR /mnt
/mnt:
total 0
tux@linux:~/tmp> grep "/mnt" /proc/mounts
tux@linux:~/tmp>
```

## Clean-up Troubleshooting

If anything went wrong, it's safe to call it again; that might be useful if one
of the mounts is busy. You might have another shell in another terminal window
running that has one of the mounts as its current working directory. Close that
shell / that terminal window and just call it again:

```console
tux@linux:~/tmp> sudo ./unshadow-mount-points -c
```

If all else fails and you got tangled up in a mess of bind-mounts or if you
accidentally unmounted things like `/proc`, `/dev` or `/sys`, simply reboot.

**Caveat:** Do not use `sudo fuser -mk /dev/sda..` on that device since it's
your root filesystem: It will kill most of the processes on your machine,
including your desktop and your login session; every process that uses a
directory on the root filesystem as its current working directory and every
process that has any file on the root filesystem open.
