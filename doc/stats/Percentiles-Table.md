# The Percentiles Table


![Percentiles Table](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-percentiles-table.png)


This table shows the percentiles of the file size statistics.

If you don't know what percentiles are, please read the
[percentiles documentation](https://github.com/shundhammer/qdirstat/blob/master/doc/stats/Median-Percentiles.md)
first.


Columns from left to right:

- Percentile number (P0 .. P100).

- Percentile value. This is the file size where that percentile is. Remember
  that percentiles are the dividing point, not an interval. Percentile P10 is
  the point where 10% of all data points are below, 90% are above.

- The name of the percentile if there is any special name:

  | Percentile | Name             |
  |------------|------------------|
  | P0         | Min              |
  | P25        | 1. Quartile (Q1) |
  | P50        | Median           |
  | P75        | 3. Quartile (Q3) |
  | P100       | Max              |


- The accumulated sum of all data points (of all files) between the
  previous percentile and this one.


If you wonder how many data points (files) fall into any interval between the
previous percentile and this one: That's always 1/100 of the total number of
data points (files). This is why this is not listed in the table; that would be
the same number for each percentile.

This table shows either all or only the interesting percentiles. Use the combo
box at the top right to switch between those two views.

Interesting percentiles in the context of this table are those with a special
name (see above), those with a number that can be divided (without remainder)
by 5, and a few around the extremes (min and max).


![Full Percentiles Table](https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-percentiles-table-full.png)

_Full percentiles table_


----------------------------

## Navigation

[Next: The Buckets Table](https://github.com/shundhammer/qdirstat/blob/master/doc/stats/Buckets-Table.md)

[Up: Statistics Top Page](https://github.com/shundhammer/qdirstat/blob/master/doc/stats/Statistics.md)

[Top: QDirStat Home Page](https://github.com/shundhammer/qdirstat/blob/master/README.md)
