// Created by borja on 18/2/22.

#include "core/Core.h"

#if IS_ANDROID()
#include <jni.h>
#include <string>
#include "firebase/admob/banner_view.h"
#include "firebase/admob/interstitial_ad.h"
#include "firebase/admob/rewarded_video.h"
#include <syslog.h>
#include <core/Engine.h>
#include "core/native/android/AndroidNative.h"

namespace engine {

    AndroidNative& AndroidNative::get() {
        static AndroidNative _androidNative;
        return _androidNative;
    }

    AndroidNative::AndroidNative() {
        androidEngine = new AndroidEngine;
    }

    void AndroidNative::init(JNIEnv *_env, jobject _context) {
        androidEngine->env = _env;
        androidEngine->context = _context;
    }

    JNIEnv *AndroidNative::getEnv() {
        return androidEngine->env;
    }

    jobject AndroidNative::getContext() {
        return androidEngine->context;
    }

    AndroidEngine *AndroidNative::getAndroidEngine() {
        return androidEngine;
    }

    void AndroidNative::destroy() {
        Engine::get().destroy();
        delete androidEngine;
    }




    NativeAds &NativeAds::get() {
        static NativeAds _nativeAds;
        return _nativeAds;
    }

    void engine::NativeAds::init(AndroidEngine* _androidEngine, jobject _context) {
        androidEngine = _androidEngine;
        app = firebase::App::Create(firebase::AppOptions(), _androidEngine->env, _androidEngine->context);
        const char* kAdMobAppID = "ca-app-pub-3940256099942544~3347511713";
        if(firebase::admob::Initialize(*app, kAdMobAppID) == firebase::InitResult::kInitResultSuccess) {
            syslog(LOG_INFO, "-------------------------------> AdMob initiated correctly!");
        } else
            syslog(LOG_CRIT, "-------------------------------> AdMob failed to initiate!");

        initInterstitial(_context);
        initBanner(_context);
        initRewarded(_context);
    }

    void NativeAds::loadAd(const AdType& _adType) {
        switch (_adType) {
            case BANNER: loadBanner(); break;
            case INTERSTITIAL: loadInterstitial(); break;
            case REWARD: loadRewarded(); break;
        }
    }

    void NativeAds::showAd(const AdType& _adType) {
        switch (_adType) {

            case BANNER: {
                auto _future = bannerView->LoadAdLastResult();
                if(_future.status() == firebase::kFutureStatusComplete) {
                    if(_future.error() == firebase::admob::kAdMobErrorNone) {
                        syslog(LOG_INFO, "-------------------------------> Showing banner");
                        bannerView->Show();
                    } else
                        checkLoading("Banner", _future);
                } else
                    syslog(LOG_WARNING, "-------------------------------> Banner not loaded yet!!");
                break;
            }
            case INTERSTITIAL: {
                auto _future = interstitialAd->LoadAdLastResult();
                if(_future.status() == firebase::kFutureStatusComplete) {
                    if(_future.error() == firebase::admob::kAdMobErrorNone) {
                        syslog(LOG_INFO, "-------------------------------> Showing interstitial");
                        interstitialAd->Show();
                    } else
                        checkLoading("Interstitial", _future);
                } else
                    syslog(LOG_WARNING, "-------------------------------> Interstitial not loaded yet!!");
                break;
            }
            case REWARD: {
                auto _future = firebase::admob::rewarded_video::LoadAdLastResult();
                if (_future.status() == firebase::kFutureStatusComplete) {
                    if(_future.error() == firebase::admob::kAdMobErrorNone) {
                        syslog(LOG_INFO, "-------------------------------> Showing rewarded");
                        firebase::admob::rewarded_video::Show(androidEngine->context);
                    } else
                        checkLoading("Rewarded", _future);
                } else
                    syslog(LOG_WARNING, "-------------------------------> Rewarded not loaded yet!!");
                break;
            }
        }
    }

    firebase::admob::AdRequest NativeAds::createRequest() {
        firebase::admob::AdRequest _request {};
        _request.gender = firebase::admob::kGenderUnknown;

        _request.birthday_day = 10;
        _request.birthday_month = 11;
        _request.birthday_year = 1976;

        static const char* kKeywords[] = {"AdMob", "C++", "Fun"};
        _request.keyword_count = sizeof(kKeywords) / sizeof(kKeywords[0]);
        _request.keywords = kKeywords;

        static const firebase::admob::KeyValuePair kRequestExtras[] = {
                {"the_name_of_an_extra", "the_value_for_that_extra"}};
        _request.extras_count = sizeof(kRequestExtras) / sizeof(kRequestExtras[0]);
        _request.extras = kRequestExtras;

        static const char* kTestDeviceIDs[] = {"2077ef9a63d2b398840261c8221a0c9b", "098fe087d987c9a878965454a65654d7"};
        _request.test_device_id_count =
                sizeof(kTestDeviceIDs) / sizeof(kTestDeviceIDs[0]);
        _request.test_device_ids = kTestDeviceIDs;
        return _request;
    }

    void NativeAds::initBanner(jobject _context) {
        syslog(LOG_INFO, "-------------------------------> Started to initiate banner");
        const char* kBannerAdUnit = "ca-app-pub-3940256099942544/6300978111";
        bannerView = new firebase::admob::BannerView();

        firebase::admob::AdSize ad_size{};
        ad_size.ad_size_type = firebase::admob::kAdSizeStandard;
        ad_size.width = 320;
        ad_size.height = 50;

        bannerView->Initialize(static_cast<firebase::admob::AdParent>(androidEngine->context), kBannerAdUnit, ad_size)
                .OnCompletion([](const firebase::Future<void>& future, void* user_data) {
                    syslog(LOG_INFO, "-------------------------------> Banner initiated");
                }, bannerView);
    }

    void NativeAds::initInterstitial(jobject _context) {
        syslog(LOG_INFO, "-------------------------------> Started to initiate interstitial");
        const char* kInterstitialAdUnit = "ca-app-pub-3940256099942544/1033173712";
        interstitialAd = new firebase::admob::InterstitialAd();
        interstitialAd->Initialize(static_cast<firebase::admob::AdParent>(_context), kInterstitialAdUnit)
            .OnCompletion([](const firebase::Future<void>& future, void* user_data) {
                syslog(LOG_INFO, "-------------------------------> Interstitial initiated");
            }, interstitialAd);
    }

    void NativeAds::initRewarded(jobject _context) {
        syslog(LOG_INFO, "-------------------------------> Started to initiate rewarded");
        firebase::admob::rewarded_video::Initialize().OnCompletion([](const firebase::Future<void>& future) {
            syslog(LOG_INFO, "-------------------------------> Rewarded initiated");
        });
    }

    void NativeAds::loadBanner() {
        syslog(LOG_INFO, "-------------------------------> Started to load banner");
        bannerView->MoveTo(firebase::admob::BannerView::kPositionBottom);
        bannerView->LoadAd(createRequest()).OnCompletion([](const firebase::Future<void>& future, void* user_data) {
            NativeAds::checkLoading("Banner", future);
        }, bannerView);
    }

    void NativeAds::loadInterstitial() {
        syslog(LOG_INFO, "-------------------------------> Started to load interstitial");
        interstitialAd->LoadAd(createRequest()).OnCompletion([](const firebase::Future<void>& future, void* user_data) {
            NativeAds::checkLoading("Interstitial", future);
        }, interstitialAd);
    }

    void NativeAds::loadRewarded() {
        syslog(LOG_INFO, "-------------------------------> Started to load rewarded");
        firebase::admob::rewarded_video::LoadAd("REWARDED_ID", createRequest()).OnCompletion([](const firebase::Future<void>& future) {
            NativeAds::checkLoading("Rewarded", future);
        });
    }

    void NativeAds::checkLoading(const char *_adType, const firebase::Future<void>& _future) {
        switch (_future.error()) {
            case firebase::admob::kAdMobErrorAlreadyInitialized :
                syslog(LOG_CRIT, "-------------------------------> %s already initialized", _adType); break;
            case firebase::admob::kAdMobErrorNone :
                syslog(LOG_INFO, "-------------------------------> %s initialized correctly", _adType); break;
            case firebase::admob::kAdMobErrorInternalError :
                syslog(LOG_CRIT, "-------------------------------> %s internal error, error %i, status %i, result %s", _adType, _future.error(), _future.status(), _future.result()); break;
            case firebase::admob::kAdMobErrorInvalidRequest :
                syslog(LOG_CRIT, "-------------------------------> %s invalid request", _adType); break;
            case firebase::admob::kAdMobErrorLoadInProgress :
                syslog(LOG_WARNING, "-------------------------------> %s is already loading", _adType); break;
            case firebase::admob::kAdMobErrorNetworkError :
                syslog(LOG_CRIT, "-------------------------------> %s network error while loading", _adType); break;
            case firebase::admob::kAdMobErrorNoFill :
                syslog(LOG_WARNING, "-------------------------------> %s loaded but not ad stock available!", _adType); break;
            case firebase::admob::kAdMobErrorUninitialized :
                syslog(LOG_CRIT, "-------------------------------> %s is uninitialized!", _adType); break;
            case firebase::admob::kAdMobErrorUnknown :
                syslog(LOG_CRIT, "-------------------------------> %s unknown error, error %i, status %i, result %s", _adType, _future.error(), _future.status(), _future.result()); break;
        }
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_android_MainActivity_initFireBaseAdds(JNIEnv *env, jobject thiz) {
    engine::AndroidNative::get().init(env, thiz);
    engine::NativeAds::get().init(engine::AndroidNative::get().getAndroidEngine(), thiz);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_android_MainActivity_destroy(JNIEnv *env, jobject thiz) {
    engine::AndroidNative::get().destroy();
}

#endif