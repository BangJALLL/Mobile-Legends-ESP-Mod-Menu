# Mobile Legends ESP Mod Menu

This src can build by Aide CMODS (help me to fix this src for Android Studio if can't to build it)

Preview:
![](https://i.imgur.com/zeumkBG.gif](https://i.ibb.co/26jWQPg/photo1675918266.jpg)

# Implementing the menu to the target game

### 1. Know your game's main activity

Now we are looking for main activity, there are 2 ways to do

1. Decompile the game's APK file. Open `AndroidManifest.xml` and search after `<action android:name="android.intent.action.MAIN"/>`.

Example the game's main activity was `com.unity3d.player.UnityPlayerActivity`

![](https://i.imgur.com/FfOtc1K.png)

Be sure to enable Word wrap so it is easier to read

![](https://i.imgur.com/7DzU8d0.png)

2. APK Easy Tool since it can read out location of main activity without decompiling APK

![](https://i.imgur.com/JQdPjyZ.png)

Note it somewhere to remember it

### 2. Making corresponding changes in the files

Decompile the game APK

Open the game's `AndroidManifest.xml`

Add the `SYSTEM_ALERT_WINDOW` permission besides other permissions if it doesn't exist. Doesn't matter where you place it as long as it's above the application tag
```xml
<uses-permission android:name="android.permission.SYSTEM_ALERT_WINDOW"/>
```

If you don't add it, you can't allow overlay permission.

![](https://i.imgur.com/XOxLU91.png)

Add the service above the end of application tag (change the package name of your menu if you had changed it)
```xml
<service android:name="uk.lgl.modmenu.FloatingModMenuService" android:enabled="true" android:exported="false" android;process=":UnityKillsMe" android:stopWithTask="true"/>
```

![](https://i.imgur.com/rw0hawa.png)

Now we need to call your mod menu activity

There are 2 ways to call your mod menu activity. Choose one of them you like to try. Don't know? just choose METHOD 1

**METHOD 1**

This simple way, we will call to `MainActivity.java`. `MainActivity.java` will never be used

Locate to the game's path of main activity and open the **smali** file. If the game have multi dexes, it may be located in smali_classes2.. please check all

With the path of the target game’s main activity which we determined earlier `com.unity3d.player.UnityPlayerActivity`. Think of it like a path `/com/unity3d/player/UnityPlayerActivity.smali`

Open the main acitivity's smali file, search for OnCreate method and paste this code inside (change the package name if you had changed it)
```
invoke-static {p0}, Luk/lgl/MainActivity;->Start(Landroid/content/Context;)V
```
 
![](https://i.imgur.com/7CxTCl8.png)

Save the file

**METHOD 2**

You can follow this it if the first method really fails, or if you really want to use `MainActivity.java` for a reason. Since this involve changing activites, it may cause some problems.

On your `MainActivity.java`, put the game's main activity to `public String GameActivity`

![](https://i.imgur.com/jdacwvH.png)

Uncomment this code

```
Toast.makeText(MainActivity.this, "Error. Game's main activity does not exist", Toast.LENGTH_LONG).show();
```

On `AndroidManifest.xml`, remove `<action android:name="android.intent.action.MAIN"/>` from the game's activity, like this:

![](https://i.imgur.com/z1RxPjc.png)

If you don't remove `<action android:name="android.intent.action.MAIN"/>` from the game's activity, your menu will not launch. `AndroidManifest.xml` can ONLY contain one `<action android:name="android.intent.action.MAIN"/>`

near the end of application tag `</application>`, add your main activity above it. `uk.lgl.MainActivity` is your main activity

```xml
<activity android:configChanges="keyboardHidden|orientation|screenSize" android:name="uk.lgl.MainActivity">
     <intent-filter>
         <action android:name="android.intent.action.MAIN"/>
         <category android:name="android.intent.category.LAUNCHER"/>
     </intent-filter>
</activity>
```

![](https://i.imgur.com/33lMPhc.png)

Save the file

_Do NOT use both methods at the same time_

### 3. Building your project and copying files

Build the project to the APK file.
**Build** -> **Build Bundle(s)/APK(s)** -> **Build APK(s)**

If no errors occured, you did everything right and build will succeded. You will be notified that it build successfully

![](https://i.imgur.com/WpSKV1L.png)

Click on **locate** to show you the location of **build.apk**. It is stored at `(your-project)\app\build\outputs\apk\app-debug.apk`

![](https://i.imgur.com/wBTPSLi.png)

Decompile your **app-debug.apk**.

Copy your mod menu from decompiled app-debug.apk smali to the game's smali folder. Example ours is uk.lgl.modmenu, we copy the `uk` folder from **app-debug** `(app-debug\smali\uk)` to the game's decompiled directory `(game name)\smali`

![](https://i.imgur.com/aO6eEab.png)
 
If the game have multidexes, just add your smali to the last `smali_classes` if possible to prevent compilation errors such as `Unsigned short value out of range: xxxxx` (Smali limit error)

Copy the library file (.so) from **app-debug.apk** to the target game. Watch out the folder names, we don't want to mix them up, so make sure to copy our lib file to the correct architecture. Like our armeabi-v7a lib goes to the target games armeabi-v7a folder, arm64-v8a lib goes to the arm64-v8a folder...

PUTTING THE LIB FILE ON A WRONG ARCHITECTURE LIKE PUTTING ARM64 LIB TO ARMV7 WILL RESULT A CRASH!
 
![](https://i.imgur.com/oZq1Wq7.png)
 
### 4. Compiling game apk
 
Now compile and sign the apk, and install it on your device

Congrats. You have successfully implemented a mod menu.

# Credits
* LGLTeam: https://github.com/LGLTeam/Android-Mod-Menu-animation
