# Grammar

    query = SELECT exprlist FROM source
    exprlist = expr | expr, exprlist
    source = "(" QUERY ")" | tbl
    tbl = ident
    expr = ident | constant | functioncall | star
    star = "*"
    
    