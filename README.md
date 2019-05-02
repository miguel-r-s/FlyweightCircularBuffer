# Flyweight Circular Buffer

The "standard" way of keeping track of the last N objects in a stream of data is by using a circular buffer. When objects are big and frequently repeated, the standard circular buffer approach wastes memory with copies of equivalent data. 

This data structure avoids the waste of memory by storing a pointer to data that was already there before whenever a repeated entry is inserted. 

## Example

This is an example where we keep track of the last 100,000 objects in a stream. These objects are 100 byte long. Each object is a slightly modified version of the previous one, so it's highly likely that we'll find many repeated objects.

A standard circular buffer would store 100,000x100 bytes = 10MB.
But if 50% of the objects can be found twice in the array, we only need to store half of these objects (and some bookkeeping data). Quick-and-dirty maths considering my implementation say this would take about 5.8MB (5MB for the data, 100,000 pointers (4 bytes each), 100,000 integers used for bookkeeping (4 bytes each)). 

## Next steps

I'm thinking of a way to minimize the bookkeeping data. I'm also taking suggestions if you want to contribute :) 

# Caveats

We still need to store a pointer and some bookkeeping data so if the size of your data is less than a few bytes, a FlyweightCircularBuffer as implemented here will end up costing more memory than a standard circular buffer approach.

# Try! 

mkdir build/ && cd build && cmake ..

