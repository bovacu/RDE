#if IS_ANDROID()
ANativeWindow* rde_android_get_native_window() {
	return ENGINE.android_native_window;
}
#endif