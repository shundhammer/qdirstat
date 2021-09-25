# The QDirStat Unpackaged Files View

QDirStat can now visualize the files in system directories that are not
packaged, i.e. that are not part of any file list of any installed software
package.

This reads the complete file lists first (i.e. all file lists from all
installed packages), then reads the requested directory as usual and puts the
files that are packaged in a special branch `<Ignored>` in the tree view.

Those files are _not_ displayed in the treemap, i.e. the treemap now only
contains unpackaged files.


## What is this Good For?

- Recovering a wrecked system after botched upgrades

- Recovering from too much `sudo make install`

- Fixing packaging problems, e.g. find leftover shared libs that tend to get in
  the way

- QA for distro release managers, QA engineers, PMs

- Satisfying user curiosity

- _Because we can!_  ;-)


## Starting the Unpackaged Files View

Menu "File" -> "Show Unpackaged Files" opens this dialog:

!["Show Unpackaged Files" Dialog Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-show-unpkg-dialog.png)

Select the starting directory from the combo box or start typing (it has
directory name completion) to enter the path of an existing directory.

A number of directories are excluded because they are expected to contain a lot
of unpackaged files. You can add more directories here (or remove ones from the
default list).

_`/usr/lib/sysimage/rpm` is the RPM database; you can safely leave that
directory there even on non-RPM systems because it doesn't exist on such
systems._

"Ignore Patterns" are wildcard patterns that are ignored in addition to the
packaged files. `*.pyc` (byte-compiled Python files) is there because Python
packages tend to leave behind a zillion unpackaged files of that kind that
would otherwise very much be in the way when exploring the result.

You can add more wildcard patterns here. If a pattern does not contain a slash,
only the filename is used for matching. If it does contain a slash, the
complete path is used.

Notice that a wildcard matches any levels of directories, so you could for
example use `*/__pycache__/*` to ignore all files in all Python cache
directories everywhere.

All changes in this dialog are saved for the next use, including starting that
view from the command line (e.g. `qdirstat unpkg:/usr/share`).


## Analyzing the Result

![Unpackaged Files Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-unpkg-usr-share-qt5.png)

_Notice that the treemap now only contains unpackaged files. Each of the
colored rectangles corresponds to one unpackaged file._

This screenshot shows some files that obviously belong to Qt 5 (judging from
the `/usr/share/qt5` path) on openSUSE Tumbleweed, but that are not in the file
list of any package. Is that a bug or not? It is still possible that some
post-uninstall script removes them, but it is also possible that they will be
left behind when the packages are uninstalled.

Notice the greyed-out ignored directories which only contain files that are
properly packaged. Those `<Ignored>` branches are there to give you some
perspective what else is in each directory; otherwise users might be tempted to
believe that the unpackaged files are all that there is, and maybe remove that
entire directory and thus wreck the system.


![Unpackaged Files Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-unpkg-boot-grub2.png)

This screenshot shows a custom Grub2 font that was manually created to have a
larger, much better readable boot font on a high-resolution monitor. That one
file clearly stands out from all the packaged files there.

`/boot/grub2` and `/etc` are some of those directories that contain a wild
mixture of packaged and unpackaged files; some are generated, some are just
backups of the previous version, some were created as templates for users to
edit.


### Ignored Files

In this view, all files that are in the file list of any installed package are
ignored. As a result, there are many directories that don't contain any
unpackaged file; that should actually be the norm.

But since the purpose of this view is to visualize unpackaged files,
directories that don't contain any packaged files (including completely empty
directories) are also moved to the highest possible `<Ignored>` branch so they
get out of the way.


## Use Cases

### Accumulated Cruft after many Package Updates

Package maintainers are also just humans, and humans make mistakes. Every once
in a while a file that belongs to a package is not listed or listed the wrong
way.

If you have been using a Linux distribution for a long time, you will get many
package updates; using a rolling release like openSUSE Tumbleweed will get you
even more.

After a while, those little packaging bugs may accumulate, and your system may
behave strangely. Some things may stop working or give you very weird error
messages.

Before you nuke your installation completely and reinstall, consider having a
look with this unpackaged files view. You may discover shared libs or symlinks
left behind in crucial system directories like `/usr/lib` or `/usr/lib64`. As a
general rule of thumb, there should be no unpackaged files in those
directories. If you find one, ask in a user forum what they are and why they
might plausibly not be part of a software package.

Or, if you feel brave, move them away (don't delete them right away!) to a
subdirectory, rebuild the linker cache (!) with `ldconfig` and reboot. Make
sure you have some live media (a bootable USB stick or a CD/DVD with a recent
version of the distro you are running) so you can do disaster recovery if
something goes terribly wrong.

**If you don't know what an unpackaged shared lib is that you found, google it
first!**

You may find that it is part of some piece of software that you installed
without using your package manager (some hardware support software comes as a
ZIP file or as a tarball). It might just be part of the crucial driver for that
fancy wireless keyboard or mouse that you use, and using your system without
that hardware may be no fun.


### Recovering from too much make install

This is very similar to the accumulated cruft after many package updates
problem, but it is typically your own fault: When you build software yourself,
it is much easier to install it directly to the system rather than building a
.deb or .rpm package:

```
make && sudo make install
```

If you don't set the install prefix to `/usr/local`, you will end up with
additional or modified files in system directories like `/usr/bin` or
`/usr/lib`. Of course, the package manager doesn't know anything about those
files, so it cannot handle them.

So if you just added a newer version of an important shared lib to your system,
that version may have priority over the system's own version of that lib.

QDirStat's unpackaged files view may help you to identify such files.

Of course it's much better not to get into that kind of trouble in the first
place and install your own custom-built software to a dedicated place like
`/usr/local`, but many users just don't think about that.


### A Useful Tool for Package Maintainers

Maintainers of Linux software packages can use this view to analyze their
packaging work:

- Does the file list contain everything? The packaging tools should complain
  if you forget to list anything, but they may also not be perfect.

- Does a package upgrade / uninstall behave properly? Are all files that the
  old version installed or created during its life time properly removed? Or is
  anything left behind? Do the pre-install / post-install / pre-uninstall /
  post-uninstall scripts do the right thing?


### A Useful Tool for Distro QA Engineers and Release Managers

Building and maintaining a Linux distribution is a complex task. There are many
packages involved; a typical Linux installation today has some 2500-4500
packages installed.

Maintaining a software package involves getting the recent sources from
upstream, getting them to build, patching them to integrate well into the
distribution, setting up package dependencies, maintaining the file list and
often enough also writing scripts: pre-install, post-install, pre-uninstall,
post-uninstall.

Upgrading from the old distro version to the new one comes with its own
challenges, in particular if upgrading not only from the last version is
required, but also two or three versions before that.

There is a lot of potential for bugs to sneak in. Multiply that with the number
of packages, and you end up with insane numbers of pitfalls.

So distro QA engineers test at least the most common cases: Upgrade from
release X-1 to X with the standard package set, with the minimal package set,
with the maximum package set; upgrade from release X-2 to X, from X-3 to X.

All those cases should work well with no major problems. Finding leftover files
may contribute to find problems quicker.


### Be Curious! Stay Curious!

What are all those files on my Linux system? Where does all that stuff belong
to?

Remember that QDirStat will show you what package a file belongs to (if any)
when you click on it; the details panel on the right shows the package name.

The package view will show you the complete contents (at least the part that
was installed to your system) in the package view (Menu "File" -> "Show
Installed Packages").

And this view solves the inverse problem: It shows you what files do _not_
belong to any package. Of course that makes most sense for system directories.

If there are unpackaged files in system directories, that may be a bug, or it
may be normal. But it is clearly a starting point for further investigation and
for asking experts.

Linux (and BSD and other Unix-like systems) was meant to be open and easy to
explore. So, become involved. Become knowledgeable what all that stuff on your
system is. You may be able to contribute to finding problems; you don't need to
be a developer for that. Just exploring things and asking the right questions
may help the community as a whole to identify problems.



## Target Platforms

This works on all Linux distributions using one of those low-level package
managers:

- Dpkg
- RPM
- ~~PacMan~~

...and of course all higher-level package managers that are based on any of
them. I.e.

- Debian

- Ubuntu / Kubuntu / Xubuntu / Lubuntu

- SUSE (openSUSE Tumbleweed or Leap, SLES)

- Red Hat (Fedora, RHEL)

- ~~Arch Linux~~

- ~~Manjaro~~

Please notice that _apt_, _synaptic_, _zypper_, _pkgkit_ and whatnot are all
higher level package managers that ultimately use one of the low level ones, so
even if you only use a higher level package manager, it still works without
restriction.

_Sorry, no support for PacMan as of now: There does not appear to be a single
command to generate the complete file list for PacMan. Advanced users can try to use the [script-based version](../scripts/pkg-tools/README.md)._

_If anybody knows a PacMan command to do this, please let me know!_


## Command Line: Unpkg URLs

As an alternative to using the menu, you can also start QDirStat from the
command line with an _unpkg_ URL:

```
qdirstat unpkg:/usr/share
```

This starts the unpackaged files view with `/usr/share` as the starting
directory. It will use the same exclude directories and ignore patterns as the
last time you used that view from the menu.

```
qdirstat unpkg:/
```

This starts the unpackaged files view with the root directory as the starting
directory.


```
qdirstat -d
```

This starts QDirStat without asking for a directory to scan, so you can go
straight to the menu.
