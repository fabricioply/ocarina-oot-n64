#ifndef OCARINA_N64_C_H
#define OCARINA_N64_C_H
#ifdef __cplusplus
extern "C" {
#endif
void OcarinaN64_InjectPad(void* inputs, int mode);
int OcarinaN64_AudioMuted(void);
void OcarinaN64_ShowSettings(void);
#ifdef __cplusplus
}
#endif
#endif
