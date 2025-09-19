#pragma once

#include <math/seadMatrix.h>

namespace al {
class IUseAudioKeeper;
}  // namespace al

namespace alSeFunction {

void startSituation(const al::IUseAudioKeeper*, const char*, s32);
void endSituation(const al::IUseAudioKeeper*, const char*, s32);
void startListenerPoser(const al::IUseAudioKeeper*, const char*, s32);
void endListenerPoser(const al::IUseAudioKeeper*, const char*, s32);

}  // namespace alSeFunction
