# Guía para crear tu propia librería de juegos 2D

Buenas a todos, soy Borja Vazquez, ingeniero de software y desarrollador de videojuegos. Como podréis imaginar por la descripción y por el hecho de estar haciendo esta guía, me encanta programar juegos, pero tambien me encanta trabajar en herramientas para facilitar la creación de videojuegos, en este caso una librería 2D (ya que es mi tipo de juego favorito) que es lo que os vengo a explicar hoy.

En esta guía aprenderemos como crear desde cero y paso a paso la misma librería que yo he creado en mi tiempo libre y que uso para mis proyectos personales. Si solo te interesa el código fuente os dejo ya por aquí el link, pero creo que seria muy interesante que leyeseis el documento entero.

Lo primero que quiero aclarar es que esta guía no es una declaración de como hay que hacer las cosas ni quiere decir que todo sea perfecto, lo que pretendo con esta guía es explicar de la manera mas clara y extensa posible como funciona OpenGL, como crear una herramienta que nos facilite muchísimo la vida como programadores y empezar a entender como motores mayores, como Godot o Unity funcionan por dentro.

Dicho esto, vamos con la instalación de todas las librerías y herramientas que vamos a necesitar.

## Instalación

Primero de todo decir que yo trabajo en Linux, específicamente en Ubuntu, por lo que la parte mas completa sera en esta plataforma, no estoy seguro del todo de como sera la instalación en Windows o Mac, pero todas las librerías son muy conocidas y no creo que haya mucho problema.

Necesitaremos las siguientes librerías:

- SDL2
- Glad
- Freetype
- Vorbis
- CMake
- Clang (o el compilador de vuestra elección)

Para hacer debug de una forma mas fácil y también tener cierta información extra, incluiremos también ImGui, pero esto lo haremos mas adelante.

Para instalar todo lo necesario en Linux usaremos:

```terminal
sudo apt-get install libsdl2-dev libsdl2-mixer-dev libfreetype6-dev libfreetype-dev vorbis-tools build-essential cmake clang libsdl2-net-dev
```

Con esto deberíamos estar mas que listos para poder empezar. Si falta alguna librería seria solo instalarla, porque el compilador os avisara de cual falta.



## IDEs

Aquí, dependiendo del sistema operativo que uséis, y sobretodo dependiendo de vuestro gusto con los editores de código podéis escoger con el que os sintáis mas cómodos, pero yo recomiendo uno que sepa trabajar o integrar fácil CMake, ya que es lo que usaremos para la compilación del proyecto. En mi caso recomiendo:

- CLion
- Visual Studio Code

Como veréis no he nombrado Visual Studio, primero porque no trabajo en Windows y segundo porque no me gusta nada de nada, pero nuevamente, elegid el que os sea mas cómodo.

La ventaja de CLion es que es un IDE que funciona de maravilla y que esta listo para poder trabajar directamente con este tipo de proyectos. Totalmente integrado con CMake, es una maravilla y el que uso yo. Integración directa tambien con Valgrind. Problema, es de pago, pero si eres estudiante es gratis.

VSCode también me gusta y con la ultima integración de CMake se ha hecho mucho mas fácil compilar y ejecutar el código, por lo que es una buena opcion, pero para mi gusto le faltan características que CLion si tiene, pero VSCode es una opcion perfecta y mas que de sobra.



## Ventana y eventos

### Que es y como funciona SDL2

### Alternativas a SDL2

### Creando nuestra primera ventana

### Capturando nuestros primeros eventos

### Creando un sistema de gestión de eventos





## Renderizado

### Que es OpenGL y como funciona

### Buffers

### Dibujando nuestro primer triangulo

### Dibujando nuestro primer rectángulo

### Texturas

### Model View Projection Matrix

### Cámaras

### Shaders

### Sprites

### Texture Atlas

### Batching

### Sprite Batch

### Entendiendo como dibujar texto 

### Abstrayendo un Renderer





## Sonido

### Como maneja SDL2 el sonido

### Creando un manejador de sonido





## Físicas





## Sistema de partículas





## Android

### Configuración y build

Aun no tengo claro si es el metodo correcto o no, pero desde luego me genera las builds de android y funcionan sin problema, asi que voy a describiros aqui como lo he conseguido.

Lo primero de todo es bajarse android studio.  Os dira que donde quereis instalar el SDK, a mi, por comodidad lo he instalado en la mi carpeta personal, porque lo necesitaremos luego para localizar diferentes cosas.

Una vez lo tengais instalado, vamos a Tools > SDK Manager y nos aparecera esta ventana en la que debeis marcar para instalar:

- Android SDK Build-Tools
- NDK
- Android SDK Platform-Tools
- Android SDK Command-line Tools
- CMake

![image-20220211222530336](/home/borja/snap/typora/49/.config/Typora/typora-user-images/image-20220211222530336.png)



Es posible que algunas de las opciones ya esten instaladas, no hay problema. Muy importante saber que el NDK se instalara en la misma carpeta donde hayamos instalado el SDK, por eso es bueno tenerlo a mano. El NDK es lo que nos va a permitir convertir nuestro codigo C++ a algo que Android entiende. No hemos terminado con el NDK, probablemente la version que os instale sea de la 23 para arriba y eso genera fallos, por lo menos a mi. Os mostrare como instalar otra version mas adelante.

Vale, ahora necesitamos el JDK para poder ejecutar y compilar con Java, pero para ello nuevamente necesitamos una version especifica, ya que si no genera muchos fallos. Necesitamos el [JDK 8](https://www.oracle.com/br/java/technologies/javase/javase8-archive-downloads.html). Este es un archivo que podeis guardar donde querais, pero por comodidad yo lo he guardado en CARPETA_ANDROID_STUDIO/jre/jdk8. Es importante que sepais todas las rutas ya que las necesitaremos.

Por ultimo creamos una carpeta nueva en la raiz del proyecto llamada targets.

Vale, ya tenemos las herramientas que necesitabamos, vamos a generar el proyecto. Para ello o lo creamos desde el panel inicial de android studio si es la primera vez que lo usamos o File > New Project > Empty Activity, ahi ponedle de nombre com.example.androidTest. La ruta que sea esta ultima carpeta que hemos creado llamada targets. Todo lo demas podeis dejarlo por defecto. Vereis algo asi:

(En la imagen de debajo hay un error, pone com.example.test y deberia ser com.example.androidTest)

![image-20220211223815772](/home/borja/snap/typora/49/.config/Typora/typora-user-images/image-20220211223815772.png)

Igual tarda un poco mientras Gradle hace sus cosas, no hay problema. Cuando haya acabado hacemos seleccionamos la carpeta raiz app y vamos a File > New > New Module, lo rellenamos asi:

![image-20220211224142622](/home/borja/snap/typora/49/.config/Typora/typora-user-images/image-20220211224142622.png)

Es posible que tambien le lleve un rato a Gradle, pero tendremos un nuevo directorio al mismo nivel de app, que se llamara sdl. 

Lo siguiente es acceder al repo de SDL2 en github e ir a los .java del proyecto de android, aqui el [link](https://github.com/libsdl-org/SDL/tree/main/android-project/app/src/main/java/org/libsdl/app). Os teneis que bajar todos los .java y copiarlos dentro de android studio en sdl/org.libsdl.app (no en las que tiene al lado test o androidTest, ignorad esas). Tambien vamos a eliminar las carpetas sdl/src/androidTest, sdl/src/test, pueden darnos problemas mas adelante.

Una vez hecho esto, vamos a generar un pequeno script en la misma carpeta que contiene a app y sdl, llamemosla ANDROID_ROOT de aqui en adelante, para inicializar algunas cosas, es el siguiente:

```bash
#!/bin/bash

# We will assume that the Android SDK is in the default location that Android Studio installs it to.
ANDROID_SDK="/home/borja/Android/Sdk"
echo "Using Android SDK at: $ANDROID_SDK"

# We will assume that we'll use Java that is bundled with Android Studio.
export JAVA_HOME="/home/borja/android-studio/jre/jdk1.8.0_321"

# We will be using a specific version of the Android NDK.
NDK_VERSION="20.1.5948944"
ANDROID_NDK="$ANDROID_SDK/ndk/$NDK_VERSION"
echo "Using Android NDK at: $ANDROID_NDK"

# This command will automatically answer 'yes' to each of the licences that a user normally has to manually agree to.
echo "Auto accepting Android SDK licenses ..."
yes | $ANDROID_SDK/tools/bin/sdkmanager --licenses

# This is the collection of Android SDK components that our code base will need.
echo "Installing required Android SDK components ..."
$ANDROID_SDK/tools/bin/sdkmanager \
    "platform-tools" \
    "build-tools;30.0.3" \
    "tools" \
    "platforms;android-28" \
    "cmake;3.18.1" \
    "ndk;$NDK_VERSION"

```

Aqui es donde os comentaba el problema del NDK, probablemente el que se os haya instalado no sea la version 20.1.5948944, sera de la 23 para arriba, y estos dan problemas, pero no os preocupeis, podeis bajarlo de [aqui](https://androidsdkoffline.blogspot.com/p/android-ndk-side-by-side-direct-download.html). Recordad meterlo en la misma carpeta que esta el de vuestra version.

Por supuesto, ajustad las rutas a vuestros directorios. Una vez este todo correcto, chmod +x setup.sh (o como hayais llamado al archivo) y ./setup.sh. Si os sale un error de java de algun tipo revisad bien las rutas y que este todo seteado al jdk8 y al ndk 20.1.5948944.

Si accedemos a la carpeta ROOT_ANDROID tendremos algo asi:

![image-20220211225741681](/home/borja/snap/typora/49/.config/Typora/typora-user-images/image-20220211225741681.png)

Generemos la estructura (solo marcare los importantes):

```
ROOT_ANDROID	
|_	+app
|_	+gradle
|_	+sdl
|_	 build.gradle
|_	 gradlew
|_	 setup.sh
```

Vamos a crear lo siguiente:

```
ROOT_ANDROID	
|_	+app
|_	+gradle
|_	+sdl
	|_ jni
       |_ Android.mk
       |_ Application.mk
|_	 build.gradle
|_	 gradlew
|_	 setup.sh
```



Dentro de Android.mk:

```mk
include $(call all-subdir-makefiles)
```

Y dentro de Application.mk

```mk
APP_STL := c++_shared
APP_ABI := armeabi-v7a arm64-v8a x86 x86_64
APP_PLATFORM := android-21
```



El de Android.mk nos servira para compilar todo el codigo de SDL2 desde sus respectivos CMakes. El de Application.mk le indica como seran las librerias (shared), los ABI son los tipos de plataformas, asi que incluimos todas y el platform lo ponemos en 21 para abarcar un gran numero de dispositivos.

Lo siguiente sera bajarnos [SDL2](https://www.libsdl.org/download-2.0.php), [SDL2_image](https://www.libsdl.org/projects/SDL_image/), [SDL2_mixer](https://www.libsdl.org/projects/SDL_mixer/), [SDL2_net](https://www.libsdl.org/projects/SDL_net/). Y los guardamos en:

```
ROOT_ANDROID	
|_	+app
|_	+gradle
|_	+sdl
|_	+SDL
|_	+SDL_image
|_	+SDL_mixer
|_	+SDL_net
|_	 build.gradle
|_	 gradlew
|_	 setup.sh
```

Perfecto, ahora vamos a toquetear unos build.gradle para poder configurar la build de android correctamente. El primero sera el build.gradle de ROOT_ANDROID, el cual vamos a rellenar con lo siguiente:

```gradle
ext.engine = [
    versionCode: 1,
    versionName: '1.0',
    buildToolsVersion: '30.0.3',
    compileSdkVersion: 28,
    minSdkVersion: 21,
    targetSdkVersion: 28,
    ndkVersion: '20.1.5948944'
]

buildscript {
    repositories {
        google()
        jcenter()

    }
    dependencies {
        classpath 'com.android.tools.build:gradle:4.2.2'
    }
}

allprojects {
    repositories {
        google()
        jcenter()
    }
}

dependencies {
   
}

task clean(type: Delete) {
    delete rootProject.buildDir
}
```

Modificad buildToolsVersion: '30.0.3' por la que tengais instalada. Ahora vamos a settings.gradle, esta en la misma carpeta y eliminamos esta parte:

```
dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {
        google()
        mavenCentral()
    }
}
```

Por alguna razon entra en conflicto con el allRepositories del build.gradle y no puede compilar (me consto un monton encontrar el fallo).

Ahora vamos al build.gradle de la carpeta sdl (no SDL) y copiamos esto (eliminad lo que esta):

```
apply plugin: 'com.android.library'

android {
    buildToolsVersion engine.buildToolsVersion
    compileSdkVersion engine.compileSdkVersion
    ndkVersion engine.ndkVersion

    defaultConfig {
        minSdkVersion engine.minSdkVersion
        targetSdkVersion engine.targetSdkVersion
        versionCode engine.versionCode
        versionName engine.versionName
    }

    buildTypes {
        release {
            minifyEnabled false
        }
    }

    externalNativeBuild {
        ndkBuild {
            path 'jni/Android.mk'
        }
    }
}
```

Ahora vamos con la parte de los XML, que es donde menos controlo, asi que confiad en mi que funciona, pero no se muy bien como hacer modificaciones.  Vamos a sdl/src/main/AndroidManifest.xml y escribimos lo siguiente:

```html
<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="org.libsdl.app">

    <uses-feature android:glEsVersion="0x00020000" />
    <uses-feature android:name="android.hardware.touchscreen" android:required="false" />
    <uses-feature android:name="android.hardware.gamepad" android:required="false" />
    <uses-feature android:name="android.hardware.type.pc" android:required="false" />
</manifest>
```

Listo, vamos a crear ahora el CMakeLists.txt que va a compilar todo nuestro codigo. Para ello en ROOT_ANDROID creamos un archivo llamado CMakeLists.txt:

```cmake
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fexceptions")

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_VARIANT_DIR)

# ${CMAKE_SOURCE_DIRECTORY} en este caso hace referencia a donde esta el archivo CMakeLists.txt, es decir, 
# en ROOT_ANDROID, por lo que todas las rutas seran relativas a este directorio, justo lo que queremos.
# Creamos una variable con la ruta de cada una de las librerias que vamos a usar.
set(SDL_DIR "${CMAKE_SOURCE_DIRECTORY}/SDL")
set(SDL_MIXER_DIR "${CMAKE_SOURCE_DIRECTORY}/SDL_mixer")
set(SDL_NET_DIR "${CMAKE_SOURCE_DIRECTORY}/SDL_net")
set(SDL_IMAGE_DIR "${CMAKE_SOURCE_DIRECTORY}/SDL_image")

# No os preocupeis por estas dos, os explico despues como bajarlas y como hay que guardarlas
set(FREETYPE_DIR "${CMAKE_SOURCE_DIRECTORY}/freetype")
set(GLM_DIR "${CMAKE_SOURCE_DIRECTORY}/glm")

# Aqui le estamos indicando donde esta el codigo del motor que hay que compilar, esta dos carpetas mas arriba
# en la jearquia del proyecto total empezando desde ROOT_ANDROID. Lo mismo para Sandbox, hay que compilarlo.
set(MAIN_SOURCE_DIR "../../engine")
set(SANDBOX_SOURCE_DIR "../../sandbox")

# Incluimos las librerias externas tambien
set(EXTERNAL_STB "../../engine/include/stb")
set(EXTERNAL_IMGUI "../../external/imgui")

# Este es el directorio donde estaran las librerias que indicamos mas abajo compiladas como .so (en linux). 
# Os explico un poco mas adelante como generarlas
set(SDL_SO_DIR "${CMAKE_SOURCE_DIR}/sdl/build/intermediates/ndkBuild/${BUILD_VARIANT_DIR}/lib/${ANDROID_ABI}")

# Anadimos cada una de las librerias necesarias para nuestro motor poder funcionar
add_library(
        hidapi
        SHARED
        IMPORTED
)

add_library(
        sdl2
        SHARED
        IMPORTED
)

add_library(
        sdl2_mixer
        SHARED
        IMPORTED
)

add_library(
        sdl2_net
        SHARED
        IMPORTED
)

add_library(
        sdl2_image
        SHARED
        IMPORTED
)

add_library(
        mpg123
        SHARED
        IMPORTED
)

# Esto es importante porque freetype y glm no las incluimos con las librerias de SDL
add_subdirectory(freetype)
add_subdirectory(glm)

find_library(log-lib log)

# Ahora le indicamos a cada libreria donde se encuentra su .so
set_target_properties(
        hidapi
        PROPERTIES
        IMPORTED_LOCATION
        ${SDL_SO_DIR}/libhidapi.so
)

set_target_properties(
        sdl2
        PROPERTIES
        IMPORTED_LOCATION
        ${SDL_SO_DIR}/libSDL2.so
)

set_target_properties(
        sdl2_mixer
        PROPERTIES
        IMPORTED_LOCATION
        ${SDL_SO_DIR}/libSDL2_mixer.so
)

set_target_properties(
        sdl2_net
        PROPERTIES
        IMPORTED_LOCATION
        ${SDL_SO_DIR}/libSDL2_net.so
)

set_target_properties(
        sdl2_image
        PROPERTIES
        IMPORTED_LOCATION
        ${SDL_SO_DIR}/libSDL2_image.so
)

set_target_properties(
        mpg123
        PROPERTIES
        IMPORTED_LOCATION
        ${SDL_SO_DIR}/libmpg123.so
)

# Tenemos que incluir los includes de todo, ya que si no, no encontrara las cabeceras de nuestro motor
include_directories(${SDL_DIR}/include)
include_directories(${SDL_MIXER_DIR}/include)
include_directories(${SDL_NET_DIR}/include)
include_directories(${SDL_IMAGE_DIR})
include_directories(${FREETYPE_DIR}/include ${FREETYPE_DIR}/freetype/include)
include_directories(${GLM_DIR}/glm)
include_directories(${MAIN_SOURCE_DIR}/include)
include_directories(${SANDBOX_SOURCE_DIR})
include_directories(${EXTERNAL_STB})
include_directories(.)

# Con esto incluimos de forma recursiva todos los archivos .h, .hpp y cpp y los compilamos de golpe
file(GLOB_RECURSE CPP_HEADERS ${MAIN_SOURCE_DIR}/include/*.h*  ${SANDBOX_SOURCE_DIR}/*.h*)
file(GLOB_RECURSE CPP_SOURCES ${MAIN_SOURCE_DIR}/src/*.cpp   ${SANDBOX_SOURCE_DIR}/*.cpp)

# Indicamos el tipo de perfil de OpenGL que queremos
set(GL_PROFILES "GL_PROFILE_GLES2" "GL_PROFILE_GLES3")

# Generamos la libreria de nuestro motor
add_library(
        androidTest
        SHARED
        ${CPP_HEADERS}
        ${CPP_SOURCES}
)

# Aqui linkeamos todo lo que nuestra libreria necesita
target_link_libraries(
        androidTest
        hidapi
        sdl2
        sdl2_mixer
        sdl2_net
        sdl2_image
        mpg123
        freetype
        glm
        ${log-lib}
        GLESv3
)

target_compile_definitions(androidTest PRIVATE ${GL_PROFILES})
```

Si, es bastante largo, pero lo he ido explicando con comentarios poco a poco. El tema de [glm](https://github.com/g-truc/glm) y de [freetype](https://download.savannah.gnu.org/releases/freetype/). De Freetype yo me baje la 2.10.0 y funciona de maravilla. Los guardais en ROOT_ANDROID igual que hemos guardado SDL_*.

Una vez tenemos el CMakeLists.txt listo podemos pasar a configurar el ultimo build.gradle que es el que esta dentro de app.

```gradle
apply plugin: 'com.android.application'

android {
    buildToolsVersion engine.buildToolsVersion
    compileSdkVersion engine.compileSdkVersion
    ndkVersion engine.ndkVersion

    defaultConfig {
        applicationId "com.example.androidTest"
        
        minSdkVersion engine.minSdkVersion
        targetSdkVersion engine.targetSdkVersion
        versionCode engine.versionCode
        versionName engine.versionName

        externalNativeBuild {
            cmake {
                arguments "-DANDROID_STL=c++_shared", "-DAPP_PLATFORM=android-21"
            }
        }

        ndk {
            abiFilters 'armeabi-v7a', 'arm64-v8a', 'x86', 'x86_64'
        }
    }

    buildTypes {
        release {
            signingConfig signingConfigs.debug
            minifyEnabled false
        }
    }

    externalNativeBuild {
        cmake {
            path "../CMakeLists.txt"
        }
    }
}

dependencies {
    implementation project(':sdl')
}
```

Como veis a este le indicamos donde esta el CMakeLists.txt que es el que construye todo. Vamos a generar ahora unos symlinks para ya casi poder compilar el proyecto. Desde la terminal nos vamos al directorio ROOT_ANDROID/sdl/jni y hacemos:

- ln -s ../../SDL
- ln -s ../../SDL_image
- ln -s ../../SDL_net
- ln -s ../../SDL_mixer
- ln -s ../../freetype

Ah, borramos tambien en ROO_ANDROID/app/sr/test y androidTest, importante. Vamos a editar el otro AndroidManifest que se encuentra en ROOT_ANDROID/app/src/main:

```xml
<?xml version="1.0" encoding="utf-8"?>
<manifest
    xmlns:android="http://schemas.android.com/apk/res/android"
    package="com.example.androidTest"
    >

    <application
        android:allowBackup="false"
        android:icon="@mipmap/ic_launcher"
        android:label="@string/app_name"
        android:roundIcon="@mipmap/ic_launcher_round"
        android:theme="@android:style/Theme.NoTitleBar.Fullscreen"
        android:hardwareAccelerated="true"
        >

        <activity
            android:name=".MainActivity"
            android:configChanges="keyboard|keyboardHidden|orientation|screenSize"
            android:screenOrientation="sensorLandscape"
            >
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
        </activity>
    </application>
</manifest>
```

Ahora vamos a MainActivity.java, en ROOT_ANDROID/app/src/main/java/com/example/androidTest y eliminamos lo que haya y ponemos:

```java
package com.example.androidTest;

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
                "androidTest"
        };
    }
}
```

Ahora vamos a generar las librerias para poder compilar todo. 

Nos dirigimos a ROOT_ANDROID/sdl/jni y ejecutamos en la terminal:

```bash
PATH_A_TU_NDK/20.1.5948944/ndk-build APP_PLATFORM=android-21 NDK_DEBUG=1 NDK_OUT=PATH_AL_PROYECTO_RAIZ/Engine2.0/targets/engineAndroid/sdl/build/intermediates/ndkBuild/debug/object NDK_LIBS_OUT=PATH_AL_PROYECTO_RAIZ/Engine2.0/targets/engineAndroid/sdl/build/intermediates/ndkBuild/debug/lib

```

Esto empezara a compilar un monton de cosas para 4 plataformas diferentes, asi que le costara un ratillo. Ahora vamos a ROOT_ANDROID/app/src/main/res y eliminamos la carpeta layout y en values eliminamos todo menos strings y en values-night eliminamos todo.

Vamos a Android Studio y le damos al martillito, o desde la terminal en ROOT_ANDROID ejecutamos ./gradlew :app:assembleDebug. A este proceso le cuesta un monton, asi que calma. Si lo haceis desde Android Studio es posible que tengais que pulsar en la barra de abajo del todo en donde pone Build para poder ver los logs, lo importante es que de un metodo u otro nos salga al final BUILD SUCCESS. Si no ha sido asi repasad todos los pasos.

Para que dejen de aparecer errores en el MainActivity File > Sync project with Gradle Files

> Nota: Si os da un fallo de este estilo 'libengineAndroid.so', missing and no known rule to make it' teneis que volver a ejecutar el comando de ndk-build, o habeis puesto mal la ruta en los OUTS o no habeis linkeado bien todas las librerias.

Finalmente, nuestro APK esta en ROOT_ANDROID/app/build/outputs/apk/debug!

Si la ejecutamos ahora desde Android Studio, fallara, porque aun tenemos que linkar los assets! Vamos con ello. Hacemos click derecho en Android Studio sobre la carpeta app > new > folder > assets folder. Ahora vamos con la terminal a ROOT_ANDROID/app/src/main/assets y generamos un symlink de la carpeta assets de nuestro motor:

```bash
ln -s ../../../../../../assets/
```

Y ahora ya si estaria!

> Nota: MUY IMPORTANTE! Si veis que nuevo codigo que os compila en la version de PC y es obvio que deberia funcionar en la version movil, obligad como sea a que el CMakeLists.txt de ROOT_ANDROID se recargue, yo en particular lo hago escribiendo en el CMakeLists.txt message(), aparece para recargar, da un fallo, lo borro, aparece de nuevo para sync y ya lo carga todo correcto.



### Integracion de Firebase y Admob

Lo primero que tenemos que hacer es transformar nuestro proyecto a AndroidX y para eso vamos en Android Studio a Refactor > Migrate to AndroidX. Aceptáis lo que os diga y hará la migración, que le costara un rato por cierto. Ahora vamos a abrir el build.gradle del proyecto, es decir, el que se encuentra en ROOT_ANDROID. Ahí vamos a modificar algunas partes, de manera que quede así:

```
// Top-level build file where you can add configuration options common to all sub-projects/modules.
ext.engine = [
        versionCode: 1,
        versionName: '1.0',
        buildToolsVersion: '30.0.3',
        compileSdkVersion: 32,
        minSdkVersion: 21,
        targetSdkVersion: 28,
        ndkVersion: '20.1.5948944'
]

buildscript {
    repositories {
        google()
        jcenter()

    }
    dependencies {
        classpath 'com.android.tools.build:gradle:4.2.2'
        classpath 'com.google.gms:google-services:4.3.10'
    }
}

allprojects {
    repositories {
        google() //<-- add
        jcenter()
    }
}

dependencies {

}

task clean(type: Delete) {
    delete rootProject.buildDir
}
```

Como veis hemos modificado el compiledSdkVersion a 32 y hemos metido una dependencia mas. Ahora vamos al build.gradle de la aplicación, el que esta en ROOT_ANDROID/app y lo modificamos por:

```
apply plugin: 'com.android.application'
apply plugin: 'com.google.gms.google-services'


android {
    buildToolsVersion engine.buildToolsVersion
    compileSdkVersion engine.compileSdkVersion
    ndkVersion engine.ndkVersion

    defaultConfig {
        applicationId "com.example.androidTest"

        minSdkVersion engine.minSdkVersion
        targetSdkVersion engine.targetSdkVersion
        versionCode engine.versionCode
        versionName engine.versionName

        externalNativeBuild {
            cmake {
                arguments "-DANDROID_STL=c++_shared", "-DAPP_PLATFORM=android-21 -DFIREBASE_CPP_SDK_DIR=$gradle.firebase_cpp_sdk_dir"
            }
        }

        ndk {
            abiFilters 'armeabi-v7a', 'arm64-v8a', 'x86', 'x86_64'
        }
    }

    buildTypes {
        release {
            signingConfig signingConfigs.debug
            minifyEnabled false
        }
    }

    externalNativeBuild {
        cmake {
            path "../CMakeLists.txt"
        }
    }
}

dependencies {
    implementation project(':sdl')
    implementation platform('com.google.firebase:firebase-bom:29.1.0')
    implementation 'com.google.firebase:firebase-analytics'
    implementation 'com.google.firebase:firebase-auth:21.0.1'
    implementation 'com.google.android.gms:play-services-ads:19.0.0'
}


apply from: "$gradle.firebase_cpp_sdk_dir/Android/firebase_dependencies.gradle"
firebaseCpp.dependencies {
    analytics
    auth
    database
    admob
}
```

> Nota: MUY IMPORTANTE! Android Studio os va a sugerir que 'com.google.android.gms:play-services-ads:19.0.0' use la versión 20 o 21 o algo mayor, NO LO HAGÁIS, hay muchos problemas de compatibilidad con las versiones mayores de 19 o 19.5 y falla durante la ejecución.

El siguiente paso es bajarnos el SDK de Firebase para C++, que podemos encontrarlo [aquí](https://firebase.google.com/download/cpp) y lo descomprimimos en ROOT_ANDROID.

Una vez descargado vamos al archivo gradle.properties y al final del archivo escribimos

```
systemProp.firebase_cpp_sdk.dir=RUTA_ABSOLUTA_A_TU_PROYECTO/targets/android/firebase_cpp_sdk
```

Ahora vamos a modificar el CMakeLists.txt para que incluya el SDK de Firebase. Incluiremos:

```cmake
add_subdirectory(firebase_cpp_sdk)
set(firebase_libs
        firebase_analytics
        firebase_auth
        firebase_database
        firebase_app
        firebase_admob
        )
```

Y en los targets:

```
${firebase_libs}
```

> Nota: En el MainActivity, en la funcion getLibraries donde hemos metido otras veces las librerias que hemos ido adicionando NO ponemos estas, ya que estas librerias son estaticas y no compartidas.

Para cualquier duda hasta aquí, podéis visitar este [enlance](https://firebase.google.com/docs/android/setup).

Vamos al AndroidManifest.xml de ROOT_ANDROID/app y lo modificamos por lo siguiente:

```xml
<manifest xmlns:android="http://schemas.android.com/apk/res/android" package="com.example.android">
    <application android:allowBackup="false" android:icon="@mipmap/ic_launcher" android:label="@string/app_name" android:roundIcon="@mipmap/ic_launcher_round" android:theme="@android:style/Theme.NoTitleBar.Fullscreen" android:hardwareAccelerated="true">
        <activity android:name=".MainActivity" android:configChanges="keyboard|keyboardHidden|orientation|screenSize" android:screenOrientation="portrait"
            android:exported="true">
            <intent-filter>
                <action android:name="android.intent.action.MAIN"/>
                <category android:name="android.intent.category.LAUNCHER"/>
            </intent-filter>
        </activity>
        <meta-data  android:name="com.google.android.gms.ads.APPLICATION_ID"
                    android:value="ca-app-pub-3940256099942544~3347511713"
            />

    </application>
</manifest>
```

Aunque en android:name="com.google.android.gms.ads.APPLICATION_ID" APPLICATION_ID pide a gritos que lo modifiquemos por un valor, no lo hagáis, debe quedarse así.

El valor que tiene  android:value="ca-app-pub-3940256099942544~3347511713" es el que Firebase nos ofrece para usar la aplicación como test. Para poder usar uno real, podéis seguir [aquí](https://console.firebase.google.com/) los pasos, pero recomiendo mucho dejar los de test por el momento, ya que tendréis que crear muchas otras cosas para poder tener anuncios reales.

Una vez registrados en la aplicación, tenéis que crear un proyecto dando a Agregar Proyecto. Le ponéis un nombre y seguis los pasos. Va a llegar a un paso donde os dirá para bajaros un .json, este archivo es extremadamente importante y hay que bajarlo y guardarlo en ROOT_ANDROID/app.

Con esto hemos terminado el Setup para los anuncios, pero no estamos listos para usarlos aun, aunque si que podéis sincronizar con Gradle y  sincronizar de nuevo el CMakeLists.txt y luego compilar la aplicación para ver que todo esta correcto. Si algo os falla podéis seguir la documentación que ofrece Firebase y releer lo descrito aquí.

Vamos ahora con una parte mas interesante que es el código nativo Java/C++. Lo que vamos a hacer en esta sección es crear una función nativa de Java, que vamos a implementar en C++ y luego vamos a llamar desde el código Java, un poco raro si, pero vais a ver que es fácil.

Lo primero vamos a la raíz total del proyecto y vamos a crear una clase llamada NativeAndroid:

```
#ifndef ENGINE2_0_NATIVE_ANDROID_H
#define ENGINE2_0_NATIVE_ANDROID_H

#include "core/Core.h"
#if defined(__ANDROID__)
#include <jni.h>
#include "firebase/admob.h"

namespace engine {
    class NativeAndroid {};
}

#endif

#endif //ENGINE2_0_NATIVE_ANDROID_H

```

Muy importante el defined ya que este código no va a compilar en plataformas Desktop.

Ahora vamos al codigo Java, al MainActivity y escribimos lo siguiente:

```java
package com.example.androidTest;

import android.content.pm.ActivityInfo;
import android.os.Bundle;

import com.google.android.gms.ads.MobileAds;
import org.libsdl.app.SDLActivity;

public class MainActivity extends SDLActivity {
    @Override
    protected String[] getLibraries() {
        return new String[]{
                "SDL2",
                "SDL2_mixer",
                "SDL2_net",
                "SDL2_image",
                "GLESv3",
                "android"
        };
    }

    public native void initFireBaseAds();
}
```

Lo que nos dara un error en rojo en initFireBaseAds. Le damos a la bombilla roja y ejecutamos la acción sugerida, eligiendo el archivo NativeAndroid.cpp que habíamos creado anteriormente. Otra forma es ir directamente a ese archivo y escribir:

```c++
extern "C"
JNIEXPORT void JNICALL
Java_com_example_android_MainActivity_initFireBaseAds(JNIEnv *env, jobject thiz) {
    
}
```

Vale, esto puede ser muy confuso, pero básicamente esta función nos deja manipular objetos de Java desde C++ y luego desde Java podemos invocar la función initFireBaseAds y ejecutara el código de C++. Con ello, vamos lo primero a escribir el código para iniciar Firebase desde C++:

```c++
extern "C"
JNIEXPORT void JNICALL
Java_com_example_android_MainActivity_initFireBaseAdds(JNIEnv *env, jobject thiz) {
    auto* app = firebase::App::Create(firebase::AppOptions(), _androidEngine->env, _androidEngine->context);
    const char* kAdMobAppID = "ca-app-pub-3940256099942544~3347511713";
    firebase::admob::Initialize(*app, kAdMobAppID);
}
```

kAdMobAppID es el mismo valor que hemos puesto en el AndroidManifest.xml y de nuevo, es un valor de testeo. JNIEnv es el Environment y jobject es el contexto o también llamado Activity. Con este código conseguimos inicializar Firebase. Vamos a inicializar un anuncio de tipo Banner:

```cpp
extern "C"
JNIEXPORT void JNICALL
Java_com_example_android_MainActivity_initFireBaseAdds(JNIEnv *env, jobject thiz) {
    // Crear la app para conectar con Firebase
    auto* app = firebase::App::Create(firebase::AppOptions(), _androidEngine->env, _androidEngine->context);
    const char* kAdMobAppID = "ca-app-pub-3940256099942544~3347511713";
    firebase::admob::Initialize(*app, kAdMobAppID);
    
    // Generamos el banner
 	const char* kBannerAdUnit = "ca-app-pub-3940256099942544/6300978111";
    auto* bannerView = new firebase::admob::BannerView();

    // Le damos un tamano al banner
    firebase::admob::AdSize ad_size{};
    ad_size.ad_size_type = firebase::admob::kAdSizeStandard;
    ad_size.width = 320;
    ad_size.height = 50;

    // El request es la informacion que mandamos del usuario a Firebase para que nos de un anuncio a corde al usuario
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
    
    // Inicializamos el anuncion para poder cargarlo y mostrarlo
    bannerView->Initialize(static_cast<firebase::admob::AdParent>(androidEngine->context), kBannerAdUnit, ad_size);
    
    // Movemos el anuncio a la parte inferior centrado
    bannerView->MoveTo(firebase::admob::BannerView::kPositionBottom);
    
    // Cargamos el anuncion con los datos del request y solo mostramos el anuncio despues de la carga (es asincrona)
    // si lo intentamos mostrar antes de que se haya cargado puede dar fallos
    bannerView->LoadAd(createRequest()).OnCompletion([](const firebase::Future<void>& future, void* user_data) {
        bannerVire->Show();
    }, bannerView);   
}
```

El código es largo, pero esta comentado como funciona. Una vez tenemos esta función implementada, de nada nos sirve si no la llamamos desde nuestro código Java, así que vamos a ello:

```java
package com.example.androidTest;

import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.view.WindowManager;

import com.google.android.gms.ads.MobileAds;
import org.libsdl.app.SDLActivity;

public class MainActivity extends SDLActivity {
    @Override
    protected String[] getLibraries() {
        return new String[]{
                "SDL2",
                "SDL2_mixer",
                "SDL2_net",
                "SDL2_image",
                "GLESv3",
                "android"
        };
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        initFireBaseAds();
    }

    public native void initFireBaseAds();
}
```

Esto lo que hara es cargar y mostrar el anuncio cuando la aplicacion se cree, por lo que en unos pocos segundos tendremos un banner asi:

![Screenshot from 2022-02-21 12-31-50](/home/borja/Pictures/Screenshot from 2022-02-21 12-31-50.png)

Y con esto ya estaría. Para los anuncios de tipo interstitial es muy similar, os dejo aquí unos enlaces que os explica todo lo que yo os he explicado por aquí:

1. https://firebase.google.com/docs/cpp/setup?platform=android
2. https://firebase.google.com/docs/admob/android/quick-start
3. https://firebase.google.com/docs/admob/cpp/quick-start

Miradlos en ese orden, que es el mismo que he explicado yo.

Para darse de alta en las diferentes plataformas:

- Firebase: https://console.firebase.google.com/
- Admob: https://admob.google.com/home/?utm_source=firebase&utm_medium=et&utm_campaign=firebase-docs&utm_content=2017Q1

> Nota: para tener anuncios reales que den dinero, es necesario que la aplicación este en alguna tienda y este aprobada, para mas información sobre ello tenéis este [enlace](https://support.google.com/admob/answer/9989980?visit_id=637810403376629893-2495017555&rd=1).
