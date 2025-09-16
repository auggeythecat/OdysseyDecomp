#pragma once

#include <math/seadMatrix.h>
#include <math/seadVector.h>
#include <prim/seadSafeString.h>
#include "Library/Se/SeFunction.h"
#include "Library/Se/SeKeeper.h"

namespace al {
class SeKeeper;
class PlayerHolder;
class AreaObjDirector;
class SeKeeper;
class IUseCamera;

// void startSe(al::IUseAudioKeeper const*, sead::SafeStringBase<char> const&);
// void verifySeKeeperNull(al::IUseAudioKeeper const*, sead::SafeStringBase<char> const&);
// void tryStartSe(al::IUseAudioKeeper const*, sead::SafeStringBase<char> const&);
// void holdSe(al::IUseAudioKeeper const*, sead::SafeStringBase<char> const&);
// void tryHoldSe(al::IUseAudioKeeper const*, sead::SafeStringBase<char> const&);
// void startSeWithParam(al::IUseAudioKeeper const*, sead::SafeStringBase<char> const&, float, char
// const*); void tryStartSeWithParam(al::IUseAudioKeeper const*, sead::SafeStringBase<char> const&,
// float, char const*); void holdSeWithParam(al::IUseAudioKeeper const*, sead::SafeStringBase<char>
// const&, float, char const*); void tryHoldSeWithParam(al::IUseAudioKeeper const*,
// sead::SafeStringBase<char> const&, float, char const*); void
// startSeWithMeInfo(al::IUseAudioKeeper const*, sead::SafeStringBase<char> const&, al::MeInfo
// const*); void holdSeWithMeInfo(al::IUseAudioKeeper const*, sead::SafeStringBase<char> const&,
// al::MeInfo const*); void startSeWithManualControl(al::IUseAudioKeeper const*,
// sead::SafeStringBase<char> const&, al::SePlayParamList const*, char const*, sead::Vector3<float>
// const*); void tryStartSeWithManualControl(al::IUseAudioKeeper const*, sead::SafeStringBase<char>
// const&, al::SePlayParamList const*, char const*, sead::Vector3<float> const*); void
// holdSeWithManualControl(al::IUseAudioKeeper const*, sead::SafeStringBase<char> const&,
// al::SePlayParamList const*, char const*, sead::Vector3<float> const*); void
// tryHoldSeWithManualControl(al::IUseAudioKeeper const*, sead::SafeStringBase<char> const&,
// al::SePlayParamList const*, char const*, sead::Vector3<float> const*); void
// stopSe(al::IUseAudioKeeper const*, sead::SafeStringBase<char> const&, int, char const*); void
// stopSeWithoutFreezed(al::IUseAudioKeeper const*, sead::SafeStringBase<char> const&, int, char
// const*); void tryStopSe(al::IUseAudioKeeper const*, sead::SafeStringBase<char> const&, int, char
// const*); void tryStopAllSeFromUser(al::IUseAudioKeeper const*, int, char const*); void
// stopAllSeFromUser(al::IUseAudioKeeper const*, int, char const*); void
// isPlayingAnySe(al::IUseAudioKeeper const*, char const*); void
// checkIsPlayingSe(al::IUseAudioKeeper const*, sead::SafeStringBase<char> const&, char const*);
// void setLifeTimeForHoldCall(al::IUseAudioKeeper const*, char const*, int, char const*);
// void setSeSourceVolume(al::IUseAudioKeeper const*, float);
// void setSeKeeperPlayNamePrefix(al::IUseAudioKeeper const*, char const*);
// void resetSeKeeperPlayNamePrefix(al::IUseAudioKeeper const*);
// void isExistSeKeeper(al::IUseAudioKeeper const*);
// void forceActivateSeKeeper(al::IUseAudioKeeper*);
// void validateSeKeeper(al::IUseAudioKeeper*);
// void invalidateSeKeeper(al::IUseAudioKeeper*);
// void setSeEmitterPoseMtxPtr(al::IUseAudioKeeper const*, sead::Matrix34<float> const*, char
// const*); void setSeEmitterPosePosPtr(al::IUseAudioKeeper const*, sead::Vector3<float> const*,
// char const*); void tryUpdateSeMaterialCode(al::IUseAudioKeeper*, char const*); void
// updateSeMaterialWater(al::IUseAudioKeeper*, bool); void updateSeMaterialWet(al::IUseAudioKeeper*,
// bool); void updateSeMaterialBeyondWall(al::IUseAudioKeeper*, bool); void
// updateSeMaterialPuddle(al::IUseAudioKeeper*, bool); void resetSeMaterialName(al::IUseAudioKeeper
// const*); void setSeModifier(al::IUseAudioKeeper const*, al::ISeModifier*); void
// setSeUserSyncParamPtr(al::IUseAudioKeeper const*, float const*, char const*); void
// setSeUserSyncParamPtrInt(al::IUseAudioKeeper const*, int const*, char const*); void
// setSeOutputFromController(al::SePlayParamList*, int, bool); void
// setSeOutputTvDrcRemoteAll(al::SePlayParamList*); void
// SeAreaTriggeredPlayer::SeAreaTriggeredPlayer(al::AudioDirector const*, al::AreaObjDirector*,
// al::PlayerHolder const*); void SeAreaTriggeredPlayer::reset(); void
// SeAreaTriggeredPlayer::update(); void SeAreaTriggeredPlayer::stopSe(al::SePlayArea*); void
// SeAreaTriggeredPlayer::startSe(al::SePlayArea*); void SeAreaTriggeredPlayer::getAudioKeeper();
// void SeAreaTriggeredPlayer::getAreaObjDirector();
}  // namespace al

namespace alSeFunction {

// void getSeKeeper(al::IUseAudioKeeper const);
// void tryGetSeKeeper(al::IUseAudioKeeper const);
// void tryGetSeUserName(al::IUseAudioKeeper const);
// void getSeDirector(al::IUseAudioKeeper const);
// void getSeBarrierKeeper(al::IUseAudioKeeper const);
// void getSeListener(al::IUseAudioKeeper const*, int);
// void getSeListenerPosition(al::IUseAudioKeeper const*, int);
// void isSeShape3DPoint(char const);
// void isSeShape3DSphere(char const);
// void isSeShape3DVector(char const);
// void isSeShape3DPlane(char const);
// void isSeShape3DRing(char const);
// void isSeShape3DCube(char const);
// void createSeShape(al::SeShapePose*, al::SeShapeInfo const);
// void stopAllSe(al::AudioDirector const*, unsigned int);
// void stopAllOneShotSe(al::AudioDirector const*, unsigned int, char const);
// void stopAllOneShotSe(al::IUseAudioKeeper const*, unsigned int, char const);
// void clearAllFreezedSe(al::AudioDirector const);
// void clearAllFreezedOneShotSe(al::AudioDirector const);
// void stopAllSeWithExceptList(al::AudioDirector const*, char const*, unsigned int);
// void setIsStateAfterGoal(al::AudioDirector const*, bool);
// void setIsStateAfterGoal(al::IUseAudioKeeper*, bool);
// void setIsExcludeCmNgSe(al::AudioDirector const);
// void separatePlayingSePosFromEmitter(al::IUseAudioKeeper const);
// void tryStartStageStartSe(al::IUseAudioKeeper const);
// void tryHoldStageStartSe(al::IUseAudioKeeper const);
// void tryStopStageStartSe(al::IUseAudioKeeper const);
// void tryStartDefaultAtmosphereSe(al::IUseAudioKeeper const);
// void tryStopDefaultAtmosphereSe(al::IUseAudioKeeper const);
// void loadSoundArchive(al::IAudioResourceLoader*, al::SeArchiveLoadingInfo const*,
// al::AudioInfoListWithParts<al::SeUserInfo> const*, bool); void
// startSituationWithAutoEnd(al::AudioDirector*, char const*, int, int, int); void
// startSituation(al::AudioDirector*, char const*, int); void endSituation(al::AudioDirector*, char
// const*, int);
void startSituation(const al::IUseAudioKeeper*, const char*, int);
void endSituation(const al::IUseAudioKeeper*, const char*, int);
// void checkIsActiveSituation(al::IUseAudioKeeper const*, char const);
// void setDirectorCameraWaterRate(al::IUseAudioKeeper const*, float);
// void setSePlayProhibitList(al::IUseAudioKeeper const*, char const**, int);
// void resetSePlayProhibitList(al::IUseAudioKeeper const);
// void setListenerParamByCamera(al::SeDirector*, int, al::IUseCamera const*, int);
// void startListenerPoser(al::SeDirector*, char const*, int);
// void endListenerPoser(al::SeDirector*, char const*, int);
// void startListenerPoser(al::AudioDirector*, char const*, int);
// void endListenerPoser(al::AudioDirector*, char const*, int);
void startListenerPoser(const al::IUseAudioKeeper*, const char*, int);
void endListenerPoser(const al::IUseAudioKeeper*, const char*, int);
// void updateSeParamListWithResInfo(al::SePlayParamList*, al::SeResourceInfo const*, bool);
// void updateSeParamListWithMeInfos(al::SePlayParamList*, al::BgmChordInfo const*, float,
// al::MeInfoList const*, al::MeInfo const); void conveySpeakerParamToHandle(al::SpeakerParam
// const*, al::AcLSoundHandle*, bool, float const*, int, bool); void
// conveyParamListToHandle(al::SePlayParamList const*, float, float, al::AcLSoundHandle*, float*,
// bool); void calcRollOff(al::SeRollOffCurveType, float, float, float, float, float, float); void
// killSeFromEmitters(al::SeDirector*, al::SeEmitterHolder); void
// startClippedSeFromEmitters(al::SeDirector*, al::SeEmitterHolder); void
// endClippedSeFromEmitters(al::SeDirector*, al::SeEmitterHolder); void
// stopAllFromEmitter(al::SeDirector*, al::SeEmitter*, unsigned int); void
// isToCalcSpeakerParamStereo(al::SeResourceInfo const); void calcDecibelToPriorityCoef(float,
// float, float); void tryFindVoiceLabelFromChar(char16_t); void
// tryFindVoiceLabelFromCharUsEu(char16_t, char16_t); void
// tryFindVoiceLabelRandomFromChar(char16_t); void startSeFromUpperLayerSeKeeper(al::IUseAudioKeeper
// const*, char const); void startSeLoopSequence(al::IUseAudioKeeper const*, char const*, al::MeInfo
// const*, int);

}  // namespace alSeFunction