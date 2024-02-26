#pragma once


/* 

    tdb is a custom file format. 

    Layout is:
        64bit int - number of records
        8bit int - number of columns
        At least one 8bit int for each column indicating type
            If it's a string, a second 8bit int indicating length of
        Records..        

*/