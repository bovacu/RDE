// // Created by borja on 10/1/22.


// #include "SoundSource.h"
// #include <iostream>
// #include <vector>
// #include <algorithm>
// #include <numeric>
// #include <future>
// #include <thread>
// #include <string>
// #include <mutex>

// namespace engine {

//     SoundSource::SoundSource()
//     {

//     }

//     SoundSource::~SoundSource()
//     {
// //        pthread_cancel(playThread.native_handle());
//         alDeleteSources(1, &p_Source);
//     }

//     void SoundSource::Play(ALuint buffer_to_play)
//     {
// //        pthread_cancel(playThread.native_handle());
// //        playThread = std::thread([&]() {
// //            asyncPlay(buffer_to_play);
// //        });
//         asyncPlay(buffer_to_play);
//     }

//     void SoundSource::asyncPlay(ALuint _buffer) {
//         if (_buffer != p_Buffer)
//         {
//             p_Buffer = _buffer;
//             alSourcei(p_Source, AL_BUFFER, (ALint)p_Buffer);
//         }

//         alSourcePlay(p_Source);

//         ALint state = AL_PLAYING;

//         ALenum _error = alGetError();

//         if(_error != AL_NO_ERROR) {
//             LOG_E("Error playing sound on buffer ", _buffer, " with code ", _error)
//             return;
//         }

//         std::cout << "playing sound\n";

//         while (state == AL_PLAYING && _error == AL_NO_ERROR) {
//             alGetSourcei(p_Source, AL_SOURCE_STATE, &state);
//             _error = alGetError();
//         }

//         std::cout << "done playing sound\n";
//     }

//     void SoundSource::init() {
//         alGenSources(1, &p_Source);
//         alSourcef(p_Source, AL_PITCH, p_Pitch);
//         alSourcef(p_Source, AL_GAIN, p_Gain);
//         alSource3f(p_Source, AL_POSITION, p_Position[0], p_Position[1], p_Position[2]);
//         alSource3f(p_Source, AL_VELOCITY, p_Velocity[0], p_Velocity[1], p_Velocity[2]);
//         alSourcei(p_Source, AL_LOOPING, p_LoopSound);
//         alSourcei(p_Source, AL_BUFFER, p_Buffer);
//     }

// }