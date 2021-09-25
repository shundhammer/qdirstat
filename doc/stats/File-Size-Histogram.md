# QDirStat's File Size Histogram

_This explains the histogram itself. The overflow ("cut off") area is described in a
[separate document](https://github.com/shundhammer/qdirstat/blob/master/doc/stats/Overflow-Area.md),
and the histogram options in
[yet another separate document](https://github.com/shundhammer/qdirstat/blob/master/doc/stats/Histogram-Options.md)._

![File Size Histogram Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-histogram.png)

## How to Get this Window

1. In QDirStat's tree view, select a directory, then menu _View_ ->
   _File Size Statistics_ or hit the F2 key.

2. In QDirStat's _File Type Statistics_, open a file type category, then select
   a filename suffix (e.g. `*.jpg`), then open the _File Type_ menu button and
   select _Size Statistics_ or hit the F2 key.

![File Size Histogram Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-histogram-jpg-work.png)

_Histogram showing only .jpg files in that directory_


## Histogram Panel Content

This histogram displays the distribution of file sizes in the selected
directory (see window heading). If a filename suffix ("*.jpg" or similar) is
displayed, it is restricted to files of that type.


### The Colored Heading Values

From left to right, you see

- The first quartile (Q1) of the file sizes

- The median of the file sizes

- The third quartile (Q3) of the file sizes

- The total number of files in the histogram, i.e. the sum of all bucket
  values.

If the terms _median_, _quartile_, _percentile_ don't mean anything to you, you
might want to read the
[intro document where they are explained](https://github.com/shundhammer/qdirstat/blob/master/doc/stats/Median-Percentiles.md).


### The Histogram

The horizontal (x) axis shows the file sizes, the vertical axis the bucket
values, i.e. the number of files in each bucket (in each size interval).

The vertical axis might be linear (if labeled just with **n**) or logarithmic
(if labeled with **log2(n)**) with a base 2 logarithm.

![Logarithmic scale](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-histogram-log-scale.png)

_Histogram with logarithmic scale_

The same principle still applies: The higher a bar, the more files are in a
bucket. But it's no longer a linear relationship between the bars; a bar that
is just slightly higher might have double the data points if a logarithmic
scale is used. Use tooltips to find out the true numbers.


### What is it Good For?

- You can see the vast amount of very small files on a Linux system.

- You can see the sizes of your MP3s, of your videos, of your photos and maybe
  make educated decisions about them.

- You might use that information for optimizations for very special servers.
  Back a long time ago, news servers were notorious for creating a large number
  of very small files. Some http servers might be similar. Some database
  servers might be completely different.

- Filesystem developers might find that information useful for optimizing or
  fine-tuning parameters.

- You might want to compare the data from your Windows partition to your Linux
  partition. You **will** notice differences.

- You might be just curious.

- Information is power. Use your imagination what to do with it.


### Tooltips

You can get a tooltip for each histogram item (bars or percentile marker) if
you hover the mouse over it. Sometimes you might have to click into the window
first so it gets focus.

A tooltip looks like this:

    Bucket #20:
    707 Files
    1.6 MB .. 1.7 MB

You can get the same information for all buckets at once by switching to the
_Buckets_ page in that dialog.


### The Markers

Markers for Q1, the median and Q3 are superimposed over the histogram: They are
the colored vertical lines. Their respective color is the same as in the
heading above the histogram.

At the right, there are some more grey markers; they show some percentiles at
the border of the histogram. They are useful to get an idea how much the
percentiles are spaced out in that area. Typically, the last few percentiles
are very wide, i.e. the data points in that area are very widely spaced. You
could say "the air gets thinner" the more you get away from the center part.


### Histogram Boundaries

It is very common for file sizes on a real life filesystem to be vastly
different: You might have a large number of tiny files, and you might also have
a couple of ISOs or videos of 2 GB each. Put into the same histogram, this
looks like this:

![File Size Histogram Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-histogram-P100.png)

This histogram does not really give away any useful information: You can see
that the vast majority of files is at the left side, and then there are wide
stretches of file sizes that don't appear anywhere in that directory tree
(where all the gaps are), and then there are some very large files in the 2.5
GB area. This is not very useful.

This is why QDirStat by default uses some heuristics to determine useful
boundaries for the histogram: It uses the _interquartile distance_ (Q3 - Q1)
and adds some generous distance to the left and to the right of Q1 and Q3:

    leftBoundary  = Q1 - 3 * (Q3 - Q1)
    rightBoundary = Q3 + 3 * (Q3 - Q1)

(maxed out at minValue (P0) and maxValue (P100), of course)

But in our extreme example (which is actually quite common), even if we leave
just one percentile out and display the histogram from P0 to P99, it becomes
much more useful:

![File Size Histogram Screenshot](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-histogram-P99.png)

As you can see, the right boundary is still at 10.7 MB, and the "interesting"
area between Q1 and Q3 is still pretty much crammed together at the left, but
now at least we can make some sense of it all (notice that it's using a
logarithmic vertical scale, so it's not nearly as evenly distributed as you
might think).

Percentile data for reference:

![Extreme Percentiles](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-percentiles-extreme.png)


### Why Not Use a Logarithmic Scale There, Too?

This is a tempting idea, but it would make the histogram display either
incorrect or very difficult to interpret: By definition, the area of each bar
corresponds to its numeric value (to the number of files in that bucket).

By using a logarithmic scale there, too, the width (!) of the bars would have
to get logarithmic, too: This would mean very fat bars on the left and very
thin bars on the right. If there are extreme differences like in this example,
the right bars would degenerate into thin lines, and even that would be
technically wrong because they would need to have sub-pixel widths. Remember
that we are comparing ranges of some few bytes with gigabytes; that's a factor
1024\*1024\*1024 difference. It's just not feasible to display that.

So the more pragmatic approach is to cut off at a sensible limit.


----------------------------------

## Navigation

[Next: The Overflow Area](https://github.com/shundhammer/qdirstat/blob/master/doc/stats/Overflow-Area.md)

[Up: Statistics Top Page](https://github.com/shundhammer/qdirstat/blob/master/doc/stats/Statistics.md)

[Top: QDirStat Home Page](https://github.com/shundhammer/qdirstat/blob/master/README.md)
