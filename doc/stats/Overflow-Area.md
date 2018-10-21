# The Overflow Area in QDirStat's File Size Histogram

Since QDirStat in most cases does not display the full data range, but rather
cuts off at a sensible point, it displays a special panel next to the histogram
when it does that as a reminder that you are not seeing the complete data set.

![File Size Histogram Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-histogram.png)

In the above case, it's pretty harmless: The overflow ("Cut off") panel shows
that a small part of the data was cut off: Percentile P98 to P100.

Remember that each percentile contains by definition 1/100 of all data points
(in this case: of all files), so in this case it's just 1% of all data points
that were cut off; in this example, 647 files. But the histogram still shows
the remaining 97%, 31732 files total (this is the rightmost / black value above
the histogram).

The upper pie chart shows that information graphically. It's no big deal,
really.

But P98..P100 in this case means cutting off from 5.3 MB to 31.1 MB, so it's a
really wide part of the x axis that gets cut off; to show that all, it would
need to be more than 6 times as wide. If scaled to the same window width, that
would mean that the meaningful part of the histogram would only be 1/6 of the
total width with vast areas of emptiness to its right. That's _why_ that part
was cut off.

But even though it's only 2% of all files, those files together account for
14.3% of the disk space in that directory tree: 7.4 GB total. This is what the
lower pie chart shows. Again, this is not really significant, much less for the
purpose of file size distribution which this window is all about. But it's a
reminder not to just disregard those few files - because they are **big**.


## A More Extreme Example

_Oh PacMan, where art thou?_

_...and why art thou red?_

![Red PacMan](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-histogram-log-scale.png)

Going one directory level up, we get this histogram, and this is an entirely
different matter:

The heuristics for determining the histogram boundaries decided on displaying
P0 to P86. This does make sense in order to have the most important part of the
distribution between Q1 (P25) and Q3 (P75) in view.

But on the other hand, it cuts off 14% of all files (14 percentiles), the range
between 1.6 MB and 2.5 GB. That's three magnitudes (1024) of difference in file
size. The red slice of the upper pie chart is considerably bigger than in the
previous example.

For the accumulated size of those files that were cut off, it means that they
now contribute to 92.0% of the total disk space in that directory tree (the red
part of the pie charts is always the part that is cut off), by far dwarfing the
accumulated sizes of those files that are displayed in the histogram.

Again, this may or may not bother you; it depends on what information exactly
you are looking for in the histogram. But this is where you might want to
decide to show just a little more of the data range by opening the histogram
options and moving the sliders.

Moving the _End Percentile_ slider to P99 in this case gets you this:

![Histogram with P99](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-histogram-P99.png)

OK, that did put things a bit more into proportion - at the price that the most
important part of the data, Q1 .. Q3, has now become a lot less visible: It now
has 1/4 of its previous display size (since the right boundary is now at 10.7 MB
from the previous 1.6 MB).

Still, the remaining 1% of the files that were cut off contribute to 64.7% of
the total disk space: The red slice is still the dominant one, albeit no longer
quite as much as before. _PacMan opened its mouth for us._ ;-)

But going full hog and moving the _End Percentile_ slider all the way to P100
is not helpful, either:

![Histogram with P100](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-histogram-P100.png)

Duh. There is no longer an overflow panel since now nothing is cut off, but the
histogram is pretty worthless: We can now see that there are some files in the
2.5 GB range, but the core of the histogram (Q1..Q3) has now degenerated to
some 5 pixels wide with colored lines. All we can see in this histogram is that
there is a lot of little stuff on the left, then it peters out for a wide
stretch, and there is another peak on the right. Notice, though, that due to
the logarithmic vertical scale that peak is not nearly as high as it appears:
It's just 18 files (!) in comparison to bucket #1 (the peak at the very left)
with 98206 files.




-----------------------------

## Navigation

[Next: Histogram Options](https://github.com/shundhammer/qdirstat/blob/master/doc/stats/Histogram-Options.md)

[Up: Statistics Top Page](https://github.com/shundhammer/qdirstat/blob/master/doc/stats/Statistics.md)

[Top: QDirStat Home Page](https://github.com/shundhammer/qdirstat/blob/master/README.md)
