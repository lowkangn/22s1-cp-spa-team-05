# Invalid Sources and Queries

A list of invalid sources and their descriptions are provided here 
as the file names have been kept generic for the purpose of automation.

The detailed descriptions of invalid queries in each file are automatically 
printed out by the autotester and are also available as comments in the file.

## List of Invalid Sources
Note there are no semantic errors for SIMPLE in iteration 1

Item | Description
-|-
1 | Missing stmtList in procedure
2 | Missing var_name after `print`
3 | Missing `;` after `read x` 
4 | Additional `;` after while
5 | Missing stmtLst in while
6 | Missing cond_expr in while
7 | Invalid cond_expr in while (`x` used as cond_expr)
8 | `!` without `(` in cond_expr
9 | `&&` without `(,)` in cond_expr
10| Missing then for if
11| Missing else for if 
12| const_value on LHS of assign
13| Unmatched bracket in assign
14| Illegal NAME (starts with digit)
15| Illegal NAME (has special character)
16| Illegal INTEGER

Item | Description
-|-
1 | Invalid declarations
2 | Invalid Select clauses
3 | Invalid Modifies clauses
4 | Invalid Parent clauses
5 | Invalid Uses clauses
6 | Invalid Follows clauses
7 | Invalid Pattern assign clauses
