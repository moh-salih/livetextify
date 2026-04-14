package com.salih.livetextify;


import android.app.Activity;
import android.content.ContentResolver;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.provider.MediaStore;
import android.provider.Settings;
import android.util.Log;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import java.util.ArrayList;
import java.util.List;

public class PermissionManager {
    private static final int STORAGE_PERMISSION_CODE = 100;
    private static final int MICROPHONE_PERMISSION_CODE = 101;
    private static final String TAG = "PermissionManager";

    /**
     * Requests storage permission based on Android version.
     */
    public static void requestStoragePermission(Activity activity) {
        if (activity == null) return;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            try {
                Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
                Uri uri = Uri.fromParts("package", activity.getPackageName(), null);
                intent.setData(uri);
                activity.startActivityForResult(intent, STORAGE_PERMISSION_CODE);
            } catch (Exception e) {
                Intent intent = new Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION);
                activity.startActivityForResult(intent, STORAGE_PERMISSION_CODE);
            }
        } else {
            if (!isStoragePermissionGranted(activity)) {
                ActivityCompat.requestPermissions(
                    activity,
                    new String[]{
                        android.Manifest.permission.READ_EXTERNAL_STORAGE,
                        android.Manifest.permission.WRITE_EXTERNAL_STORAGE
                    },
                    STORAGE_PERMISSION_CODE
                );
            }
        }
    }

    /**
     * Requests microphone permission.
     */
    public static void requestMicrophonePermission(Activity activity) {
        if (activity == null) return;

        if (!isMicrophonePermissionGranted(activity)) {
            ActivityCompat.requestPermissions(
                activity,
                new String[]{
                    android.Manifest.permission.RECORD_AUDIO
                },
                MICROPHONE_PERMISSION_CODE
            );
        }
    }

    /**
     * Requests both storage and microphone permissions at once.
     */
    public static void requestAllPermissions(Activity activity) {
        if (activity == null) return;

        List<String> permissionsToRequest = new ArrayList<>();

        // Check and add storage permissions if needed
        if (!isStoragePermissionGranted(activity)) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                // For Android 11+, we need to request all files access separately
                requestStoragePermission(activity);
            } else {
                permissionsToRequest.add(android.Manifest.permission.READ_EXTERNAL_STORAGE);
                permissionsToRequest.add(android.Manifest.permission.WRITE_EXTERNAL_STORAGE);
            }
        }

        // Check and add microphone permission if needed
        if (!isMicrophonePermissionGranted(activity)) {
            permissionsToRequest.add(android.Manifest.permission.RECORD_AUDIO);
        }

        // Request permissions if there are any to request
        if (!permissionsToRequest.isEmpty()) {
            ActivityCompat.requestPermissions(
                activity,
                permissionsToRequest.toArray(new String[0]),
                MICROPHONE_PERMISSION_CODE // Using microphone code as the main one
            );
        }
    }

    /**
     * Checks if storage permission is granted.
     */
    public static boolean isStoragePermissionGranted(Activity activity) {
        if (activity == null) return false;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            return Environment.isExternalStorageManager();
        } else {
            return ContextCompat.checkSelfPermission(activity, android.Manifest.permission.READ_EXTERNAL_STORAGE)
                    == PackageManager.PERMISSION_GRANTED &&
                   ContextCompat.checkSelfPermission(activity, android.Manifest.permission.WRITE_EXTERNAL_STORAGE)
                    == PackageManager.PERMISSION_GRANTED;
        }
    }

    /**
     * Checks if microphone permission is granted.
     */
    public static boolean isMicrophonePermissionGranted(Activity activity) {
        if (activity == null) return false;

        return ContextCompat.checkSelfPermission(activity, android.Manifest.permission.RECORD_AUDIO)
                == PackageManager.PERMISSION_GRANTED;
    }

    /**
     * Checks if all required permissions are granted.
     */
    public static boolean areAllPermissionsGranted(Activity activity) {
        return isStoragePermissionGranted(activity) && isMicrophonePermissionGranted(activity);
    }

    /**
     * Handles the result of permission requests.
     */
    public static void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        switch (requestCode) {
            case STORAGE_PERMISSION_CODE:
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    Log.d(TAG, "Storage permission granted");
                } else {
                    Log.w(TAG, "Storage permission denied");
                }
                break;

            case MICROPHONE_PERMISSION_CODE:
                boolean allMicrophoneGranted = true;
                for (int result : grantResults) {
                    if (result != PackageManager.PERMISSION_GRANTED) {
                        allMicrophoneGranted = false;
                        break;
                    }
                }
                if (allMicrophoneGranted) {
                    Log.d(TAG, "Microphone permission granted");
                } else {
                    Log.w(TAG, "Microphone permission denied");
                }
                break;
        }
    }

    /**
     * Handles activity result for Android 11+ storage permission.
     */
    public static void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == STORAGE_PERMISSION_CODE) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                if (Environment.isExternalStorageManager()) {
                    Log.d(TAG, "Storage management permission granted");
                } else {
                    Log.w(TAG, "Storage management permission denied");
                }
            }
        }
    }
}
