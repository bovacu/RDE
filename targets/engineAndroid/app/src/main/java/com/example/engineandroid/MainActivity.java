package com.example.engineandroid;

import android.annotation.TargetApi;
import android.content.pm.ActivityInfo;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;

import org.libsdl.app.SDLActivity;

import java.io.File;
import java.util.Arrays;
import java.util.Objects;

public class MainActivity extends SDLActivity {
    @Override
    protected String[] getLibraries() {
        return new String[]{
                "hidapi",
                "SDL2",
                "SDL2_mixer",
                "SDL2_net",
                "SDL2_image",
                "engineAndroid"
        };
    }

    @TargetApi(Build.VERSION_CODES.N)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
    }
}