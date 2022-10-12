#List of Invalid Source test cases

The test cases have been given numbered names rather than descriptive names for ease of automation. Hence, a description of the test cases is provided below. Note that all test cases use the same dummy queries as QPS should not be answering any queries.

Name            | Description
----------------|------------
invalidSource1  |  procedure calling non-existent procedure
invalidSource2  |  two procedures with the same name
invalidSource3  |  procedure calling itself
invalidSource4  |  cyclic call in program
