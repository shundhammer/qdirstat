# Histogram Options in QDirStat's File Size Histogram

When you click on the `Options >>` button below the histogram, some
interactive controls become visible:

![Histogram Options](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-histogram-options.png)

Here you can change the left and right boundary of the histogram.

Typically, the left boundary is P0 (percentile #0, the minimum file size), but
the right boundary is below P100 (percentile #100, the maximum file size).

As explained in the
[file size histogram article](https://github.com/shundhammer/qdirstat/blob/master/doc/stats/File-Size-Histogram.md),
QDirStat automatically determines a useful values for both boundaries.

But that's just heuristics, and the value might not always be the optimal
one. So here you have the tool to experiment with better values: Use the
sliders or the _spin boxes_ below them to change the start and end percentile.

Notice you can't cut off above Q1 or below Q3: That part of the histogram will
always remain visible.

If you set the end percentile to P100, the overflow ("Cut off") panel will
disappear. This is normal and intentional since in that situation nothing is
cut off.

The _Auto_ button resets both to the automatically calculated values:

    leftBoundary  = Q1 - 3 * (Q3 - Q1)
    rightBoundary = Q3 + 3 * (Q3 - Q1)

(maxed out at minValue (P0) and maxValue (P100), of course)


Hit `<< Options` again to close the histogram controls.



------------------------------

## Navigation

[Next: The Percentiles Table](https://github.com/shundhammer/qdirstat/blob/master/doc/stats/Percentiles-Table.md)

[Up: Statistics Top Page](https://github.com/shundhammer/qdirstat/blob/master/doc/stats/Statistics.md)

[Top: QDirStat Home Page](https://github.com/shundhammer/qdirstat/blob/master/README.md)
