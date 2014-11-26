%module CanTalon
%{
#include "ctre/CanTalonSRX.h"
%}

%include "cpointer.i"
%pointer_functions(double, doublep);
%pointer_functions(int, intp);
%pointer_functions(float, floatp);
%include "CtreCanNode.h"
%include "CanTalonSRX.h"
