Sequence Generator UDF for Teradata

Copyright (c) 2014 Akira Kuroda <akuroda@gmail.com>
See the LICENSE file for license rights and limitations (MIT)

1. Summary

This table UDF generate sequence numbers utilizing all AMPs.


2. Syntax

 select * from table(gen_sequence(seq_start, seq_end)) as t;

This table function returns sequence numbers as rows starting seq_start
and ending seq_end (both inclusive). The returned rows are not sorted.

If seq_start and seq_end are equal, it returns one row. If seq_start is
less than seq_end, it returns nothing.


3. Installation

1 unpack the archive and go to src directory
2 start bteq and login to teradata
3 register UDFs by runnning scripts in the src diretcory:
 .run file = gen_sequence.sql


4. Example

select * from table(gen_sequence(2,4)) as t;

   sequence
-----------
          2
          4
          3
