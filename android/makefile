ANDROID_HOME:=
TARGET:=android-16
ANDROID_SDK:=$(ANDROID_HOME)/platforms/$(TARGET)
APK_BUILDER = java -classpath $(ANDROID_HOME)/tools/lib/sdklib.jar com.android.sdklib.build.ApkBuilderMain
TARGET_NAME:=dream
PACKAGE_NAME:=com.changer.dream
PACKAGE_PATH=$(subst .,/,$(PACKAGE_NAME))

all:clean check $(TARGET_NAME).apk

$(TARGET_NAME).apk:$(TARGET_NAME)_unaligned.apk
	zipalign -v 4 $< $@

check:
ifeq ($(ANDROID_HOME),)
	$(error YOU SHOULD USE ANDROID_HOME=XXX)
endif

$(TARGET_NAME)_unaligned.apk:unsigned_$(TARGET_NAME).apk
	echo dingjincheng|jarsigner -verbose -keystore android.keystore -signedjar $@ bin/$< android
	
unsigned_$(TARGET_NAME).apk:bin/classes.dex bin/resources.ap_
	$(APK_BUILDER)  bin/$@ -v -u -z bin/resources.ap_  -f bin/classes.dex  -rf src/ -rj libs/ -nf libs/ 
	
gen/$(PACKAGE_PATH)/R.java:
	mkdir gen
	aapt package -f -m -J gen -S res -I $(ANDROID_SDK)/android.jar -M AndroidManifest.xml
	
bin/classes.dex:gen/$(PACKAGE_PATH)/R.java
	mkdir bin
	javac -encoding UTF-8 -target 1.6 -bootclasspath $(ANDROID_SDK)/android.jar -d bin src/$(PACKAGE_PATH)/*.java $<
	cmd /C "dx.bat --dex --output=bin\classes.dex bin\ "

bin/resources.ap_:
	aapt package -f -S res -M AndroidManifest.xml -A assets -I $(ANDROID_SDK)/android.jar -F $@
	
install:$(TARGET_NAME).apk
	adb install $(TARGET_NAME).apk

clean:
	-rm -rf gen
	-rm -rf bin
	-rm $(TARGET_NAME)_unaligned.apk $(TARGET_NAME).apk