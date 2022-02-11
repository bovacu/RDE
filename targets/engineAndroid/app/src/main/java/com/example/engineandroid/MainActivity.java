package com.example.engineandroid;

import org.libsdl.app.SDLActivity;

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
}