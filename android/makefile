ANDROID_SDK:=$(ANDROID_HOME)/platforms/android-16
APK_BUILDER = java -classpath $(ANDROID_HOME)/tools/lib/sdklib.jar com.android.sdklib.build.ApkBuilderMain

dream.apk:dream_unaligned.apk
	zipalign -v 4 $< $@
	
dream_unaligned.apk:unsigned_dream.apk
	echo dingjincheng|jarsigner -verbose -keystore android.keystore -signedjar $@ bin/$< android
	
unsigned_dream.apk:bin resources.ap_
	$(APK_BUILDER)  bin/$@ -v -u -z bin/resources.ap_  -f bin/classes.dex  -rf src/ -rj libs/ -nf libs/ 
	
gen/com/changer/dream/R.java:
	mkdir gen
	aapt package -f -m -J gen -S res -I $(ANDROID_SDK)/android.jar -M AndroidManifest.xml
	
bin:gen/com/changer/dream/R.java
	javac -encoding UTF-8 -target 1.6 -bootclasspath $(ANDROID_SDK)/android.jar -d bin src/com/changer/dream/*.java $<
	cmd /C "dx.bat --dex --output=bin\classes.dex bin\ "

resources.ap_:
	aapt package -f -S res -M AndroidManifest.xml -A assets -I $(ANDROID_SDK)/android.jar -F bin/$@
	
install:dream.apk
	adb install dream.apk

clean:
	-rm -rf gen
	-rm -rf bin
	-rm dream_unaligned.apk