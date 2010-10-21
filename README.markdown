*Speedyarray* is a very fast stack (the data structure) written in C++. It's optimized for the creation of lots of small stacks.

In my tests it's about 4x faster than std::stack for small stacks, if caching is used. YMMV.