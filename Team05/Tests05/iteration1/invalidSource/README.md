#List of Invalid Source test cases

The test cases have been given numbered names rather than descriptive names for ease of automation. Hence, a description of the test cases is provided below. Note that all test cases use the same dummy queries as QPS should not be answering any queries.

Name            | Description
----------------|------------
invalidSource1  |  procedure with empty stmtLst
invalidSource2  |  print constant
invalidSource3  |  read with no semicolon
invalidSource4  |  while with semicolon after close brace
invalidSource5  |  while with empty stmtLst
invalidSource6  |  while with empty condition
invalidSource7  |  while with variable as condition
invalidSource8  |  missing brackets around while condition
invalidSource9  |  missing brackets around LHS argument of && in while condition
invalidSource10 |  if with missing "then"
invalidSource11 |  if with missing "else"
invalidSource12 |  if with empty then-stmtLst
invalidSource13 |  if with empty else-stmtLst
invalidSource14 |  if with missing else and else-stmtLst
invalidSource15 |  assign with constant on LHS
invalidSource16 |  unmatched bracket in expression
invalidSource17 |  invalid variable name (starts with digit)
invalidSource18 |  invalid variable name (has underscore)
invalidSource19 |  invalid integer
