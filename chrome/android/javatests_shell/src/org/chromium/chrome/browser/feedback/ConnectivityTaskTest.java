// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.feedback;

import android.test.suitebuilder.annotation.MediumTest;
import android.test.suitebuilder.annotation.SmallTest;

import org.chromium.base.ThreadUtils;
import org.chromium.base.test.util.Feature;
import org.chromium.chrome.browser.feedback.ConnectivityTask.FeedbackData;
import org.chromium.chrome.browser.feedback.ConnectivityTask.Type;
import org.chromium.chrome.browser.profiles.Profile;
import org.chromium.content.browser.test.util.Criteria;
import org.chromium.content.browser.test.util.CriteriaHelper;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.atomic.AtomicReference;

/**
 * Tests for {@link ConnectivityTask}.
 */
public class ConnectivityTaskTest extends ConnectivityCheckerTestBase {
    private static final int RESULT_CHECK_INTERVAL_MS = 10;

    @MediumTest
    @Feature({"Feedback"})
    public void testNormalCaseShouldWork() throws InterruptedException {
        final AtomicReference<ConnectivityTask> task = new AtomicReference<>();
        ThreadUtils.runOnUiThreadBlocking(new Runnable() {
            @Override
            public void run() {
                // Intentionally make HTTPS-connection fail which should result in NOT_CONNECTED.
                ConnectivityChecker.overrideUrlsForTest(GENERATE_204_URL, GENERATE_404_URL);

                task.set(ConnectivityTask.create(Profile.getLastUsedProfile(), TIMEOUT_MS));
            }
        });

        boolean gotResult = CriteriaHelper.pollForUIThreadCriteria(new Criteria() {
            @Override
            public boolean isSatisfied() {
                return task.get().isDone();
            }
        }, TIMEOUT_MS, RESULT_CHECK_INTERVAL_MS);
        assertTrue("Should be finished by now.", gotResult);
        FeedbackData feedback = getResult(task);
        verifyConnections(feedback, ConnectivityCheckResult.NOT_CONNECTED);
        assertEquals("The timeout value is wrong.", TIMEOUT_MS, feedback.getTimeoutMs());
    }

    private static void verifyConnections(FeedbackData feedback, int expectedHttpsValue) {
        Map<Type, Integer> results = feedback.getConnections();
        assertEquals("Should have 4 results.", 4, results.size());
        for (Map.Entry<Type, Integer> result : results.entrySet()) {
            switch (result.getKey()) {
                case CHROME_HTTP:
                case SYSTEM_HTTP:
                    assertResult(ConnectivityCheckResult.CONNECTED, result);
                    break;
                case CHROME_HTTPS:
                case SYSTEM_HTTPS:
                    assertResult(expectedHttpsValue, result);
                    break;
                default:
                    fail("Failed to recognize type " + result.getKey());
            }
        }
        assertTrue("The elapsed time should be non-negative.", feedback.getElapsedTimeMs() >= 0);
    }

    private static void assertResult(int expectedValue, Map.Entry<Type, Integer> actualEntry) {
        assertEquals("Wrong result for " + actualEntry.getKey(),
                ConnectivityTask.getHumanReadableString(expectedValue),
                ConnectivityTask.getHumanReadableString(actualEntry.getValue()));
    }

    @MediumTest
    @Feature({"Feedback"})
    public void testTwoTimeoutsShouldFillInTheRest() throws InterruptedException {
        final AtomicReference<ConnectivityTask> task = new AtomicReference<>();
        ThreadUtils.runOnUiThreadBlocking(new Runnable() {
            @Override
            public void run() {
                // Intentionally make HTTPS connections slow which should result in UNKNOWN.
                ConnectivityChecker.overrideUrlsForTest(GENERATE_204_URL, GENERATE_204_SLOW_URL);

                task.set(ConnectivityTask.create(Profile.getLastUsedProfile(), TIMEOUT_MS));
            }
        });

        boolean gotResult = CriteriaHelper.pollForUIThreadCriteria(new Criteria() {
            @Override
            public boolean isSatisfied() {
                return task.get().isDone();
            }
        }, TIMEOUT_MS / 5, RESULT_CHECK_INTERVAL_MS);
        assertFalse("Should not be finished by now.", gotResult);
        FeedbackData feedback = getResult(task);
        verifyConnections(feedback, ConnectivityCheckResult.UNKNOWN);
        assertEquals("The timeout value is wrong.", TIMEOUT_MS, feedback.getTimeoutMs());
    }

    @SmallTest
    @Feature({"Feedback"})
    public void testFeedbackDataConversion() {
        Map<Type, Integer> connectionMap = new HashMap<>();
        connectionMap.put(Type.CHROME_HTTP, ConnectivityCheckResult.NOT_CONNECTED);
        connectionMap.put(Type.CHROME_HTTPS, ConnectivityCheckResult.CONNECTED);
        connectionMap.put(Type.SYSTEM_HTTP, ConnectivityCheckResult.UNKNOWN);
        connectionMap.put(Type.SYSTEM_HTTPS, ConnectivityCheckResult.CONNECTED);

        FeedbackData feedback = new FeedbackData(connectionMap, 42, 21);
        Map<String, String> map = feedback.toMap();

        assertEquals("Should have 6 entries.", 6, map.size());
        assertTrue(map.containsKey(ConnectivityTask.CHROME_HTTP_KEY));
        assertEquals("NOT_CONNECTED", map.get(ConnectivityTask.CHROME_HTTP_KEY));
        assertTrue(map.containsKey(ConnectivityTask.CHROME_HTTPS_KEY));
        assertEquals("CONNECTED", map.get(ConnectivityTask.CHROME_HTTPS_KEY));
        assertTrue(map.containsKey(ConnectivityTask.SYSTEM_HTTP_KEY));
        assertEquals("UNKNOWN", map.get(ConnectivityTask.SYSTEM_HTTP_KEY));
        assertTrue(map.containsKey(ConnectivityTask.SYSTEM_HTTPS_KEY));
        assertEquals("CONNECTED", map.get(ConnectivityTask.SYSTEM_HTTPS_KEY));
        assertTrue(map.containsKey(ConnectivityTask.CONNECTION_CHECK_TIMEOUT_MS));
        assertEquals("42", map.get(ConnectivityTask.CONNECTION_CHECK_TIMEOUT_MS));
        assertTrue(map.containsKey(ConnectivityTask.CONNECTION_CHECK_ELAPSED_MS));
        assertEquals("21", map.get(ConnectivityTask.CONNECTION_CHECK_ELAPSED_MS));
    }

    private static FeedbackData getResult(final AtomicReference<ConnectivityTask> task) {
        final AtomicReference<FeedbackData> result = new AtomicReference<>();
        ThreadUtils.runOnUiThreadBlocking(new Runnable() {
            @Override
            public void run() {
                result.set(task.get().get());
            }
        });
        return result.get();
    }
}
