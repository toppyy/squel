![Tests](https://github.com/toppyy/squel/actions/workflows/tests.yml/badge.svg)

# squel

A useless row-oriented SQL query engine written to understand something about query processing.

With it, you can:
1. Run SQL-queries on CSV-files
2. Store and query TDB-files (see [below](./README.md#memory-management--storage))

## Design

The system is implemented using the iterator / volcano processing model where each relational operator implements a *getTuple* function. Each operator in the query plan calls *getTuple* of it's children and after processing it, passes the tuple to it's parent (if any).

There are three subsystems:
- **The parser** which is responsible for parsing the raw sql-query into a parse tree describing the query
- **The planner** which is responsible for composing a query plan of the parse tree consisting of relational operators and binding identifiers to data
- **The executor** which is responsible for executing the query described by the query plan. Execution means iterating over the operators.

## SQL-support

The query engine has support for some basic SQL-features like:
- Aggregation (COUNT,AVG,SUM,MIN,MAX)
- Inner joins between two tables
- Subqueries
- Filtering (WHERE-clauses)
- EXPLAIN, CREATE TABLE and INSERT INTO statements

This is about as complicated as it can get. 


    SELECT COUNT(unemployed)
    FROM (
        SELECT lt.long_term_unemployed,lt.time,u.unemployed
        FROM
        './test/data/lt_unemployed.csv' AS lt 
        JOIN
        './test/data/unemployed.csv' AS u
            ON u.time=lt.time
    )
    WHERE long_term_unemployed > 10000 OR unemployed > 50000


See [./test/](./test/)  for further examples.


## Query planning

The query planner is relatively straight forward. It creates a query plan that comprises of the following operators (see [src/include/planner/planner.h](src/include/planner/planner.h)):

    OP_SCANTDB
    OP_SCAN
    OP_PROJECT
    OP_FILTER
    OP_JOIN
    OP_AGGREGATE
    OP_HASHJOIN

Two operators are noteworthy:
    - `OP_SCANTDB` describes `.tdb` -table, whereas `OP_SCAN` is a scan on a plaintext file.
    - `OP_HASHJOIN` describes a hash-join. Any join operation based only on a single equality comparison is turned into a hashjoin. `OP_JOIN` is a mere nested loop join.

## Custom file format `.tbd`

A table can be persisted in the custom `tdb`-file format. It consists of a header (describing the table: column types, names, etc.) and the actual data. The data is stored in a contiguous block where each record is of fixed size.

A `.tbd` file consists of a header and the actual records. The layout is described in [src/include/io/tdb.h](src/include/io/tdb.h).

During query execution, a tuple is represented in memory in the same format. A column can be extracted from the block using an offset and datatype information. Queries on plaintext files are converted to the same format (and thus are slower than queries on `.tdb`-files).

See [test/test_tdb.bats](./test/test_tdb.bats) for an example of how to create and insert into a tdb-table.

## Memory management & storage

Thanks to the iterator model, the system needs to maintain only a single tuple in memory at a time during execution. This means that querying a 1KB table consumes as much memory as querying 1GB table (assuming the same table definition). 

An exception to this is joins and other blocking operators. Currently the inner table of the join is stored in memory in it's entirety. So one must be careful when crafting the query (choose the smaller table as the inner table).


## Building

Simply run make to build under `./build`:

    make

This should able you to run the following query:

    ./build/squel "SELECT * FROM './test/data/small.csv'"

For running tests / cleaning up, you can do:

    make tests
    make clean




