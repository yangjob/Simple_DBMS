#include "CLExecutive.h"
#include "CLExecutiveFunctionProvider.h"

CLExecutive::CLExecutive(CLExecutiveFunctionProvider *pExecutiveFunctionProvider) {
    if(pExecutiveFunctionProvider == 0)
        throw "在CLExecutive::CLExecutive()中, 指针pExecutiveFunctionProvider错误";

    _pExecutiveFunctionProvider = pExecutiveFunctionProvider;
}

CLExecutive::~CLExecutive() {

}