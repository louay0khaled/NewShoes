# Android Port

This directory is the first isolated Android-native port layer for Project New Shoes.

## Goal

Run the genuine Generals Zero Hour engine on Android without wrapping the browser launcher in a WebView.

## Strategy

The existing WebAssembly build remains untouched. Android gets its own platform adapter around the portable engine boundary:

- Android Activity + SurfaceView/GL rendering host
- JNI bridge between Kotlin and native C++
- Android filesystem adapter for user-owned game data
- Android audio adapter
- touch, mouse/keyboard, camera and command input translation
- Android lifecycle, pause/resume and safe shutdown
- native graphics backend derived from the existing D3D8 compatibility boundary

The first milestone is deliberately a buildable Android shell and native bridge. Later milestones replace the probe renderer with the real engine/device implementation one boundary at a time.

## Game data

Retail Generals/Zero Hour data is never committed to this repository. The Android launcher will consume data from a copy owned by the user.

## Status

- [x] Android project skeleton
- [x] JNI/native bridge
- [x] Native render heartbeat
- [ ] Real engine bootstrap
- [ ] Android filesystem backend
- [ ] Android input backend
- [ ] Native graphics backend
- [ ] Audio/video backend
- [ ] Skirmish on device
- [ ] Campaign validation
- [ ] Release APK
