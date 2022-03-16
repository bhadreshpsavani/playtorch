/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

import {NativeModules} from 'react-native';
import type {NativeJSRef} from '../NativeJSRef';

const resolveAssetSource = require('react-native/Libraries/Image/resolveAssetSource');

const {PyTorchCoreAudioModule: AudioModule} = NativeModules;

export interface Audio extends NativeJSRef {
  /**
   * Play an audio.
   */
  play(): void;
}

export const wrapRef = (ref: NativeJSRef): Audio => ({
  ...ref,
  play(): void {
    return AudioModule.play(ref);
  },
});

export const AudioUtil = {
  /**
   * Records an audio of a specific time duration.
   *
   * ```typescript
   * const audio: Audio = await AudioUtil.record(5);
   * ```
   *
   * @param length Time duration of the audio in seconds.
   * @returns A promise resolving into an [[Audio]]..
   */
  async record(length: number): Promise<Audio> {
    const ref: NativeJSRef = await AudioModule.record(length);
    return wrapRef(ref);
  },

  /**
   * Saves an audio to a file.
   *
   * ```typescript
   * const path: string = await AudioUtil.toFile(audio);
   * ```
   *
   * @param audio Audio to save.
   * @returns path Path to the audio file.
   */
  async toFile(audio: Audio): Promise<string> {
    return await AudioModule.toFile(audio);
  },

  /**
   * The `fromFile` function loads an [[Audio]] at the filepath (e.g., stored
   * on the file system).
   *
   * ```typescript
   * const audio: Audio = await AudioUtil.fromFile('/data/0/pytorch/audio.wav');
   * ```
   *
   * @param filePath The file path to the audio.
   * @returns A promise resolving into an [[Audio]].
   */
  async fromFile(filePath: string): Promise<Audio> {
    const ref: NativeJSRef = await AudioModule.fromFile(filePath);
    return wrapRef(ref);
  },

  /**
   * The `fromBundle` function loads an [[Audio]] that is bundled with the
   * React Native app bundle. The function param is a `require` with a relative
   * path (the path is relative to the file that contains the
   * [[AudioUtil.fromBundle]] call)
   *
   * ```typescript
   * const audioAsset = require('../../assets/audio/audio_file.wav');
   * const audio: Audio = await AudioUtil.fromBundle(audioAsset);
   * ```
   *
   * @param audioPath The audio path (i.e., a `require`).
   * @returns A promise resolving into an [[Audio]].
   */
  async fromBundle(path: number): Promise<Audio> {
    const source = resolveAssetSource(path);
    const ref: NativeJSRef = await AudioModule.fromBundle(source.uri);
    return wrapRef(ref);
  },
};
