// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_KEYED_SERVICE_CONTENT_BROWSER_CONTEXT_KEYED_SERVICE_FACTORY_H_
#define COMPONENTS_KEYED_SERVICE_CONTENT_BROWSER_CONTEXT_KEYED_SERVICE_FACTORY_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "components/keyed_service/core/keyed_service_export.h"
#include "components/keyed_service/core/keyed_service_factory.h"

class BrowserContextDependencyManager;
class KeyedService;

namespace content {
class BrowserContext;
}

// Base class for Factories that take a BrowserContext object and return some
// service on a one-to-one mapping. Each factory that derives from this class
// *must* be a Singleton (only unit tests don't do that).
//
// We do this because services depend on each other and we need to control
// shutdown/destruction order. In each derived classes' constructors, the
// implementors must explicitly state which services are depended on.
class KEYED_SERVICE_EXPORT BrowserContextKeyedServiceFactory
    : public KeyedServiceFactory {
 public:
  // Registers preferences used in this service on the pref service of
  // |context|. This is the public interface and is safe to be called multiple
  // times because testing code can have multiple services of the same type
  // attached to a single |context|. Only test code is allowed to call this
  // method.
  // TODO(gab): This method can be removed entirely when
  // PrefService::DeprecatedGetPrefRegistry() is phased out.
  void RegisterUserPrefsOnBrowserContextForTest(
      content::BrowserContext* context);

  // A function that supplies the instance of a KeyedService for a given
  // BrowserContext. This is used primarily for testing, where we want to feed
  // a specific mock into the BCKSF system.
  typedef KeyedService* (*TestingFactoryFunction)(
      content::BrowserContext* context);

  // Associates |factory| with |context| so that |factory| is used to create
  // the KeyedService when requested.  |factory| can be NULL to signal that
  // KeyedService should be NULL. Multiple calls to SetTestingFactory() are
  // allowed; previous services will be shut down.
  void SetTestingFactory(content::BrowserContext* context,
                         TestingFactoryFunction factory);

  // Associates |factory| with |context| and immediately returns the created
  // KeyedService. Since the factory will be used immediately, it may not be
  // NULL.
  KeyedService* SetTestingFactoryAndUse(content::BrowserContext* context,
                                        TestingFactoryFunction factory);

 protected:
  // BrowserContextKeyedServiceFactories must communicate with a
  // BrowserContextDependencyManager. For all non-test code, write your subclass
  // constructors like this:
  //
  //   MyServiceFactory::MyServiceFactory()
  //     : BrowserContextKeyedServiceFactory(
  //         "MyService",
  //         BrowserContextDependencyManager::GetInstance())
  //   {}
  BrowserContextKeyedServiceFactory(const char* name,
                                    BrowserContextDependencyManager* manager);
  ~BrowserContextKeyedServiceFactory() override;

  // Common implementation that maps |context| to some service object. Deals
  // with incognito contexts per subclass instructions with
  // GetBrowserContextRedirectedInIncognito() and
  // GetBrowserContextOwnInstanceInIncognito() through the
  // GetBrowserContextToUse() method on the base.  If |create| is true, the
  // service will be created using BuildServiceInstanceFor() if it doesn't
  // already exist.
  KeyedService* GetServiceForBrowserContext(content::BrowserContext* context,
                                            bool create);

  // Interface for people building a concrete FooServiceFactory: --------------

  // Finds which browser context (if any) to use.
  virtual content::BrowserContext* GetBrowserContextToUse(
      content::BrowserContext* context) const;

  // By default, we create instances of a service lazily and wait until
  // GetForBrowserContext() is called on our subclass. Some services need to be
  // created as soon as the BrowserContext has been brought up.
  virtual bool ServiceIsCreatedWithBrowserContext() const;

  // By default, TestingBrowserContexts will be treated like normal contexts.
  // You can override this so that by default, the service associated with the
  // TestingBrowserContext is NULL. (This is just a shortcut around
  // SetTestingFactory() to make sure our contexts don't directly refer to the
  // services they use.)
  bool ServiceIsNULLWhileTesting() const override;

  // Interface for people building a type of BrowserContextKeyedFactory: -------

  // All subclasses of BrowserContextKeyedServiceFactory must return a
  // KeyedService instead of just a BrowserContextKeyedBase.
  virtual KeyedService* BuildServiceInstanceFor(
      content::BrowserContext* context) const = 0;

  // A helper object actually listens for notifications about BrowserContext
  // destruction, calculates the order in which things are destroyed and then
  // does a two pass shutdown.
  //
  // First, BrowserContextShutdown() is called on every ServiceFactory and will
  // usually call KeyedService::Shutdown(), which gives each
  // KeyedService a chance to remove dependencies on other
  // services that it may be holding.
  //
  // Secondly, BrowserContextDestroyed() is called on every ServiceFactory
  // and the default implementation removes it from |mapping_| and deletes
  // the pointer.
  virtual void BrowserContextShutdown(content::BrowserContext* context);
  virtual void BrowserContextDestroyed(content::BrowserContext* context);

 private:
  friend class BrowserContextDependencyManagerUnittests;

  // Registers any user preferences on this service. This is called by
  // RegisterPrefsIfNecessaryForContext() and should be overriden by any service
  // that wants to register profile-specific preferences.
  virtual void RegisterProfilePrefs(
      user_prefs::PrefRegistrySyncable* registry) {}

  // KeyedServiceFactory:
  KeyedService* BuildServiceInstanceFor(
      base::SupportsUserData* context) const final;
  bool IsOffTheRecord(base::SupportsUserData* context) const final;

  // KeyedServiceBaseFactory:
  base::SupportsUserData* GetContextToUse(
      base::SupportsUserData* context) const final;
  bool ServiceIsCreatedWithContext() const final;
  void ContextShutdown(base::SupportsUserData* context) final;
  void ContextDestroyed(base::SupportsUserData* context) final;
  void RegisterPrefs(user_prefs::PrefRegistrySyncable* registry) final;

  DISALLOW_COPY_AND_ASSIGN(BrowserContextKeyedServiceFactory);
};

#endif  // COMPONENTS_KEYED_SERVICE_CONTENT_BROWSER_CONTEXT_KEYED_SERVICE_FACTORY_H_
