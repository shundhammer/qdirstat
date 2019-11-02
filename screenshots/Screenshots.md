# QDirStat Screenshots

Qt-based directory statistics: KDirStat without any KDE -- from the original KDirStat author.

(c) 2015-2019 Stefan Hundhammer <Stefan.Hundhammer@gmx.de>

Target Platforms: Linux, BSD, Unix-like systems

License: GPL V2


## QDirStat Main Window

![Main Window Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-main-win.png)


### Different Main Window Layouts

![Layout 1 (short)](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-details-file-L1.png)
_Layout 1 (short): Only the bare minimum of tree columns plus the details panel for the selected item._

![Layout 2 (classic)](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-details-file-L2.png)
_Layout 2 (classic): The classic QDirStat tree columns plus the details panel for the selected item._

![Layout 3 (full)](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-details-file-L3.png)
_Layout 3 (full): All tree columns including file owner, group and permissions in both "rwxrwxrwx" and octal,
but no details panel to make room for all the columns._


### Details Panel

![Details Panel for a File](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-details-file-L2.png)

_File selected. Notice the "Package" field: For system files, QDirStat queries
the package manager (dpkg / rpm / pacman) which package the file belongs to._

![Details Panel for a Directory](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-details-dir.png)

_Directory selected_

![Details Panel for a "Files" Pseudo Directory](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-details-dot-entry.png)

_"Files" pseudo directory selected_

![Details Panel for Multiple Selected Items](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-details-multi-sel.png)

_Multi-selection_



## Output During Cleanup Actions

![Cleanup Action Output Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-cleanup-output.png)

_Screenshot of output during cleanup actions. Of course this window is purely optional._


## Error Reporting

![Reporting Directory Read Errors Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-err-dirs-light.png)

_Reporting errors during directory reading. Typically this is because of
missing permissions, so it may or may not be important to the user. Those
errors are reported in small panels at the bottom of the directory tree
widget. The user can leave them open or close them._

![Details about Directory Read Errors](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-unreadable-dirs-window.png)

_Clicking on the "Details..." link opens a separate window that lists all the
directories that could not be read. This window is non-modal, i.e. the user can
interact with the main window while it is open. A click on any directory in
that list locates that directory in the main window, i.e. opens all parent
branches and scrolls to ensure that directory is visible._


## File Type Statistics

![File Type Statistics Window Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-file-type-stats.png)

_Menu View -> File Type Statistics..._


## Locating Files by Type

![Locate Files Window](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-locate-file-types-window.png)

_Results after clicking on the "Locate" button in the "File Type Statistics" window._

![Locating Files](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-locating-file-types.png)

_Locating files with a specific extension. That branch is automatically opened
in the tree view in the main window, and all matching files in that directory are selected._


## Packages View

![Packages View Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-pkg-details.png)

_Packages view: All packages of a Xubuntu 18.04 LTS installation. Notice that
each directory contains only the files that belong to that package: /usr/bin
only contains the chromium-browser binary in this view._


![Packages Summary Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-pkg-summary.png)

_Packages Summary_


![Packages View Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-pkg-emacs.png)

_Packages view limited to package names starting with "emacs"._


!["Open Packages" Dialog Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-open-pkg-dialog.png)

_Dialog for selecting which packages to display. Use menu "File" -> "Show
Installed Packages"._

_To avoid the initial "Choose Directory" dialog, start QDirStat with the "-d"
or "--dont-ask" command line parameter (or simply click "Cancel" in the "Choose
Directory" dialog)._


## Unpackaged Files View

![Unpackaged Files Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-unpkg-usr-share-qt5.png)

_Unpackaged files view: Some files that obviously belong to Qt 5 on openSUSE
Tumbleweed, but that are not in the file list of any package. It is still
possible that some post-uninstall script removes them, but it is also possible
that they will be left behind when the packages are uninstalled._

_Notice the greyed-out ignored directories which only contain files that are
properly packaged._


![Unpackaged Files Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-unpkg-boot-grub2.png)

_Unpackaged files view: I created my own font for Grub2 which clearly stands out as an unpackaged file._


!["Show Unpackaged Files" Dialog Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-show-unpkg-dialog.png)

_Dialog to start the "unpackaged files" view (Menu "File" -> "Show Unpackaged
Files"). Some directories are excluded by default because they are expected to
contain a lot of unpackaged files. You can add more directories or remove
predefined ones._

_Since Python tends to leave behind so many unpackaged files, all `*.pyc` files
are ignored by default. You can add more wildcard patterns to ignore or remove
the predefined one._



## Configuration


![Cleanup Action Configuration Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-config-cleanups.png)

_Screenshot of cleanup actions configuration._

![MIME Categories Configuration Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-config-mime.png)

_Screenshot of MIME category configuration where you can set the treemap colors
for different file types (MIME types), complete with a real treemap widget as preview._

![Exclude Rules Configuration Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-config-exclude.png)

_Screenshot of the exclude rules configuration where you can define rules which
directories to exclude when reading directory trees._

![General Options Configuration Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-config-general.png)

_Screenshot of the general (miscellaneous) configuration options._

------------------

![Tree Column Configuration Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-column-config.png)

_Context menu of the tree header where you can configure the columns._


-----------------

## File Size Statistics


![File Size Histogram Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-histogram.png)

_File size histogram for a directory_

![Histogram with only JPGs](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-histogram-jpg-work.png)

_File size histogram for all files of type .jpg (start from the File Type Statistics window)_

![Histogram Options](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-histogram-options.png)

_Histogram settings can be tweaked_

![Logarithmic Scale](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-histogram-log-scale.png)

_Logarithmic scale for the vertical axis if needed_


![Histogram with P100](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-histogram-P100.png)

_Degenerated histogram if the last percentiles are too far spread out_

![Histogram with P99](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-histogram-P99.png)

_Just one percentile less makes all the difference: Scaled down from P100 to P99_



![Percentiles Table](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-percentiles-table.png)

_Numeric percentiles table_


![Full Percentiles Table](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-percentiles-table-full.png)

_Full percentiles table_

![Extreme Percentiles](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-percentiles-extreme.png)

_Notice the leap from P99 to P100_



![Buckets Table](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-buckets-table.png)

_Buckets table (information also available by tooltips in the histogram)_


![File Size Help](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-stats-help.png)

_Dedicated help start page for file size statistics_


--------------------

## Navigation

[Top: QDirStat home page](https://github.com/shundhammer/qdirstat)

[Statistics help page](https://github.com/shundhammer/qdirstat/blob/master/doc/stats/Statistics.md)
