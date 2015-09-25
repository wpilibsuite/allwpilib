%module CanTalon
%{
#include "ctre/CanTalonSRX.h"
%}

%include "cpointer.i"
%pointer_functions(double, doublep);
%pointer_functions(int, intp);
%pointer_functions(uint32_t, uint32_tp);
%pointer_functions(int32_t, int32_tp);
%pointer_functions(uint8_t, uint8_tp);
%pointer_functions(CTR_Code, CTR_Codep);
%pointer_functions(float, floatp);
%include "CtreCanNode.h"
%include "CanTalonSRX.h"
