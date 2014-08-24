package com.changer.dream;

import java.io.*;
import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;
import android.os.Environment;


/** Utils functions. */
public class Utils {


    /** open file */
    public static FileInputStream tryToGetLocalFile(Context ctx, String filename) {
        try {
            FileInputStream fin = ctx.openFileInput(filename);
            return fin;
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            return null;
        }
    }

    /** svg image on sdcard */
    public static void unpackOnSdCard(AssetManager assetManager) throws IOException {
        if (Environment.getExternalStorageState().compareTo(Environment.MEDIA_MOUNTED)==0) {
            File sdcard = Environment.getExternalStorageDirectory();
            String DreamPath = sdcard.getAbsoluteFile() + "/dream/";
            File DreamDir = new File(DreamPath);
            if (DreamDir.exists() && !DreamDir.isDirectory()) {
                throw new IOException("Dream exists and is not a directory on SD Card");
            } else if (!DreamDir.exists()) {
                DreamDir.mkdirs();
            }
            // Note: /sdcard/Dream dir exists
            String[] filenames = assetManager.list("data");
            for(String filename:filenames) {
                InputStream inputStream = assetManager.open("data/" + filename);
                OutputStream outputStream = new FileOutputStream(DreamPath + "/" + filename);
                // copy
                byte[] buffer = new byte[4096];
                int length;
                while ( (length = inputStream.read(buffer)) > 0 ) {
                    outputStream.write(buffer, 0, length);
                }
                outputStream.flush();
                outputStream.close();
                inputStream.close();
            }           
        } else {
            throw new IOException("SD Card not available");
        }

    }

    public static boolean isSdCardAvailable() {
        if (Environment.getExternalStorageState().compareTo(Environment.MEDIA_MOUNTED)==0) {
            return true;
        } else {
            return false;
        }
    }


    /** compute md5 */
    public static String getMd5Hash(String input) {
        try {
            MessageDigest md = MessageDigest.getInstance("MD5");
            byte[] messageDigest = md.digest(input.getBytes());
            BigInteger number = new BigInteger(1, messageDigest);
            String md5 = number.toString(16);
            while (md5.length() < 32) {
                md5 = "0" + md5;
            }
            return md5;
        } catch (NoSuchAlgorithmException e) {
            Log.e("MD5", e.getMessage());
            return null;
        }
    }

    /** read binary input stream */
    private static byte[] readBinaryInput(InputStream in) throws IOException {
        ByteArrayOutputStream bo = new ByteArrayOutputStream();
        byte[] tmpStorage = new byte[4096];
        int pos = 0;
        for (int n; (n = in.read(tmpStorage)) != -1;) {
            bo.write(tmpStorage, 0, n);
            pos += n;
        }
        return bo.toByteArray();
    }

    /** read string input stream */
    private static String readStringInput(InputStream in) throws IOException {
        StringBuilder sb = new StringBuilder();

        byte[] buffer = new byte[4096];
        for (int n; (n = in.read(buffer)) != -1;) {
            sb.append(new String(buffer, 0, n));
        }
        return sb.toString();
    }

}
