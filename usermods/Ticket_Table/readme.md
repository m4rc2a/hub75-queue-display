# Ticket Table

This usermod will simply display ticket numbers that are received via a serial (UART) connection.

Later, more features can be added, such as animations, keeping track of which numbers are currently waiting and which ones are being called.

In the future, a table layout like the following could be implemented:
+--------------------------+----------------------------+
| Currently waiting...     | Please go to the table     |
|--------------------------|----------------------------|
| 101                      | 100   -> Table 3           |
| 102                      | 099   -> Table 1           |
| 103                      |                            |
| ...                      |                            |
+--------------------------+----------------------------+

But for now, the main goal is just to print out random ticket numbers as they arrive.