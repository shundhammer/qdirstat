#
# spec file for package qdirstat
#
# Copyright (c) 2026 SUSE LLC
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via https://bugs.opensuse.org/
#


Name:           qdirstat
Version:        2.0
Release:        0
Summary:        Directory Statistics
License:        GPL-2.0-only
Url:            https://github.com/shundhammer/qdirstat
Source:         %{name}-%{version}.tar.zst

BuildRequires:  qt6-core-devel
BuildRequires:  qt6-gui-devel
BuildRequires:  qt6-widgets-devel
BuildRequires:  qt6-qt5compat-devel
BuildRequires:  qt6-base-common-devel
BuildRequires:  zlib-devel
Recommends:	xdg-utils

%description

QDirStat is a graphical application to show where your disk space has
gone and to help you to clean it up.

It shows the total size of directories and of their files both in a
traditional tree view and in a colored [treemap
graphics](doc/Treemap.md) where a large file is shown as a large
rectangle, and small files are shown as small rectangles. Click on it,
and you will see where in the tree the file is, and you can instantly
move it to the trash if you like. The color corresponds to the file
type: Images, videos or whatever.

This is a Qt-only port of the old Qt3/KDE3-based KDirStat, now based
on the latest Qt 5. It does not need any KDE libs or
infrastructure. It runs on every X11-based desktop on Linux, BSD and
other Unix-like systems, and in a Docker container.

QDirStat has a number of new features compared to KDirStat.
To name a few:

- Multi-selection in both the tree and the treemap.

- Unlimited number of user-defined cleanup actions.

- Properly show errors of cleanup actions
  (and their output, if desired).

- Configurable file categories (MIME types), treemap colors,
  exclude rules, tree columns.

- Package manager support:

  - Show what software package a system file belongs to.

  - Packages view showing disk usage of installed software
    packages and their individual files.

  - Unpackaged files view showing what files in system directories
    do not belong to any installed software package.

- New views:

  - Disk usage per file type (by filename extension).

  - File size histogram view.

  - File age view.

  - Free, used and reserved disk size for each mounted filesystem
    (like df)


For more details and screenshots, see

    https://github.com/shundhammer/qdirstat

and the local documentation in

    /usr/share/doc/packages/qdirstat/README.md

%prep
%autosetup -p1

%build
%qmake6
%make_build

%install
%qmake6_install

# %suse_update_desktop_file -N "QDirStat" -G "Directory Statistics" %name Filesystem

%files
%{_bindir}/qdirstat
%{_bindir}/qdirstat-cache-writer
%{_datadir}/applications/qdirstat.desktop
%dir %{_datadir}/icons/hicolor
%dir %{_datadir}/icons/hicolor/scalable
%dir %{_datadir}/icons/hicolor/scalable/apps
%{_datadir}/icons/hicolor/scalable/apps/qdirstat.svg
%license %{_docdir}/qdirstat/LICENSE
%doc README.md doc/*.txt doc/*.md
%dir %{_docdir}/%{name}/stats
%{_docdir}/%{name}/stats/*.md
%{_mandir}/man1/qdirstat.1.gz
%{_mandir}/man1/qdirstat-cache-writer.1.gz

%changelog
