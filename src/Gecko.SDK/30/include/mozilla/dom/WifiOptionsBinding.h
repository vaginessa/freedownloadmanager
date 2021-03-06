/* THIS FILE IS AUTOGENERATED - DO NOT EDIT */

#ifndef mozilla_dom_WifiOptionsBinding_h__
#define mozilla_dom_WifiOptionsBinding_h__

#include "js/RootingAPI.h"
#include "jspubtd.h"
#include "mozilla/ErrorResult.h"
#include "mozilla/dom/BindingDeclarations.h"
#include "mozilla/dom/Nullable.h"

namespace mozilla {
namespace dom {

class NativePropertyHooks;
class ProtoAndIfaceArray;
struct WifiCommandOptionsAtoms;
struct WifiResultOptionsAtoms;

} // namespace dom
} // namespace mozilla

namespace mozilla {
namespace dom {

struct WifiCommandOptions : public DictionaryBase
{
  nsString mCmd;
  Optional<nsString > mDefaultValue;
  Optional<int32_t > mDns1;
  Optional<int32_t > mDns2;
  Optional<int32_t > mGateway;
  int32_t mId;
  Optional<nsString > mIfname;
  Optional<int32_t > mIpaddr;
  Optional<nsString > mKey;
  Optional<int32_t > mMask;
  Optional<nsString > mRequest;
  Optional<int32_t > mRoute;
  Optional<nsString > mValue;

  WifiCommandOptions();

  explicit inline WifiCommandOptions(const WifiCommandOptions& aOther)
  {
    *this = aOther;
  }

protected:
  explicit inline WifiCommandOptions(int )
  {
    // Do nothing here; this is used by our "Fast" subclass
  }

private:
  static bool
  InitIds(JSContext* cx, WifiCommandOptionsAtoms* atomsCache);

public:
  bool
  Init(JSContext* cx, JS::Handle<JS::Value> val, const char* sourceDescription = "Value");

  bool
  Init(const nsAString& aJSON);

  bool
  ToObject(JSContext* cx, JS::Handle<JSObject*> parentObject, JS::MutableHandle<JS::Value> rval) const;

  void
  TraceDictionary(JSTracer* trc);

  void
  operator=(const WifiCommandOptions& aOther);
};

namespace binding_detail {
struct FastWifiCommandOptions : public WifiCommandOptions
{
  inline FastWifiCommandOptions()
    : WifiCommandOptions(42)
  {
    // Doesn't matter what int we pass to the parent constructor
  }
};
} // namespace binding_detail


struct WifiResultOptions : public DictionaryBase
{
  nsString mBroadcast_str;
  int32_t mDns1;
  nsString mDns1_str;
  int32_t mDns2;
  nsString mDns2_str;
  nsString mError;
  int32_t mGateway;
  nsString mGateway_str;
  int32_t mId;
  int32_t mIpaddr;
  nsString mIpaddr_str;
  int32_t mLease;
  int32_t mMask;
  nsString mMask_str;
  nsString mReply;
  nsString mRoute;
  int32_t mServer;
  nsString mServer_str;
  int32_t mStatus;
  nsString mValue;
  nsString mVendor_str;

  WifiResultOptions();

  explicit inline WifiResultOptions(const WifiResultOptions& aOther)
  {
    *this = aOther;
  }

protected:
  explicit inline WifiResultOptions(int )
  {
    // Do nothing here; this is used by our "Fast" subclass
  }

private:
  static bool
  InitIds(JSContext* cx, WifiResultOptionsAtoms* atomsCache);

public:
  bool
  Init(JSContext* cx, JS::Handle<JS::Value> val, const char* sourceDescription = "Value");

  bool
  Init(const nsAString& aJSON);

  bool
  ToObject(JSContext* cx, JS::Handle<JSObject*> parentObject, JS::MutableHandle<JS::Value> rval) const;

  void
  TraceDictionary(JSTracer* trc);

  void
  operator=(const WifiResultOptions& aOther);
};

namespace binding_detail {
struct FastWifiResultOptions : public WifiResultOptions
{
  inline FastWifiResultOptions()
    : WifiResultOptions(42)
  {
    // Doesn't matter what int we pass to the parent constructor
  }
};
} // namespace binding_detail


} // namespace dom
} // namespace mozilla

#endif // mozilla_dom_WifiOptionsBinding_h__
