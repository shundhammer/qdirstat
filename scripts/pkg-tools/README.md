# QDirStat pkg-tools Scripts

## Overview

This directory contains scripts that may be useful to find out information
about unpackaged files in your system, i.e. files that do not belong to any
software package under the control of your system's package manager.

_This is the script version. This feature is also available with full GUI
integration in the QDirStat main program at "File" -> "Show Unpackaged Files"._


### tl;dr

Make sure you have `sudo` permissions for your user account and call

```
./show-unpkg-files
```

**Be careful not to delete entire directories in that view; you only see part of
your system, not the whole thing!**


### Use Cases

- You regularly manually build software and install it with `make install`, and
  now you'd like to find those files again after the system behaves strangely.

- You constantly upgrade your system from one distro release to the
  next. Sometimes, files are left behind because of packaging errors. For
  example, you may find very old libraries in `/usr/lib` that may get in the
  way (this happened to me).

- You are a software packager, a QA engineer or a release manager working for a
  Linux distributor, and need to check if all the files belonging to a software
  package are packaged properly, our you want to find files left behind by
  package upgrades, or files that a package leaves behind because of some
  slightly broken pre- or post-uninstall script (leftover Python *.pyc files,
  anyone?).


## Caveat

The scripts in this scripts/pkg-tools directory are intentionally not being
installed. They are meant for advanced users who know what they are doing.

There are no man pages, but some have a `--help` command line option. This
document is an overview what they do and how they are meant to be used.

Some of the scripts may feel a little clunky; they are less than perfect (but
they do their job). See them more as an inspiration of what can be done with
QDirStat and its tools rather than a production-ready solution.

If you decide that you do want to use them in a production environment, then
more power to you, but please be very careful, and no complaints. ;-)



## which-pkg-manager

This checks which (if any) of the low-level package managers supported by
QDirStat is available on the system as a primary package manager:

- rpm
- dpkg
- pacman

If you use _rpm_ on top of _dpkg_ (or the other way round), this will still
only report _dpkg_.



## complete-filelist-dpkg

This creates a list of all installed packages and a complete file list of all
those installed packages on a dpkg-based system.

Both files are stored in the QDirStat tmp directory in /tmp/qdirstat-$USER/.
The script writes the number of lines and the exact path of each one to stdout:


```
[sh @ balrog] ...qdirstat/scripts/pkg-tools % complete-filelist-dpkg
2459 /tmp/qdirstat-sh/pkglist.txt
268507 /tmp/qdirstat-sh/filelist.txt
```

If the QDirStat tmp directory does not exist yet, it will be created.



## complete-filelist-rpm

Very much like complete-filelist-dpkg, this creates a complete file list of all
installed packages, but for an rpm-based system.



## complete-filelist-pacman

Very much like complete-filelist-dpkg, this creates a complete file list of all
installed packages, but for a pacman-based system.



## cache-exclude

This excludes a list of files from an (uncompressed) QDirStat cache file.

**The cache file needs to be created in long format**, i.e. with the full path
for every entry:

```
qdirstat-cache-writer -l / qdirstat-cache.gz
```

This script can be used to exclude a file list created by one of the
complete-filelist-* scripts from a QDirStat cache file, i.e. it can create a
diff between the entire system (the content of the cache file) and the files
that the package manager knows about.

The script uses stdin for the cache file and stdout for the result, and it
expects the input file to be uncompressed, and it does not compress the output.

Thus, invoke it like this:

```
zcat qdirstat-cache.gz | ./cache-exclude exclude-file | gzip >result-cache.gz
```



## cache-kill-empty-dirs

This removes empty directories from a QDirStat cache file.

The general idea is that nobody is much interested in a system skeleton with a
lot of empty directories. If a directory is completely empty after excluding
all packaged files from it (see `cache-exclude`), this gets rid of that
directory.

Mount points are kept, though.

Add this command to the command line above:

```
zcat qdirstat-cache.gz \
  | ./cache-exclude exclude-file \
  | ./cache-kill-empty-dirs \
  | gzip >result-cache.gz
```



## show-unpkg-files

This is the wrapper script for all of the above. In addition to that, it
creates a qdirstat cache file from the root directory, and it also filters out
some directories that are expected to contain unpackaged files.

Finally, it invokes `qdirstat` with the resulting cache file. This will show
you only unpackaged files. QDirStat will operate normally on that tree,
i.e. you can (if you have the necessary permissions) delete files and
directories from there.

**Be careful** when doing that; you don't see the complete thing, only the
portion of your system that does not belong to any package, so don't be tempted
to remove a directory /lib/foo/ because you just deleted the only file there;
it may very well just be the only unpackaged file, i.e. you didn't see the
others, yet they are killed just as dead when you `rm -rf` their parent
directory!

Usage:

```
./show-unpkg-files
```

Notice that QDirStat is **not** invoked with `sudo` in this script. This is
intentional.


### Using sudo

This script uses `sudo` to invoke `qdirstat-cache-writer` to make sure to get
all files in the root directory.

If you did not set up `sudo` to work with your user account, if you are
uncomfortable doing such an operation with root permissions or if you simply
don't have root permissions on your system, you can either create the cache
file manually or simply remove `sudo` from the `create_cache_file` function in
that script.

Creating a cache file manually:

```
qdirstat-cache-writer -l / /tmp/qdirstat-$USER/root.cache.gz
```

If you do this without root permissions, you will get some warnings because
some directories are accessible only with root permissions. You can safely
ignore those warnings, but of course you will not see the complete system in
the result; you may miss some unpackaged files (which may or may not be
important to you).


### Keeping Expensive Data

When called repeatedly, `show-unpkg-files` keeps the root cache file and the
file list. Use the `-f` (force) command line option to enforce recreating
them:

```
./show-unpkg-files -f
```


### Excluded Directories

- /home
- /tmp
- /root
- /var
- /usr/lib/sysimage/rpm   (the RPM database on RPM systems)

The above directories don't contain any packaged files; they are meant for user
or variable data. They are excluded because they would greatly distort the
result, dwarfing _interesting_ (i.e. unpackaged / left over) files, in
particular in the treemap.

You can modify this list in the `filter_out_pkg_files_from_cache()` function in
`show_unpkg_files`; it's just some `egrep -v` calls.


## GUI vs. Scripting

This functionality is now also available with full GUI integration and also
visualizing the packaged files in the tree. Start QDirStat and then use "File"
-> "Show Unpackaged Files".

Still, the scripts (albeit a bit clunky) are also there, they work quite well,
and they may be more suitable than the GUI version for automated tests, e.g. in
QA workflows for Linux distributions.

You can also simply comment out the final step that starts the QDirStat GUI
with the resulting cache file and analyze that file with other tools.
