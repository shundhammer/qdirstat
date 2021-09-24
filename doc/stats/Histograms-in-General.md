# Histograms in General

(c) 2017 Stefan Hundhammer

License: GNU Free Documentation License


## Quick Overview

A histogram is a graphical representation of data: The data are grouped into
_buckets_ (or _bins_) of intervals (usually of equal width), and bars are drawn
to show how many data points fall into each bucket. The higher the bar, the
more data are in a bucket.

From the resulting graphics you can easily see if data points are concentrated
in any area, if there are any peaks, and how data are distributed.



## Detailed Explanation

### Number of Buckets

First you need to determine the number of buckets to use.

This is not anything as easy as it may sound: Too many buckets, and you will
end up with many buckets without any data, leaving gaps in the graphics; and at
the same time the buckets that do have data will have very few of them which
makes any comparison between them difficult.

If you have too few buckets, it is very likely that many data will be
concentrated in very few of them, which leaves you with the bars for those few
buckets being huge in comparison with the others. Again, it makes analysis of
the resulting histogram very difficult.

There are a number of formulas to calculate a reasonable number of
buckets. QDirStat uses the
[Rice Rule](https://en.wikipedia.org/wiki/Histogram#Rice_Rule).
It uses a maximum of 100 buckets so the resulting histogram bars don't
degenerate to very thin lines, but remain well visible and also clickable.


### Buckets vs. Percentiles

All buckets have the same width, while percentiles typically have very
different widths. Each percentile interval (the interval between a percentile
and the previous one) always has the same number of data points (by definition,
each of them has 1/100 of the data points).


### Histogram Bars

Each histogram bar corresponds to one bucket. In normal histograms, the height
of each bar shows the number of data points in the bucket.

_There are also histograms where the buckets have different widths; in that
case, it is not the height, but the area of the histogram bar that corresponds
to the number of data points. But that is hard to interpret, so that kind of
histogram shall be disregarded here._

Notice that unlike with bar graphs, there are no gaps between histogram bars:
This is because of the way the buckets are defined. The end point of one bucket
is also the start point of the next bucket; this is how each possible data
point can be unambiguously put into one bucket.

If you see any gaps in a histogram, you are seeing an empty bucket (or possibly
multiple empty buckets).


### Method

Scenario:

We measured 1000 data points. The minimum measured value is 100, the maximum
200.

According to the Rice Rule, we need `2 * 1000^(1/3) = 20` buckets.

The range we need to display is `max - min = 200 - 100 = 100`.

The bucket width is `range / bucketCount = 100 / 20 = 5`.

So we get those buckets:

| Bucket # | Start | End |
| -------: | ----: | --: |
|        1 |   100 | 105 |
|        2 |   105 | 110 |
|        3 |   110 | 115 |
|        4 |   115 | 120 |
|       .. |   ... | ... |
|       .. |   ... | ... |
|       18 |   185 | 190 |
|       19 |   190 | 195 |
|       20 |   195 | 200 |

How to handle the boundaries for its bucket is open to definition. A
practicable definition would be to check for `start <= x < end`.

Then we go through all data and determine the bucket where each data point
belongs. For that bucket, we increment its counter by one.


| Data Value | Bucket # |
| ---------: | -------: |
|        118 |        4 |
|        187 |       18 |
|        101 |        1 |
|        119 |        4 |
|        ... |      ... |
|        ... |      ... |

Notice that the data don't need to be sorted for that.

Finally, we determine the maximum counter of all buckets; this is the maximum
for the vertical scale. Then we can begin drawing the histogram bars.

            ##
           ###
         ######
        #########   ##
      ############ ####
    #####################
    ---------------------
    1   5    10   15   20
        Bucket No. --->


In this example, we have two clearly defined peaks: One around bucket #9, and
another one around bucket #17.

Notice how the histogram shows information even without knowing anything about
the vertical scale; just the relative sizes of the bars against each other are
enough. It does help, though, to mark the horizontal (x) axis to get any idea
where the peaks or valleys are.


## Reference

https://en.wikipedia.org/wiki/Histogram

(Much better readable than the Wikipedia article about percentiles)


----------------------------------

## Navigation

[Next: The File Size Histogram](https://github.com/shundhammer/qdirstat/blob/master/doc/stats/File-Size-Histogram.md)

[Up: Statistics Top Page](https://github.com/shundhammer/qdirstat/blob/master/doc/stats/Statistics.md)

[Top: QDirStat Home Page](https://github.com/shundhammer/qdirstat/blob/master/README.md)
