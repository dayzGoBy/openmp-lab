# Test log [windows]

## More info and test log [ubuntu] in Github.Actions: [![statusbadge](../../actions/workflows/buildtest.yaml/badge.svg?branch=main&event=pull_request)](../../actions/workflows/buildtest.yaml)

Build log (can be empty):
```

```

Stdout+stderr (./omp4 0 in.pgm out0.pgm):
```
OK [program completed with code 0]
    [STDERR]:  
    [STDOUT]: Time (1 thread(s)): 17.5486 ms
Time (2 thread(s)): 9.20861 ms
Time (3 thread(s)): 11.186 ms
Time (4 thread(s)): 9.16335 ms
Time (5 thread(s)): 10.0571 ms
Time (6 thread(s)): 9.29031 ms
Time (7 thread(s)): 9.38901 ms
Time (8 thread(s)): 9.05818 ms
Time (9 thread(s)): 9.3075 ms
Time (10 thread(s)): 9.21691 ms
Time (11 thread(s)): 9.19111 ms
77
130
187

```
     
Stdout+stderr (./omp4 -1 in.pgm out-1.pgm):
```
OK [program completed with code 0]
    [STDERR]:  
    [STDOUT]: Time (1 thread(s)): 17.4792 ms
Time (2 thread(s)): 9.17075 ms
Time (3 thread(s)): 11.1316 ms
Time (4 thread(s)): 9.31761 ms
Time (5 thread(s)): 9.87719 ms
Time (6 thread(s)): 9.08584 ms
Time (7 thread(s)): 9.45335 ms
Time (8 thread(s)): 9.06931 ms
Time (9 thread(s)): 9.12821 ms
Time (10 thread(s)): 9.23716 ms
Time (11 thread(s)): 9.20637 ms
77
130
187

```

Input image:

![Input image](test_data/in.png?sanitize=true&raw=true)

Output image 0:

![Output image 0](test_data/out0.pgm.png?sanitize=true&raw=true)

Output image -1:

![Output image -1](test_data/out-1.pgm.png?sanitize=true&raw=true)