// Created by borja on 18/2/22.


#ifndef ENGINE2_0_NATIVE_ADS_H
#define ENGINE2_0_NATIVE_ADS_H

#include "core/Core.h"
#if IS_ANDROID()
#include <jni.h>
#include "firebase/admob.h"

namespace engine {

    struct AndroidEngine {
        jobject context;
        JNIEnv* env;
    };

    class AndroidNative {
        private:
            AndroidEngine* androidEngine;

        private:
            AndroidNative();

        public:
            static AndroidNative& get();

        public:
            void init(JNIEnv* _env, jobject _context);
            AndroidEngine* getAndroidEngine();
            JNIEnv* getEnv();
            jobject getContext();
            void destroy();
    };

    enum AdType {
        BANNER,
        INTERSTITIAL,
        REWARD
    };

    class NativeAds {
        private:
            AndroidEngine* androidEngine;
            firebase::admob::BannerView* bannerView;
            firebase::admob::InterstitialAd* interstitialAd;
            firebase::App* app;

        private:
            static firebase::admob::AdRequest createRequest();
            static void checkLoading(const char* _adType, const firebase::Future<void>& _future);
            void initInterstitial(jobject _context);
            void initBanner(jobject _context);
            void initRewarded(jobject _context);
            void loadBanner();
            void loadInterstitial();
            void loadRewarded();

        public:
            static NativeAds& get();
            void init(AndroidEngine* _androidEngine, jobject _context);
            void loadAd(const AdType& _adType);
            void showAd(const AdType& _adType);
    };

}

#endif

#endif //ENGINE2_0_NATIVE_ADS_H
