program testMethodDesignatorComplexThis;

class room

BEGIN
   VAR doors  : integer;
      windows : integer;

FUNCTION countRenters(value :integer) : integer;
BEGIN
   countRenters := 7
END

END	      


class house

BEGIN
   VAR users	 : integer;
      this	 : room;
      livingroom : room;
      garage	 : room;
END		 


class testMethodDesignatorComplexThis

BEGIN
   
   VAR renters : integer;
       my      : house;
       yours   : house;

FUNCTION testMethodDesignatorComplexThis;
BEGIN

   renters := my.this.countRenters(8)
   
END

END
.
