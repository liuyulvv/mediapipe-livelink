#ifndef MEDIAPIPE_API_H_
#define MEDIAPIPE_API_H_

#ifdef _WIN32
#ifndef LibraryExport
#define LibraryExport __declspec(dllexport)
#endif
#else
#ifndef LibraryExport
#define LibraryExport __attribute__((visibility("default")))
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif


#include "mediapipe_struct.h"

LibraryExport void CreateFaceMeshInterface(const char* graph_name);
LibraryExport void ReleaseFaceMeshInterface();
LibraryExport void StartFaceMesh();
LibraryExport void FaceMeshProcess(void* mat);
LibraryExport void SetFaceMeshObserveCallback(landmark_callback callback);
LibraryExport void ObserveFaceMesh();
LibraryExport void AddFaceMeshPoller();
LibraryExport void GetFaceMeshOutput(NormalizedLandmark* normalized_landmark_list, unsigned size);
LibraryExport void StopFaceMesh();

LibraryExport void CreateHandTrackInterface(const char* graph_name);
LibraryExport void ReleaseHandTrackInterface();
LibraryExport void StartHandTrack();
LibraryExport void HandTrackProcess(void* mat);
LibraryExport void SetHandTrackObserveCallback(landmark_callback callback);
LibraryExport void ObserveHandTrack();
LibraryExport void AddHandTrackPoller();
LibraryExport void GetHandTrackOutput(NormalizedLandmark* normalized_landmark_list, unsigned size);
LibraryExport void StopHandTrack();

LibraryExport void CreatePoseTrackInterface(const char* graph_name);
LibraryExport void ReleasePoseTrackInterface();
LibraryExport void StartPoseTrack();
LibraryExport void PoseTrackProcess(void* mat);
LibraryExport void SetPoseTrackObserveCallback(landmark_callback callback);
LibraryExport void ObservePoseTrack();
LibraryExport void AddPoseTrackPoller();
LibraryExport void GetPoseTrackOutput(NormalizedLandmark* normalized_landmark_list, unsigned size);
LibraryExport void StopPoseTrack();

LibraryExport void CreateHolisticTrackInterface(const char* graph_name);
LibraryExport void ReleaseHolisticTrackInterface();
LibraryExport void StartHolisticTrack();
LibraryExport void HolisticTrackProcess(void* mat);
LibraryExport void SetHolisticTrackObserveCallback(landmark_callback callback, HolisticCallbackType type);
LibraryExport void ObserveHolisticTrack();
// LibraryExport void AddHolisticTrackPoller();
// LibraryExport void GetHolisticTrackOutput();
LibraryExport void StopHolisticTrack();

LibraryExport void CreateFaceBlendShapeInterface(const char* graph_name);
LibraryExport void ReleaseFaceBlendShapeInterface();
LibraryExport void StartFaceBlendShape();
LibraryExport void FaceBlendShapeProcess(void* mat);
LibraryExport void SetFaceBlendShapeCallback(blend_shape_callback callback);
LibraryExport void ObserveFaceBlendShape();
LibraryExport void AddFaceBlendShapePoller();
LibraryExport void GetFaceBlendShapeOutput(float* blend_shape_list, unsigned size);
LibraryExport void StopFaceBlendShape();

#ifdef __cplusplus
}
#endif

#endif