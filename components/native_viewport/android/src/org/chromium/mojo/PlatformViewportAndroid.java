// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.mojo;

import android.app.Activity;
import android.content.Context;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import org.chromium.base.CalledByNative;
import org.chromium.base.JNINamespace;

/**
 * Exposes SurfaceView to native code.
 */
@JNINamespace("native_viewport")
public class PlatformViewportAndroid extends SurfaceView {

    private long mNativeMojoViewport;
    private final SurfaceHolder.Callback mSurfaceCallback;

    @CalledByNative
    public static PlatformViewportAndroid createForActivity(
            Activity activity, long nativeViewport) {
        PlatformViewportAndroid rv = new PlatformViewportAndroid(activity, nativeViewport);
        activity.setContentView(rv);
        return rv;
    }

    public PlatformViewportAndroid(Context context, long nativeViewport) {
        super(context);

        setFocusable(true);
        setFocusableInTouchMode(true);

        mNativeMojoViewport = nativeViewport;
        assert mNativeMojoViewport != 0;

        final float density = context.getResources().getDisplayMetrics().density;

        mSurfaceCallback = new SurfaceHolder.Callback() {
            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                assert mNativeMojoViewport != 0;
                nativeSurfaceSetSize(mNativeMojoViewport, width, height, density);
            }

            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                assert mNativeMojoViewport != 0;
                nativeSurfaceCreated(mNativeMojoViewport, holder.getSurface(), density);
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                assert mNativeMojoViewport != 0;
                nativeSurfaceDestroyed(mNativeMojoViewport);
            }
        };
        getHolder().addCallback(mSurfaceCallback);

    }

    @CalledByNative
    public void detach() {
        getHolder().removeCallback(mSurfaceCallback);
        mNativeMojoViewport = 0;
    }

    @Override
    protected void onWindowVisibilityChanged(int visibility) {
        super.onWindowVisibilityChanged(visibility);
        if (visibility == View.VISIBLE) {
            requestFocusFromTouch();
            requestFocus();
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        final int actionMasked = event.getActionMasked();
        if (actionMasked == MotionEvent.ACTION_POINTER_DOWN
                || actionMasked == MotionEvent.ACTION_POINTER_UP) {
            // Up/down events identify a single point.
            return notifyTouchEventAtIndex(event, event.getActionIndex());
        }
        assert event.getPointerCount() != 0;
        // All other types can have more than one point.
        boolean result = false;
        for (int i = 0, count = event.getPointerCount(); i < count; i++) {
            final boolean sub_result = notifyTouchEventAtIndex(event, i);
            result |= sub_result;
        }
        return result;
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        if (privateDispatchKeyEvent(event)) {
            return true;
        }
        return super.dispatchKeyEvent(event);
    }

    @Override
    public boolean dispatchKeyEventPreIme(KeyEvent event) {
        if (privateDispatchKeyEvent(event)) {
            return true;
        }
        return super.dispatchKeyEventPreIme(event);
    }

    @Override
    public boolean dispatchKeyShortcutEvent(KeyEvent event) {
        if (privateDispatchKeyEvent(event)) {
            return true;
        }
        return super.dispatchKeyShortcutEvent(event);
    }

    private boolean notifyTouchEventAtIndex(MotionEvent event, int index) {
        return nativeTouchEvent(mNativeMojoViewport, event.getEventTime(), event.getActionMasked(),
                event.getPointerId(index), event.getX(index), event.getY(index),
                event.getPressure(index), event.getTouchMajor(index), event.getTouchMinor(index),
                event.getOrientation(index), event.getAxisValue(MotionEvent.AXIS_HSCROLL, index),
                event.getAxisValue(MotionEvent.AXIS_VSCROLL, index));
    }

    private boolean privateDispatchKeyEvent(KeyEvent event) {
        if (event.getAction() == KeyEvent.ACTION_MULTIPLE) {
            boolean result = false;
            if (event.getKeyCode() == KeyEvent.KEYCODE_UNKNOWN && event.getCharacters() != null) {
                String characters = event.getCharacters();
                for (int i = 0; i < characters.length(); ++i) {
                    char c = characters.charAt(i);
                    int codepoint = c;
                    if (codepoint >= Character.MIN_SURROGATE
                            && codepoint < (Character.MAX_SURROGATE + 1)) {
                        i++;
                        char c2 = characters.charAt(i);
                        codepoint = Character.toCodePoint(c, c2);
                    }
                    result |= nativeKeyEvent(mNativeMojoViewport, true, 0, codepoint);
                    result |= nativeKeyEvent(mNativeMojoViewport, false, 0, codepoint);
                }
            } else {
                for (int i = 0; i < event.getRepeatCount(); ++i) {
                    result |= nativeKeyEvent(
                            mNativeMojoViewport, true, event.getKeyCode(), event.getUnicodeChar());
                    result |= nativeKeyEvent(
                            mNativeMojoViewport, false, event.getKeyCode(), event.getUnicodeChar());
                }
            }
            return result;
        } else {
            return nativeKeyEvent(mNativeMojoViewport, event.getAction() == KeyEvent.ACTION_DOWN,
                    event.getKeyCode(), event.getUnicodeChar());
        }
    }

    private static native void nativeDestroy(long nativePlatformViewportAndroid);

    private static native void nativeSurfaceCreated(
            long nativePlatformViewportAndroid, Surface surface, float devicePixelRatio);

    private static native void nativeSurfaceDestroyed(
            long nativePlatformViewportAndroid);

    private static native void nativeSurfaceSetSize(
            long nativePlatformViewportAndroid, int width, int height, float density);

    private static native boolean nativeTouchEvent(long nativePlatformViewportAndroid, long timeMs,
            int maskedAction, int pointerId, float x, float y, float pressure, float touchMajor,
            float touchMinor, float orientation, float hWheel, float vWheel);

    private static native boolean nativeKeyEvent(
            long nativePlatformViewportAndroid, boolean pressed, int keyCode, int unicodeCharacter);
}
