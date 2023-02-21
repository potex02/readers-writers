# readers-writers
A simple implementation of readers-writers problem in C++.\
It uses the pthread library and a file as shared resource.
## how to run
To run the program, you need a unix system with a c++ compiler installed.\
To compile th code, execute the following command:

```
g++ -o <output_file> reader_writer.cpp
```

The -o parameter is optional. If you omit it, the output file will be a.out\
To run the program, you only need to execute the output file

```
./<output_file> <num_of_writers> <num_of_readers> <num_of_executions_of_a_writer> <num_of_executions_of_a_reader>
```
