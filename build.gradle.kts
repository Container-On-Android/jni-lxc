@file:Suppress("UnstableApiUsage")
plugins {
    id("com.android.library")
}

android {
    namespace = "io.github.coap"
    compileSdk = 35

    defaultConfig {
        minSdk = 30

        ndk {
            ndkVersion = "28.2.13676358"
            abiFilters.add("arm64-v8a")
        }

        vectorDrawables {
            useSupportLibrary = true
        }

        /*externalNativeBuild {
            cmake {
                arguments("-DANDROID_STL=c++_shared")
            }
        }*/
    }

    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_21
        targetCompatibility = JavaVersion.VERSION_21
    }

    buildTypes {
        release {
            isMinifyEnabled = true
            proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
        }
    }

    lint {
        disable += "JniMissingFunction"
    }

    buildFeatures {
        aidl = true
        viewBinding = true
        buildConfig = true
    }

    packaging {
        jniLibs {
            useLegacyPackaging = false
        }
    }
}

dependencies {
    implementation("com.github.topjohnwu.libsu:service:6.0.0")
    implementation("com.github.topjohnwu.libsu:core:6.0.0")
}