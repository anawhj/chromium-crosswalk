// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_TASK_MANAGER_TASK_MANAGER_PANEL_RESOURCE_PROVIDER_H_
#define CHROME_BROWSER_TASK_MANAGER_TASK_MANAGER_PANEL_RESOURCE_PROVIDER_H_

#include <map>

#include "base/basictypes.h"
#include "base/string16.h"
#include "chrome/browser/task_manager/task_manager_render_resource.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"
#include "ui/gfx/image/image_skia.h"

class Panel;
class TaskManager;

namespace content {
class WebContents;
}

namespace extensions {
class Extension;
}

class TaskManagerPanelResource : public TaskManagerRendererResource {
 public:
  explicit TaskManagerPanelResource(Panel* panel);
  virtual ~TaskManagerPanelResource();

  // TaskManager::Resource methods:
  virtual Type GetType() const OVERRIDE;
  virtual string16 GetTitle() const OVERRIDE;
  virtual string16 GetProfileName() const OVERRIDE;
  virtual gfx::ImageSkia GetIcon() const OVERRIDE;
  virtual content::WebContents* GetWebContents() const OVERRIDE;
  virtual const extensions::Extension* GetExtension() const OVERRIDE;

 private:
  Panel* panel_;
  // Determines prefix for title reflecting whether extensions are apps
  // or in incognito mode.
  int message_prefix_id_;

  DISALLOW_COPY_AND_ASSIGN(TaskManagerPanelResource);
};

class TaskManagerPanelResourceProvider
    : public TaskManager::ResourceProvider,
      public content::NotificationObserver {
 public:
  explicit TaskManagerPanelResourceProvider(TaskManager* task_manager);

  // TaskManager::ResourceProvider methods:
  virtual TaskManager::Resource* GetResource(int origin_pid,
                                             int render_process_host_id,
                                             int routing_id) OVERRIDE;
  virtual void StartUpdating() OVERRIDE;
  virtual void StopUpdating() OVERRIDE;

  // content::NotificationObserver method:
  virtual void Observe(int type,
                       const content::NotificationSource& source,
                       const content::NotificationDetails& details) OVERRIDE;

 private:
  virtual ~TaskManagerPanelResourceProvider();

  void Add(Panel* panel);
  void Remove(Panel* panel);

  // Whether we are currently reporting to the task manager. Used to ignore
  // notifications sent after StopUpdating().
  bool updating_;

  TaskManager* task_manager_;

  // Maps the actual resources (the Panels) to the Task Manager resources.
  typedef std::map<Panel*, TaskManagerPanelResource*> PanelResourceMap;
  PanelResourceMap resources_;

  // A scoped container for notification registries.
  content::NotificationRegistrar registrar_;

  DISALLOW_COPY_AND_ASSIGN(TaskManagerPanelResourceProvider);
};

#endif  // CHROME_BROWSER_TASK_MANAGER_TASK_MANAGER_PANEL_RESOURCE_PROVIDER_H_
