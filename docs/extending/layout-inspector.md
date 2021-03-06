---
id: layout-inspector
title: Extending Layout Inspector
---

The Layout Inspector plugin can be extended to support new kinds of UI components. You can also extend it to customize the data made available in the sidebar.
Depending on whether you want to expose new data on Android or iOS, there are different interfaces you can use.

![Layout Inspector](/docs/assets/layoutinspector.png)

## Android

### Node Descriptor
To expose an object to the Layout Inspector in Flipper you have to implement a `NodeDescriptor` which describes your object. For example the `ViewDescriptor` describes `View` objects and the `FragmentDescriptor` describe `Fragment` instances. These descriptors have a set of callbacks used to expose children and data associated with the object they describe. See [`NodeDescriptor`](https://github.com/facebook/flipper/blob/b0d2983bd440dc41ec67089e11acd394e6566b8f/android/src/main/java/com/facebook/flipper/plugins/inspector/NodeDescriptor.java) for the full API.

`NodeDescriptor` implementations should not subclass other `NodeDescriptor` implementations. Instead to re-use existing behavior from a more generic descriptor, you should prefer to use delegate.

**Don't**

```java
class ViewGroupDescriptor extends ViewDescriptor<ViewGroup> {
  public String getName(ViewGroup node) {
    return super.getName(node);
  }
}
```

**Do**

```java
class ViewGroupDescriptor extends NodeDescriptor<ViewGroup> {
  public String getName(ViewGroup node) {
    NodeDescriptor descriptor = descriptorForClass(View.class);
    return descriptor.getName(node);
  }
}
```

### Register a Descriptor

Register your descriptor in the `DescriptorMapping` used to instantiate the `InspectorFlipperPlugin`.

```java
final FlipperClient client = FlipperClient.createInstance(mContext);
final DescriptorMapping descriptorMapping = DescriptorMapping.withDefaults();
descriptorMapping.register(MyObject.class, new MyObjectDescriptor());
client.addPlugin(new InspectorFlipperPlugin(mContext, descriptorMapping));
```

### Extending an existing Descriptor

You may not need to create a whole new descriptor but instead you may just want to change extend an existing one to expose some new piece of data. In that case just locate the correct descriptor and edit its `getData`, `getAttributes` and perhaps `setData` methods.

## iOS

### SKNodeDescriptor

To expose an object to the layout inspector in Sonar you have to implement a `SKNodeDescriptor` which describes the object. For example `SKViewDescriptor` describes `UIView` objects, and the `SKComponentDescriptor` describes `CKComponent` objects. These descriptors have necessary callbacks which is used to expose its children and data associated with the object they describe. See [SKNodeDescriptor](https://github.com/facebook/flipper/blob/b0d2983bd440dc41ec67089e11acd394e6566b8f/iOS/Plugins/FlipperKitLayoutPlugin/FlipperKitLayoutPlugin/SKNodeDescriptor.h) for the full available API.

`SKNodeDescriptor` implementations should **never** be subclass other `SKNodeDescriptor` implementations. Instead re-use existing behaviour by explicitly using other descriptors and delegate behaviour.

**Don't**

```objc
@interface SKArbitraryViewDescriptor : SKViewDescriptor<ArbitraryView *>

@end

@implementation SKArbitraryViewDescriptor

- (NSString *)identifierForNode:(ArbitraryView *)node 
{
  return [super identifierForNode:node];
}

@end
```


**Do**

```objc
@interface SKArbitraryViewDescriptor : SKNodeDescriptor<ArbitraryView *>
@end

@implementation SKArbitraryViewDescriptor

- (NSString *)identifierForNode:(ArbitraryView *)node 
{
  SKNodeDescriptor *descriptor = [self descriptorForClass:[UIView class]];
  return [descriptor identifierForNode:node];
}

@end
```

### Register a Descriptor

In order to register your descriptor for an object, you use `SKDescriptorMapper`. After registering all descriptors you pass on the descriptor-mapper object to the plugin during initialisation.

```objc
[descriptorMapper registerDescriptor:[SKArbitraryViewDescriptor new]
                            forClass:[AbritraryView class]];
                                           
```

There's already a set of descriptors registered by default in
`SKDescriptorMapper`, and if you want to add a descriptor to the default set
you can do it in [`SKDescriptorMapper`](https://github.com/facebook/flipper/blob/b0d2983bd440dc41ec67089e11acd394e6566b8f/iOS/Plugins/FlipperKitLayoutPlugin/FlipperKitLayoutPlugin/SKDescriptorMapper.mm).

### Extending an existing Descriptor

Sometimes all you need is to extend the functionality of an existing descriptor. In that case you just need to locate the correct descriptor and edit the methods `dataForNode`, `attributesForNode`, and possibly `dataMutationsForNode`.


### Subdescriptors

If you want to extend the `SKComponentKitLayoutDescriptor` and add an additional section based on the nodes of the `SKComponentLayoutDescriptor`, you can use `SKSubDescriptor`.

```objc
#import <FlipperKitLayoutComponentKitSupport/SKComponentLayoutWrapper.h>

NSString *YourSubDescriptor(SKComponentLayoutWrapper *)node {
	return @"Meta data";
}

// At setup time, you must register it:
[SKComponentLayoutDescriptor registerSubDescriptor:&YourSubDescriptor forName:@"Section Name"];
```

Swift support is not yet available because you must access `SKComponentLayoutWrapper` to implement a subdescriptor.
