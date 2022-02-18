// Created by borja on 18/2/22.

#include "core/native/android/NativeAds.h"
#include "core/Core.h"

#if IS_ANDROID()
#include <jni.h>
#include <string>
#include "firebase/admob/banner_view.h"
#include <syslog.h>


extern "C"
JNIEXPORT void JNICALL
Java_com_example_android_MainActivity_initFireBaseAdds(JNIEnv *env, jobject thiz) {
    firebase::App* app = firebase::App::Create(firebase::AppOptions(), env, thiz);
    const char* kAdMobAppID = "ca-app-pub-3940256099942544~3347511713";
    firebase::admob::Initialize(*app, kAdMobAppID);

    const char* kBannerAdUnit = "ca-app-pub-3940256099942544/6300978111";
    const char* kInterstitialAdUnit = "ca-app-pub-3940256099942544/1033173712";

    firebase::admob::BannerView* banner_view;
    banner_view = new firebase::admob::BannerView();

    firebase::admob::AdSize ad_size{};
    ad_size.ad_size_type = firebase::admob::kAdSizeStandard;
    ad_size.width = 320;
    ad_size.height = 50;
// my_ad_parent is a reference to an iOS UIView or an Android Activity.
// This is the parent UIView or Activity of the banner view.
    banner_view->Initialize(static_cast<firebase::admob::AdParent>(thiz), kBannerAdUnit, ad_size);
    banner_view->MoveTo(banner_view->kPositionBottom);

    firebase::admob::AdRequest my_ad_request = {};

    my_ad_request.gender = firebase::admob::kGenderUnknown;

// The user's birthday, if known. Note that months are indexed from one.
    my_ad_request.birthday_day = 10;
    my_ad_request.birthday_month = 11;
    my_ad_request.birthday_year = 1976;

// Additional keywords to be used in targeting.
    static const char* kKeywords[] = {"AdMob", "C++", "Fun"};
    my_ad_request.keyword_count = sizeof(kKeywords) / sizeof(kKeywords[0]);
    my_ad_request.keywords = kKeywords;

// "Extra" key value pairs can be added to the request as well.
    static const firebase::admob::KeyValuePair kRequestExtras[] = {
            {"the_name_of_an_extra", "the_value_for_that_extra"}};
    my_ad_request.extras_count = sizeof(kRequestExtras) / sizeof(kRequestExtras[0]);
    my_ad_request.extras = kRequestExtras;

// Register the device IDs associated with any devices that will be used to
// test your app. Below are sample test device IDs used for making the ad request.
    static const char* kTestDeviceIDs[] = {"2077ef9a63d2b398840261c8221a0c9b", "098fe087d987c9a878965454a65654d7"};
    my_ad_request.test_device_id_count =
            sizeof(kTestDeviceIDs) / sizeof(kTestDeviceIDs[0]);
    my_ad_request.test_device_ids = kTestDeviceIDs;

    banner_view->LoadAd(my_ad_request).OnCompletion([](const firebase::Future<void>& future, void* user_data) {
        auto* banner = static_cast<firebase::admob::BannerView*>(user_data);
        banner->Show();
    }, banner_view);

//    firebase::admob::InterstitialAd* interstitial_ad;
//    interstitial_ad = new firebase::admob::InterstitialAd();
//    interstitial_ad->Initialize(static_cast<firebase::admob::AdParent>(thiz), kInterstitialAdUnit);
//    interstitial_ad->LoadAd(my_ad_request).OnCompletion([](const firebase::Future<void>& future, void* user_data) {
//        auto* inter = static_cast<firebase::admob::InterstitialAd*>(user_data);
//        inter->Show();
//    }, interstitial_ad);
}
#endif