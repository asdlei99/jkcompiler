program testFunctionCallArgObject;

class customClass

BEGIN

FUNCTION call(value : integer): integer;
BEGIN
   call := value + 1
END

END


class testFunctionCallArgObject

BEGIN
   
   VAR object : customClass;
       retval : integer;

FUNCTION setCompilerWorks(value	: customClass ): integer;
BEGIN
   PRINT value.call(2);
   setCompilerWorks := 4
END;
   
FUNCTION testFunctionCallArgObject;
BEGIN
   object := NEW customClass;
   retval := setCompilerWorks(object)
END

END
.
