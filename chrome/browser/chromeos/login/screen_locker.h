// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CHROMEOS_LOGIN_SCREEN_LOCKER_H_
#define CHROME_BROWSER_CHROMEOS_LOGIN_SCREEN_LOCKER_H_

#include <string>

#include "base/task.h"
#include "chrome/browser/chromeos/login/login_status_consumer.h"
#include "chrome/browser/chromeos/login/user_manager.h"
#include "chrome/browser/views/info_bubble.h"

namespace gfx {
class Rect;
}  // namespace gfx

namespace views {
class WidgetGtk;
}  // namespace views

namespace chromeos {

class Authenticator;
class MessageBubble;
class MouseEventRelay;
class ScreenLockView;

namespace test {
class ScreenLockerTester;
}  // namespace test

// ScreenLocker creates a background view as well as ScreenLockView to
// authenticate the user. ScreenLocker manages its life cycle and will
// delete itself when it's unlocked.
class ScreenLocker : public LoginStatusConsumer,
                     public InfoBubbleDelegate {
 public:
  explicit ScreenLocker(const UserManager::User& user);

  // Initialize and show the screen locker with given |bounds|.
  void Init(const gfx::Rect& bounds);

  // LoginStatusConsumer implements:
  virtual void OnLoginFailure(const std::string& error);
  virtual void OnLoginSuccess(const std::string& username,
                              const std::string& credentials);

  // Overridden from views::InfoBubbleDelegate.
  virtual void InfoBubbleClosing(InfoBubble* info_bubble,
                                 bool closed_by_escape);
  virtual bool CloseOnEscape() { return true; }
  virtual bool FadeInOnShow() { return false; }

  // Authenticates the user with given |password| and authenticator.
  void Authenticate(const string16& password);

  // Close message bubble to clear error messages.
  void ClearErrors();

  // (Re)enable input field.
  void EnableInput();

  // Exit the chrome, which will sign out the current session.
  void Signout();

  // Called when the screen locker is ready.
  void ScreenLockReady();

  // Returns the user to authenticate.
  const UserManager::User& user() const {
    return user_;
  }

  // Initialize ScreenLocker class. It will listen to
  // LOGIN_USER_CHANGED notification so that the screen locker accepts
  // lock event only after a user is logged in.
  static void InitClass();

  // Show the screen locker.
  static void Show();

  // Hide the screen locker.
  static void Hide();

  // Notifies that PowerManager rejected UnlockScreen request.
  static void UnlockScreenFailed();

  // Returns the tester
  static test::ScreenLockerTester* GetTester();

 private:
  friend class DeleteTask<ScreenLocker>;
  friend class test::ScreenLockerTester;

  virtual ~ScreenLocker();

  // Sets the authenticator.
  void SetAuthenticator(Authenticator* authenticator);

  // The screen locker window.
  views::WidgetGtk* lock_window_;

  // TYPE_CHILD widget to grab the keyboard/mouse input.
  views::WidgetGtk* lock_widget_;

  // A view that accepts password.
  ScreenLockView* screen_lock_view_;

  // Logged in user.
  UserManager::User user_;

  // Used to authenticate the user to unlock.
  scoped_refptr<Authenticator> authenticator_;

  // ScreenLocker grabs all keyboard and mouse events on its
  // gdk window and never let other gdk_window to handle inputs.
  // This MouseEventRelay object is used to forward events to
  // the message bubble's gdk_window so that close button works.
  scoped_ptr<MouseEventRelay> mouse_event_relay_;

  // An info bubble to display login failure message.
  MessageBubble* error_info_;

  // Reference to the single instance of the screen locker object.
  // This is used to make sure there is only one screen locker instance.
  static ScreenLocker* screen_locker_;

  DISALLOW_COPY_AND_ASSIGN(ScreenLocker);
};

}  // namespace chromeos

#endif  // CHROME_BROWSER_CHROMEOS_LOGIN_SCREEN_LOCKER_H_
