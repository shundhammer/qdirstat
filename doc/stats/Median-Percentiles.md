# Median, Quartiles and Percentiles Explained

(c) 2017 Stefan Hundhammer

License: GNU Free Documentation License


## Motivation

_"Do not trust any statistics you did not fake yourself."_

(Joseph Goebbels, often wrongly attributed to Winston Churchill)

This is the instant reply some moron will ALWAYS bring up whenever anybody
begins talking about statistics, thus instantly discrediting whatever facts
were brought up.

But all that does is to make any intelligent discussion (that is, any
fact-based discussion) virtually impossible, replacing hard facts with mere
sentiments. While there might be some virtue to something meteorologists call
"perceived temperature" (because it takes the wind chill factor into account),
"perceived facts" are completely worthless. Yet they seem to rule the day in a
society where it is considered chic to state "I've always been bad at math"
(and nobody replies "you should be ashamed of yourself").

But important decisions should be based on facts, not on feelings. That's where
statistics come into play; and that's when average people feel overwhelmed
rather than informed. What is all that stuff? Everybody knows what an average
value is, but what is a median, what are percentiles? And who cares anyway?

It's actually very simple. It doesn't take a math genius to understand; every
average (here we go again!) person can do that. Just read on.


## The Farmers of Dairyville

Dairyville is a fictional village with a number of farmers; some small ones
with only very few cows, most with a pretty medium-sized number, and there is
also that big large corporation Agricorp, Inc. with a lot of cows:


     #1  Collins            1 cow
     #2  Myers              2 cows
     #3  Davis             12 cows
     #4  Thompson          12 cows
     #5  Fletcher          14 cows
     #6  Allen             15 cows
     #7  Brown             16 cows
     #8  Eliott            16 cows
     #9  Robinson          17 cows
    #10  Jones             18 cows
    #11  Simpson           38 cows
    #12  Agricorp, Inc.   400 cows

    Total                 561 cows


The average is 561 / 12 = 46.75.

But that does not describe any of the farmers well; worse, that average is a
meaningless number: It does not fit any of the normal farmers, much less the
big Agricorp, Inc. corporation.

Why this is so is obvious in this case: That big Agricorp, Inc. is greatly
distorting the result. There must be a better way to do this; one that actually
makes a meaningful statement about the typical farmer of Dairyville.

Well, there is. It is called the median.


## The Median

The median is a value determined by putting all the data in a sorted list and
then choosing the middle point. It is the point where as many data points are
below as there are above.


     #1  Collins            1 cow
     #2  Myers              2 cows
     #3  Davis             12 cows
     #4  Thompson          12 cows
     #5  Fletcher          14 cows
     #6  Allen             15 cows
    ------------------------------ Median
     #7  Brown             16 cows
     #8  Eliott            16 cows
     #9  Robinson          17 cows
    #10  Jones             18 cows
    #11  Simpson           38 cows
    #12  Agricorp, Inc.   400 cows


In our example the data are already conveniently sorted and numbered, so we
just have to pick the middle point from the 12 data points: Between #6 and #7,
i.e. between farmer Allen and farmer Brown, between 15 and 16 cows, i.e. 15.5
cows.

If you look at the data, that is a much more accurate description of the
typical Dairyville farmer.

Better yet, if you were to take Agricorp out of the calculation, you'd end up
with a very similar number: Then the median would be at #6, farmer Allen, with
15 cows. No big change (from the previous 15.5).

On the other hand, the average would change from 46.75 to 14.63 - quite a
drastic change.

If you were to disregard the very small farms #1 and #2, the median would be
16, no matter if Agricorp were still in the table or not.

The median is a very useful measure; it is very stable against weird "outliers"
(outlying data points very far from the center), unlike the average.

This is why the median is typically used for professional statistics of any
kind; never again let anyone fool you into believing that "the average income
in our country is only so high because of a few billionaires who greatly
distort the statistics". That is a flat-out lie. The pros use the median, and
the billionaires don't make a difference at all.

If you watch closely, the pros always talk about the "median household income",
never about the "average household income" because that would indeed be a
meaningless number. It's just the media who tend to misquote it and change it
from "median" to "average" because they think that makes it easier to
understand.


## Quartiles

The concept goes further. Now that we know what the median is, can we make any
more meaningful statements about the typical Dairyville farmer?

We can. Let's just cut the list in half at the median and apply the same
principle again:

Lower half:

     #1  Collins            1 cow
     #2  Myers              2 cows
     #3  Davis             12 cows
     ----------------------------- 1st Quartile
     #4  Thompson          12 cows
     #5  Fletcher          14 cows
     #6  Allen             15 cows


Upper half:

     #1  Brown             16 cows
     #2  Eliott            16 cows
     #3  Robinson          17 cows
     ----------------------------- 3rd Quartile
     #4  Jones             18 cows
     #5  Simpson           38 cows
     #6  Agricorp, Inc.   400 cows


Voila, we just cut our farmer population into quarters.

The dividing points (the medians of the lower and upper halves) are called the
"first quartile" and the "third quartile". The second quartile technically
exists, but it is much better known for its other name: The median.

In more advanced statistics, the segment between the 1st quartile (Q1) and the
3rd quartile (Q3) is considered the most important part of the data. The value
difference between them is called the "interquartile distance"; in our example,
this would be 17.5 - 12 = 7.5.

This is an important number for a lot of things, such as deciding where and how
to leave out data in certain types of graphical representations such as "box
plots": They typically cut off outlying data points beyond 1.5 * (Q3-Q1) on
either side so the graphics remain meaningful.

In our example, that would mean to cut off below Q1 - 1.5 * 7.5 = 0.75 and
above Q3 + 1.5 * 7.5 = 28.

That would mean that farmer Collins would still be in the graph, but Agricorp
and farmer Simpson would not.


## n-Quantiles, Deciles and Percentiles

In a mathematical sense, the concept is easy to generalize; it is called
"n-quantiles" where "n" is the number of segments after the division.

So, the median would be the 2-quantile, the quartiles would be 4-quantiles.

When used with n = 10, they are called "deciles"; with n = 100, they are called
"percentiles". And the percentiles are the most useful and come most natural to
us modern people who are used to think in terms of percent.

Dairyville has not enough farms to come up with a useful percentile table; with
a full one, that is.

But just think about it: The median is the middle point of the percentiles,
i.e. the 50th percentile (P50); the first quartile is the 25th percentile
(P25), the third quartile the 75th percentile (P75):


     #1  Collins            1 cow
     #2  Myers              2 cows
     #3  Davis             12 cows
    ------------------------------ P25 (Q1)
     #4  Thompson          12 cows
     #5  Fletcher          14 cows
     #6  Allen             15 cows
    ------------------------------ P50 (Median)
     #7  Brown             16 cows
     #8  Eliott            16 cows
     #9  Robinson          17 cows
    ------------------------------ P75 (Q3)
    #10  Jones             18 cows
    #11  Simpson           38 cows
    #12  Agricorp, Inc.   400 cows


Notice that strictly speaking, there is no P100, and no P0. But it is useful to
extend the concept a little further, and define P100 the maximum of the data
and P0 the minimum, so the percentiles (even if only very few of them can be
calculated with only 12 data points) can serve to completely describe the data
set:


    ------------------------------ P0 (Min)
     #1  Collins            1 cow
     #2  Myers              2 cows
     #3  Davis             12 cows
    ------------------------------ P25 (Q1)
     #4  Thompson          12 cows
     #5  Fletcher          14 cows
     #6  Allen             15 cows
    ------------------------------ P50 (Median)
     #7  Brown             16 cows
     #8  Eliott            16 cows
     #9  Robinson          17 cows
    ------------------------------ P75 (Q3)
    #10  Jones             18 cows
    #11  Simpson           38 cows
    #12  Agricorp, Inc.   400 cows
    ------------------------------ P100 (Max)


**Notice:** The percentiles/quartiles/median are the **dividing points**, not
the interval at either of their sides.


## Further Reading

Read the full description that is no doubt mathematically correct, but utterly
incomprehensible and thus useless to the normal human here:

https://en.wikipedia.org/wiki/Quantile

https://en.wikipedia.org/wiki/Median

https://en.wikipedia.org/wiki/Quartile

(This bunch of uselessness is why I wrote this document)


## Disclaimer

No animals (or farmers) were harmed in the making of this.

We'd like to thank the farmers and cows of Dairyville and the Dairyville
chamber of commerce for their kind cooperation.

;-)


## Navigation

[Next: Histograms in General](https://github.com/shundhammer/qdirstat/blob/master/doc/stats/Histograms-in-General.md)

[Up: Statistics Top Page](https://github.com/shundhammer/qdirstat/blob/master/doc/stats/Statistics.md)

[Top: QDirStat Home Page](https://github.com/shundhammer/qdirstat/blob/master/README.md)
