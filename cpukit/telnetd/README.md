telnetd
=======

Author: Fernando Ruiz Casas <fruizcasas@gmail.com>

This directory contains a telnetd server with the primary features:

   + create a user shell pseudo-terminal task.

It is provided as a tool for RTEMS users to open more shell tcp/ip
pseudo-terminal.  Suggestions and comments are appreciated.

Read libmisc/shell for more information.

NOTES:

1.  OOB not yet implemented. Only a reduced negotiation is implemented.
    
2.  If you have tcp/ip initialied you can start telnetd daemon.
    You need register pseudo-terminals driver into device drivers table.

Enjoy it.

