# The QDirStat Packages View

QDirStat can now visualize the file lists of installed packages:

![Packages View Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-pkg-details.png)

I.e. files are now grouped by the package they belong to, and in each subtree
only the files that belong to the package are displayed: In this example, in
`/usr/bin` only the `chromium-browser` binary is displayed, not all the other
files in `/usr/bin`. This is intentional.

Notice that you can also see the grouping in the treemap.

A click on the toplevel item in the tree or on `Pkg:/` in the URL shows the
packages summary:

![Packages Summary Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-pkg-summary.png)



## Target Platforms

This works on all Linux distributions using one of those low-level package
managers:

- Dpkg
- RPM
- PacMan

...and of course all higher-level package managers that are based on any of
them. I.e.

- Debian

- Ubuntu / Kubuntu / Xubuntu / Lubuntu

- SUSE (openSUSE Tumbleweed or Leap, SLES)

- Red Hat (Fedora, RHEL)

- Arch Linux

- Manjaro

It can even handle _rpm_ installed as a foreign package manager on a _dpkg_
based system (and the other way round); it tries the primary package manager
first, then any others that are also installed.

Please notice that _apt_, _synaptic_, _zypper_, _pkgkit_ and whatnot are all
higher level package managers that ultimately use one of the low level ones, so
even if you only use a higher level package manager, it still works without
restriction.



## Package Selection

QDirStat can fetch the information for all installed packages (which may take a
while) or only for a subset (which is much quicker, of course):

![Screenshot: Packages View limited to "emacs*"](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-pkg-emacs.png)

This example shows all installed packages whose name starts with "emacs".


### GUI

Menu _File_ -> _Show Installed Packages_ opens this package selection dialog:

!["Open Packages" Dialog Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-open-pkg-dialog.png)

Enter the search pattern in the input field. The default match mode is _Auto_
which tries to guess a reasonable mode, but you can also explicitly select one
of

- Contains
- Starts with
- Exact match
- Wildcard (very much like in the shell)
- Regular expression

_Auto_ uses heuristics to make an educated guess. The default is "starts with",
but if the search pattern contains typical regexp special characters, it
interprets the pattern as a regular expression.

If it detects just `*` (not `.*`) or `?`, it uses _wildcard_.

If the first character is `=`, it uses _exact match_.

All match modes are case insensitive.


### Command Line: Pkg URLs

You can also start QDirStat with a `pkg:` or `pkg:/` URL from the command
line. This also implicitly uses _auto_ mode, so the same heuristics apply to
interpret the URL:

```
qdirstat pkg:/chrom
```

All packages starting with "chrom" or "Chrom" or "CHROM".

_Notice that all patterns are case insensitive -- mostly because distro makers
and packagers tend to get creative when to use uppercase characters in package
names, and most users can't remember when or why._


```
qdirstat pkg:/=emacs
```

Only package "emacs", not "emacs25" or any other starting with "emacs".


```
qdirstat "pkg:/*gtk*"
```

_Notice that you have to protect the `*` wildcards from the shell by escaping
them with quotes._

All packages that have "gtk" somewhere in their name.

```
qdirstat "pkg:/.*qt[45].*"
```

All Qt4 or Qt5 packages. Notice that the patterns are case insensitive.

```
qdirstat "pkg:/(firefox|mozilla|chrome|chromium|opera)"
```

All the well-known browsers on the system. Notice that regular expressions use
a partial match, so use the `^` and `$` anchors when appropriate.


### Command Line: No initial "Choose Directory" Dialog

If you don't pass a command line argument to QDirStat, it will by default open
a directory selection dialog. This can be avoided with the `-d` or `--dont-ask`
command line option:

```
qdirstat -d
```

Of course you can always simply click that dialog away with the _Cancel_
button, but it might be useful for `.desktop` files to have that command line
parameter.


## Behind the Scenes

QDirStat uses external commands to get the information, both the list of
installed packages and the file list for each of the selected packages.

It always fetches the complete package list and then does the filtering
internally based on the package filter/URL; it does not rely on how each
external package manager interprets any patterns.

Packages that are installed in multiple versions or for multiple architectures
are treated internally as separate packages.


To get the package list, QDirStat uses any of those commands:

```
dpkg-query --show --showformat='${Package} | ${Version} | ${Architecture} | ${Status}\n'
```

```
rpm -qa --queryformat '%{name} | %{version}-%{release} | %{arch}\n'
```

```
pacman -Qn
```

Then it parses the output of those commands, removes those that don't fit the
filter criteria and iterates over the remaining ones to get the file list for
each one with any of those commands:

```
dpkg-query --listfiles pkgname
```

```
rpm -ql pkgname
```

```
pacman -Qlq pkgname
```

Then for each file list entry QDirStat does the usual `lstat()` syscall to get
up-to-date information about it from the filesystem (the file lists also don't
contain any information other than the path, not even if it's a file or a
directory, much less the size).

As you can imagine, waiting for all those external commands to return
information takes a while, in particular when all installed packages are
selected to display.

The first version of this took about 3 minutes to fetch all the information for
a quite normal Xubuntu 18.04 LTS installation -- much longer than just reading
the complete root filesystem with cold kernel buffers.

This is why now QDirStat starts a number of those external commands in
parallel; the sweet spot turned out to be 6 of those processes at the same time
(whenever one is finished, a new one is started so there are always 6 of them
running). This improved the speed on the same machine to 54 seconds.

The number of background processes can be configured in
`~/.config/QDirStat/QDirStat.conf`:

```
[Pkg]
MaxParallelProcesses=6
```

For `dpkg` and `rpm` it now uses a single command that fetches the complete
file list, i.e. all file lists for all installed packages at once. This reduced
the time on the same machine to 38.5 seconds.

However, this is a tradeoff since fetching that complete file list and parsing
it takes some time, so this is useful only above a certain number of packages
(about 200). This limit can be configured in

`~/.config/QDirStat/QDirStat.conf`:

```
[Pkg]
MinCachePkgListSize=200
```

Below that, it uses multiple parallel single calls to get individual file
lists.

Together with more performance tuning it's now down to 6.5 seconds.


| sec   |  Version   | Description                                                         |
|------:|------------|---------------------------------------------------------------------|
| 180.0 | ce3e793298 | First pkg view; sequential separate `dpkg -L` calls                 |
|  53.4 | 68038f0525 | Separate `dpkg -L` calls in multiple background processes           |
|  38.5 | ce54879a48 | Single `dpkg -S "*"` call to get all file lists for all pkg at once |
|  37.7 | 45b0a7a941 | Use cache for `lstat()` syscalls                                    |
|  24.6 | c50e9a7686 | Use slower update timer for tree view while reading                 |
|   6.5 | a1043a20fb | Keep tree collapsed during reading                                  |

See also [GitHub Issue #101](https://github.com/shundhammer/qdirstat/issues/101).


## Limitations

### Cleanups

Some of the cleanup actions are still useful in the packages view, in
particular "Open Filemanager Here" or "Open Terminal Here".

With everything else, be really, really careful!

Since all the files in that display are by definition system files, think twice
before deleting any one of them. If in doubt, just don't do it.

Be even more careful when dealing with directories: Remember that you only see
that part of the directory that belongs to that package. There might be many
more files in such a directory; for most directories, this is almost
guaranteed.

If you identify a disk space hog with the packages view, it is strongly advised
that you use your package manager to try to get rid of it, i.e. uninstall that
package -- and even then only if you know really well what you are doing.


### Dependencies

QDirStat only displays the raw information in the packages view: Just the disk
usage of each package.

It does not and very likely will not display any dependencies of a package,
much less indirect dependencies. This is the package manager's job.

If you identified a package that uses a lot of disk space, you will very likely
find that some other package requires it, and yet another package requires that
one etc. until you get to the level where you can even recognize a package that
you actively use.

Big fat `libfoo-locale` might be required by `libfoo` which might be required
by `foomatic` which might be required by `bar-desktop-base` which may be
required by not only your favourite desktop environment, but also by your
favourite media player application `boomplayer`.

As a matter of fact, that is more the norm than the exception. Distro makers
usually don't include packages just for fun, but because they are needed or at
least useful in the context of your system.


### What...if Scenarios

It would have been very nice to to include a feature like "what would happen if
I tried to remove that package?", showing the user what other packages would
also need to be removed.

Most higher-level package managers such as `zypper` or `apt` have a _dry run_
feature that could be used for this: Try a dry run of removing the `libfoo`
package, and it will show you all the direct and indirect dependencies, so you
can make an informed decision if you really can (and want to) live without all
those packages.

But those commands tend to require root permissions, even for the _dry run_
mode, and a desktop program like QDirStat asking for your _sudo_ password to
get those root permissions might be scary (and for good reasons) to many users.


## Support for More Package Managers

It's not that hard to add more package managers to the existing set, but there
must be reasonable support for that by people who actively use that platform
and who have the know-how to identify and fix problems.

Check out the existing ones to get an idea; it's little more than copy & paste
programming. Finding the right external programs and the right parameters is
the major challenge; then there might be some minor programming work for
parsing the output.

https://github.com/shundhammer/qdirstat/blob/master/src/RpmPkgManager.h
https://github.com/shundhammer/qdirstat/blob/master/src/RpmPkgManager.cpp

https://github.com/shundhammer/qdirstat/blob/master/src/DpkgPkgManager.h
https://github.com/shundhammer/qdirstat/blob/master/src/DpkgPkgManager.cpp

https://github.com/shundhammer/qdirstat/blob/master/src/PacManPkgManager.h
https://github.com/shundhammer/qdirstat/blob/master/src/PacManPkgManager.cpp
