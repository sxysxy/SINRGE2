#ifndef _SIN_INPUT_H_
#define _SIN_INPUT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif


int OnFocus();
int MouseWheel();
int MouseDblClk(int iKey);
int GetMouseMove();
void HideMouse(int hide);

#ifdef __cplusplus
}
#endif

#endif //_SIN_INPUT_H_