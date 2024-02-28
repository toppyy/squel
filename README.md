# squel (sic)

A useless SQL query engine written for practice.

## Design

The system is implemented using the iterator / volcano processing model where each relational operator implements a *getTuple* function. Each operator in the query plan calls *getTuple* of it's children and after processing it passes the tuple to it's parent (if any).


## Features


### SQL-features

The query engine has support for some basic SQL-features like:
- Aggregation (COUNT,AVG,SUM)
- Joins (only inner join between two tables)
- Subqueries
- Filtering (WHERE-clauses)

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

### Storage

Besides querying CSV-files, one can persist data in `.tdb` -format. The data is stored in a custom binary format along with header information on the structure of the stored table.

See [test/test_tdb.bats](./test/test_tdb.bats) for an example of how to and [src/include/io/tdb.h](src/include/io/tdb.h) for a brief description of the file layout.

## Building

If building for the first time, start with:

    make dirs

After which simply:

    make

This should able you to run the following query:

    ./build/squel "SELECT * FROM './test/data/small.csv'"

For running tests / cleaning up, you can do:

    make tests
    make clean




