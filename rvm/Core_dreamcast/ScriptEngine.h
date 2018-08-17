//
//  ScriptEngine.h
//  rvm
//

#ifndef ScriptEngine_h
#define ScriptEngine_h

struct ScriptEngine {
    int operands[10];
    int tempValue[8];
    int arrayPosition[3];
    int checkResult;
    int sRegister;
};

#endif /* ScriptEngine_h */
