# Heavykeeper

Finding top-k elephant flows is a critical task in network traffic measurement. As the line rate increases in today's network, designing accurate and fast algorithms for this task becomes more and more challenging. There are several well-known algorithms, including Lossy counting, Space-Saving, CSS, etc. However, the performances of all existing algorithms are poor. In this paper, we propose a novel data structure, named Heavykeeper, which achieves high precision in finding top-k elephant flows. It also works at fast and constant speed. The key idea of heavykeeper is to intelligently record the frequencies of elephant flows and omit mice flows. Experimental results show that our heavykeeper algorithm achieves almost 100% precision with a small memory size, and reduces the error by around 3 orders of magnitude on average compared to the state-of-the-art.

# About the source codes, dataset and parameters setting

The source code contains the C++ implementation of the Space-Saving, Lossycounting, CSS, heavykeeper and stream-summary (which is used in Space-Saving, Lossycounting and heavykeeper). We complete these codes on WINDOWS 10 and compile successfully using g++ 4.8.4.

The file u1 is comprised of IP packets captured from the network of our campus. And the hash functions are 64-bit Bob hash functions and 32-bit Bob hash functions, obtained from  http://burtleburtle.net/bob/hash/evahash.html.

You can also use your own dataset and other hash functions.

# How to run

Suppose you've already cloned the respository and start from the Codes directory.

You just need to compile and run main.cpp.

# Input format

You need to input two integers MEM and K, which means "memory = MEM KB" and "top-K".

# Output format

Our program will print the PRESICION, ARE and AAE of these sketches on the screen.
