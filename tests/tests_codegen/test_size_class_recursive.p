PROGRAM LinkedList;

CLASS Node
BEGIN
VAR value : integer;
    next : Node;

END

CLASS LinkedList
BEGIN

VAR list, eol : Node;

FUNCTION LinkedList;
BEGIN
	list := NEW Node;
	list.value := 5
END

END
.
