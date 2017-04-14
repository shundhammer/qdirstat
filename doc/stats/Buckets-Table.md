# The Buckets Table

![Buckets Table](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-buckets-table.png)

This shows the _buckets_ of the histogram as a table. It is the same
information you can get when hovering over the histogram bars.

For a histogram, files are put into _buckets_, i.e. they are classified by
their size: Files of similar sizes are put into the same bucket, and the
histogram shows how many files there are in each bucket.

Columns from left to right:

- Bucket number. How many buckets there are depends on the number of data
  points, i.e. the total number of files. QDirStat uses the
  [Rice Rule](https://en.wikipedia.org/wiki/Histogram#Rice_Rule)
  to determine the number of buckets so there is a reasonable number of data
  elements in each bucket. If there are too many buckets, many of them won't
  have any data, and the others will have very similar numbers, so it would be
  hard to make any sense of the histogram.

  QDirStat uses a maximum of 100 buckets so each bucket is still wide enough to
  be well visible and clickable.

- The start value of the bucket, i.e. the minimum file size for a file to be
  put into that bucket.

- The end value of the bucket, i.e. the maximum file size for a file to be put
  into that bucket. The end value of this bucket is also the start value for
  the next one.

- The number of files in that bucket. This is what the histogram displays.


------------------------

## Navigation

[Up: Statistics Top Page](https://github.com/shundhammer/qdirstat/blob/master/doc/stats/Statistics.md)

[Top: QDirStat Home Page](https://github.com/shundhammer/qdirstat/blob/master/README.md)
