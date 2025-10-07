Hunter McGraw

Design:

Parent Process calls create_nodes, which is a recursive function to create the ring of nodes.

Inside create_nodes(), a pipe is created that the child reads from and the parent writes to.

For example, for 5 nodes:

reads from fd\[4\]\[0\] - node 0 (parent) - writes to fd\[0\]\[1\]

reads from fd\[0\]\[0\] - node 1 - writes to fd\[1\]\[1\]

reads from fd\[1\]\[0\] - node 2 - writes to fd\[2\]\[1\]

reads from fd\[2\]\[0\] - node 3- writes to fd\[3\]\[1\]

reads from fd\[3\]\[0\] - node 4 - writes to fd\[4\]\[1\]


Each node (except node 0) in create_nodes() starts the send_message() function which loops forever and reads and writes information.
Node 0 does not run the send_message() function to give it more control as the process that creates the messages.

Node 0 waits until create_nodes() finishes creating all the nodes by waiting for the last node to write to it that it has been created.
Then, Node 0 asks for user input to send a message.

Nodes 0 writes the information and the next node receives the information and checks if it is the intended recipient.
If so, that node prints the message and writes an empty string to the next node. Otherwise, it passes the message to the next node.

Node 0 receives the empty string and asks for a new message.

On ctrl-c, each individual process exits.
