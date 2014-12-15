// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_DETACHABLE_TOOLBAR_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_DETACHABLE_TOOLBAR_VIEW_H_

#include "chrome/browser/ui/host_desktop.h"
#include "ui/views/accessible_pane_view.h"

// DetachableToolbarView contains functionality common to views that can detach
// from the Chrome frame, such as the BookmarkBarView and the Extension shelf.
class DetachableToolbarView : public views::AccessiblePaneView {
 public:
  DetachableToolbarView() {}
  ~DetachableToolbarView() override {}

  // Whether the view is currently detached from the Chrome frame.
  virtual bool IsDetached() const = 0;

  // Gets the current state of the resize animation (show/hide).
  virtual double GetAnimationValue() const = 0;

  // Gets the current amount of overlap atop the browser toolbar.
  virtual int GetToolbarOverlap() const = 0;

  // Paints the background (including the theme image behind content area) for
  // the bar/shelf when it is attached to the top toolbar into |bounds|.
  // |background_origin| is the origin to use for painting the theme image.
  static void PaintBackgroundAttachedMode(
      gfx::Canvas* canvas,
      ui::ThemeProvider* theme_provider,
      const gfx::Rect& bounds,
      const gfx::Point& background_origin,
      chrome::HostDesktopType host_desktop_type);

  // Paint the horizontal border separating the shelf/bar from the toolbar or
  // page content according to |at_top| with |color|.
  static void PaintHorizontalBorder(gfx::Canvas* canvas,
                                    DetachableToolbarView* view,
                                    bool at_top,
                                    SkColor color);

 private:
  DISALLOW_COPY_AND_ASSIGN(DetachableToolbarView);
};

#endif  // CHROME_BROWSER_UI_VIEWS_DETACHABLE_TOOLBAR_VIEW_H_
