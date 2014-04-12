/* script to install the UDF */

replace function gen_sequence
  (start_num integer,
   end_num integer)
  returns table (sequence INTEGER)
  language c
  no sql
  external name 'cs:gen_sequence:gen_sequence.c:F:gen_sequence'
  parameter style sql;
