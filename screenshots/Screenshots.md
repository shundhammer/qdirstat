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


## File Type Statistics

![File Type Statistics Window Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-file-type-stats.png)

_Menu View -> File Type Statistics..._


## Locating Files by Type

![Locate Files Window](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-locate-file-types-window.png)

_Results after clicking on the "Locate" button in the "File Type Statistics" window._

![Locating Files](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-locating-file-types.png)

_Locating files with a specific extension. That branch is automatically opened
in the tree view in the main window, and all matching files in that directory are selected._


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
